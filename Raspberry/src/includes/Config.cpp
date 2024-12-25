#include "Config.h"

Config::Config(const std::string &section)
{
    valueMap = sys->getConfig(section);
}

Config::~Config()
{
}