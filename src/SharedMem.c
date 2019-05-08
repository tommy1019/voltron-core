#include "SharedMem.h"

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "Debug.h"

void* openSharedMemory(char* name, size_t size)
{
    //Open shared memory
    int fd = shm_open(name, O_RDWR, 0777);
    if (fd == -1)
    {
        //Create new shared memory if none exists
        fd = shm_open(name, O_RDWR | O_CREAT, 0777);
        if (fd == -1)
        {
            writeDebugMessage("[ERROR] Failed to create shared memory.\n");
            return NULL;
        }

        //Resize new shared memory to correct size
        if (ftruncate(fd, size) == -1)
        {
            writeDebugMessage("[ERROR] Could not resize shared memory to correct size.\n");
            return NULL;
        }
    }

    //Map shared memory to ram
    void* memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (memory == MAP_FAILED)
    {
        writeDebugMessage("[ERROR] Could not map memory to shared memory\n");
        return NULL;
    }

    return memory;
}
