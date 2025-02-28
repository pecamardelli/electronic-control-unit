#include "DBCParser.h"

DBCParser::DBCParser()
{
}

DBCParser::~DBCParser()
{
}

bool DBCParser::loadDBC(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return false;
    }
    file >> dbcData;
    file.close();

    // Populate CAN ID to message mapping
    for (const auto &msg : dbcData["messages"])
    {
        uint32_t canID = msg["id"];
        std::string name = msg["name"];
        messageMap[canID] = name;
    }

    std::cout << "DBC File Loaded Successfully!" << std::endl;
    return true;
}

std::string DBCParser::getMessageName(uint32_t canID)
{
    if (messageMap.find(canID) != messageMap.end())
    {
        return messageMap[canID];
    }
    return "Unknown Message";
}

void DBCParser::parseCANData(uint32_t canID, uint8_t *data, uint8_t len)
{
    if (messageMap.find(canID) == messageMap.end())
    {
        std::cout << "Unknown CAN ID: " << std::hex << canID << std::dec << std::endl;
        return;
    }

    std::cout << "Parsing Message: " << messageMap[canID] << std::endl;

    for (const auto &msg : dbcData["messages"])
    {
        if (msg["id"] == canID)
        {
            for (const auto &signal : msg["signals"])
            {
                std::string name = signal["name"];
                // int startBit = signal["start_bit"];
                int length = signal["bit_length"];
                float factor = signal["factor"];
                float offset = signal["offset"];

                uint32_t rawValue = 0;
                for (int i = 0; i < length / 8; i++)
                {
                    rawValue |= (data[i] << (i * 8));
                }

                float value = (rawValue * factor) + offset;
                std::cout << name << ": " << value << std::endl;
            }
        }
    }
}