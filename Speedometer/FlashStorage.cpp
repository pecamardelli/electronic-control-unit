#include "FlashStorage.h"
#include <string.h>
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"

// Constructor
FlashStorage::FlashStorage(uint32_t flash_offset, uint8_t sectors_count)
    : flash_start_offset(flash_offset), sectors_used(sectors_count)
{
    // Ensure flash_offset is aligned to FLASH_SECTOR_SIZE
    if (flash_offset % FLASH_SECTOR_SIZE != 0)
    {
        flash_start_offset = (flash_offset / FLASH_SECTOR_SIZE) * FLASH_SECTOR_SIZE;
    }
}

// Calculate CRC for data integrity verification
uint32_t FlashStorage::calculateCRC(const DataRecord *record) const
{
    // Skip the CRC field itself in calculation
    const uint8_t *data = (const uint8_t *)record;
    uint32_t crc = 0;

    // Sum bytes before CRC field
    for (int i = 0; i < offsetof(DataRecord, crc); i++)
    {
        crc += data[i];
    }

    // Sum bytes after CRC field
    for (int i = offsetof(DataRecord, crc) + sizeof(record->crc);
         i < sizeof(DataRecord); i++)
    {
        crc += data[i];
    }

    return crc;
}

// Find sector with most recent valid record, with robust power-failure handling
int FlashStorage::findLatestSector(uint32_t *latest_sequence) const
{
    int best_sector = -1;
    uint32_t highest_sequence = 0;

#ifdef DEBUG_FLASH
    printf("Scanning %d sectors for valid records\n", sectors_used);
#endif

    // Check each sector for valid records
    for (int sector = 0; sector < sectors_used; sector++)
    {
        uint32_t sector_addr = flash_start_offset + (sector * FLASH_SECTOR_SIZE);
        const DataRecord *record = (const DataRecord *)(XIP_BASE + sector_addr);

        // Check if this record potentially has valid data
        if (record->magic == MAGIC_VALUE)
        {
            // Verify CRC to ensure the data wasn't corrupted during a power failure
            uint32_t calc_crc = calculateCRC(record);

#ifdef DEBUG_FLASH
            printf("Sector %d: magic=0x%08lx, seq=%lu, CRC match=%s\n",
                   sector, record->magic, record->sequence,
                   (calc_crc == record->crc) ? "Yes" : "No");
#endif

            // Only consider records with valid CRC and higher sequence numbers
            if (calc_crc == record->crc)
            {
                if (record->sequence > highest_sequence)
                {
                    highest_sequence = record->sequence;
                    best_sector = sector;
#ifdef DEBUG_FLASH
                    printf("  Found new best sector: %d (sequence %lu)\n", best_sector, highest_sequence);
#endif
                }
            }
            else
            {
// This indicates a likely power failure during write
#ifdef DEBUG_FLASH
                printf("  Corrupted record in sector %d (CRC mismatch)\n", sector);
#endif
            }
        }
        else
        {
#ifdef DEBUG_FLASH
            // This sector either has never been written or was erased but not written
            // (which can happen during a power failure)
            printf("Sector %d: No valid magic value (0x%08lx)\n", sector, record->magic);
#endif
        }
    }

    if (latest_sequence)
    {
        *latest_sequence = (best_sector >= 0) ? highest_sequence : 0;
    }

#ifdef DEBUG_FLASH
    if (best_sector >= 0)
    {
        printf("Best sector found: %d with sequence %lu\n", best_sector, highest_sequence);
    }
    else
    {
        printf("No valid sectors found\n");
    }
#endif

    return best_sector;
}

// Save data using wear-leveling
bool FlashStorage::saveData(double value1, double value2)
{
    // Find current state
    uint32_t latest_sequence;
    int current_sector = findLatestSector(&latest_sequence);

    // Determine the next sector to use (round-robin)
    int next_sector = (current_sector + 1) % sectors_used;
    uint32_t next_sequence = latest_sequence + 1;

    // Prepare the record
    DataRecord record;
    memset(&record, 0, sizeof(record));
    record.magic = MAGIC_VALUE;
    record.sequence = next_sequence;
    record.value1 = value1;
    record.value2 = value2;
    record.crc = calculateCRC(&record);

    // Calculate flash address for next sector
    uint32_t flash_addr = flash_start_offset + (next_sector * FLASH_SECTOR_SIZE);

    // Disable interrupts during flash operation
    uint32_t ints = save_and_disable_interrupts();

    // Erase the sector
    flash_range_erase(flash_addr, FLASH_SECTOR_SIZE);

    // Program the new record
    flash_range_program(flash_addr, (const uint8_t *)&record, sizeof(record));

    // Restore interrupts
    restore_interrupts(ints);

    // Verify the write was successful
    const DataRecord *saved = (const DataRecord *)(XIP_BASE + flash_addr);
    return (saved->magic == MAGIC_VALUE && saved->sequence == next_sequence);
}

// Read latest data
bool FlashStorage::readData(double *value1, double *value2)
{
    int latest_sector = findLatestSector(nullptr);

    // Check if we found a valid record
    if (latest_sector < 0)
    {
        return false; // No valid data found
    }

    // Calculate the address of the latest record
    uint32_t flash_addr = flash_start_offset + (latest_sector * FLASH_SECTOR_SIZE);
    const DataRecord *record = (const DataRecord *)(XIP_BASE + flash_addr);

    // Validate the record
    if (record->magic != MAGIC_VALUE || record->crc != calculateCRC(record))
    {
        return false;
    }

    // Extract the values
    if (value1)
        *value1 = record->value1;
    if (value2)
        *value2 = record->value2;

    return true;
}

// Erase all sectors used by this storage instance
bool FlashStorage::eraseAllData()
{
    uint32_t ints = save_and_disable_interrupts();

    // Erase all sectors
    for (int sector = 0; sector < sectors_used; sector++)
    {
        uint32_t flash_addr = flash_start_offset + (sector * FLASH_SECTOR_SIZE);
        flash_range_erase(flash_addr, FLASH_SECTOR_SIZE);
    }

    restore_interrupts(ints);
    return true;
}

// Get the current write cycle count
uint32_t FlashStorage::getWriteCount() const
{
    uint32_t sequence = 0;
    findLatestSector(&sequence);
    return sequence;
}