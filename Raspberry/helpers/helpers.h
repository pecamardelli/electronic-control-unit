#ifndef HELPERS_H_
#define HELPERS_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <csignal>
#include <sys/wait.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <map>

#include "../includes/Common.h"
#include "../includes/Logger.h"

void signalHandler(int);
void terminateChildProcesses(std::vector<ChildProcess>);
std::string trim(const std::string &);

#endif