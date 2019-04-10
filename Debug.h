#ifndef DEBUG_H
#define DEBUG_H

int debugSock;

void createDebugPipe(void);

void writeDebugMessage(char* msg);

#endif
