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
#include "../includes/Common.h"
#include "../includes/Logger.h"

void signal_handler(int);
void terminateChildProcesses(std::vector<ChildProcess>);

#endif