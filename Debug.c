#include "Debug.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "Packets.h"
#include "Net.h"

void createDebugPipe(void)
{
    debugSock = createSocket(DEBUG_PORT);
    if (debugSock < 0)
    {
        printf("ERROR: Could not create debug pipe, quitting program.");
        exit(-1);
    }
}

void writeDebugMessage(char* format, ...)
{
    struct DebugPacket pkt;

    va_list arg;
    va_start(arg, format);
    sprintf(pkt.str, format, arg);
    va_end(arg);

    pkt.strLength = strlen(pkt.str);

    printf("%s", pkt.str);

    if (write(debugSock, &pkt, sizeof(int) + pkt.strLength) == -1)
    {
        return;
    }
}
