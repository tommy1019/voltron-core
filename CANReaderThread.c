#include "Defines.h"
#ifdef CAN_THREAD

#include "CANReaderThread.h"
#include "Debug.h"

#include <linux/can.h>
#include <linux/can/raw.h>

#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

void* canReaderThread(void* args)
{
    int s;
    if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        writeDebugMessage("[CAN] Error while opening can0 socket\n");
        return NULL;
    }

    struct ifreq ifr;

    const char *ifname = "can0";
    strcpy(ifr.ifr_name, ifname);
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0)
    {
        writeDebugMessage("[CAN] Error getting index for can0\n");
        return NULL;
    }

    struct sockaddr_can addr;
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    writeDebugMessage("[CAN] %s at index %d\n", ifname, ifr.ifr_ifindex);

    if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        writeDebugMessage("[CAN] Error binding can0 socket\n");
        return NULL;
    }

    while(1)
    {
        struct can_frame frame;

        int nbytes;
        nbytes = read(s, &frame, sizeof(struct can_frame));

        writeDebugMessage("[CAN] Read %d bytes\n", nbytes);
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
    }

    return NULL;
}

#endif
