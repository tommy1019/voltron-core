#include "Debug.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Packets.h"
#include "Net.h"

void createDebugPipe(void)
{
    //Open the udp socket for writing
    debugSock = createSocket(DEBUG_PORT);
    if (debugSock < 0)
    {
        printf("ERROR: Could not create debug pipe, quitting program.");
        exit(-1);
    }
}

void writeDebugMessage(const char* format, ...)
{
    //Packet to be written
    struct DebugPacket pkt;

    //Use sprintf to write formatted debug output to buffer
    va_list arg;
    va_start(arg, format);
    vsprintf(pkt.str, format, arg);
    va_end(arg);

    //Get length of formatted string
    pkt.strLength = strlen(pkt.str);
    time(&pkt.timestamp);

    //Print to stdout
    printf("%s", pkt.str);

    //Write packet
    if (write(debugSock, &pkt, sizeof(int) + sizeof(time_t) + pkt.strLength) == -1)
    {
        return;
    }
}
