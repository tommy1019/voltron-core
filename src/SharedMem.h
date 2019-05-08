#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include <stdlib.h>

void* openSharedMemory(const char* name, size_t size);

#endif
