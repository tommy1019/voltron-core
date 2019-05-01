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

/*
 *  Reads CAN data from the cart's CAN bus. Sends out packets as requested by CAN sender id
 */
void* canReaderThread(void* args);

/*
 *  Listens for packets on the CAN control port, and registerers 
 */
void* canControlThread(void* args);

#endif
