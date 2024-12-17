#include "helpers.h"

extern bool terminateProgram;
extern bool terminateChildProcess;

void signal_handler(int signal)
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
        terminateChildProcess = true;
        break;
    default:
        break;
    }
}