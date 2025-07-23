#include "Logger.h"

Logger::Logger(std::string _className)
{
    description = _className;
    openLogFile();
}

Logger::~Logger()
{
    if (logFile.is_open())
    {
        logFile.close();
    }
}

std::shared_ptr<Logger> Logger::instance = nullptr;
std::mutex Logger::logMutex;

std::shared_ptr<Logger> Logger::getInstance()
{
    std::lock_guard<std::mutex> lock(logMutex);
    if (!instance)
    {
        instance = std::shared_ptr<Logger>(new Logger());
    }
    return instance;
}

void Logger::setDescription(const std::string &_description)
{
    description = _description;
}

std::string Logger::getCurrentTime()
{
    time_t now = time(0);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return std::string(buffer);
}

std::string Logger::getCurrentDate()
{
    time_t now = time(0);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", localtime(&now));
    return std::string(buffer);
}

void Logger::openLogFile()
{
    std::string newDate = getCurrentDate();

    // If the date has changed, update the log file
    if (newDate != currentDate)
    {
        if (logFile.is_open())
        {
            logFile.close();
        }

        currentDate = newDate;
        std::string logFileName = logFilePath + currentDate + ".log";

        logFile.open(logFileName, std::ios::app); // Open file in append mode
        if (!logFile.is_open())
        {
            std::cerr << "Failed to open log file: " << logFileName << std::endl;
        }
    }
}

void Logger::log(const std::string &level, const std::string &message)
{
    openLogFile(); // Ensure the log file is up-to-date

    std::string timestamp = getCurrentTime();
    std::string logMessage = "[" + timestamp + "] [" + level + "] " + "[" + description + "] " + message;

    // Log to console
    std::cout
        << logMessage << std::endl;

    // Log to file
    if (logFile.is_open())
    {
        logFile << logMessage << std::endl;
    }
}

void Logger::info(const std::string &message)
{
    std::lock_guard<std::mutex> lock(logMutex);
    log("INFO", message);
}

void Logger::warning(const std::string &message)
{
    std::lock_guard<std::mutex> lock(logMutex);
    log("WARNING", message);
}

void Logger::error(const std::string &message)
{
    std::lock_guard<std::mutex> lock(logMutex);
    log("ERROR", message);
}

void Logger::debug(const std::string &message)
{
    if (debugModeEnabled)
        log("DEBUG", message);
}