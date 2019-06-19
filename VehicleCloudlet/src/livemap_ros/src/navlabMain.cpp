#include "ros/ros.h"
#include "std_msgs/String.h"
//#include <stdio.h>

#include <iostream>
#include <fstream>
#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/NavSatStatus.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include "navlab.h"
#include "utils.h"

#define PI 3.14159265




/*void chatterCallback(const std_msgs::String::ConstPtr &msg)
{
  ROS_INFO("I heard: [%s]", msg->data.c_str());
}
*/

//Global Variables

/*
static const std::string OPENCV_WINDOW = "What Vehicle Cloudlet Sees";
static const std::string OPENCV_WINDOW2 = "What Zone Cloudlet Sees";
static const std::string OPENCV_WINDOW3 = "Hazard Warnings to Vehicle Cloudlet";

cv::Mat NOHAZARD = cv::imread("/home/kchrist/ros_bags/data/no_hazard.png", CV_LOAD_IMAGE_COLOR);
cv::Mat BLANK = cv::imread("/home/kchrist/ros_bags/data/blank.png", CV_LOAD_IMAGE_COLOR);

int counterIm = 0;
std::string filename;
int imFlag = 0;
std::vector<int> haz;
std::vector<int>::iterator it;
double navX, navY, oldX, oldY, oldAngle,navAngle;

*/

/*
void imageCallback(const sensor_msgs::ImageConstPtr &msg){
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
}

void imageCallback2(const sensor_msgs::ImageConstPtr &msg)
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
 
  cv::imshow(OPENCV_WINDOW2, cv_ptr->image);
  cv::waitKey(3);

  counterIm++;

  return;
}

void chatterCallback(const sensor_msgs::NavSatFixConstPtr &fix){

  if (fix->status.status == sensor_msgs::NavSatStatus::STATUS_NO_FIX)
  {
    std::cout << "Unable to get a fix on the location." << std::endl;
    return;
  }
  
  if(counterIm>0){
    oldX = navX;
    oldY = navY;
  }
  navX = fix->latitude;
  navY = fix->longitude;

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
  double showX,showY,thetaC2C, thetaC2H, distance,navAngle;
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
          if(std::abs(navX - oldX) > 0.000004 && std::abs(navY - oldY) > 0.000004){
            oldAngle = navAngle;
            navAngle = atan2(navY - oldY, navX - oldX)*180/PI;
          }
          thetaC2H = atan2(y - navY, x - navX)*180/PI;
          distance = sqrt( pow(y - navY,2.0) + pow(x - navX,2.0));
          double isTurning = std::abs(navAngle - oldAngle);
          if(std::abs(navAngle-thetaC2H) < 22 && distance <= 0.01)
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
  if(showFlag == 1){
      cv::Mat warningIm = BLANK.clone();
      std::string warning = "Warning," + showType +" reported Ahead";
      cv::putText(warningIm, warning, cv::Point2f(40, 240), cv::FONT_HERSHEY_SIMPLEX, 1.0,  cv::Scalar(1,1,1,255), 4, cv::LINE_AA);
      //std::string loc = "At (Latitude, Longitude): (" + std::to_string(showX) + " , " + std::to_string(showY) + ")";
      //cv::putText(warningIm, loc, cv::Point2f(40, 290), cv::FONT_HERSHEY_SIMPLEX, 1.0,  cv::Scalar(1,1,1,255), 4, cv::LINE_AA);

      cv::imshow(OPENCV_WINDOW3, warningIm);
      cv::waitKey(3);
  }
  else{
    cv::imshow(OPENCV_WINDOW3, NOHAZARD);
    cv::waitKey(3);
  }
  
}
*/
/*

void imageCallback(const sensor_msgs::ImageConstPtr& msg) {

    imFlag = 1;
    std::ostringstream name ;
    if(counterIm<=9)
      name << "/home/kchrist/ros_bags/data/out/out-frame000" << counterIm << ".jpg" ;
    else if(counterIm<=99)
      name << "/home/kchrist/ros_bags/data/out/out-frame00" << counterIm << ".jpg" ;
    else if(counterIm<=999)
      name << "/home/kchrist/ros_bags/data/out/out-frame0" << counterIm << ".jpg" ;
    else if(counterIm>999)
      name << "/home/kchrist/ros_bags/data/out/out-frame" << counterIm << ".jpg" ;
    
    filename = name.str();
    //std::cout >> filename >> std::endl;
    counterIm++;

    image = cv::imread(filename, CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        //std::cout >>  "Could not open or find the image" >> std::endl;
        return;
    }


    return;

}
    */

int main(int argc, char **argv)
{
  ros::init(argc, argv, "listener2");
  ros::NodeHandle nodeHandle("~");


  Navlab myNavlab(nodeHandle);
  
  myNavlab.runROS();

/*
  string	address  = (argc > 1) ? string(argv[1]) : DFLT_SERVER_ADDRESS,
			clientID = (argc > 2) ? string(argv[2]) : DFLT_CLIENT_ID;

	cout << "Initializing for server '" << address << "'..." << endl;
	mqtt::async_client client(address, clientID);


	callback cb;
	client.set_callback(cb);
    

	mqtt::connect_options conopts;
  try {
		cout << "\nConnecting..." << endl;
		mqtt::token_ptr conntok = client.connect(conopts);
		cout << "Waiting for the connection..." << endl;
		conntok->wait();
		cout << "  ...OK" << endl;
  }
  catch (const mqtt::exception& exc) {
		cerr << exc.what() << endl;
		return 1;
	}
  */
  //cv::Mat imv = cv::imread("./images/frame109.jpg");
  //cv::imwrite("yolo.jpg",IMAGE);
  //cv::namedWindow("TEST", cv::WINDOW_AUTOSIZE);

  /*
  std::string encodedIm;
  Utils::encodeThisImage(imv,encodedIm);
  

  std::string message;
  DetectionMessage s;
	s.HazardType_ = "big one";
	s.HazardID_="1234a";
  s.HazardType_="deer";
  s.UserID_="1234b";
  s.DriveID_="1234c"; 
  s.Date_="date_please";
  // Position
  s.Latitude_=-40.26276372111;
  s.Longitude_=72.29483928293111;
  // Time & Latency
  s.TimestampSent_=0.000000001234;
  s.TimestampReceived_=0.0000001234;
  s.Latency_=0.00000000001;
  // Detected Image
  s.Image_=encodedIm; //Conver to Mat?
  s.ImageID_="1234e";
  s.HazardBoundingBox_[0] = 22;
	s.HazardBoundingBox_[1] = 45;
	s.HazardBoundingBox_[2] = 45;
	s.HazardBoundingBox_[3] = 75;
    // Categories
  s.IsActive_= true;
  s.IsVirtual_=false;

  Utils::makeDetectionJSON(message,s);
	std::cout << message << std::endl;

  Utils::parseDetectionJSON(message,s);
	std::cout << message << std::endl;
  */
  /*
  cout << "\nSending message..." << endl;
  mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC, message);
  pubmsg->set_qos(QOS);
	client.publish(pubmsg)->wait_for(TIMEOUT);
	cout << "  ...OK" << endl;
*/
  //

  
  /*
  while (ros::ok())
  {

    ros::spinOnce();

    imFlag = 1;
    std::ostringstream name ;
    if(counterIm<=9)
      name << "/home/kchrist/ros_bags/data/out/out-frame000" << counterIm << ".jpg" ;
    else if(counterIm<=99)
      name << "/home/kchrist/ros_bags/data/out/out-frame00" << counterIm << ".jpg" ;
    else if(counterIm<=999)
      name << "/home/kchrist/ros_bags/data/out/out-frame0" << counterIm << ".jpg" ;
    else if(counterIm>999)
      name << "/home/kchrist/ros_bags/data/out/out-frame" << counterIm << ".jpg" ;
    
    filename = name.str();
    //std::cout >> filename >> std::endl;
    counterIm++;

    //image = cv::imread(filename, CV_LOAD_IMAGE_COLOR);   // Read the file

    //if(! image.data )                              // Check for invalid input
    //{
        //std::cout >>  "Could not open or find the image" >> std::endl;
    //}


    std::cout << filename << std::endl;
    
    //cv::imshow("Display window", image );
  
    //cv::waitKey(10);
    //imFlag=1;
    //}

    loop_rate.sleep();
  
  }
  */

  return 0;
}