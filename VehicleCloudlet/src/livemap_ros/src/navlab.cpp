#include "navlab.h"
#include <math.h>
#include <stdlib.h>  


Navlab::Navlab(ros::NodeHandle &n)
    : m_nodeHandle(n), m_loop_rate(30),
      m_fps(30), m_videoFile("Video.mp4"), m_numVideos(0), m_numRawImages(0), m_nameGPSLog("GPS_log.txt") , m_numGPSCoords(0) ,
      OPENCV_WINDOW("What Vehicle Cloudlet Sees"), OPENCV_WINDOW2("What Zone Cloudlet Sees"), OPENCV_WINDOW3("All Detections"),
      m_navLat(-40), m_navLong(70.), m_oldLat(0), m_oldLong(0), m_oldAngle(0), m_navAngle(0), m_distFromLastSend(100),
      m_LatLastDetection(0.),m_LongLastDetection(0.),
      m_numDetections(0), m_HAZARD_TOPIC("HAZARDS_DETECTED"), m_DRIVE_TOPIC("DRIVE")
{
    // Get roslaunch param _directory
    m_nodeHandle.getParam("directory", m_workingDirectory);

    m_frameDirectory = m_workingDirectory + "/frame";
    const char *c_frameDirectory = m_frameDirectory.c_str();
    mkdir(c_frameDirectory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    m_detectionsDirectory = m_workingDirectory + "/detections";
    const char *c_detectionsDirectory = m_frameDirectory.c_str();
    mkdir(c_detectionsDirectory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    m_videoDirectory = m_workingDirectory + "/video";
    const char *c_videoDirectory = m_videoDirectory.c_str();
    mkdir(c_videoDirectory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    std::cout << "Working Directory: " << m_workingDirectory << std::endl;
    std::cout << "Frame Directory: " << m_frameDirectory << std::endl;
    std::cout << "Detections Directory: " << m_detectionsDirectory << std::endl;
    std::cout << "Video Directory: " << m_videoDirectory << std::endl;

    // VideoWriter initialization
    //m_videoFile = m_workingDirectory + "/" + m_videoFile;

    //Subscribe to 3 topics 1) Raw Image 2) Detected Images 3) GPS fix
    //m_rawImageSub = m_nodeHandle.subscribe("/usb_cam/image_raw", 1000, &Navlab::rawImageCallback, this);
    
    m_rawImageSub = m_nodeHandle.subscribe("/camera/image_raw", 1, &Navlab::rawImageCallback, this);
    //commented for writing frames for ffmpeg
    m_detectedImageSub = m_nodeHandle.subscribe("/image_detected", 1, &Navlab::detectionImageCallback, this);
    
    //m_detectedImageSub = m_nodeHandle.subscribe("/camera/image_raw", 1, &Navlab::detectionImageCallback, this);
    m_navlabGPSSub = m_nodeHandle.subscribe("/fix", 10, &Navlab::navlabGPSCallback, this);

    //Create 2 OpenCV windows to view
    cv::namedWindow(OPENCV_WINDOW, cv::WINDOW_NORMAL);
    cv::namedWindow(OPENCV_WINDOW2, cv::WINDOW_NORMAL);
    cv::namedWindow(OPENCV_WINDOW3, cv::WINDOW_NORMAL);

    // Move Windows
    cv::moveWindow(OPENCV_WINDOW, 0, 0);
    cv::moveWindow(OPENCV_WINDOW2, 0, 540);
    cv::moveWindow(OPENCV_WINDOW3, 600, 540);
    // Resize Windows
    cv::resizeWindow(OPENCV_WINDOW, 500, 440);
    cv::resizeWindow(OPENCV_WINDOW2, 500, 440);
    cv::resizeWindow(OPENCV_WINDOW3, 500, 440);

    const std::string SERVER_ADDRESS = "tcp://squall.elijah.cs.cmu.edu:1883" ;
    const std::string CLIENT_ID =  "async_publish" ;

    string	address  = SERVER_ADDRESS,
			clientID = CLIENT_ID;

	cout << "Initializing for server '" << address << "'..." << endl;
	m_client =  new mqtt::async_client(address, clientID);


	m_client->set_callback(m_cb);
    

	
    m_conopts.set_keep_alive_interval(20);
  try {
		cout << "\nConnecting..." << endl;
		m_conntok = m_client->connect(m_conopts);
		cout << "Waiting for the connection..." << endl;
		m_conntok->wait();
		cout << "  ...OK" << endl;
  }
  catch (const mqtt::exception& exc) {
		cerr << exc.what() << endl;
		//return 1;
	}

    m_startTime = std::clock();
}

Navlab::~Navlab()
{
    delete m_client;

    if(m_lastDetection != nullptr)
        delete m_lastDetection;
    //writeToVideo();
}

void Navlab::rawImageCallback(const sensor_msgs::ImageConstPtr &msg)
{
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception &e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }
    cv::imshow(OPENCV_WINDOW, cv_ptr->image);
    cv::waitKey(3);
    //Write image to file, make video of it later
    std::string filename = m_frameDirectory + "/frame" + std::to_string(m_numRawImages) + ".jpg";
    cv::imwrite(filename, cv_ptr->image);
    m_numRawImages++;
    logGPS(m_navLat , m_navLong);
    return;
}

void Navlab::detectionImageCallback(const sensor_msgs::ImageConstPtr &msg)
{
    /*
    clock_t end = clock();
    double elapsed_secs = double(end - m_startTime) / CLOCKS_PER_SEC;

    if(elapsed_secs < 0.1)
        return;
    else
        m_startTime = end;
    */
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception &e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    m_distFromLastSend = this->measureDistance(m_navLat,m_navLong,m_LatLastDetection, m_LongLastDetection);

    if(abs(m_distFromLastSend) > 1.0)
    {
        cv::imshow(OPENCV_WINDOW2, cv_ptr->image);
        cv::waitKey(3);
        //*m_lastDetection = cv_ptr->image.clone();
        std::string filename = m_frameDirectory + "/detections/" + std::to_string(m_numDetections) + ".jpg";
        cv::imwrite(filename, cv_ptr->image);

        m_numDetections++;
        std::string encodedIm;
        Utils::encodeThisImage(cv_ptr->image,encodedIm);
        std::string message;
        DetectionMessage s;

        s.HazardType_ = "cone";
        s.HazardID_=std::to_string(m_numDetections);
        s.UserID_="kevin";
        s.DriveID_="1"; 
        s.Date_="Today";
        // Position
        s.Latitude_=m_navLat;
        s.Longitude_=m_navLong;
        // Time & Latency
        s.TimestampSent_=1;
        s.TimestampReceived_=1;
        s.Latency_=0;
        // Detected Image
        s.Image_=encodedIm; 
        s.ImageID_="1a";
        s.HazardBoundingBox_[0] = 22;
        s.HazardBoundingBox_[1] = 45;
        s.HazardBoundingBox_[2] = 45;
        s.HazardBoundingBox_[3] = 75;
            // Categories
        s.IsActive_= true;
        s.IsVirtual_=false;

        Utils::makeDetectionJSON(message,s);
        mqtt::message_ptr pubmsg = mqtt::make_message(m_HAZARD_TOPIC, message);

        //action_listener alistener; //added line

        int qos = 1;
        pubmsg->set_qos(qos);
	    m_client->publish(pubmsg);//->wait_for(TIMEOUT);
	    cout << "Sent Image # " << m_numDetections << endl;
        m_LatLastDetection = m_navLat;
        m_LongLastDetection = m_navLong;
        
    }
    else{
        
        //cv::imshow(OPENCV_WINDOW2, *m_lastDetection);
        //cv::waitKey(3);
        cv::imshow(OPENCV_WINDOW3, cv_ptr->image);
        cv::waitKey(3);
        
    }
    
    //Write image to file, make video of it later
    

    return;
}

double Navlab::measureDistance(double lat1, double lon1, double lat2, double lon2)
{
    double R = 6378.137; // Radius of earth in KM
    double dLat = lat2 * M_PI / 180.0 - lat1 * M_PI / 180.0;
    double dLon = lon2 * M_PI / 180.0 - lon1 *M_PI / 180.0;
    double a = sin(dLat/2.0) * sin(dLat/2.0) + cos(lat1 *M_PI / 180.0) * cos(lat2 * M_PI / 180.0) * sin(dLon/2.0) * sin(dLon/2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0-a));
    double d = R * c;
    return d * 1000.0; // meters
}


void Navlab::navlabGPSCallback(const sensor_msgs::NavSatFixConstPtr &fix)
{

    if (fix->status.status == sensor_msgs::NavSatStatus::STATUS_NO_FIX)
    {
        std::cout << "Unable to get a fix on the location." << std::endl;
        return;
    }

    m_oldLat = m_navLat;
    m_oldLong = m_navLong;

    m_navLat = fix->latitude;
    m_navLong = fix->longitude;


    //clock_t end = clock();
    //double elapsed_secs = double(end - m_startTime) / CLOCKS_PER_SEC;
/*
    if(elapsed_secs < 1.)
        return;
    else
        m_startTime = end;
    */
    std::string message;
    DrivingCompleteMessage s;
    s.Latitude_ = m_navLat+0.00002;
    s.Longitude_ = m_navLong+0.00002;
    s.Angle_ = 0.;

    Utils::makeDriveJSON(message,s);
    mqtt::message_ptr pubmsg = mqtt::make_message(m_DRIVE_TOPIC, message);

        //action_listener alistener; //added line

    int qos = 1;
    pubmsg->set_qos(qos);
    m_client->publish(pubmsg);//->wait_for(TIMEOUT);
    

    // commented for now
    //logGPS(m_navX , m_navY);


    /*
    std::ifstream hazardFile;
    double x;
    double y;
    double garb;

    hazardFile.open("/home/kchrist/ros_bags/data/ex_haz.txt");
    std::string line;
    std::string lineIt;
    //std::vector<double> stringValues;
    std::string type;
    int showFlag = 0;
    double showX, showY, thetaC2C, thetaC2H, distance, navAngle;
    std::string showType;
    //std::cout << "Navlab x " << std::setprecision(10) << navX << std::endl;
    //std::cout << "Navlab y " << std::setprecision(10)<< navY << std::endl;

    while (std::getline(hazardFile, line))
    {
        std::stringstream ss(line);

        for (int i = 0; std::getline(ss, lineIt, ' '); i++)
        {
            if (i == 0)
                type = lineIt;
            else if (i == 1)
                y = std::atof(lineIt.c_str());
            else if (i == 2)
                x = std::atof(lineIt.c_str());
            //std::cout << "Hazard x " <<std::setprecision(10) << x << std::endl;
            //std::cout <<"Hazard y " <<std::setprecision(10)<< y << std::endl;
            //stringValues.insert(values.end(), atoi(data.c_str()));

            //std::cout << i << std::endl;
            //std::cout << std::atof(lineIt.c_str()) << std::endl;
            if (std::abs(navX - oldX) > 0.000004 && std::abs(navY - oldY) > 0.000004)
            {
                oldAngle = navAngle;
                navAngle = atan2(navY - oldY, navX - oldX) * 180 / PI;
            }
            thetaC2H = atan2(y - navY, x - navX) * 180 / PI;
            distance = sqrt(pow(y - navY, 2.0) + pow(x - navX, 2.0));
            double isTurning = std::abs(navAngle - oldAngle);
            if (std::abs(navAngle - thetaC2H) < 22 && distance <= 0.01)
            {
                //std::cout <<"Diff x:  " <<std::setprecision(10)<< std::abs(x-navX) << std::endl;
                // std::cout <<"Diff y:  " <<std::setprecision(10)<< std::abs(y-navY) << std::endl;

                showFlag = 1;
                showX = x;
                showY = y;
                showType = type;
            }
        }
    }
    if (showFlag == 1)
    {
        cv::Mat warningIm = BLANK.clone();
        std::string warning = "Warning," + showType + " reported Ahead";
        cv::putText(warningIm, warning, cv::Point2f(40, 240), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(1, 1, 1, 255), 4, cv::LINE_AA);
        //std::string loc = "At (Latitude, Longitude): (" + std::to_string(showX) + " , " + std::to_string(showY) + ")";
        //cv::putText(warningIm, loc, cv::Point2f(40, 290), cv::FONT_HERSHEY_SIMPLEX, 1.0,  cv::Scalar(1,1,1,255), 4, cv::LINE_AA);

        cv::imshow(OPENCV_WINDOW3, warningIm);
        cv::waitKey(3);
    }
    else
    */
    //{
    //cv::imshow(OPENCV_WINDOW3, NOHAZARD);
    //cv::waitKey(3);
    //}

    return;
}

void Navlab::runROS()
{
    // main ROS loop
    //while (ros::ok())
   // {
        ros::spin();
       // m_loop_rate.sleep();
   // }
}

void Navlab::writeToVideo()
{
    // Create video command 1) Frames are in m_frameDirectory 2) Write video file to m_videoDirectory as output.mp4
    // Do in background
    std::string command = "avconv -f image2 -i " + m_frameDirectory + "/frame%d.jpg " + m_videoDirectory + "/output" + std::to_string(m_numVideos) + ".mp4 &";
    const char *c_command = command.c_str();

    // Call Command to write video using avconv
    std::cout << "Writing Video file to: " + m_videoDirectory + "/output" + std::to_string(m_numVideos) + ".mp4";
    system(c_command);

    // Increment counter for number of videos (unused)
    m_numVideos++;
    return;
}

void Navlab::logGPS(const double &latitude , const double &longitude)
{

    // Debug
    /*
    std::cout << "Navlab x " << std::setprecision(10) << navX << std::endl;
    std::cout << "Navlab y " << std::setprecision(10)<< navY << std::endl;
    */
    
    // Open log of GPS Coordinates.  Print them out one on top of the other for minimal overhead of transferring

    m_GPSFile.open(m_nameGPSLog, std::ofstream::app); //append
    m_GPSFile << std::setprecision(10) << latitude << "\n" << longitude << "\n";
    m_GPSFile.close();

    // Increment number of GPS coordinates printed
    m_numGPSCoords++;

    return;
}


void Navlab::sendDetectionToZoneCloudlet(DetectionMessage &message)
{
    
}
