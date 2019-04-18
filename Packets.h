#ifndef PACKETS_H
#define PACKETS_H

#define MULTICAST_GROUP "224.0.0.155"

#define DEBUG_PORT 12000
#define BATTERY_PORT 12001
#define LIDAR_PORT 12002

struct DebugPacket
{
    int strLength;
};

struct BatteryPacket
{
    int cellNum;

    float charge;
};


#define LIDAR_MEMORY_NAME "/voltron_lidar_data"

#define LIDAR_DATA_NUM_POINTS 910 * 16 * 2
#define LIDAR_DATA_NUM_REGIONS 4

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

struct LIDARPacket
{
    int updated;
};

#endif
