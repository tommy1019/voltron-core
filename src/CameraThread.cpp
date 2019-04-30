#ifdef COMPILE_ALL

#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#include <sl/Camera.hpp>

#include "Packets.h"

extern "C"
{
    void writeDebugMessage(char* format, ...);
    int createSocket(int port);
    void* cameraThread(void* args);
};

void* cameraThread(void* args)
{
    sl::Camera zed;

    sl::InitParameters init_params;
    init_params.camera_resolution = sl::RESOLUTION_HD720;
    init_params.camera_fps = 60;
    init_params.sdk_verbose = false;

    sl::ERROR_CODE err = zed.open(init_params);
    if (err != sl::SUCCESS)
    {
        writeDebugMessage("[CAM] Failed to open zed camera\n");
        return NULL;
    }

    if (shm_unlink(CAM_MEMORY_NAME) == -1)
    {
        writeDebugMessage("[CAM] Failed to unlink shared memory.\n");
    }

    int fd = shm_open(CAM_MEMORY_NAME, O_RDWR | O_CREAT, 0777);
    if (fd == -1)
    {
        writeDebugMessage("[CAM] Failed to open shared memory.\n");
        zed.close();
    }

    size_t dataSize = sizeof(struct CAMData) * CAM_NUM_IMAGES;
    if (ftruncate(fd, dataSize) == -1)
    {
        writeDebugMessage("[CAM] Could not resize shared memory to correct size.\n");
        zed.close();
    }

    struct CAMData* sharedMemory;
    sharedMemory = (struct CAMData*)mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sharedMemory == MAP_FAILED)
    {
        writeDebugMessage("[CAM] Could not map shared memory.\n");
        zed.close();
    }

    writeDebugMessage("[CAM] Created and mapped shared memory.\n");

    int sockfd = createSocket(CAMERA_PORT);
    if (sockfd < 0)
    {
        writeDebugMessage("[CAM] Error opening socket");
    }

    int curImage = 0;

    while(true)
    {
        if (zed.grab() == sl::SUCCESS)
        {
            sl::Mat image;
            sl::Mat depth_map;

            zed.retrieveImage(image, sl::VIEW_SIDE_BY_SIDE);
            zed.retrieveMeasure(depth_map, sl::MEASURE_DEPTH);

            curImage++;
            if (curImage == CAM_NUM_IMAGES)
                curImage = 0;

            memcpy(sharedMemory[curImage].rgbImage, image.getPtr<sl::uchar1>(), CAM_WIDTH * CAM_HEIGHT * 4);
            memcpy(sharedMemory[curImage].depth, depth_map.getPtr<sl::uchar1>(), CAM_WIDTH * CAM_HEIGHT * 4);

            struct CameraPacket pkt;
            pkt.updated = curImage;
            if (write(sockfd, &pkt, sizeof(struct CameraPacket)) != sizeof(struct CameraPacket))
            {
                writeDebugMessage("[CAM] Failed to write entire packet");
            }
        }
    }

    zed.close();
}

#endif
