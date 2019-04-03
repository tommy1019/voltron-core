#ifndef DEBUG_H
#define DEBUG_H

int debugPipe;

void createDebugPipe(void);

void writeDebugMessage(char* msg);

#endif
