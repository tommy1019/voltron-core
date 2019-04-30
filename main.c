#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#include "Defines.h"

#include "BatteryThread.h"
#include "LIDARThread.h"
#include "Debug.h"

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

    #ifdef CAN_THREAD
    startThread(&canReaderThreadId, canReaderThread, "CAN Reader");
    #endif

    writeDebugMessage("[CORE] Threads started\n");

    pthread_join(batteryThreadId, NULL);
    pthread_join(lidarThreadId, NULL);

    #ifdef CAN_THREAD
    pthread_join(canReaderThreadId, NULL);
    #endif

    writeDebugMessage("[CORE] Threads joined\n");
}
