#ifndef NAVLAB_H
#define NAVLAB_H

#include "ros/ros.h"
#include "std_msgs/String.h"
#include <iostream>
#include <fstream>
#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/NavSatStatus.h>
#include <image_transport/image_transport.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstdlib>
#include <thread>	// For sleep
#include <atomic>
#include <chrono>
#include <cstring>
#include "mqtt/async_client.h"
#include <opencv2/opencv.hpp>

#include <fstream>
#include <streambuf>
#include <ctime>

#include "utils.h"

const std::string DFLT_SERVER_ADDRESS	{ "tcp://squall.elijah.cs.cmu.edu:1883" };
const std::string DFLT_CLIENT_ID		{ "async_publish" };

const std::string TOPIC { "HAZARDS_DETECTED" };

const char* PAYLOAD1 = "Hello World!";
const char* PAYLOAD2 = "Hi there!";
const char* PAYLOAD3 = "Is anyone listening?";
const char* PAYLOAD4 = "Someone is always listening.";

const char* LWT_PAYLOAD = "Last will and testament.";

const int  QOS = 1;

const auto TIMEOUT = std::chrono::seconds(10);

using namespace std;

class callback : public virtual mqtt::callback
{
public:
	void connection_lost(const string& cause) override {
		cout << "\nConnection lost" << endl;
		if (!cause.empty())
			cout << "\tcause: " << cause << endl;
	}

	void delivery_complete(mqtt::delivery_token_ptr tok) override {
		cout << "\tDelivery complete for token: "
			<< (tok ? tok->get_message_id() : -1) << endl;
	}
};

/////////////////////////////////////////////////////////////////////////////

/**
 * A base action listener.
 */
class action_listener : public virtual mqtt::iaction_listener
{
protected:
	void on_failure(const mqtt::token& tok) override {
		cout << "\tListener failure for token: "
			<< tok.get_message_id() << endl;
	}

	void on_success(const mqtt::token& tok) override {
		cout << "\tListener success for token: "
			<< tok.get_message_id() << endl;
	}
};

/////////////////////////////////////////////////////////////////////////////

/**
 * A derived action listener for publish events.
 */
class delivery_action_listener : public action_listener
{
	atomic<bool> done_;

	void on_failure(const mqtt::token& tok) override {
		action_listener::on_failure(tok);
		done_ = true;
	}

	void on_success(const mqtt::token& tok) override {
		action_listener::on_success(tok);
		done_ = true;
	}

public:
	delivery_action_listener() : done_(false) {}
	bool is_done() const { return done_; }
};

class Navlab{
    public:
        Navlab(ros::NodeHandle &n);
        ~Navlab();
        void rawImageCallback(const sensor_msgs::ImageConstPtr &msg);
        void detectionImageCallback(const sensor_msgs::ImageConstPtr &msg);
        void navlabGPSCallback(const sensor_msgs::NavSatFixConstPtr &fix);
        void runROS();
        void logGPS(const double &latitude , const double &longitude);

        //void makeDetectionJSON(std::string &sendMessage, DetectionMessage &message);

        void sendDetectionToZoneCloudlet(DetectionMessage &message); //TODO
        void writeToVideo();

        double measureDistance(double lat1, double lon1, double lat2, double lon2);


    private:
        //ROS member variables
        ros::NodeHandle m_nodeHandle;
        ros::Rate m_loop_rate;
        ros::Subscriber m_rawImageSub;
        ros::Subscriber m_detectedImageSub;
        ros::Subscriber m_navlabGPSSub;
        //
        int m_numRawImages;
        //cv::VideoWriter m_videoWriter;
        std::string m_videoFile;
        int m_fourcc;
        int m_fps;
        int m_numVideos;
        // 
        double m_navLat; 
        double m_navLong;
        double m_oldLat;
        double m_oldLong;
        double m_oldAngle;
        double m_navAngle;
        double m_distFromLastSend;
        double m_LatLastDetection;
        double m_LongLastDetection;
        //
        cv::Mat* m_lastDetection;
        //
        int m_numDetections;
        //
        std::string m_workingDirectory;
        std::string m_frameDirectory;
        std::string m_detectionsDirectory;
        std::string m_videoDirectory;
        std::string m_HAZARD_TOPIC;
        std::string m_DRIVE_TOPIC;
        //
        const std::string OPENCV_WINDOW; 
        const std::string OPENCV_WINDOW2; 
        const std::string OPENCV_WINDOW3;
        //
        const std::string m_nameGPSLog;
        std::ofstream m_GPSFile;
        int m_numGPSCoords;

        mqtt::async_client* m_client;
        callback m_cb;
        mqtt::connect_options m_conopts;
        mqtt::token_ptr m_conntok;
        
        clock_t m_startTime; 
        

        


};



#endif //NAVLAB_H