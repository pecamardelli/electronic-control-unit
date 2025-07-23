#include "helpers.h"

std::string getProgramName(char *argv0)
{
    char *basename = std::strrchr(argv0, '/');
    if (basename)
    {
        basename++; // Skip the '/' character
    }
    else
    {
        basename = argv0; // No path, just the program name
    }

    return basename;
}