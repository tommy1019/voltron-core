#ifndef LIDAR_THREAD_H
#define LIDAR_THREAD_H

#include <pthread.h>

pthread_t lidarThreadId;
pthread_t lidarGPSThreadId;

void* lidarThread(void* args);
void* lidarGPSThread(void* args);

#endif
