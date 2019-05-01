#ifndef DEBUG_H
#define DEBUG_H

#include <stdarg.h>

int debugSock;

/*
 *  Sets up debug printing for later in the program, must be called before writeDebugMessage
 */
void createDebugPipe(void);

/*
 *  Writes a debug message to stdout and to the debug udp socket
 */
void writeDebugMessage(const char* format, ...);

#endif
