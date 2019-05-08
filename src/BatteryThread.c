#include "BatteryThread.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "Packets.h"
#include "Net.h"
#include "Debug.h"

void* batteryThread(void* args)
{
    //Seed random
    srand(time(0));

    //Create udp socket for writing data
    int sockfd = createSocket(BATTERY_PORT);
    if (sockfd < 0)
    {
        writeDebugMessage("[Battery] Error opening socket\n");
        return NULL;
    }

    writeDebugMessage("[Battery] Socket open\n");

    //Infinitely send dummy data
    while (1)
    {
        //Packet to be sent
        struct BatteryPacket pkt;

        //Fill packet with dummy data
        pkt.cellNum = rand() % 20;
        pkt.charge = rand() % 100 / 100.0;
        time(&pkt.timestamp);

        //Send packet
        if (write(sockfd, &pkt, sizeof(struct BatteryPacket)) != sizeof(struct BatteryPacket))
        {
            writeDebugMessage("[Battery] Failed to write packet\n");
            return NULL;
        }

        //Sleep as to not flood data collectors
        usleep(80000);
    }

    //Close udp socket
    close(sockfd);

    return NULL;
}
