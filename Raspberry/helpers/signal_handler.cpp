#include "helpers.h"

void signal_handler(int signal)
{
    switch (signal)
    {
    case SIGINT:
        // System Exit
        printf("\r\nSignal Handler: Program stop!\r\n");
        exit(0);
        break;
    default:
        break;
    }
}