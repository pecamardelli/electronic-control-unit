#include "Process.h"

Process::Process(/* args */)
{
    // Register signal handler
    std::signal(SIGTERM, Process::handleSignal);
}

Process::~Process()
{
    // Reset signal handler (optional)
    std::signal(SIGTERM, SIG_DFL);
}

void Process::handleSignal(int signal)
{
    if (signal == SIGTERM)
    {
        terminateFlag.store(true); // Notify processes
    }
}
