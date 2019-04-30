#ifndef CAMERA_THREAD
#define CAMERA_THREAD

#include <pthread.h>

pthread_t cameraThreadId;

/*
 *  Grabs images from attached ZED camera and stores them in shared memory
 */
void* cameraThread(void* args);

#endif
