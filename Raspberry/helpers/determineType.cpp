#include "helpers.h"

// Function to determine the type of a value using regex
ConfigValue determineType(const std::string &value)
{
    static const std::regex intRegex("^-?\\d+$");
    static const std::regex floatRegex("^-?\\d*\\.\\d+$");
    static const std::regex boolRegex("^(true|false|1|0)$", std::regex::icase);

    if (std::regex_match(value, intRegex))
    {
        return std::stoi(value); // Integer
    }
    else if (std::regex_match(value, floatRegex))
    {
        return std::stof(value); // Float
    }
    else if (std::regex_match(value, boolRegex))
    {
        std::string lowerValue = value;
        std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
        return (lowerValue == "true" || lowerValue == "1"); // Boolean
    }
    else
    {
        return value; // String
    }
}