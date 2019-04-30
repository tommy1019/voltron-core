#include "Defines.h"
#ifdef CAN_THREAD

#include "CANReaderThread.h"
#include "Debug.h"
#include "Net.h"

#include <linux/can.h>
#include <linux/can/raw.h>

#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void* canReaderThread(void* args)
{
    head = NULL;
    sem_init(&listLock, 0, 1);

    if (pthread_create(&canControlThreadId, NULL, canControlThread, NULL) != 0)
    {
        writeDebugMessage("[CAN] Error: Could not create CAN control thread thread\n");
    }

    int dataSocket = createSocket(CAN_DATA_PORT);
    if (dataSocket < 0)
    {
        writeDebugMessage("[CAN] Error: Opening data socket\n");
        return NULL;
    }

    int s;
    if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        writeDebugMessage("[CAN] Error: While opening can0 socket\n");
        return NULL;
    }

    struct ifreq ifr;

    const char *ifname = "can0";
    strcpy(ifr.ifr_name, ifname);
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0)
    {
        writeDebugMessage("[CAN] Error: Getting index for can0\n");
        return NULL;
    }

    struct sockaddr_can addr;
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    writeDebugMessage("[CAN] %s at index %d\n", ifname, ifr.ifr_ifindex);

    if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        writeDebugMessage("[CAN] Error: binding can0 socket\n");
        return NULL;
    }

    while(1)
    {
        struct can_frame frame;

        int nbytes;
        nbytes = read(s, &frame, sizeof(struct can_frame));

        writeDebugMessage("[CAN] %#010x - %#04x%02x%02x%02x%02x%02x%02x%02x - %d (%d)\n",
               frame.can_id,
               frame.data[0],
               frame.data[1],
               frame.data[2],
               frame.data[3],
               frame.data[4],
               frame.data[5],
               frame.data[6],
               frame.data[7],
               nbytes,
               frame.can_dlc);

        if (head == NULL)
            continue;

        struct CANList* curElement = head;

        while(curElement->next != NULL)
        {
            if (curElement->pkt.sender)
            {
                struct CANDataPacket pkt;
                pkt.pktId = curElement->pkt.pktId;
                pkt.sender = frame.can_id;
                for (int i = 0; i < 8; i++)
                    pkt.data[i] = frame.data[i];

                write(dataSocket, &pkt, sizeof(struct CANDataPacket));
            }

            curElement = curElement->next;
        }
    }

    return NULL;
}

void* canControlThread(void* args)
{
    int sockfd = createReadSocket(CAN_CONTROL_PORT);
    if (sockfd < 0)
    {
        writeDebugMessage("[CAN] Error opening control socket\n");
        return NULL;
    }

    writeDebugMessage("[CAN] Control socket open\n");

    while(1)
    {
        struct CANControlPacket pkt;

        read(sockfd, &pkt, sizeof(struct CANControlPacket));

        struct CANList* newElement = (struct CANList*)malloc(sizeof(struct CANControlPacket));
        newElement->pkt = pkt;
        newElement->next = NULL;

        sem_wait(&listLock);

        if (head == NULL)
        {
            head = newElement;
        }
        else
        {
            struct CANList* curElement = head;

            while(curElement->next != NULL)
                curElement = curElement->next;

            curElement->next = newElement;

            writeDebugMessage("[CAN] Registered new CAN listener id: %i, sender: %i\n", newElement->pkt.pktId, newElement->pkt.sender);
        }

        sem_post(&listLock);
    }
}

#endif
