#ifdef COMPILE_ALL

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
    //Create head of linked list for control packets
    head = NULL;

    //Create semaphore for control packet sync
    sem_init(&listLock, 0, 1);

    //Create thread to receive control packets
    if (pthread_create(&canControlThreadId, NULL, canControlThread, NULL) != 0)
    {
        writeDebugMessage("[CAN] Error: Could not create CAN control thread thread\n");
    }

    //Create udp socket for writing CAN data packets
    int dataSocket = createSocket(CAN_DATA_PORT);
    if (dataSocket < 0)
    {
        writeDebugMessage("[CAN] Error: Opening data socket\n");
        return NULL;
    }

    //Create CAN socket for reading from CAN bus
    int s;
    if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        writeDebugMessage("[CAN] Error: While opening can0 socket\n");
        return NULL;
    }

    //Find interface for can0 (CAN bus)
    struct ifreq ifr;
    const char *ifname = "can0";
    strcpy(ifr.ifr_name, ifname);
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0)
    {
        writeDebugMessage("[CAN] Error: Getting index for can0\n");
        return NULL;
    }

    //Create address for can0
    struct sockaddr_can addr;
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    writeDebugMessage("[CAN] %s at index %d\n", ifname, ifr.ifr_ifindex);

    //Bind the CAN socket to the created address
    if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        writeDebugMessage("[CAN] Error: binding can0 socket\n");
        return NULL;
    }
    
    while(1)
    {
        //Read a fram from the can bus
        struct can_frame frame;
        int nbytes;
        nbytes = read(s, &frame, sizeof(struct can_frame));

        //writeDebugMessage("[CAN] %#010x - %#04x%02x%02x%02x%02x%02x%02x%02x - %d (%d)\n", frame.can_id, frame.data[0], frame.data[1], frame.data[2], frame.data[3], frame.data[4], frame.data[5], frame.data[6], frame.data[7], nbytes, frame.can_dlc);

        //If linked list is empty continue
        if (head == NULL)
            continue;

        //Search list for received sender id
        struct CANList* curElement = head;
        do
        {
            //If found correct sender
            if (curElement->pkt.sender == frame.can_id)
            {
                //Create CAN Data packet to be sent over UDP
                struct CANDataPacket pkt;

                //Fill with correct data
                pkt.pktId = curElement->pkt.pktId;
                pkt.sender = frame.can_id;
                for (int i = 0; i < 8; i++)
                    pkt.data[i] = frame.data[i];

                //Write the packet
                if (write(dataSocket, &pkt, sizeof(struct CANDataPacket)) != sizeof(struct CANDataPacket))
                {
                    writeDebugMessage("[CAN] Failed to write entire packet\n");
                }
            }

            curElement = curElement->next;
        }
        while(curElement->next != NULL);
    }

    return NULL;
}

void* canControlThread(void* args)
{
    //Open udp socket to read CAN control messages
    int sockfd = createReadSocket(CAN_CONTROL_PORT);
    if (sockfd < 0)
    {
        writeDebugMessage("[CAN] Error opening control socket\n");
        return NULL;
    }

    writeDebugMessage("[CAN] Control socket open\n");

    while(1)
    {
        //CAN control packet to be read
        struct CANControlPacket pkt;

        //Read packet
        if (read(sockfd, &pkt, sizeof(struct CANControlPacket)) != sizeof(struct CANControlPacket))
        {
            continue;
        }

        //Append to linked list
        struct CANList* newElement = (struct CANList*)malloc(sizeof(struct CANControlPacket));
        newElement->pkt = pkt;
        newElement->next = NULL;

        //Get mutex lock for list
        sem_wait(&listLock);

        //Append new element to linked list
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

        //Relese lock on list
        sem_post(&listLock);
    }
}

#endif
