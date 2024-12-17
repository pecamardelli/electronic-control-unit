#include "helpers.h"

extern bool terminateProgram;

void signal_handler(int signal)
{
    Logger logger("SignalHandler");

    switch (signal)
    {
    case SIGINT:
        // System Exit
        logger.info("Sigint received: breaking main loop...");
        terminateProgram = true;
        break;
    default:
        break;
    }
}