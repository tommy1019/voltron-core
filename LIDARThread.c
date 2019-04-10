#include "LIDARThread.h"

#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Pipes.h"
#include "Packets.h"

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
    for (int i = 0; i < LASER_NUM; i++)
    {
        angles[i] = angles[i] * M_PI / 180.0;
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

    while(1)
    {
        Packet packet;

        int length = 0;
        length = recv(soc, &packet, sizeof(Packet), MSG_WAITALL);

        if (length != sizeof(Packet))
        {
            printf("Error: Incorrect packet size: %i should be %lu\n", length, sizeof(Packet));

            if (length == 27)
                break;

            continue;
        }
    }

    return NULL;
}
