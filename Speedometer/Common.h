
#ifndef SPEED_SENSOR_DATA_H_
#define SPEED_SENSOR_DATA_H_

typedef struct _speedSensorData
{
    uint64_t transitions;
    double speed;
    double distanceCovered;
    double averageSpeed;
} SpeedSensorData;

#endif