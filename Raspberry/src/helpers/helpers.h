#pragma once

#include <sys/wait.h>

#include "../includes/Common.h"
#include "../includes/Logger.h"

void signalHandler(int);
void terminateChildProcesses(std::vector<ChildProcess>);
std::string trim(const std::string &);