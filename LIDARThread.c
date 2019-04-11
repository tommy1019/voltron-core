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

struct LIDARData* memoryRegions;

#define PORT 2368

#define LASER_NUM 16

void* lidarThread(void* args)
{
    int sockfd = createSocket(LIDAR_PORT);
    if (sockfd < 0)
    {
        perror("[LIDAR] Error opening socket");
    }

    if (shm_unlink(LIDAR_MEMORY_NAME) == -1)
    {
        printf("[LIDAR] Warn: Failed to unlink shared memory\n");
    }

    int fd = shm_open(LIDAR_MEMORY_NAME, O_RDWR | O_CREAT, 0777);
    if (fd == -1)
    {
        printf("[LIDAR] Error: Could not create shared memory region\n");
        return NULL;
    }

    size_t dataSize = sizeof(struct LIDARData) * LIDAR_DATA_NUM_REGIONS;

    if (ftruncate(fd, dataSize) == -1)
    {
        printf("[LIDAR] Error: Could not resize shared memory region to %zu\n", dataSize);
        return NULL;
    }

    memoryRegions = mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (memoryRegions == MAP_FAILED)
    {
        printf("[LIDAR] Error: Could not map memory to shared memory\n");
        return NULL;
    }

    printf("[LIDAR] Created and mapped shared memory\n");

    float angles[16] = { -15, 1, -13, -3, -11, 5, -9, 7, -7, 9, -5, 11, -3, 13, -1, 15 };
    float cosAng[16];
    float sinAng[16];
    for (int i = 0; i < LASER_NUM; i++)
    {
        cosAng[i] = cos(angles[i] * M_PI / 180.0);
        sinAng[i] = sin(angles[i] * M_PI / 180.0);
    }

    int soc = socket(AF_INET, SOCK_DGRAM, 0);
    if (soc < 0)
    {
        printf("[LIDAR] Failed to create socket\n");
        return NULL;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(soc, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("[LIDAR] Failed to bind socket\n");
        exit(EXIT_FAILURE);
    }

    int curBlock = 0;
    int curPoint = 0;

    while(1)
    {
        struct Packet packet;

        int length = 0;
        length = recv(soc, &packet, sizeof(struct Packet), MSG_WAITALL);

        if (length != sizeof(struct Packet) && length != 512)
        {
            printf("Error: Incorrect packet size: %i should be %lu\n", length, sizeof(struct Packet));

            if (length == 27)
                break;

            continue;
        }

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

                memoryRegions[curBlock].point[curPoint].x = x;
                memoryRegions[curBlock].point[curPoint].y = y;
                memoryRegions[curBlock].point[curPoint].z = z;
                memoryRegions[curBlock].point[curPoint].reflectivity = r;

                curPoint++;
            }
        }

        if (curPoint >= LIDAR_DATA_NUM_POINTS)
        {
            struct LIDARPacket pkt;
            pkt.updated = curBlock;
            write(sockfd, &pkt, sizeof(struct LIDARPacket));

            curPoint = 0;
            curBlock++;
            if (curBlock >= LIDAR_DATA_NUM_REGIONS)
                curBlock = 0;
        }
    }

    return NULL;
}
