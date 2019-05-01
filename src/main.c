#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#include "Debug.h"
#include "BatteryThread.h"
#include "LIDARThread.h"
#include "CANReaderThread.h"
#include "CameraThread.h"

/*
 * Helper function for starting threads
 */
void startThread(pthread_t* threadId, void* function, char* name);

int main(int argc, char** argv)
{
    //Set up debug pipe for debug messages
    createDebugPipe();

    //Start threads
    startThread(&batteryThreadId, batteryThread, "Battery");
    startThread(&lidarThreadId, lidarThread, "LIDAR");

    #ifdef COMPILE_ALL //Macro guard to not compile/start threads which cannot be compiler off the cart
    startThread(&canReaderThreadId, canReaderThread, "CAN Reader");
    startThread(&cameraThreadId, cameraThread, "Camera");
    #endif

    writeDebugMessage("[CORE] Threads started\n");

    //Join threads (Will never be called)
    pthread_join(batteryThreadId, NULL);
    pthread_join(lidarThreadId, NULL);

    #ifdef COMPILE_ALL  //Macro guard to not compile/stop threads which cannot be compiler off the cart
    pthread_join(canReaderThreadId, NULL);
    pthread_join(cameraThreadId, NULL);
    #endif

    writeDebugMessage("[CORE] Threads joined\n");
}

void startThread(pthread_t* threadId, void* function, char* name)
{
    writeDebugMessage("[CORE] Starting %s thread\n", name);

    //Create the thread
    if (pthread_create(threadId, NULL, function, NULL) != 0)
    {
        writeDebugMessage("[CORE] Error: Could not create %s thread\n", name);
    }
}
