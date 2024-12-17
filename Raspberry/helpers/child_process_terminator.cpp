#include "helpers.h"

// Function to terminate all child processes
void terminateChildProcesses(std::vector<ChildProcess> childProcesses)
{
    Logger logger("ChildProcessTerminator");
    logger.info("Terminating child processes...");

    for (const auto &child : childProcesses)
    {
        if (kill(child.pid, SIGTERM) == 0)
        { // Send SIGTERM to each child
            logger.info("Terminated child process for " + child.description + " (" + std::to_string(child.pid) + ")");
        }
        else
        {
            logger.info("Failed to terminate child process for " + child.description + " (" + std::to_string(child.pid) + ")");
        }
    }

    // Wait for all children to exit
    int status;
    for (const auto &child : childProcesses)
    {
        waitpid(child.pid, &status, 0);

        if (WIFEXITED(status))
        {
            logger.info(child.description + " process exited normally.");
        }
        else if (WIFSIGNALED(status))
        {
            logger.warning(child.description + " process killed by signal.");
        }
    }
}