#include <fstream>
#include <iostream>
#include <time.h>

#include "Packets.h"

int main()
{
    std::ifstream file ("lidar.cap", std::ios::in | std::ios::binary);

    while(!file.eof())
    {
        time_t timestamp;
        LIDARData data;

        file.read(reinterpret_cast<char*>(&timestamp), sizeof(time_t));
        file.read(reinterpret_cast<char*>(&data), sizeof(LIDARData));

        std::cout << ctime(&timestamp);

        for (int i = 0; i < LIDAR_DATA_NUM_POINTS; i++)
            std::cout
            << "("  << data.point[i].x << ", "
                    << data.point[i].y << ", "
                    << data.point[i].z << ") = "
                    << data.point[i].reflectivity
            <<std::endl;
    }
}
