#pragma once

#ifndef IOSTREAM_H_
#define IOSTREAM_H_
#include <iostream>
#endif

#ifndef FSTREAM_H_
#define FSTREAM_H_
#include <fstream>
#endif

#ifndef STRING_H_
#define STRING_H_
#include <string>
#endif

#ifndef CTIME_H_
#define CTIME_H_
#include <ctime>
#endif

class Logger
{
private:
    std::ofstream logFile;
    std::string logFilePath = "/var/log/";
    std::string description = "Main";
    std::string currentDate;
    std::string getCurrentDate();
    std::string getCurrentTime();
    bool debugModeEnabled = false;
    void openLogFile();
    void log(const std::string &, const std::string &);

public:
    Logger(std::string);
    ~Logger();

    void info(const std::string &);
    void warning(const std::string &);
    void error(const std::string &);
    void debug(const std::string &);
    void setDescription(const std::string &);
};