#ifndef COMMON_H_
#define COMMON_H_

#include <cstdint>

typedef struct _engineValues
{
    uint8_t temp = 0;
    float kml = 0;
    float volts = 0;
    bool ignition = false;
} EngineValues;

#endif