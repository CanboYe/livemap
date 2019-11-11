#ifndef CLOUDLET_H
#define CLOUDLET_H

#include <string>
#include <sstream> 
#include <algorithm>
#include <queue>
#include <cstdlib>

#include "cloudletListener.h"
#include "utils.h"
#include "settings.h"


class Cloudlet
{
  public:
    Cloudlet(); //TODO- start-up ./nodeListen.js
    ~Cloudlet(); //TODO- clean up
    int insertHazard(DetectionMessage &msg);
    void saveImage(cv::Mat img);
    // Before running the system, we call ./testSystem  
    // This executable sets up the database, hazard table, and Notify Triggers

    // Helper functions for setup
    int createDatabase(const std::string dbName, const std::string userName);
    int createHazardTable(); 
    int createDriveTable(); 
    int updateHazardActiveStatus(std::string hazard_id, bool newStatus); //TODO

    int deleteHazardTable();
    int deleteDriveTable();
    int createDriveNotify();
    // Creates the NOTIFY and TRIGGER statements for PostgreSQL
    // This is needed by the front-end visualization via HTML
    // Currently this is called once before the system is installed on the system
    // Then whenever a data row is added to our hazard database table, it sends the info
    // to display
    int createNotify(); 
    int insertDrive(DrivingCompleteMessage &msg);
    int selectHazard(DetectionMessage &msg);
    double get_dLat(double lat1, double lon1, double distance);
    double get_dLon(double lat1, double lon1, double distance);
    double measureDistance(double lat1, double lon1, double lat2, double lon2);

  private:
  // Queue for Detection Messages
    std::queue<DetectionMessage> m_DetectionQueue;
  // Queue for Drive Messages
    std::queue<DrivingCompleteMessage> m_DrivingCompleteQueue;
    
  // Parameters for MQTT transactions
    std::string m_serverAddress;
    std::string m_hazardTopic;
    std::string m_driveTopic;
    std::string m_serverID;

    
    DatabaseContainer m_HazardDB;
    DatabaseContainer m_DriveDB;
    
    //detectionCallback *m_DetectionListener;
    //driveCompleteCallback *m_DriveCompleteListener;

    // Parameters for general 
    std::string m_DirectoryLocation;

    //TODO: Struct for statistics
    int m_numInsertions;
    int num;
    int m_numReceived;
};  

#endif //CLOUDLET_H