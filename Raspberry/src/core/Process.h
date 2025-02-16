#pragma once

#ifndef STRING_H_
#define STRING_H_
#include <string>
#endif

#include <iostream>
#include <csignal>
#include <atomic>
#include <memory>
#include <chrono>
#include <thread>

#include "Base.h"
#include "Logger.h"
#include "Config.h"
#include "common.h"

class Process : public Base
{
private:
    /* data */
public:
    Process();
    virtual ~Process();
    // Pure virtual function
    virtual void loop() = 0;
    static void handleSignal(int);

protected:
    static std::atomic<bool> terminateFlag; // Shared termination flag
    useconds_t loopInterval;
};