#include "cloudlet.h"

Cloudlet::Cloudlet()
    : m_serverAddress(SERVER_ADDR), m_hazardTopic(HAZARD_TOPIC), m_driveTopic(DRIVE_TOPIC), m_serverID(SERVER_ID) ,
      m_numInsertions(0), m_numReceived(0)
{
    // Creates Hazard Database Container
    Utils::parseDatabaseConfig(m_HazardDB);

    Utils::parseDatabaseConfig(m_DriveDB);
    // See settings.h for script definition
    //system(INIT_NODE_JS);
}

Cloudlet::~Cloudlet()
{
    // See settings.h for script definition
    //system(KILL_NODE_JS);
}
void Cloudlet::saveImage(cv::Mat img)
{
    cv::imwrite("../images/frame" + std::to_string(m_numReceived) +".jpg" ,img);
}
int Cloudlet::insertHazard(DetectionMessage &msg)
{
    std::cout << "inserting hazard" << std::endl;
    std::string sqlInsert;

    try
    {
        pqxx::connection C(m_HazardDB.dbCommand_);

        if (C.is_open())
        {
            // std::cout << "Opened database successfully: " << C.dbname() << std::endl;
        }
        else
        {
            std::cout << "Can't open database" << std::endl;
            return 1;
        }
        std::string imageLocation = "frame" + std::to_string(m_numReceived) + ".jpg";
        m_numReceived++;
        std::cout << Utils::to_string_precision(msg.Latitude_) << std::endl;
        std::cout << Utils::to_string_precision(msg.Longitude_) << std::endl;

        /* Create SQL statement */
        sqlInsert = "INSERT INTO HAZARDS (ID_NUMBER, HAZARD_ID, TYPE, LATITUDE, LONGITUDE, HAZARD_BOUNDING_BOX," \
        "DATE, TIMESTAMP_SENT, TIMESTAMP_RECEIVED, LATENCY, ACTIVE, VIRTUAL, USER_ID, DRIVE_ID, IMAGE, IMAGE_ID)";
        sqlInsert += "VALUES (DEFAULT, '" + msg.HazardID_ + "', '" + msg.HazardType_ + "', " + Utils::to_string_precision(msg.Latitude_) + ", " +
        Utils::to_string_precision(msg.Longitude_) + ", '{" + std::to_string(msg.HazardBoundingBox_[0]) + ", " + 
        std::to_string(msg.HazardBoundingBox_[1]) + ", " + std::to_string(msg.HazardBoundingBox_[2]) + ", " +
        std::to_string(msg.HazardBoundingBox_[3]) + "}', '" + msg.Date_ + "', " + Utils::to_string_precision(msg.TimestampSent_) + ", " + 
        Utils::to_string_precision(msg.TimestampReceived_) + ", " + Utils::to_string_precision(msg.Latency_) + ", " + Utils::bool_to_string(msg.IsActive_) + ", " + 
        Utils::bool_to_string(msg.IsVirtual_) + ", '" + msg.UserID_ + "', '" + msg.DriveID_ + "', '" + imageLocation + "', '" + msg.ImageID_ + "');";
        
        //DEBUG_STDOUT(sqlInsert);

        /* Create a transactional object. */
        pqxx::work W(C);

        /* Execute SQL query */
        W.exec(sqlInsert);
        W.commit();
        // std::cout << "Records created successfully" << std::endl;
        std::cout << "New hazard records created successfully" << std::endl<< std::endl;
        C.disconnect();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 0;
    }
    m_numInsertions++;
    return 1;
}

int Cloudlet::selectHazard(DetectionMessage &msg)
{
    // std::cout << "selecting hazards " << std::endl;
    std::string sqlSelect;
    try
    {
        pqxx::connection C(m_HazardDB.dbCommand_);

        if (C.is_open())
        {
            // std::cout << "#######Opened database successfully: " << C.dbname() << std::endl;
        }
        else
        {
            std::cout << "Can't open database" << std::endl;
            return 1;
        }
        double distance = 1;//meter
        double dLat = this->get_dLat(msg.Latitude_, msg.Longitude_, distance);
        double dLon = this->get_dLon(msg.Latitude_, msg.Longitude_, distance);
        double dist_jud1 = this-> measureDistance( msg.Latitude_,  msg.Longitude_, msg.Latitude_ - abs(dLat), msg.Longitude_);
        double dist_jud2 = this-> measureDistance( msg.Latitude_,  msg.Longitude_, msg.Latitude_, msg.Longitude_ - abs(dLon));
        // std::cout << std::endl << "#### dLat =  " <<  dLat << ",  dLon =  " <<  dLon << ", dist = " << dist_jud1 << ", dist = " << dist_jud2 << std::endl;
        
        sqlSelect = "SELECT count(*) FROM hazards WHERE"\
                    " latitude BETWEEN " + Utils::to_string_precision(msg.Latitude_ - abs(dLat)) + " AND " + Utils::to_string_precision(msg.Latitude_ + abs(dLat)) +
                    " AND longitude BETWEEN " + Utils::to_string_precision(msg.Longitude_ - abs(dLon)) + " AND " + Utils::to_string_precision(msg.Longitude_ + abs(dLon));
        std::cout << sqlSelect << std::endl;
        pqxx::nontransaction N(C);

        pqxx::result R(N.exec(sqlSelect));
        // for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) 
        // {
        std::cout << "#### The select result is " <<  R[0][0].as<int>() << std::endl;
        // }

        
        if (R[0][0].as<int>() != 0)
        {
            C.disconnect();
            return 1;//exist
        } 
        else
        {
            C.disconnect();
            return 0;
        }
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 0;
    }
    m_numInsertions++;
    return 1;

}

double Cloudlet::get_dLat(double lat1, double lon1, double distance)
{
    double R = 6378.137; // Radius of earth in KM
    distance = distance / 1000;
    double dlat = (distance / R) * 180 / M_PI;
    return dlat;
}

double Cloudlet::get_dLon(double lat1, double lon1, double distance)
{
    double R = 6378.137; // Radius of earth in KM
    distance = distance / 1000;
    double dlon = 2 * asin( sin(distance/(2*R)) / cos(lat1) );
    dlon = dlon * 180 / M_PI;
    return dlon;
}


double Cloudlet::measureDistance(double lat1, double lon1, double lat2, double lon2)
{
    double R = 6378.137; // Radius of earth in KM
    double dLat = lat2 * M_PI / 180.0 - lat1 * M_PI / 180.0;
    double dLon = lon2 * M_PI / 180.0 - lon1 *M_PI / 180.0;
    double a = sin(dLat/2.0) * sin(dLat/2.0) + cos(lat1 *M_PI / 180.0) * cos(lat2 * M_PI / 180.0) * sin(dLon/2.0) * sin(dLon/2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0-a));
    double d = R * c;
    return d * 1000.0; // meters
}

int Cloudlet::createDatabase(const std::string dbName, const std::string userName)
{
    std::string createCommand = "CREATE DATABASE \"" + dbName + "\"";
    pqxx::connection c(CONNECTION_CREATE_DATABASE);

    pqxx::nontransaction w(c);
    try
    {
        w.exec(createCommand);
    }
    catch (pqxx::sql_error &e)
    {
        std::string alreadyCreated = "ERROR:  database \"" + dbName + "\" already exists\n";
        if (!alreadyCreated.compare(e.what()))
        {
            std::cout << "Database " << dbName << " already exists! Counting test as passed\n";
            return 2;
        }
        else
        {
            abort();
            return 0;
        }
    }
    std::string grant = "GRANT ALL ON DATABASE \"" + dbName + "\" TO \"" + userName + "\"";
    w.exec(grant);
    w.commit();
    c.disconnect();
    return 1;
}

int Cloudlet::createHazardTable()
{
    std::string sqlTable;
    try
    {
        pqxx::connection C(CONNECTION_CREATE_HAZARD_TABLE);

        if (C.is_open())
        {
            std::cout << "Opened database successfully: " << C.dbname() << std::endl;
        }
        else
        {
            std::cout << "Can't open database" << std::endl;
            return 0;
        }

        // See settings.h for hazard creation SQL syntax
        sqlTable = SQL_HAZARDS_TABLE;

        //Create a transactional object.
        pqxx::work W(C);

        //Execute SQL query
        W.exec(sqlTable);
        W.commit();
        std::cout << "Table created successfully" << std::endl;
        C.disconnect();
    }
    catch (const std::exception &e)
    {
        std::string alreadyExists = "ERROR:  relation \"hazards\" already exists\n";
        if (!alreadyExists.compare(e.what()))
        {
            std::cout << "Table already exists! Counting test as passed\n";
            return 2;
        }
        else
        {

            std::cerr << e.what() << std::endl;
            return 0;
        }
    }

    return 1;
}

int Cloudlet::createDriveTable()
{
    std::string sqlTable;
    try
    {
        pqxx::connection C(CONNECTION_CREATE_HAZARD_TABLE);

        if (C.is_open())
        {
            std::cout << "Opened database successfully: " << C.dbname() << std::endl;
        }
        else
        {
            std::cout << "Can't open database" << std::endl;
            return 0;
        }

        // See settings.h for hazard creation SQL syntax
        sqlTable = SQL_DRIVE_TABLE;

        //Create a transactional object.
        pqxx::work W(C);

        //Execute SQL query
        W.exec(sqlTable);
        W.commit();
        std::cout << "Table created successfully" << std::endl;
        C.disconnect();
    }
    catch (const std::exception &e)
    {
        std::string alreadyExists = "ERROR:  relation \"drive\" already exists\n";
        if (!alreadyExists.compare(e.what()))
        {
            std::cout << "Table already exists! Counting test as passed\n";
            return 2;
        }
        else
        {

            std::cerr << e.what() << std::endl;
            return 0;
        }
    }

    return 1;
}

int Cloudlet::deleteHazardTable()
{
    std::string sqlDeleteTable;
    try
    {
        pqxx::connection C(CONNECTION_CREATE_HAZARD_TABLE);

        if (C.is_open())
        {
            std::cout << "Opened database successfully: " << C.dbname() << std::endl;
        }
        else
        {
            std::cout << "Can't open database" << std::endl;
            return 0;
        }

        // See settings.h for hazard creation SQL syntax
        sqlDeleteTable = SQL_DELETE_HAZARDS_TABLE;

        //Create a transactional object.
        pqxx::work W(C);

        //Execute SQL query
        W.exec(sqlDeleteTable);
        W.commit();
        std::cout << "Table deleted successfully" << std::endl;
        C.disconnect();
    }
    catch (const std::exception &e)
    {
        std::string alreadyExists = "ERROR:  relation \"hazards\" already exists\n";
        if (!alreadyExists.compare(e.what()))
        {
            std::cout << "Table already exists! Counting test as passed\n";
            return 2;
        }
        else
        {

            std::cerr << e.what() << std::endl;
            return 0;
        }
    }

    return 1;
}

int Cloudlet::deleteDriveTable()
{
    std::string sqlDeleteTable;
    try
    {
        pqxx::connection C(CONNECTION_CREATE_HAZARD_TABLE);

        if (C.is_open())
        {
            std::cout << "Opened database successfully: " << C.dbname() << std::endl;
        }
        else
        {
            std::cout << "Can't open database" << std::endl;
            return 0;
        }

        // See settings.h for hazard creation SQL syntax
        sqlDeleteTable = SQL_DELETE_DRIVE_TABLE;

        //Create a transactional object.
        pqxx::work W(C);

        //Execute SQL query
        W.exec(sqlDeleteTable);
        W.commit();
        std::cout << "Table deleted successfully" << std::endl;
        C.disconnect();
    }
    catch (const std::exception &e)
    {
        std::string alreadyExists = "ERROR:  relation \"hazards\" already exists\n";
        if (!alreadyExists.compare(e.what()))
        {
            std::cout << "Table already exists! Counting test as passed\n";
            return 2;
        }
        else
        {

            std::cerr << e.what() << std::endl;
            return 0;
        }
    }

    return 1;
}

int Cloudlet::insertDrive(DrivingCompleteMessage &msg)
{
    std::string sqlInsert;

    try
    {
        pqxx::connection C(m_HazardDB.dbCommand_);

        if (C.is_open())
        {
            // std::cout << "Opened database successfully: " << C.dbname() << std::endl;
        }
        else
        {
            std::cout << "Can't open database" << std::endl;
            return 1;
        }
        //std::string imageLocation = "frame" + std::to_string(m_numReceived) + ".jpg";
        //m_numReceived++;
        std::cout << Utils::to_string_precision(msg.Latitude_) << std::endl;
        std::cout << Utils::to_string_precision(msg.Longitude_) << std::endl;

        /* Create SQL statement */
        sqlInsert = "INSERT INTO DRIVE (ID_NUMBER, LAT, LONG, ANGLE)";
        sqlInsert += "VALUES (DEFAULT, '" + Utils::to_string_precision(msg.Latitude_) + "', '" + Utils::to_string_precision(msg.Longitude_) + "', '" + Utils::to_string_precision(msg.Angle_) + "');";
        
        //DEBUG_STDOUT(sqlInsert);

        /* Create a transactional object. */
        pqxx::work W(C);

        /* Execute SQL query */
        W.exec(sqlInsert);
        W.commit();
        // std::cout << "Records created successfully" << std::endl;
        std::cout << "Inserted a GPS record" << std::endl<< std::endl;
        C.disconnect();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 0;
    }
    m_numInsertions++;
    return 1;
}


int Cloudlet::updateHazardActiveStatus(std::string hazard_id, bool newStatus)
{
    return 0;
}

int Cloudlet::createNotify()
{
    std::string sqlNotify;
    std::string sqlTrigger;
    try
    {
        pqxx::connection C(CONENCTION_CREATE_NOTIFY);

        if (C.is_open())
        {
            std::cout << "Opened database successfully: " << C.dbname() << std::endl;
        }
        else
        {
            std::cout << "Can't open database" << std::endl;
            return 0;
        }

        // See settings.h for string literals
        sqlNotify = SQL_NOTIFY;
        sqlTrigger = SQL_TRIGGER;
    
        pqxx::work W(C);
        /* Execute SQL statement */
        W.exec(sqlNotify);
        W.commit();
        pqxx::work W2(C);
        W2.exec(sqlTrigger);
        W2.commit();
        std::cout << "Notify statement created successfully" << std::endl;
        C.disconnect();
    }
    catch (const std::exception &e)
    {
        std::string alreadyExists = "ERROR:  relation \"hazards\" already exists\n";
        if (!alreadyExists.compare(e.what()))
        {
            std::cout << "Table already exists! Counting test as passed\n";
            return 2;
        }
        else
        {

            std::cerr << e.what() << std::endl;
            return 0;
        }
    }
    return 1;
    
}

int Cloudlet::createDriveNotify()
{
    std::string sqlNotify;
    std::string sqlTrigger;
    try
    {
        pqxx::connection C(CONENCTION_CREATE_NOTIFY);

        if (C.is_open())
        {
            std::cout << "Opened database successfully: " << C.dbname() << std::endl;
        }
        else
        {
            std::cout << "Can't open database" << std::endl;
            return 0;
        }

        // See settings.h for string literals
        sqlNotify = SQL_NOTIFY_DRIVE;
        sqlTrigger = SQL_TRIGGER_DRIVE;
    
        pqxx::work W(C);
        /* Execute SQL statement */
        W.exec(sqlNotify);
        W.commit();
        pqxx::work W2(C);
        W2.exec(sqlTrigger);
        W2.commit();
        std::cout << "Notify statement created successfully" << std::endl;
        C.disconnect();
    }
    catch (const std::exception &e)
    {
        std::string alreadyExists = "ERROR:  relation \"drive\" already exists\n";
        if (!alreadyExists.compare(e.what()))
        {
            std::cout << "Table already exists! Counting test as passed\n";
            return 2;
        }
        else
        {

            std::cerr << e.what() << std::endl;
            return 0;
        }
    }
    return 1;
    
}

