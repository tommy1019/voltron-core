#ifndef PACKETS_H
#define PACKETS_H

#define BATTERY_PIPE_NAME "/tmp/voltron_battery"
#define DEBUG_PIPE_NAME "/tmp/voltron_debug"

struct BatteryPacket
{
    int cellNum;

    float charge;
};

struct DebugPacket
{
    int strLength;
};

#define LIDAR_MEMORY_NAME "/tmp/voltron_lidar_data"

#define LIDAR_DATA_NUM_POINTS 64
#define LIDAR_DATA_NUM_REGIONS 8

struct LIDARData
{
    struct
    {
        float x;
        float y;
        float z;
        float reflectivity;
    } point[LIDAR_DATA_NUM_POINTS];
};

#endif
