#include "utils.h"

void Utils::makeDetectionJSON(std::string &sendMessage, DetectionMessage &msg)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Value array(rapidjson::kArrayType);

    rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

    size_t sz = allocator.Size();
    
    d.AddMember("HazardType",  msg.HazardType_, allocator);
    d.AddMember("UserID",  msg.UserID_ , allocator);
    d.AddMember("DriveID", msg.DriveID_   , allocator);
    d.AddMember("Date",  msg.Date_  , allocator);

    d.AddMember("Latitude", msg.Latitude_ , allocator);
    d.AddMember("Longitude", msg.Longitude_ , allocator);
    d.AddMember("Timestamp Sent",  msg.TimestampSent_  , allocator);
    d.AddMember("Timestamp Received",  msg.TimestampReceived_  , allocator);
    d.AddMember("Latency", msg.Latency_ , allocator);
    d.AddMember("IsActive", msg.IsActive_ , allocator);
    d.AddMember("IsVirtual", msg.IsVirtual_  , allocator);

    array.PushBack(msg.HazardBoundingBox_[0], allocator);
    array.PushBack(msg.HazardBoundingBox_[1], allocator);
    array.PushBack(msg.HazardBoundingBox_[2], allocator);
    array.PushBack(msg.HazardBoundingBox_[3], allocator);
    d.AddMember("HazardBoundingBox",    array, allocator);

    d.AddMember("ImageID",  msg.ImageID_ , allocator);
    d.AddMember("Image", msg.Image_, allocator);
    
    rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	d.Accept(writer);

    sendMessage = strbuf.GetString();

    return;
}

void Utils::parseDetectionJSON(std::string &sendMessage, DetectionMessage &msg)
{
    rapidjson::Document d;
    d.Parse(sendMessage);

    assert(d.HasMember("HazardType"));
    assert(d["HazardType"].IsString());
    msg.HazardType_ = d["HazardType"].GetString();

    assert(d.HasMember("UserID"));
    assert(d["UserID"].IsString());
    msg.UserID_ = d["UserID"].GetString();

    assert(d.HasMember("DriveID"));
    assert(d["DriveID"].IsString());
    msg.DriveID_ = d["DriveID"].GetString();

    assert(d.HasMember("Date"));
    assert(d["Date"].IsString());
    msg.Date_ = d["Date"].GetString();

    assert(d.HasMember("Latitude"));
    assert(d["Latitude"].IsDouble());
    msg.Latitude_ = d["Latitude"].GetDouble();

    assert(d.HasMember("Longitude"));
    assert(d["Longitude"].IsDouble());
    msg.Longitude_ = d["Longitude"].GetDouble();

    assert(d.HasMember("Timestamp Sent"));
    assert(d["Timestamp Sent"].IsDouble());
    msg.TimestampSent_ = d["Timestamp Sent"].GetDouble();

    assert(d.HasMember("Timestamp Received"));
    assert(d["Timestamp Received"].IsDouble());
    msg.TimestampReceived_ = d["Timestamp Received"].GetDouble();

    assert(d.HasMember("Latency"));
    assert(d["Latency"].IsDouble());
    msg.Latency_ = d["Latency"].GetDouble();

    assert(d.HasMember("IsActive"));
    assert(d["IsActive"].IsBool());
    msg.IsActive_ = d["IsActive"].GetBool();

    assert(d.HasMember("IsVirtual"));
    assert(d["IsVirtual"].IsBool());
    msg.Latitude_ = d["IsVirtual"].GetBool();

    const rapidjson::Value& hb = d["HazardBoundingBox"];
    assert(hb.IsArray());
    for (rapidjson::SizeType i = 0; i < hb.Size(); i++) 
        msg.HazardBoundingBox_[i] = hb[i].GetInt();

    assert(d.HasMember("ImageID"));
    assert(d["ImageID"].IsString());
    msg.ImageID_ = d["ImageID"].GetString();

    assert(d.HasMember("Image"));
    assert(d["Image"].IsString());
    msg.Image_ = d["Image"].GetString();
    return;


    /*
    std::string line;
    std::string type;
    std::string value;
    
    DetectionMessage det;
    

    //double
    // We know the format, use indices to get values
    
    std::istringstream msg(sendMessage);
    int ln = 0;
    while ( std::getline(msg,line))
    {
        std::istringstream nline(line);
        std::getline(nline, type, ':');  //gets type

        
        switch(ln)
        {
            case 0:
                det.HazardType_ = value;
            case 1:
                det.UserID_ = value;
            case 2:
                det.DriveID_ = value;
            case 3:
                det.Date_ = value;
            case 4:
                det.Latitude_ = std::stod(value);
            case 5:
                det.Longitude_ = value;
            case 6:
                det.TimestampSent_ = value;
            case 7:
                det.TimestampReceived_ = value;
            case 8:
                det.Latency_ = value;
            case 9:
                det.HazardType_ = value;
            case 10:
                det.HazardType_ = value;
            case 11:
                det.HazardType_ = value;
            case 12:
                det.HazardType_ = value;
            case 13:
                det.HazardType_ = value;
            case 14:
                det.HazardType_ = value;
            case 15:
                det.HazardType_ = value;


            default:
                std::cout << "Error Code 2: Message does not follow intended format" << std::endl;
        }
        

        ln++;
    }

    return;
    */
}

void Utils::makeDriveJSON(std::string &sendMessage, DrivingCompleteMessage &msg)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Value array(rapidjson::kArrayType);

    rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

    size_t sz = allocator.Size();
    
    d.AddMember("Lat",  msg.Latitude_, allocator);
    d.AddMember("Long",  msg.Longitude_ , allocator);
    d.AddMember("Angle", msg.Angle_   , allocator);

    rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	d.Accept(writer);

    sendMessage = strbuf.GetString();

    return;
}

void Utils::parseDriveJSON(std::string &sendMessage, DrivingCompleteMessage &msg)
{
    rapidjson::Document d;
    d.Parse(sendMessage);

    assert(d.HasMember("Lat"));
    assert(d["Lat"].IsDouble());
    msg.Latitude_ = d["Lat"].GetDouble();

    assert(d.HasMember("Long"));
    assert(d["Long"].IsDouble());
    msg.Longitude_ = d["Long"].GetDouble();

    assert(d.HasMember("Angle"));
    assert(d["Angle"].IsDouble());
    msg.Angle_ = d["Angle"].GetDouble();

    return;
}


std::string Utils::bool_to_string(bool value)
{
    std::string retVal;
    if(value)
        retVal = "TRUE";
    else
        retVal = "FALSE";
    
    return retVal;
}

void Utils::encodeThisImage(cv::Mat image, std::string &encodedIm){
    if (!image.isContinuous())
            image = image.clone();
        std::vector<uchar> buf;
        cv::imencode(".jpg", image, buf);
        //std::string eIm(buf.begin(), buf.end());
        //encodedIm = Utils::jsonEscape(eIm);
        encodedIm = base64_encode(buf.data(),buf.size());
}


/*
 Base64 Converter code brought to you by:
 
 Copyright (C) 2004-2008 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/

static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";




std::string Utils::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}
std::string Utils::base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}