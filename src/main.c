#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#include "Debug.h"
#include "BatteryThread.h"
#include "LIDARThread.h"
#include "CameraThread.h"

#ifdef CAN_THREAD
#include "CANReaderThread.h"
#endif

void startThread(pthread_t* threadId, void* function, char* name)
{
    writeDebugMessage("[CORE] Starting %s thread\n", name);
    if (pthread_create(threadId, NULL, function, NULL) != 0)
    {
        writeDebugMessage("[CORE] Error: Could not create %s thread\n", name);
    }
}

int main(int argc, char** argv)
{
    //Seed random
    srand(time(0));
    
    createDebugPipe();

    startThread(&batteryThreadId, batteryThread, "Battery");
    startThread(&lidarThreadId, lidarThread, "LIDAR");
    startThread(&canReaderThreadId, canReaderThread, "CAN Reader");
    startThread(&cameraThreadId, cameraThread, "Camera");

    writeDebugMessage("[CORE] Threads started\n");

    pthread_join(batteryThreadId, NULL);
    pthread_join(lidarThreadId, NULL);
    pthread_join(canReaderThreadId, NULL);
    pthread_join(cameraThreadId, NULL);

    writeDebugMessage("[CORE] Threads joined\n");
}
