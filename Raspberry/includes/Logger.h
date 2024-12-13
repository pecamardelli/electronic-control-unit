#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

class Logger
{
private:
    std::ofstream logFile;
    std::string logFilePath = "/var/log/";
    std::string currentDate;
    std::string getCurrentDate();
    std::string getCurrentTime();
    void openLogFile();
    void log(const std::string &, const std::string &);

public:
    Logger();
    ~Logger();

    void info(const std::string &);
    void warning(const std::string &);
    void error(const std::string &);
    void debug(const std::string &);
};

#endif