#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#include "BatteryThread.h"

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

    pthread_join(batteryThreadId, NULL);
    printf("Thread joined\n");
}
