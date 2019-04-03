#ifndef PACKETS_H
#define PACKETS_H

#define BATTERY_PIPE_NAME "/tmp/voltron_battery"
#define DEBUG_PIPE_NAME "/tmp/voltron_debug"

struct BatteryPacket
{
    int cellNum;

    float charge;
};

struct DebugPacket
{
    int strLength;
};

#endif
