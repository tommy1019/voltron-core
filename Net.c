#include "Net.h"

#include <string.h>
#include <arpa/inet.h>

int createSocket(int port)
{
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        return -1;
    }

    struct sockaddr_in groupSock;
    memset(&groupSock, 0, sizeof(groupSock));
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = inet_addr("224.0.0.155");
    groupSock.sin_port = htons(port);

    struct in_addr localInterface;
    localInterface.s_addr = inet_addr("127.0.0.1");
    if(setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
    {
        return -1;
    }

    if (connect(sockfd, (const struct sockaddr *)&groupSock, sizeof(groupSock)))
    {
        return -1;
    }

    return sockfd;
}
