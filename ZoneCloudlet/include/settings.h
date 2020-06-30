#ifndef SETTINGS_H
#define SETTINGS_H

// If not release version (NDEBUG is not set)
#ifndef NDEBUG
//#define DEBUG_STDERR(x) (std::cerr << (x))
#define DEBUG_STDOUT(x) (std::cout << (x))
#else 
//#define DEBUG_STDERR(x)
#define DEBUG_STDOUT(x)
#endif //debug

// Preprocessor definitions for Zone Cloudlet
#define DEFAULT_SQL_DB         std::string("testdb")
#define DEFAULT_SQL_USER       std::string("osm")
#define DEFAULT_SQL_PWD        std::string("vehicle2016")
#define DEFAULT_SQL_IPA        std::string("127.0.0.1")
#define DEFAULT_SQL_PORT       std::string("5432")
#define DB_CONFIG              std::string("../config/DatabaseConfig.JSON")

#define HAZARD_TOPIC        std::string("HAZARDS_DETECTED")
#define DRIVE_TOPIC         std::string("DRIVE_COMPLETE")
#define SERVER_ADDR         std::string("tcp://localhost:1883")
#define SERVER_ID           std::string("ZONE_CLOUDLET")
#define HAZARDS_DB          std::string("HAZARDS")

#define INIT_NODE_JS        "../nodejs/nodeListen.js &"
#define KILL_NODE_JS        "pkill -fn nodeListen.js &"

#define SQL_NOTIFY      "CREATE OR REPLACE FUNCTION notify_map() RETURNS trigger\n \
                        LANGUAGE plpgsql\n \
                        AS $$\n \
                        BEGIN\n \
                            PERFORM pg_notify('addrecord', json_build_object('table', TG_TABLE_NAME, 'hazard_id', NEW.hazard_id, \
                            'type', NEW.type, 'latitude', NEW.latitude, 'longitude', NEW.longitude, 'image', NEW.image)::text);\n \
                            RETURN NULL;\n \
                        END;\n \
                        $$;"

#define SQL_TRIGGER     "DROP TRIGGER IF EXISTS update_map_notify ON hazards;\n \
                        CREATE TRIGGER update_map_notify AFTER INSERT ON hazards \
                         FOR EACH ROW EXECUTE PROCEDURE notify_map();"

#define SQL_HAZARDS_TABLE   "CREATE TABLE HAZARDS(\
                   ID_NUMBER                SERIAL PRIMARY KEY      NOT NULL, \
                   HAZARD_ID                TEXT    NOT NULL, \
                   TYPE                     TEXT    NOT NULL, \
                   LATITUDE                 DOUBLE PRECISION        NOT NULL, \
                   LONGITUDE                DOUBLE PRECISION        NOT NULL, \
                   HAZARD_BOUNDING_BOX      INTEGER[4], \
                   DATE                     TEXT, \
                   TIMESTAMP_SENT           DOUBLE PRECISION, \
                   TIMESTAMP_RECEIVED       DOUBLE PRECISION, \
                   LATENCY                  DOUBLE PRECISION, \
                   ACTIVE                   BOOLEAN                 NOT NULL, \
                   VIRTUAL                  BOOLEAN                 NOT NULL, \
                   USER_ID                  TEXT, \
                   DRIVE_ID                 TEXT, \
                   IMAGE                    TEXT, \
                   IMAGE_ID                 TEXT);"



#define SQL_DELETE_HAZARDS_TABLE    "DROP TABLE HAZARDS;"
#define SQL_DELETE_DRIVE_TABLE    "DROP TABLE DRIVE;"
// Used to connect to various PostgreSQL Databases
// The actual database used is automatically generated based on the config file in ../configs/DatabaseConfig.JSON
// These are used to help setup the database system

#define CONNECTION_CREATE_HAZARD_TABLE      "dbname = testdb user = osm password = vehicle2016 \
                                            hostaddr = 127.0.0.1 port = 5432"

#define CONENCTION_CREATE_NOTIFY            "dbname = testdb user = osm password = vehicle2016 \
                                            hostaddr = 127.0.0.1 port = 5432"   

#define CONNECTION_CREATE_DATABASE          "dbname = template1 user = osm password = vehicle2016 \
                                            hostaddr = 127.0.0.1 port = 5432"  

 #define SQL_DRIVE_TABLE   "CREATE TABLE DRIVE(\
                   ID_NUMBER                SERIAL PRIMARY KEY      NOT NULL, \
                   LAT                      DOUBLE PRECISION    NOT NULL, \
                   LONG                     DOUBLE PRECISION    NOT NULL, \
                   ANGLE                    DOUBLE PRECISION);" 

#define SQL_NOTIFY_DRIVE        "CREATE OR REPLACE FUNCTION notify_drive_map() RETURNS trigger\n \
                                LANGUAGE plpgsql\n \
                                AS $$\n \
                                BEGIN\n \
                                    PERFORM pg_notify('addDrive', json_build_object('table', TG_TABLE_NAME, 'LAT', NEW.lat, \
                                    'LONG', NEW.long)::text);\n \
                                    RETURN NULL;\n \
                                END;\n \
                                $$;"

#define SQL_TRIGGER_DRIVE   "DROP TRIGGER IF EXISTS update_drive_notify ON drive;\n \
                            CREATE TRIGGER update_drive_notify AFTER INSERT ON drive \
                            FOR EACH ROW EXECUTE PROCEDURE notify_drive_map();"     
                                                        
#endif //SETTINGS_H