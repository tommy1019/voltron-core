#ifndef DEBUG_H
#define DEBUG_H

#include <stdarg.h>

int debugSock;

void createDebugPipe(void);

void writeDebugMessage(const char* format, ...);

#endif
