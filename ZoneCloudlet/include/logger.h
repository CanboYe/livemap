#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <string>
#include "utils.h"
#include "cloudletListener.h"

class Logger
{
    public:
        Logger();
        ~Logger();
        void addLogAndPrint(std::string statement);
        void addBeginTestLog(std::string statement);
        void addEndTestLog(std::string statement);

        void logSuccess()   {m_numTests++; m_numTestsPassed++;};
        void logFailure()   {m_numTests++;};
        void endTests();

    private:
        std::string m_file;
        std::string m_log;
        std::string m_success;
        std::string m_failure;

        int m_numTests;
        int m_numTestsPassed;


};


#endif //LOGGER_H