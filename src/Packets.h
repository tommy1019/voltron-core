#ifndef PACKETS_H
#define PACKETS_H

//Multicast group for writing/reading packets
#define MULTICAST_GROUP "224.0.0.155"

//Ports for diffrent communications
#define DEBUG_PORT 12000
#define BATTERY_PORT 12001
#define LIDAR_PORT 12002
#define CAN_CONTROL_PORT 12003
#define CAN_DATA_PORT 12004
#define LOGGING_CONTROL_PORT 12005
#define CAMERA_PORT 12006

//Packet for sending debug messages, and max length of debug messages
#define DEBUG_MAX_LENGTH 1024
struct DebugPacket
{
    int strLength;
    time_t timestamp;
    char str[DEBUG_MAX_LENGTH];
};

//Temporary battery packet
struct BatteryPacket
{
    time_t timestamp;
    int cellNum;
    float charge;
};

//Packet to be sent to the core program to select which CAN senders should be relayed to the other programs
//pktId == -1 clears list of packets
struct CANControlPacket
{
    int pktId;
    int sender;
};

//Packet for an updated CAN message
struct CANDataPacket
{
    int pktId;
    time_t timestamp;
    int sender;
    char data[8];
};

//Control options for the logging program
enum LoggingControlCode
{
    Shutdown = 0,
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
    EndZEDCapture = 12
};

//Packet to be sent to control the logging program
struct LoggingControlPacket
{
    enum LoggingControlCode code;
};

//Name of shared memory region for LIDAR
#define LIDAR_MEMORY_NAME "/voltron_lidar_data"

//Amount of points in one sweep of a LIDAR
#define LIDAR_DATA_NUM_POINTS 384 * 75

//Number of cached LIDAR regions
#define LIDAR_DATA_NUM_REGIONS 4

//Structure for LIDAR shared memory
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

//Packet sent when the LIDAR finishes one sweep
struct LIDARPacket
{
    int updated;
    time_t timestamp;
};

//Name of shared memory region for camera data
#define CAM_MEMORY_NAME "/voltron_camera_data"

//Width and height of captured camera image
#define CAM_WIDTH 1280
#define CAM_HEIGHT 720

//Number of cached camera images
#define CAM_NUM_IMAGES 4

//Structure for holding camera data
struct CAMData
{
    char rgbImage[CAM_WIDTH * 2][CAM_HEIGHT][4];
    float depth[CAM_WIDTH][CAM_HEIGHT];
};

//Packet sent when an image is captured
struct CameraPacket
{
    int updated;
    time_t timestamp;
};

#endif
