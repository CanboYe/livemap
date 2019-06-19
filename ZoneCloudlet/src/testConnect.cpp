// async_subscribe.cpp

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cctype>
#include <thread>
#include <chrono>
#include "mqtt/async_client.h"
#include <opencv2/opencv.hpp>
#include "utils.h"

#include <fstream>
#include <string>
#include <iostream>
#include <pqxx/pqxx>

#include "cloudletListener.h"
#include "cloudlet.h"

static const std::string OPENCV_WINDOW = "Arrived";

using namespace cv;

/////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	std::string SERVER_ADDRESS("tcp://localhost:1883");
	std::string CLIENT_ID("async_subcribe_cpp");
	std::string TOPIC("HAZARDS_DETECTED");

	int QOS = 1;
	int N_RETRY_ATTEMPTS = 5;
	//Begin MQTT code for testing

	mqtt::connect_options connOpts;
	connOpts.set_keep_alive_interval(20);
	connOpts.set_clean_session(true);

	mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);

	detectionCallback cb(client, connOpts, SERVER_ADDRESS, CLIENT_ID, TOPIC, QOS, N_RETRY_ATTEMPTS);
	client.set_callback(cb);

	// Start the connection.
	// When completed, the callback will subscribe to topic.

	
	try
	{
		std::cout << "Connecting to the MQTT server..." << std::flush;
		client.connect(connOpts, nullptr, cb);
	}
	catch (const mqtt::exception &)
	{
		std::cerr << "\nERROR: Unable to connect to MQTT server: '"
				  << SERVER_ADDRESS << "'" << std::endl;
		return 1;
	}

	// Just block till user tells us to quit.

	while (std::tolower(std::cin.get()) != 'q')
		;

	// Disconnect

	try
	{
		std::cout << "\nDisconnecting from the MQTT server..." << std::flush;
		client.disconnect()->wait();
		std::cout << "OK" << std::endl;
	}
	catch (const mqtt::exception &exc)
	{
		std::cerr << exc.what() << std::endl;
		return 1;
	}

	return 0;

	//End MQTT code for testing


	DatabaseContainer db;
	Utils::parseDatabaseConfig(db);

	try {
      pqxx::connection C(db.dbCommand_);
      if (C.is_open()) {
         std::cout << "Opened database successfully: " << C.dbname() << std::endl;
      } else {
         std::cout << "Can't open database" << std::endl;
         return 1;
      }
      C.disconnect ();
   } catch (const std::exception &e) {
      std::cerr << e.what() << std::endl;
      return 1;
   }

	
}