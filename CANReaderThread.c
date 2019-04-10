#include "Defines.h"
#ifdef CAN_THREAD

#include "CANReaderThread.h"

#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

void* canReaderThread(void* args)
{
    struct ifreq;

    int s;
    if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        perror("[CAN] Error while opening socket");
        return;
    }

    struct ifr;
    const char *ifname = "can0";
    strcpy(ifr.ifr_name, ifname);
    ioctl(s, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    printf("[CAN] %s at index %d\n", ifname, ifr.ifr_ifindex);

    if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("[CAN] Error in socket bind");
        return -2;
    }

    while(1)
    {
        struct can_frame frame;

        int nbytes;
        nbytes = read(s, &frame, sizeof(struct can_frame));

        printf("[CAN] Read %d bytes\n", nbytes);
        printf("[CAN] %#010x - %#04x%02x%02x%02x%02x%02x%02x%02x - %d (%d)\n",
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

    return 0;
}

#endif
