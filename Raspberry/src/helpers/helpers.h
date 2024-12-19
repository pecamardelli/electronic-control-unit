#pragma once

#ifndef SYSWAIT_H_
#define SYSWAIT_H_
#include <sys/wait.h>
#endif

#ifndef VECTOR_H_
#define VECTOR_H_
#include <vector>
#endif

#include "../includes/Common.h"
#include "../includes/Logger.h"

void signalHandler(int);
void terminateChildProcesses(std::vector<ChildProcess>);
std::string trim(const std::string &);