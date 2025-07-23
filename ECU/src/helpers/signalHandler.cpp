#include "helpers.h"

extern bool terminateProgram;
extern volatile bool terminateChildProcess;

void signalHandler(int signal)
{
    Logger logger("SignalHandler");

    switch (signal)
    {
    case SIGINT:
        // System Exit
        logger.info("SIGINT received: breaking main loop...");
        terminateProgram = true;
        break;
    case SIGTERM:
        // Terminate child processes
        logger.info("SIGTERM received: terminating child processes...");
        terminateChildProcess = true;
        break;
    default:
        break;
    }
}