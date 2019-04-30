#ifndef PACKETS_H
#define PACKETS_H

#define MULTICAST_GROUP "224.0.0.155"

#define DEBUG_PORT 12000
#define BATTERY_PORT 12001
#define LIDAR_PORT 12002
#define CAN_CONTROL_PORT 12003
#define CAN_DATA_PORT 12004
#define LOGGING_CONTROL_PORT 12005

#define DEBUG_MAX_LENGTH 1024
struct DebugPacket
{
    int strLength;
    char str[DEBUG_MAX_LENGTH];
};

struct BatteryPacket
{
    int cellNum;
    float charge;
};

struct CANControlPacket
{
    int pktId;
    int sender;
};

struct CANDataPacket
{
    int pktId;
    int sender;
    char data[8];
};

#define LIDAR_MEMORY_NAME "/voltron_lidar_data"

#define LIDAR_DATA_NUM_POINTS 384 * 3 * 16
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

enum LoggingControlCode {Shutdown = 0,
                        StartDrive = 1,
                        EndDrive = 2,
                        StartBatteryCapture = 3,
                        EndBatteryCapture = 4,
                        StartCANCapture = 5,
                        EndCANCapture = 6,
                        StartLIDARCapture = 7,
                        EndLIDARCapture = 8,
                        StartGPSCapture = 9,
                        EndGPSCapture = 10,
                        StartZEDCapture = 11,
                        EndZEDCapture = 12};

struct LoggingControlPacket
{
    LoggingControlCode code;
};

#endif
