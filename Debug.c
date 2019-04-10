#include "Debug.h"

#include <unistd.h>
#include <string.h>

#include "Packets.h"
#include "Net.h"

void createDebugPipe(void)
{
    debugSock = createSocket(DEBUG_PORT);
}

void writeDebugMessage(char* msg)
{
    struct DebugPacket pkt;
    pkt.strLength = strlen(msg);

    if (write(debugSock, &pkt, sizeof(struct DebugPacket)) == -1)
    {
        return;
    }

    write(debugSock, msg, pkt.strLength);
}
