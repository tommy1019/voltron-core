#include <fstream>
#include <iostream>
#include <time.h>

#include "Packets.h"

int main()
{
    std::ifstream file ("can.cap", std::ios::in | std::ios::binary);

    while(!file.eof())
    {
        CANDataPacket data;

        file.read(reinterpret_cast<char*>(&data), sizeof(CANDataPacket));

        std::cout << "ID:        " << data.pktId << std::endl;
        std::cout << "timestamp: " << ctime(&data.timestamp) << std::endl;
        std::cout << "Sender:    " << data.sender << std::endl;
        std::cout << "Data:      " << data.data[0] << " " << data.data[1] << " " << data.data[2] << " " << data.data[3] << " " << data.data[4] << " " << data.data[5] << " " << data.data[6] << " " << data.data[7] << " " << std::endl;
    }
}
