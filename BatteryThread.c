#include "BatteryThread.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "Pipes.h"
#include "Packets.h"

void* batteryThread(void* args)
{
    if (mkfifo(BATTERY_PIPE_NAME, READ_WRITE_ALL) == -1)
    {
        if (errno != EEXIST)
        {
            printf("Error: Could not create battery pipe.\n");
        }
    }

    while (1)
    {
        int batteryPipe = open(BATTERY_PIPE_NAME, O_WRONLY);
        printf("Battery pipe open\n");

        while (1)
        {
            struct BatteryPacket pkt;

            pkt.cellNum = rand() % 20;
            pkt.charge = rand() % 100 / 100.0;

            if (write(batteryPipe, &pkt, sizeof(struct BatteryPacket)) == -1)
            {
                if (errno == EPIPE)
                {
                    printf("Error: Battery pipe closed\n");
                    break;
                }
                else
                {
                    printf("Error: Battery write failed\n");
                    return NULL;
                }
            }

            usleep(2000);
        }

        close(batteryPipe);
    }


    return NULL;
}
