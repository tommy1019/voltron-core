#ifndef NET_H
#define NET_H

/*
 *  Creates a UDP socket for writing
 */
int createSocket(int port);

/*
 *  Creates a UDP socket for reading
 */
int createReadSocket(int port);

#endif
