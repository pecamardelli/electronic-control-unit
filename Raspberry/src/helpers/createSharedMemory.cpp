#include "helpers.h"

// Templated function to create and map shared memory
template <typename T>
T *createSharedMemory(const char *shmName, bool create)
{
    Logger logger("CreateSharedMemory");

    // Open the shared memory
    int shmFd = shm_open(shmName, create ? O_CREAT | O_RDWR : O_RDWR, 0666);
    if (shmFd == -1)
    {
        logger.error("Failed to create shared memory (shm_open).");
        exit(EXIT_FAILURE);
    }

    // Set the size of the shared memory
    if (create && ftruncate(shmFd, sizeof(T)) == -1)
    {
        logger.error("Failed to create shared memory (ftruncate).");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory into the process's address space
    void *ptr = mmap(0, sizeof(T), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (ptr == MAP_FAILED)
    {
        logger.error("Failed to create shared memory (mmap).");
        exit(EXIT_FAILURE);
    }

    return static_cast<T *>(ptr); // Return the mapped memory as a pointer to T
}

// Explicit instantiation for required types
template EngineValues *createSharedMemory<EngineValues>(const char *, bool);
template SpeedSensorData *createSharedMemory<SpeedSensorData>(const char *, bool);