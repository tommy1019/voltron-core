#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#include "Defines.h"

#include "BatteryThread.h"
#include "Debug.h"

#ifdef CAN_THREAD
#include "CANReaderThread.h"
#endif

int main(int argc, char** argv)
{
    //See random
    srand(time(0));

    //Ignore pipe failures
    sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);

    printf("Starting battery thread\n");
    if (pthread_create(&batteryThreadId, NULL, batteryThread, NULL) != 0)
    {
        printf("Error: Could not create thread\n");
    }

    #ifdef CAN_THREAD
    printf("Starting CAN thread\n");
    if (pthread_create(&canReaderThreadId, NULL, canReaderThread, NULL) != 0)
    {
        printf("Error: Could not create thread\n");
    }
    #endif

    createDebugPipe();
    writeDebugMessage("Hello World\n");

    pthread_join(batteryThreadId, NULL);
    printf("Battery thread joined\n");

    #ifdef CAN_THREAD
    pthread_join(canReaderThreadId, NULL);
    printf("CAN reader thread joined\n");
    #endif

}
