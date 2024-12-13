#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cxxabi.h>

class Logger
{
private:
    std::ofstream logFile;
    std::string logFilePath = "/var/log/";
    std::string currentDate;
    std::string getCurrentDate();
    std::string getCurrentTime();
    bool debugModeEnabled = false;
    void openLogFile();

public:
    Logger(std::string);
    Logger();
    ~Logger();

    void info(const std::string &);
    void warning(const std::string &);
    void error(const std::string &);
    void debug(const std::string &);

protected:
    void log(const std::string &, const std::string &);
    std::string className = "Main";
};

#endif