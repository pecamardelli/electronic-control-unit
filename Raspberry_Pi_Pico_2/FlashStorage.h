#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

// Uncomment this line to enable debug output for flash operations
// #define DEBUG_FLASH

#include <stdint.h>

/**
 * @class FlashStorage
 * @brief A class to manage persistent storage in flash memory with wear-leveling
 */
class FlashStorage
{
public:
    /**
     * @brief Constructor
     * @param flash_offset The offset (in bytes) from start of flash memory
     * @param sectors_count Number of sectors to use for wear-leveling
     */
    FlashStorage(uint32_t flash_offset = 1024 * 1024, uint8_t sectors_count = 16);

    /**
     * @brief Saves two double values to flash with wear-leveling
     * @param value1 First double value
     * @param value2 Second double value
     * @return true if successful, false otherwise
     */
    bool saveData(double value1, double value2);

    /**
     * @brief Reads the most recent values from flash
     * @param value1 Pointer to store the first value
     * @param value2 Pointer to store the second value
     * @return true if successful, false if no valid data found
     */
    bool readData(double *value1, double *value2);

    /**
     * @brief Erases all storage sectors used by this instance
     * @return true if successful
     */
    bool eraseAllData();

    /**
     * @brief Get the current write cycle count
     * @return The sequence number of the last write
     */
    uint32_t getWriteCount() const;

private:
    // Structure for data records with metadata
    typedef struct
    {
        uint32_t magic;     // Magic number to verify valid data (0xAA55AA55)
        uint32_t sequence;  // Sequence number to determine newest record
        uint32_t crc;       // Simple checksum for data integrity
        double value1;      // First double value to store
        double value2;      // Second double value to store
        uint8_t padding[4]; // Padding to make record size aligned
    } DataRecord;

    // Flash memory parameters
    uint32_t flash_start_offset; // Starting offset in flash memory
    uint8_t sectors_used;        // Number of sectors for wear-leveling

    // Constants
    static const uint32_t MAGIC_VALUE = 0xAA55AA55;

    // Private methods
    uint32_t calculateCRC(const DataRecord *record) const;
    int findLatestSector(uint32_t *latest_sequence = nullptr) const;
};

#endif // FLASH_STORAGE_H