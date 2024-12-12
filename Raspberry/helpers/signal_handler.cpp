#include "helpers.h"

extern bool terminateProgram;

void signal_handler(int signal)
{
    switch (signal)
    {
    case SIGINT:
        // System Exit
        printf("\r\nSigint received: breaking main loop...\r\n");
        terminateProgram = true;
        break;
    default:
        break;
    }
}