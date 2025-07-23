#pragma once

#ifndef STRING_H_
#define STRING_H_
#include <string>
#endif

#ifndef CONFIG_H_
#define CONFIG_H_
#include "Config.h"
#endif

#ifndef BCM2835_H_
#define BCM2835_H_
#include <bcm2835.h>
#endif

#ifndef MEMORY_H_
#define MEMORY_H_
#include <memory>
#endif

#ifndef UNISTD_H_
#define UNISTD_H_
#include <unistd.h>
#endif

class Base
{
private:
    /* data */

public:
    Base(/* args */);
    ~Base();

protected:
    std::string description;
    std::unique_ptr<Logger> logger;
    std::unique_ptr<Config> config;
};