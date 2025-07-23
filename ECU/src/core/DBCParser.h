#pragma once

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_map>

using json = nlohmann::json;

class DBCParser
{
private:
    json dbcData;
    std::unordered_map<uint32_t, std::string> messageMap;

public:
    DBCParser();
    ~DBCParser();

    bool loadDBC(const std::string &);
    std::string getMessageName(uint32_t);
    void parseCANData(uint32_t, uint8_t *, uint8_t);
};
