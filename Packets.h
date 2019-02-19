#ifndef PACKETS_H
#define PACKETS_H

#define BATTERY_PIPE_NAME "/tmp/voltron_battery"

struct BatteryPacket
{
    int cellNum;

    float charge;
};

#endif
