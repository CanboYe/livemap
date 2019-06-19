#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Int16.h"
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

/**
 * This tutorial demonstrates simple receipt of messages over the ROS system.
 */
/*void chatterCallback(const std_msgs::String::ConstPtr &msg)
{
  ROS_INFO("I heard: [%s]", msg->data.c_str());
}
*/
static const std::string OPENCV_WINDOW = "What Vehicle Cloudlet Sees ROSBAG";
//static const std::string OPENCV_WINDOW2 = "What Zone Cloudlet Sees";
int counterIm = 0;
std::string filename;
int imFlag = 0;
std::vector<int> haz;
std::vector<int>::iterator it;
double x,y;

int FLAG;

cv::Mat image; // = cv::imread("/home/kchrist/ros_bags/data/out/out-frame0000.jpg", CV_LOAD_IMAGE_COLOR);

void chatterCallback(const sensor_msgs::NavSatFixConstPtr &fix)
{

  if (fix->status.status == sensor_msgs::NavSatStatus::STATUS_NO_FIX)
  {
    std::cout << "Unable to get a fix on the location." << std::endl;
    return;
  }

  std::cout << std::setprecision(10) << "Current Latitude: " << fix->latitude << std::endl;
  std::cout << std::setprecision(10) << "Current Longitude " << fix->longitude << std::endl;

  std::ofstream myFile;
  //myFile.open("/tmp/navlabLog.txt", std::ofstream::app); //append to file

  x = fix->latitude;
  y = fix->longitude;


  myFile.open("/tmp/navlabLog.txt", std::ofstream::out); //overwrite
  myFile << std::setprecision(10) << fix->latitude << " " << fix->longitude << "\n";
  myFile.close();
  if(FLAG==1){
    std::ofstream myHazardFile;
    myHazardFile.open("/home/kchrist/livemap/pittsburgh/pittsburgh-hazard2.csv", std::ofstream::app); 
    myHazardFile << "pothole " << std::setprecision(10) << x << " " << y << "\n";
    myHazardFile.close();
  }
  


}

void imageCallback(const sensor_msgs::ImageConstPtr &msg)
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
  //std::ofstream myHazardFile;
  // Update opencv gui window
  //std::ostringstream name;
  //name << "/home/kchrist/ros_bags/data/out/out-frame" << counterIm << ".jpg";
  //filename = name.str();
  image = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
  cv::imshow(OPENCV_WINDOW, cv_ptr->image);
  cv::waitKey(3);
  //cv::imshow(OPENCV_WINDOW2, image);

  //std::ostringstream name;
  //name << "/home/kchrist/ros_bags/data/in2/frame" << counterIm << ".jpg";
  //filename = name.str();
  //cv::imwrite(filename,cv_ptr->image);
  /*
  for (it = haz.begin(); it != haz.end(); it++){
    //it = haz.begin();
    std::cout << *it << std::endl;
  }
  */
 /*
  if (it != haz.end() && *it <= counterIm)
  {
    
    //std::cout << *it << std::endl;
    cv::imshow(OPENCV_WINDOW2, image);
    cv::waitKey(3);
    //add value to hazardfile
    myHazardFile.open("/home/kchrist/livemap/pittsburgh/pittsburgh-hazard2.csv", std::ofstream::app); 
    myHazardFile << "pothole " << std::setprecision(10) << x << " " << y << "\n";
    myHazardFile.close();
    it++;
    
  }
*/
  counterIm++;
  /*
  std::ofstream myFile2;
    myFile2.open("/home/kchrist/ros_bags/data/in2/counter.txt", std::ofstream::out); //overwrite
    myFile2 << counterIm << "\n";
    myFile2.close();
    */

  return;
}

void detectCallback(const std_msgs::Int16& flag){
  FLAG = flag.data;
}
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

  std::ifstream newHazards;

  newHazards.open("/home/kchrist/ros_bags/data/positive.csv");
  std::string line;
  std::string lineIt;
  //std::vector<double> stringValues;
  std::string type;
  int val;
  while (std::getline(newHazards, line))
  {
    std::stringstream ss(line);
    //std::cout << ss.str() << std::endl;
    std::getline(ss, lineIt, ' ');
    val = std::atoi(lineIt.c_str());
    haz.push_back(val);
    //std::cout << val << std::endl;
  }
  it = haz.begin();
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "listener");

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;
  /**
   * The subscribe() call is how you tell ROS that you want to receive messages
   * on a given topic.  This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing.  Messages are passed to a callback function, here
   * called chatterCallback.  subscribe() returns a Subscriber object that you
   * must hold on to until you want to unsubscribe.  When all copies of the Subscriber
   * object go out of scope, this callback will automatically be unsubscribed from
   * this topic.
   *
   * The second parameter to the subscribe() function is the size of the message
   * queue.  If messages are arriving faster than they are being processed, this
   * is the number of messages that will be buffered up before beginning to throw
   * away the oldest ones.
   */

  ros::Subscriber sub = n.subscribe("/fix", 1000, chatterCallback);
  ros::Subscriber sub2 = n.subscribe("/camera/image_raw", 1000, imageCallback);
  //ros::Subscriber sub2 = n.subscribe("/camera/image_raw", 1000, imageCallback);

  ros::Subscriber sub_detect = n.subscribe("detect_flag", 1000, detectCallback);

  /**
   * ros::spin() will enter a loop, pumping callbacks.  With this version, all
   * callbacks will be called from within this thread (the main one).  ros::spin()
   * will exit when Ctrl-C is pressed, or the node is shutdown by the master.
   */

  // FIFO file path
  std::string myName = "/tmp/navlabLog.txt";
  std::ifstream counterFile;

  counterFile.open("/home/kchrist/ros_bags/data/in2/counter.txt");
  counterFile >> counterIm;

  std::ofstream myFile2;
  myFile2.open("/home/kchrist/ros_bags/data/in2/counter.txt", std::ofstream::out); //overwrite
  myFile2 << 2497 << "\n";
  myFile2.close();

  std::ofstream myFile;
  myFile.open("/tmp/navlabLog.txt", std::ofstream::out | std::ofstream::trunc);
  myFile.close();                                    //open and close will erase previous contents
  ros::Rate loop_rate(30);                           // 10 Hz
  cv::namedWindow(OPENCV_WINDOW, cv::WINDOW_NORMAL); // Create a window for display.
  cv::setWindowProperty(OPENCV_WINDOW, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
  //cv::namedWindow(OPENCV_WINDOW2, cv::WINDOW_NORMAL);

  while (ros::ok())
  {
    ros::spinOnce();
    loop_rate.sleep();
    /*
  if(!sub){
    ros::Subscriber sub = n.subscribe("/fix", 1000, chatterCallback);
    ros::Subscriber sub2 = n.subscribe("/camera/image_raw", 1000, imageCallback);
    std::cout << "Im here" << std::endl;
  }
  */
  }
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