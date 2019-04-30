#ifndef CAMERA_THREAD
#define CAMERA_THREAD

#include <pthread.h>

pthread_t cameraThreadId;

void* cameraThread(void* args);

#endif
