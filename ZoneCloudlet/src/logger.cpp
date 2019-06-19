#include "logger.h"
#include <fstream>

Logger::Logger()
    :m_file("../logs/logFile.txt") , m_numTests(0) , m_numTestsPassed(0)
{
}

// Writes all output to file on termination
// Location: "../logs/logFile.txt"
Logger::~Logger()
{
    std::ofstream log(m_file,std::ofstream::out);
    log << m_log;
    log.close();
}

// Print to console and record the statement for output to file later
void Logger::addLogAndPrint(std::string statement)
{
    m_log += statement + "\n";
    std::cout << statement << std::endl;
    return;
}

void Logger::addBeginTestLog(std::string statement)
{
    std::string mod("[" +std::to_string(m_numTests+1) + "] " + statement + "\n");
    m_log += mod;
    std::cout << mod;
    return;
}

void Logger::addEndTestLog(std::string statement)
{
    m_log += statement + "\n";
    std::cout << statement << std::endl;
    return;
}

void Logger::endTests()
{
    std::string finalPrint = "Passed " + std::to_string(m_numTestsPassed) + " out of " + std::to_string(m_numTests) + " tests.";
    this->addLogAndPrint(finalPrint);

}