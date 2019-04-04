#include "Debug.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include "Pipes.h"
#include "Packets.h"

void createDebugPipe(void)
{
    if (mkfifo(DEBUG_PIPE_NAME, READ_WRITE_ALL) == -1)
    {
        if (errno != EEXIST)
        {
            printf("Error: Could not create debug pipe.\n");
        }
    }

    debugPipe = open(DEBUG_PIPE_NAME, O_RDWR);
    printf("Debug pipe open\n");
}

void writeDebugMessage(char* msg)
{
    struct DebugPacket pkt;
    pkt.strLength = strlen(msg);

    if (write(debugPipe, &pkt, sizeof(struct DebugPacket)) == -1)
    {
        return;
    }

    write(debugPipe, msg, pkt.strLength);
}
