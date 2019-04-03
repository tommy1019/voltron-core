#ifndef CAN_READER_THREAD_H
#define CAN_READER_THREAD_H

#include <pthread.h>

pthread_t canReaderThreadId;

void* canReaderThread(void* args);

#endif
