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

//Declare methods to be used by C
extern "C"
{
    void writeDebugMessage(const char* format, ...);
    int createSocket(int port);
    void* cameraThread(void* args);
};

void* cameraThread(void* args)
{
    //Create ZED camera object
    sl::Camera zed;

    //Set resolution to 720p and FPS to 60
    sl::InitParameters init_params;
    init_params.camera_resolution = sl::RESOLUTION_HD720;
    init_params.camera_fps = 60;
    init_params.sdk_verbose = false;

    //Open camera
    sl::ERROR_CODE err = zed.open(init_params);
    if (err != sl::SUCCESS)
    {
        writeDebugMessage("[CAM] Failed to open zed camera\n");
        return NULL;
    }

    //Remove old shared memory
    if (shm_unlink(CAM_MEMORY_NAME) == -1)
    {
        writeDebugMessage("[CAM] Failed to unlink shared memory.\n");
    }

    //Open new shared memory
    int fd = shm_open(CAM_MEMORY_NAME, O_RDWR | O_CREAT, 0777);
    if (fd == -1)
    {
        writeDebugMessage("[CAM] Failed to open shared memory.\n");
        zed.close();
    }

    //Resize new shared memory to correct size
    size_t dataSize = sizeof(struct CAMData) * CAM_NUM_IMAGES;
    if (ftruncate(fd, dataSize) == -1)
    {
        writeDebugMessage("[CAM] Could not resize shared memory to correct size.\n");
        zed.close();
    }

    //Map shared memory to sharedMemory
    struct CAMData* sharedMemory;
    sharedMemory = (struct CAMData*)mmap(NULL, dataSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sharedMemory == MAP_FAILED)
    {
        writeDebugMessage("[CAM] Could not map shared memory.\n");
        zed.close();
    }

    writeDebugMessage("[CAM] Created and mapped shared memory.\n");

    //Open udp socket for writing
    int sockfd = createSocket(CAMERA_PORT);
    if (sockfd < 0)
    {
        writeDebugMessage("[CAM] Error opening socket");
    }

    //Variable to store which image is currently being written to
    int curImage = 0;

    while(true)
    {
        //Tell zed camera to grab a frame
        if (zed.grab() == sl::SUCCESS)
        {
            //Mats to store image and depth data
            sl::Mat image;
            sl::Mat depth_map;

            //Reterive rgb and depth data from the camera
            zed.retrieveImage(image, sl::VIEW_SIDE_BY_SIDE);
            zed.retrieveMeasure(depth_map, sl::MEASURE_DEPTH);

            //Move to next image, reseting to zero if max images is hit
            curImage++;
            if (curImage == CAM_NUM_IMAGES)
                curImage = 0;

            //Copy from mat objects to shared memory
            memcpy(sharedMemory[curImage].rgbImage, image.getPtr<sl::uchar1>(), CAM_WIDTH * CAM_HEIGHT * 4);
            memcpy(sharedMemory[curImage].depth, depth_map.getPtr<sl::uchar1>(), CAM_WIDTH * CAM_HEIGHT * 4);

            //Send UDP packet about updated image
            struct CameraPacket pkt;
            pkt.updated = curImage;
            if (write(sockfd, &pkt, sizeof(struct CameraPacket)) != sizeof(struct CameraPacket))
            {
                writeDebugMessage("[CAM] Failed to write entire packet");
            }
        }
    }

    //Close camera
    zed.close();
}

#endif
