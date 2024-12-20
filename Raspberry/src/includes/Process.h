#pragma once

#ifndef STRING_H_
#define STRING_H_
#include <string>
#endif

class Process
{
private:
    /* data */
public:
    virtual ~Process() = default;
    std::string description;
    // Pure virtual function
    virtual void loop() = 0;
    virtual void setup() {};

protected:
    useconds_t loopInterval;
};