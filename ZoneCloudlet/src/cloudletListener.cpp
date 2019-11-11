#include "cloudletListener.h"
#include "cloudlet.h"
/*
void CloudletListener::listen()
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

	/*
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
    
}
*/
void CloudletListener::on_failure(const mqtt::token &tok)
{
    std::cout << name_ << " failure";
    if (tok.get_message_id() != 0)
        std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
    std::cout << std::endl;
}

void CloudletListener::on_success(const mqtt::token &tok)
{
    std::cout << name_ << " success";
    if (tok.get_message_id() != 0)
        std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
    auto top = tok.get_topics();
    if (top && !top->empty())
        std::cout << "\ttoken topic: '" << (*top)[0] << "', ..." << std::endl;
    std::cout << std::endl;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * Local callback & listener class for use with the client connection.
 * This is primarily intended to receive messages, but it will also monitor
 * the connection to the broker. If the connection is lost, it will attempt
 * to restore the connection and re-subscribe to the topic.
 */

// This deomonstrates manually reconnecting to the broker by calling
// connect() again. This is a possibility for an application that keeps
// a copy of it's original connect_options, or if the app wants to
// reconnect with different options.
// Another way this can be done manually, if using the same options, is
// to just call the async_client::reconnect() method.
void detectionCallback::reconnect()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    try
    {
        cli_.connect(connOpts_, nullptr, *this);
    }
    catch (const mqtt::exception &exc)
    {
        std::cerr << "Error: " << exc.what() << std::endl;
        exit(1);
    }
}

// Re-connection failure
void detectionCallback::on_failure(const mqtt::token &tok)
{
    std::cout << "Connection failed" << std::endl;
    if (++nretry_ > N_RETRY_ATTEMPTS)
        exit(1);
    reconnect();
}

// Re-connection success
void detectionCallback::on_success(const mqtt::token &tok)
{
    std::cout << "\nConnection success" << std::endl;
    std::cout << "\nSubscribing to topic '" << TOPIC << "'\n"
              << "\tfor client " << CLIENT_ID
              << " using QoS" << QOS << "\n"
              << "\nPress Q<Enter> to quit\n"
              << std::endl;

    cli_.subscribe(TOPIC, QOS, nullptr, subListener_);
}

// Callback for when the connection is lost.
// This will initiate the attempt to manually reconnect.
void detectionCallback::connection_lost(const std::string &cause)
{
    std::cout << "\nConnection lost" << std::endl;
    if (!cause.empty())
        std::cout << "\tcause: " << cause << std::endl;

    std::cout << "Reconnecting..." << std::endl;
    nretry_ = 0;
    reconnect();
}

// Callback for when a message arrives.
void detectionCallback::message_arrived(mqtt::const_message_ptr msg)
{
    std::cout << "Detection Message arrived" << std::endl;
    std::string detectionJSON(msg->get_payload_str().begin(),
                               msg->get_payload_str().end());
    //std::cout << detectionJSON << std::endl;
    //std::string detectionJSON = msg->to_string();
    DetectionMessage dmessage;
    Utils::parseDetectionJSON(detectionJSON, dmessage);
    
    //std::cout << dmessage.ImageID_ << std::endl;
    const std::string toDecode = dmessage.Image_;
    dmessage.Image_ = Utils::base64_decode(toDecode);
    std::vector<uchar> vectordata(dmessage.Image_.begin(),
                                        dmessage.Image_.end());
		//Convert buffer to Mat
    //std::cout << dmessage.Image_ << std::endl;
    cv::Mat img = cv::imdecode(vectordata, CV_LOAD_IMAGE_COLOR);

    //saveImage and insert the hazard
    m_cloudlet->saveImage(img);

    //determine if the hazard is new
    if (!m_cloudlet->selectHazard(dmessage))
    {
        m_cloudlet->insertHazard(dmessage);

        // publish a message to vehicle cloudlet
        std::string message; 
        std::string NEW_HAZARD_TOPIC("NEW_HAZARDS_DETECTED");
        Utils::makeDetectionJSON(message,dmessage);
        mqtt::message_ptr pubmsg = mqtt::make_message(NEW_HAZARD_TOPIC, message);
        int qos = 1;
        pubmsg->set_qos(qos);
        cli_.publish(pubmsg);//->wait_for(TIMEOUT);
    }
    else
    {
        std::cout << "****duplicated detection messages" << std::endl<< std::endl;
    }
    


    // TODO: Add msg to msg queue
    // In msg queue, threads will parse and write to DB 

    /*
    //Begin: Video Code
    //if(msg->get_topic == )
    std::string input;
    std::ofstream out("file.mp4");
    out << msg->get_payload_str();
    out.close();
    return;
    */

    //End:Video Code
    /*
        //Begin: Image Code
        //convert std::string to buffer
        std::vector<uchar> vectordata(msg->get_payload_str().begin(),
                                        msg->get_payload_str().end());
		//Convert buffer to Mat
        Mat img = imdecode(vectordata, CV_LOAD_IMAGE_COLOR);
        imwrite("name.jpg",img);
        namedWindow(OPENCV_WINDOW, WINDOW_NORMAL); // Create a window for display.
        imshow(OPENCV_WINDOW, img);
        waitKey(1000);
        //End Image code 
        */

    //std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
    //std::cout << "\tpayload: '" << msg->to_string() << "'\n" << std::endl;
}

void detectionCallback::delivery_complete(mqtt::delivery_token_ptr token) {}

/////////////////////////////////////////////////////////////////////////////

void driveCompleteCallback::reconnect()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    try
    {
        cli_.connect(connOpts_, nullptr, *this);
    }
    catch (const mqtt::exception &exc)
    {
        std::cerr << "Error: " << exc.what() << std::endl;
        exit(1);
    }
}

// Re-connection failure
void driveCompleteCallback::on_failure(const mqtt::token &tok)
{
    std::cout << "Connection failed" << std::endl;
    if (++nretry_ > N_RETRY_ATTEMPTS)
        exit(1);
    reconnect();
}

// Re-connection success
void driveCompleteCallback::on_success(const mqtt::token &tok)
{
    std::cout << "\nConnection success" << std::endl;
    std::cout << "\nSubscribing to topic '" << TOPIC << "'\n"
              << "\tfor client " << CLIENT_ID
              << " using QoS" << QOS << "\n"
              << "\nPress Q<Enter> to quit\n"
              << std::endl;

    cli_.subscribe(TOPIC, QOS, nullptr, subListener_);
}

// Callback for when the connection is lost.
// This will initiate the attempt to manually reconnect.
void driveCompleteCallback::connection_lost(const std::string &cause)
{
    std::cout << "\nConnection lost" << std::endl;
    if (!cause.empty())
        std::cout << "\tcause: " << cause << std::endl;

    std::cout << "Reconnecting..." << std::endl;
    nretry_ = 0;
    reconnect();
}

// Callback for when a message arrives.
void driveCompleteCallback::message_arrived(mqtt::const_message_ptr msg)
{

    // std::cout << "Drive GPS Message arrived" << std::endl;
    std::string detectionJSON(msg->get_payload_str().begin(),
                               msg->get_payload_str().end());
    DrivingCompleteMessage dmessage;
    Utils::parseDriveJSON(detectionJSON, dmessage);
    m_cloudlet->insertDrive(dmessage);

    /*
    //Begin: Video Code
    //if(msg->get_topic == )
    std::string input;
    std::ofstream out("file.mp4");
    out << msg->get_payload_str();
    out.close();
    return;
    //End:Video Code
    
        //Begin: Image Code
        //convert std::string to buffer
        std::vector<uchar> vectordata(msg->get_payload_str().begin(),
                                        msg->get_payload_str().end());
		//Convert buffer to Mat
        Mat img = imdecode(vectordata, CV_LOAD_IMAGE_COLOR);
        imwrite("name.jpg",img);
        namedWindow(OPENCV_WINDOW, WINDOW_NORMAL); // Create a window for display.
        imshow(OPENCV_WINDOW, img);
        waitKey(1000);
        //End Image code 
        */

    //std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
    //std::cout << "\tpayload: '" << msg->to_string() << "'\n" << std::endl;
}

void driveCompleteCallback::delivery_complete(mqtt::delivery_token_ptr token) {}
