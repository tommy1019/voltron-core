#include <fstream>
#include <iostream>
#include <time.h>

#include "Packets.h"

int main()
{
    std::ifstream file ("battery.cap", std::ios::in | std::ios::binary);

    while(!file.eof())
    {
        BatteryPacket data;

        file.read(reinterpret_cast<char*>(&data), sizeof(BatteryPacket));

        std::cout << ctime(&data.timestamp) << " - " << data.cellNum << " - " << data.charge << std::endl;
    }
}
