cmake_minimum_required (VERSION 2.8.11)
project(cloudlet)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
#find_package(Mosquitto REQUIRED)
find_library(PAHO_MQTT_CPP paho-mqttpp3 REQUIRED)
find_library(PAHO_MQTT paho-mqtt3a REQUIRED)
find_package(OpenCV REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(RapidJSON REQUIRED)

#find_library(LIBPQXX libpqxx REQUIRED)
#find_library(PQ lpq REQUIRED)

#find_library(MQTTP paho-mqttpp3 REQUIRED)

#if(MOSQUITTO_FOUND)

  #include_directories(${MOSQUITTO_INCLUDE_DIRS})
#endif (MOSQUITTO_FOUND)

find_package( Qt4 REQUIRED )
include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )
include_directories( ${CMAKE_BINARY_DIR} )

#message(STATUS ${PostgreSQL_INCLUDE_DIR})
include_directories(${PostgreSQL_INCLUDE_DIR} 
                    ${cloudlet_SOURCE_DIR}/include)


#file(GLOB SOURCES "include/*.h" "src/*.cpp")
#add_library(CLOUDLIB utils.cpp)
add_executable(cloudlet src/main.cpp
                          src/cloudlet.cpp
                          src/utils.cpp
                          src/cloudletListener.cpp
                          src/logger.cpp)

add_executable(testSystem   src/testSystem.cpp
                              src/cloudlet.cpp
                              src/utils.cpp
                              src/cloudletListener.cpp
                              src/logger.cpp)

add_executable(makeDatabase   src/makeDatabase.cpp
                                src/cloudlet.cpp
                                src/utils.cpp
                                src/cloudletListener.cpp
                                src/logger.cpp)

add_executable(testConnect  src/testConnect.cpp
                            src/cloudlet.cpp
                            src/utils.cpp
                            src/cloudletListener.cpp
                            src/logger.cpp)


                    

set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC TRUE)

#if(MOSQUITTO_FOUND)
#  target_link_libraries (talker1 ${MOSQUITTO_LIBRARIES})
#endif (MOSQUITTO_FOUND)
target_link_libraries (cloudlet     ${PAHO_MQTT_CPP} 
                                    ${PAHO_MQTT} 
                                    ${OpenCV_LIBS} 
                                    ${RapidJSON}
                                    ${CLOUDLIB}
                                    -lpqxx -lpq
                                    )
target_link_libraries (testSystem   ${PAHO_MQTT_CPP} 
                                    ${PAHO_MQTT} 
                                    ${OpenCV_LIBS} 
                                    ${RapidJSON}
                                    ${CLOUDLIB}
                                    -lpqxx -lpq
                                    )

target_link_libraries (makeDatabase ${PAHO_MQTT_CPP} 
                                    ${PAHO_MQTT} 
                                    ${OpenCV_LIBS} 
                                    ${RapidJSON}
                                    ${CLOUDLIB}
                                    -lpqxx -lpq
                                    )  
target_link_libraries (testConnect  ${PAHO_MQTT_CPP} 
                                    ${PAHO_MQTT} 
                                    ${OpenCV_LIBS} 
                                    ${RapidJSON}
                                    ${CLOUDLIB}
                                    -lpqxx -lpq
                                    )
                    
