#pragma once

#include <cstdint>

// ASCII
typedef struct _tFont
{
    const uint8_t *table;
    uint16_t Width;
    uint16_t Height;
} sFONT;

extern sFONT LiberationSansNarrow_Bold28;