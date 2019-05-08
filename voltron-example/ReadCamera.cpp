#include <fstream>
#include <iostream>

#include "Packets.h"

int main()
{
    std::ifstream file ("camera.cap", std::ios::in | std::ios::binary);

    while(!file.eof())
    {
        time_t timestamp;
        CameraData data;

        file.read(reinterpret_cast<char*>(&timestamp), sizeof(time_t));
        file.read(reinterpret_cast<char*>(&data), sizeof(CameraData));

        std::cout << ctime(&timestamp) << std::endl;
    }
}
