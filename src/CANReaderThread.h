#ifndef CAN_READER_THREAD_H
#define CAN_READER_THREAD_H

#include <pthread.h>
#include <semaphore.h>

#include "Packets.h"

pthread_t canReaderThreadId;
pthread_t canControlThreadId;

sem_t listLock;

struct CANList
{
    struct CANControlPacket pkt;
    struct CANList* next;
};

struct CANList* head;

void* canReaderThread(void* args);
void* canControlThread(void* args);

#endif
