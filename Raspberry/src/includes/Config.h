#pragma once

#ifndef STRING_H_
#define STRING_H_
#include <string>
#endif

#ifndef MAP_H_
#define MAP_H_
#include <map>
#endif

#include "System.h"

extern System *sys;

class Config
{
private:
    /* data */
    std::map<std::string, std::string> valueMap;

public:
    Config(const std::string &);
    ~Config();

    // Implementing this template method in here in order to avoid linkage issues.
    template <typename T>
    T get(const std::string &key) const
    {
        // Locate the key within the section
        auto keyIt = valueMap.find(key);
        if (keyIt == valueMap.end())
        {
            throw std::runtime_error("Key not found:" + key);
        }

        // Retrieve the value as a string
        const std::string &valueStr = keyIt->second;
        // Convert the string value to the requested type
        if constexpr (std::is_same<T, bool>::value)
        {
            if (valueStr == "1" || valueStr == "true" || valueStr == "TRUE" || valueStr == "True")
            {
                return true;
            }
            else if (valueStr == "0" || valueStr == "false" || valueStr == "FALSE" || valueStr == "False")
            {
                return false;
            }
            else
            {
                throw std::runtime_error("Conversion failed for key [" + key + "]: Invalid boolean value " + valueStr);
            }
        }
        else
        {
            std::istringstream iss(valueStr);
            T convertedValue;
            if (!(iss >> convertedValue))
            {
                throw std::runtime_error("Conversion failed for key [" + key + "]: " + valueStr);
            }
            return convertedValue;
        }
    }
};
