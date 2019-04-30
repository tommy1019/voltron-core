#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#include "Debug.h"
#include "BatteryThread.h"
#include "LIDARThread.h"
#include "CANReaderThread.h"
#include "CameraThread.h"

void startThread(pthread_t* threadId, void* function, char* name);

int main(int argc, char** argv)
{
    createDebugPipe();

    startThread(&batteryThreadId, batteryThread, "Battery");
    startThread(&lidarThreadId, lidarThread, "LIDAR");

    #ifdef COMPILE_ALL
    startThread(&canReaderThreadId, canReaderThread, "CAN Reader");
    startThread(&cameraThreadId, cameraThread, "Camera");
    #endif

    writeDebugMessage("[CORE] Threads started\n");

    pthread_join(batteryThreadId, NULL);
    pthread_join(lidarThreadId, NULL);

    #ifdef COMPILE_ALL
    pthread_join(canReaderThreadId, NULL);
    pthread_join(cameraThreadId, NULL);
    #endif

    writeDebugMessage("[CORE] Threads joined\n");
}

/*
 * Helper function for starting threads
 */
void startThread(pthread_t* threadId, void* function, char* name)
{
    writeDebugMessage("[CORE] Starting %s thread\n", name);
    if (pthread_create(threadId, NULL, function, NULL) != 0)
    {
        writeDebugMessage("[CORE] Error: Could not create %s thread\n", name);
    }
}
