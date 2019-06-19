#ifndef UTILS_H
#define UTILS_H
#include <opencv2/opencv.hpp>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
//#include <libwebsockets.h> not needed

/////////////////////////////////////////////////////////////////////////////
//Messages//
/////////////////////////////////////////////////////////////////////////////
// Message is Constructed for each Hazard Detected
struct DetectionMessage
{
    // Hazard Identifiers
    std::string HazardID_;
    std::string HazardType_;
    std::string UserID_;
    std::string DriveID_;
    std::string Date_;
    // Position
    double Latitude_;
    double Longitude_;
    // Time & Latency
    double TimestampSent_;
    double TimestampReceived_;
    double Latency_;
    // Detected Image
    std::string Image_; //Conver to Mat?
    std::string ImageID_;
    int HazardBoundingBox_[4]; // [0] Top Left X [1] Top Left Y [2] Bottom Right X [3] Bottom Right Y
                                 // (Same convention as OpenCV)
    // Categories
    bool IsActive_;
    bool IsVirtual_;
};

// Message is Constructed for each Drive Completed
struct DrivingCompleteMessage
{
    // Drive Identifiers
    /*
    std::string UserID_;
    std::string DriveID_;
    */
    double Latitude_;
    double Longitude_;
    double Angle_;
    // Beginning and End Coord's of Drive
    /*
    double StartLat_;
    double StartLong_;
    double EndLat_;
    double EndLong_;
    // Drive Timing
    std::string Date_;
    std::string StartTime_;
    std::string EndTime_;
    double Duration_;
    // Drive Statistics
    int NumberHazardsObserved_;
    double NumberBytesReceived_;
    double NumberBytesTransferred_;
    double TotalBytes_;

    // Large File Locations
    // Save them to a file Location instead of storing in Database
    // Filename convention = DriveID_ + extension
    std::string ListGPSCoordsLocation_;
    std::string VideoFileLocation_;
    bool IsVirtual_;
    */
    // Catgeories
    
};

struct DatabaseContainer
{
    //Command is the command to open a connection with the database;
    std::string dbCommand_;
    std::string dbName_;
    std::string dbUser_;
    std::string dbPort_;
    std::string dbHost_;
    std::string dbPass_;

};

namespace Utils
{
void makeDriveJSON(std::string &sendMessage, DrivingCompleteMessage &msg);
void parseDriveJSON(std::string &sendMessage, DrivingCompleteMessage &msg);

void makeDetectionJSON(std::string &sendMessage, DetectionMessage &msg);
void parseDetectionJSON(std::string &sendMessage, DetectionMessage &msg);
void encodeThisImage(cv::Mat image, std::string &encodedIm);
template <typename T>
inline std::string to_string_precision(const T a_value, const int n = 10)
{
    std::ostringstream out;
    out << std::setprecision(n) << a_value;
    return out.str();

}
static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
std::string base64_decode(std::string const& encoded_string);
std::string bool_to_string(bool value);

}; // namespace Utils

#endif //UTILS_H