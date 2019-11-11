#include <iostream>
#include "utils.h"
#include "logger.h"
#include "cloudletListener.h"
#include "cloudlet.h"
#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace cv;

DetectionMessage makeSampleDetectionJSON()
{
    double r1 = (double) (rand()%100) - 50;
    double r2 = (double) (rand()%100) - 50;

    Mat image = imread("../images/test.jpg");
    if (!image.isContinuous())
        image = image.clone();
    std::vector<uchar> buf;
    imencode(".jpg", image, buf);
    std::string encodedIm(buf.begin(), buf.end());

    std::string message;
    DetectionMessage s;
    s.HazardID_ = "1234a";
    s.HazardType_ = "deer";
    s.UserID_ = "1234b";
    s.DriveID_ = "1234c";
    s.Date_ = "date_please";
    // Position
    s.Latitude_ = 40.4380 + r1*0.0008;
    s.Longitude_ = -79.9560 + r2*0.0008;
    //s.Latitude_ = 40.4600;
    //s.Longitude_ = -79.9400;
    // Time & Latency
    s.TimestampSent_ = 0.000000001234;
    s.TimestampReceived_ = 0.0000001234;
    s.Latency_ = 0.00000000001;
    // Detected Image
    s.Image_ = encodedIm;
    s.ImageID_ = "1234e";
    s.HazardBoundingBox_[0] = 22;
    s.HazardBoundingBox_[1] = 45;
    s.HazardBoundingBox_[2] = 45;
    s.HazardBoundingBox_[3] = 75;
    // Categories
    s.IsActive_ = true;
    s.IsVirtual_ = false;

    Utils::makeDetectionJSON(message, s);

    return s;
}
int testParser(Logger &log)
{
    log.addBeginTestLog("Testing the Detection Message JSON Parser/Generator");
    // Create Dummy Message //
    DetectionMessage s = makeSampleDetectionJSON();
    std::string message;

    Utils::makeDetectionJSON(message, s);
    //std::cout << message << std::endl;

    DetectionMessage t;
    Utils::parseDetectionJSON(message, t);
    //std::cout << t.Image_ << std::endl;

    std::string checkMessage;
    Utils::makeDetectionJSON(checkMessage, s);
    //std::cout << checkMessage << std::endl;

    if (message == checkMessage)
    {
        log.addEndTestLog("Passed");
        log.logSuccess();
        return 1;
    }

    else
    {
        log.addEndTestLog("Failed");
        log.logFailure();
        return 0;
    }
}

int testDatabaseConnection(Logger &log)
{
    log.addBeginTestLog("Testing the Connection to PostgreSQL by Opening a Database");

    DatabaseContainer db;
    Utils::parseDatabaseConfig(db);

    try
    {
        pqxx::connection C(db.dbCommand_);
        if (C.is_open())
        {
            log.addEndTestLog("Passed");
            log.logSuccess();
            C.disconnect();
            return 1;
        }
        else
        {
            log.addEndTestLog("Failed");
            log.logFailure();
            return 0;
        }
    }
    catch (const std::exception &e)
    {
        log.addEndTestLog("Failed");
        log.logFailure();
        return 0;
    }
}

int testDeleteDatabase(Logger &log, Cloudlet &cloud)
{
    int success;
    log.addBeginTestLog("Clearing old Hazards Database.");
    success = cloud.deleteHazardTable();

    if (success > 0)
    {
        log.addEndTestLog("Passed");
        log.logSuccess();
    }
    else
    {
        log.addEndTestLog("Failed");
        log.logFailure();
    }
}

int testDeleteDatabase2(Logger &log, Cloudlet &cloud)
{
    int success;
    log.addBeginTestLog("Clearing old Drive Database.");
    success = cloud.deleteDriveTable();

    if (success > 0)
    {
        log.addEndTestLog("Passed");
        log.logSuccess();
    }
    else
    {
        log.addEndTestLog("Failed");
        log.logFailure();
    }
}

int testMakeDatabase(Logger &log, Cloudlet &cloud, const std::string dbName, const std::string userName)
{
    int success;
    log.addBeginTestLog("Testing Creation of a Database");
    success = cloud.createDatabase(dbName, userName);

    if (success > 0)
    {
        log.addEndTestLog("Passed");
        log.logSuccess();
    }
    else
    {
        log.addEndTestLog("Failed");
        log.logFailure();
    }
}


int testCreateHazardTable(Logger &log, Cloudlet &cloud)
{
    int success;
    log.addBeginTestLog("Testing Creation of a Table");
    success = cloud.createHazardTable();
    cloud.createNotify();
    if (success > 0)
    {
        log.addEndTestLog("Passed");
        log.logSuccess();
    }
    else
    {
        log.addEndTestLog("Failed");
        log.logFailure();
    }
}

int testCreateDriveTable(Logger &log, Cloudlet &cloud)
{
    int success;
    log.addBeginTestLog("Testing Creation of a Table");
    success = cloud.createDriveTable();
    cloud.createDriveNotify();
    if (success > 0)
    {
        log.addEndTestLog("Passed");
        log.logSuccess();
    }
    else
    {
        log.addEndTestLog("Failed");
        log.logFailure();
    }
}

int testInsertHazard(Logger &log, Cloudlet &cloud)
{
    int success;
    log.addBeginTestLog("Testing Hazard Insertion");

    DetectionMessage sampleMsg = makeSampleDetectionJSON();

    success = cloud.insertHazard(sampleMsg);
    if (success > 0)
    {
        log.addEndTestLog("Passed");
        log.logSuccess();
    }
    else
    {
        log.addEndTestLog("Failed");
        log.logFailure();
    }

}

int testStoringMessages(Logger &log, Cloudlet &cloud, int numMessages)
{
    for(int i = 0; i < numMessages; i++)
    {
        testInsertHazard(log,cloud);

    }
    return 0;
}

int main()
{
    Logger log;
    Cloudlet cloud;
    std::string userName = "osm";
    std::string dbName = "testdb";
    srand (time(NULL));


    testDeleteDatabase(log,cloud);


    // Test the detection JSON generator/parser
    // Test [1] //
    testParser(log);
    // Test the connection to a database
    // Test [2] //
    testDatabaseConnection(log);
    // Test the creation of a database
    // If already exists, it counts as passed
    // Test [3] //
    testMakeDatabase(log, cloud, dbName, userName);

    // Test the creation of a table
    // If already exists, it counts as passed
    // Test [4] //
    testCreateHazardTable(log, cloud);
    // Test insertion of a hazard
    // Test [5] //
    //testInsertHazard(log, cloud);


    // Drive database
    testDeleteDatabase2(log,cloud);


    // Test the detection JSON generator/parser
    // Test [1] //
    testParser(log);
    // Test the connection to a database
    // Test [2] //
    // Test the creation of a database
    // If already exists, it counts as passed
    // Test [3] //

    // Test the creation of a table
    // If already exists, it counts as passed
    // Test [4] //
    testCreateDriveTable(log, cloud);

    //testStoringMessages(log,cloud,100);
    // End the tests, print final results
    log.endTests();

    //5 tests total (so far)
}