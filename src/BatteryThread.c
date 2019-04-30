#include "BatteryThread.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Packets.h"
#include "Net.h"
#include "Debug.h"

void* batteryThread(void* args)
{
    int sockfd = createSocket(BATTERY_PORT);
    if (sockfd < 0)
    {
        writeDebugMessage("[Battery] Error opening socket\n");
        return NULL;
    }

    writeDebugMessage("[Battery] Socket open\n");

    do
    {
        struct BatteryPacket pkt;

        pkt.cellNum = rand() % 20;
        pkt.charge = rand() % 100 / 100.0;

        write(sockfd, &pkt, sizeof(struct BatteryPacket));

        usleep(80000);
    }
    while (1);

    close(sockfd);

    return NULL;
}
