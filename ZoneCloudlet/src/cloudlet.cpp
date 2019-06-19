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
            std::cout << "Opened database successfully: " << C.dbname() << std::endl;
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
        std::cout << "Records created successfully" << std::endl;
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
            std::cout << "Opened database successfully: " << C.dbname() << std::endl;
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
        std::cout << "Records created successfully" << std::endl;
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

