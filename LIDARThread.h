#ifndef LIDAR_THREAD_H
#define LIDAR_THREAD_H

#include <pthread.h>

pthread_t lidarThreadId;

void* lidarThread(void* args);

#endif
