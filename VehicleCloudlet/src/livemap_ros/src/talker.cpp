#include "ros/ros.h"
#include "std_msgs/String.h"
#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <sstream>
#include <boost/foreach.hpp>
#include <std_msgs/Int32.h>
#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/NavSatStatus.h>
#define foreach BOOST_FOREACH

/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
int main(int argc, char **argv)
{
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
  ros::init(argc, argv, "talker");

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;

  /**
   * The advertise() function is how you tell ROS that you want to
   * publish on a given topic name. This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing. After this advertise() call is made, the master
   * node will notify anyone who is trying to subscribe to this topic name,
   * and they will in turn negotiate a peer-to-peer connection with this
   * node.  advertise() returns a Publisher object which allows you to
   * publish messages on that topic through a call to publish().  Once
   * all copies of the returned Publisher object are destroyed, the topic
   * will be automatically unadvertised.
   *
   * The second parameter to advertise() is the size of the message queue
   * used for publishing messages.  If messages are published more quickly
   * than we can send them, the number here specifies how many messages to
   * buffer up before throwing some away.
   */
  ros::Publisher chatter_pub = n.advertise<std_msgs::String>("chatter", 1000);

  ros::Rate loop_rate(0.1);

  /**
   * A count of how many messages we have sent. This is used to create
   * a unique string for each message.
   */
  rosbag::Bag bag;
  bag.open("/home/kchrist/ros_bags/data/2018-03-19-15-54-48.bag", rosbag::bagmode::Read);
  std::vector<std::string> topics;
  topics.push_back(std::string("/fix"));

  //topics.push_back(std::string("numbers"));



  rosbag::View viewLat(bag, rosbag::TopicQuery(std::string("/latitude")));
  rosbag::View viewLong(bag, rosbag::TopicQuery(std::string("/longitude")));

  rosbag::View::iterator LatIt = viewLat.begin();
  rosbag::View::iterator LongIt = viewLong.begin();

  sensor_msgs::NavSatFixPtr longitude, latitude;
  
  int count = 0;

  while (ros::ok())
  {
    /**
     * This is a message object. You stuff it with data, and then publish it.
     */

    //if (LatIt == viewLat.end())
    //  break;

    rosbag::MessageInstance &vlat = *LatIt;
    rosbag::MessageInstance &vlong = *LongIt;
    latitude = vlat.instantiate<sensor_msgs::NavSatFix>();
    longitude = vlong.instantiate<sensor_msgs::NavSatFix>();
    ROS_INFO_STREAM(longitude->header.seq << " " << latitude->header.seq);
    std::cout <<longitude << "\n";
    LatIt++;
    LongIt++;


    /**
     * The publish() function is how you send messages. The parameter
     * is the message object. The type of this object must agree with the type
     * given as a template parameter to the advertise<>() call, as was done
     * in the constructor above.
     */
    chatter_pub.publish(longitude);
    chatter_pub.publish(latitude);

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
  }
  bag.close();

  return 0;
}