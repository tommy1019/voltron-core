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
    void* cameraThread(void* args);

    void writeDebugMessage(const char* format, ...);
    int createSocket(int port);
    void* openSharedMemory(const char* name, size_t size);
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

    //Open shared memory to sharedMemory
    struct CAMData* sharedMemory = (struct CAMData*)openSharedMemory(LIDAR_MEMORY_NAME, sizeof(struct CAMData) * CAM_NUM_IMAGES);
    if (sharedMemory == NULL)
    {
        writeDebugMessage("[CAM] Could not create shared memory.\n");
        zed.close();
        return NULL;
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
            memcpy(sharedMemory[curImage].rgbImage, image.getPtr<sl::uchar1>(), CAM_WIDTH * 2 * CAM_HEIGHT * 4);
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
