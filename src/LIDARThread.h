#ifndef LIDAR_THREAD_H
#define LIDAR_THREAD_H

#include <pthread.h>

pthread_t lidarThreadId;
pthread_t lidarGPSThreadId;

/*
 *  Reads in LIDAR data from UDP into shared memory notifying others over UDP when a full sweep of the LIDAR is complete
 */
void* lidarThread(void* args);

/*
 *  Reads GPS data from UDP and writes debug messages containing the data
 */
void* lidarGPSThread(void* args);

#endif
