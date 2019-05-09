#include "Net.h"

#include <string.h>
#include <arpa/inet.h>

#include "Packets.h"

int createSocket(int port)
{
    //Create socket for writing
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        return -1;
    }

    //Create multicast address for writing
    struct sockaddr_in groupSock;
    memset(&groupSock, 0, sizeof(groupSock));
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
    groupSock.sin_port = htons(port);

    //Set writing to loopback interface
    struct in_addr localInterface;
    localInterface.s_addr = inet_addr("127.0.0.1");
    if(setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
    {
        return -1;
    }

    //Connect the socket for writing on the multicast loopback interface
    if (connect(sockfd, (struct sockaddr *)&groupSock, sizeof(groupSock)))
    {
        return -1;
    }

    return sockfd;
}

int createReadSocket(int port)
{
    //Create socket for reading
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        return -1;
    }

    //Set reuse port so others can read on the selected port
    int one = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0)
        return -1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one)) < 0)
        return -1;

    //Create address to read on
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    //Bind the socket to the address
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        return -1;
    }

    //Allow the socket to read on muilticast
    struct ip_mreq group;
    group.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    group.imr_interface.s_addr = inet_addr("127.0.0.1");
    if(setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
    {
        return -1;
    }

    return sockfd;
}
