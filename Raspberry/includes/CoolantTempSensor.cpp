#include "CoolantTempSensor.h"

CoolantTempSensor::CoolantTempSensor(std::string address) : address(oneWireRootFolder + address + oneWireFile)
{
    std::cout << "Set address to " << address << std::endl;

    return;
}

CoolantTempSensor::CoolantTempSensor()
{
    std::cout << "Find CoolantTempSensor sensor ..." << std::endl;

    // Find first folder whose name starts with "28-"
    for (const std::filesystem::directory_entry &val : std::filesystem::directory_iterator(oneWireRootFolder))
    {
        if (!val.is_directory())
            continue;

        // Get folder name
        std::string folderName{val.path().stem().string()};

        if (folderName.find("28-"))
            continue;

        address = oneWireRootFolder + folderName + oneWireFile;

        std::cout << "Found sensor with address " << folderName << std::endl;

        return;
    }

    // If code gets here, no connected sensor was found
    throw std::runtime_error("No connected CoolantTempSensor sensor found!");
}

CoolantTempSensor::~CoolantTempSensor() {}

// Set static constants
const std::string CoolantTempSensor::oneWireRootFolder{"/sys/bus/w1/devices/"};
const std::string CoolantTempSensor::oneWireFile{"/w1_slave"};

float CoolantTempSensor::readTemp()
{
    // Open one-Wire reader as file stream and read
    std::ifstream reader{address};

    // Check if read was good
    if (!reader.good())
    {
        std::cerr << "Read from CoolantTempSensor failed!" << std::endl;

        return std::numeric_limits<float>::quiet_NaN();
    }

    // Read first and second lines from message
    // Format of message:
    // 64 01 55 05 7f a5 81 66 7c : crc=7c YES
    // 64 01 55 05 7f a5 81 66 7c t=22250
    std::string firstLine, secondLine;
    getline(reader, firstLine);
    getline(reader, secondLine);

    // Get status from message (Last word of first line)
    std::string status{firstLine.substr(firstLine.find_last_of(" ") + 1)};

    // Check if temperature was read properly
    if ("YES" != status)
    {
        std::cerr << "Temperature was not measured properly!" << std::endl;

        return std::numeric_limits<float>::quiet_NaN();
    }

    // Get temperature from message (Number after "t=")
    float tempRaw{std::stof(secondLine.substr(secondLine.find("t=") + 2))};

    // Return temperature divided by 1000
    return tempRaw / 1000;
}

bool CoolantTempSensor::isValid(float temperature) { return !std::isnan(temperature); }