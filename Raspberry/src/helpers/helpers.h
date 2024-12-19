#ifndef HELPERS_H_
#define HELPERS_H_

#include "../includes/Common.h"
#include "../includes/Logger.h"

void signalHandler(int);
void terminateChildProcesses(std::vector<ChildProcess>);
std::string trim(const std::string &);

#endif