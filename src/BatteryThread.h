#ifndef BATTERY_THREAD_H
#define BATTERY_THREAD_H

#include <pthread.h>

pthread_t batteryThreadId;

/*
 * Sends random battery data for testing
 */
void* batteryThread(void* args);

#endif
