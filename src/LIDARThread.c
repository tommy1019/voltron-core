#include "LIDARThread.h"

#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>
#include <string.h>

#include "Packets.h"
#include "Net.h"
#include "Debug.h"

//Structures to read data from Lidar udp socket
struct Channel
{
    uint16_t distance;
    uint8_t reflectivity;
} __attribute__((packed));

struct DataBlock
{
    uint16_t flag;
    uint16_t azimuth;

    struct Channel channelData[32];
} __attribute__((packed));

struct Packet
{
    struct DataBlock dataBlocks[12];

    uint32_t timestamp;
    uint8_t returnMode;
    uint8_t productID;
} __attribute__((packed));

//Shared memory region
struct LIDARData* memoryRegions;

//Port for lidar reading
#define PORT 2368

//Number of lasers in our specific lidar
#define LASER_NUM 16

void* lidarThread(void* args)
{
    //Create thread to listen for GPS packets
    if (pthread_create(&lidarGPSThreadId, NULL, lidarGPSThread, NULL) != 0)
    {
        writeDebugMessage("[LIDAR] Error: Could not create LIDAR GPS thread thread\n");
    }

    //size of shared memory to be opened
    size_t dataSize = sizeof(struct LIDARData) * LIDAR_DATA_NUM_REGIONS;

    //Open shared memory
    int fd = shm_open(LIDAR_MEMORY_NAME, O_RDWR, 0777);
    if (fd == -1)
    {
        //Create new shared memory if none exists
        fd = shm_open(LIDAR_MEMORY_NAME, O_RDWR | O_CREAT, 0777);
        if (fd == -1)
        {
            writeDebugMessage("[LIDAR] Failed to create shared memory.\n");
            return NULL;
        }

        //Resize new shared memory to correct size
        if (ftruncate(fd, dataSize) == -1)
        {
            writeDebugMessage("[LIDAR] Could not resize shared memory to correct size.\n");
            return NULL;
        }
    }

    //Map shared memory to ram
    memoryRegions = (struct LIDARData *)mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (memoryRegions == MAP_FAILED)
    {
        writeDebugMessage("[LIDAR] Error: Could not map memory to shared memory\n");
        return NULL;
    }

    writeDebugMessage("[LIDAR] Created and mapped shared memory\n");

    //Calculations for comverting lidar input to world coords
    float angles[16] = { -15, 1, -13, -3, -11, 5, -9, 7, -7, 9, -5, 11, -3, 13, -1, 15 };
    float cosAng[16];
    float sinAng[16];
    for (int i = 0; i < LASER_NUM; i++)
    {
        cosAng[i] = cos(angles[i] * M_PI / 180.0);
        sinAng[i] = sin(angles[i] * M_PI / 180.0);
    }

    //Create soket for writing lidar packet updates
    int sockfd = createSocket(LIDAR_PORT);
    if (sockfd < 0)
    {
        writeDebugMessage("[LIDAR] Error opening socket");
    }

    //Create socket for reading lidar data
    int soc = socket(AF_INET, SOCK_DGRAM, 0);
    if (soc < 0)
    {
        writeDebugMessage("[LIDAR] Failed to create socket\n");
        return NULL;
    }

    //Create address for reading lidar data
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    //Bind lidar reading socket to address
    if (bind(soc, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        writeDebugMessage("[LIDAR] Failed to bind socket\n");
        return NULL;
    }

    //Indexes of currently read point and block
    int curBlock = 0;
    int curPoint = 0;

    while(1)
    {
        //Packet to be read
        struct Packet packet;

        //Read lidar packet
        int length = 0;
        length = recv(soc, &packet, sizeof(struct Packet), MSG_WAITALL);

        //Ignore packets of incorrect length
        if (length != sizeof(struct Packet) && length != 512)
        {
            writeDebugMessage("Error: Incorrect packet size: %i should be %lu\n", length, sizeof(struct Packet));
            continue;
        }

        //Convert read lidar packet to world corrds
        for (int i = 0; i < 12; i++)
        {
            float azim = (float)(packet.dataBlocks[i].azimuth) / 100.0 * M_PI / 180.0;
            float cosa = cos(azim);
            float sina = sin(azim);

            for (int j = 0; j < 32; j++)
            {
                float dist = (float)(packet.dataBlocks[i].channelData[i].distance) * 2.0 / 10000.0;
                float x, y, z, r;

                x = dist * cosAng[i] * cosa;
                y = dist * cosAng[i] * sina;
                z = dist * sinAng[i];
                r = packet.dataBlocks[i].channelData[i].reflectivity;

                //Store calculated world coords in shared memory
                memoryRegions[curBlock].point[curPoint].x = x;
                memoryRegions[curBlock].point[curPoint].y = y;
                memoryRegions[curBlock].point[curPoint].z = z;
                memoryRegions[curBlock].point[curPoint].reflectivity = r;

                curPoint++;
            }
        }

        //Check if a full sweep of the lidar is complete
        if (curPoint >= LIDAR_DATA_NUM_POINTS)
        {
            //Write packet notifying of full sweep
            struct LIDARPacket pkt;
            pkt.updated = curBlock;
            if (write(sockfd, &pkt, sizeof(struct LIDARPacket)) != sizeof(struct LIDARPacket))
            {
                writeDebugMessage("[LIDAR] Failed to write entire packet\n");
            }

            //Reset curPoint and increment curBlock
            curPoint = 0;
            curBlock++;
            if (curBlock >= LIDAR_DATA_NUM_REGIONS)
                curBlock = 0;
        }
    }

    return NULL;
}

#define GPS_PORT 8308

//Structure for reading GPS data
struct GPSPacket
{
    char unused[198];
    uint32_t timestamp;
    uint8_t pulsePerSecond;
    char padding[3];
    char gpsString[306];
};

void* lidarGPSThread(void* args)
{
    //Create socket for reading GPS data
    int soc = socket(AF_INET, SOCK_DGRAM, 0);
    if (soc < 0)
    {
        writeDebugMessage("[LIDAR] Failed to create GPS socket\n");
        return NULL;
    }

    //Create address for reading GPS data
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(GPS_PORT);

    //Bind socket to address
    if (bind(soc, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        writeDebugMessage("[LIDAR] Failed to bind GPS socket\n");
        return NULL;
    }

    while(1)
    {
        //Read GPS data
        struct GPSPacket gpsPacket;
        recv(soc, &gpsPacket, sizeof(struct GPSPacket), MSG_WAITALL);

        //Print GPS data to debug stream. TODO: Create channel for GPS data
        writeDebugMessage("GPS: %s\n", gpsPacket.gpsString);
    }
}
