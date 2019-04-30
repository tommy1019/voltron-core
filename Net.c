#include "Net.h"

#include <string.h>
#include <arpa/inet.h>

#include "Packets.h"

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
    groupSock.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
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

int createReadSocket(int port)
{
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        return -1;
    }

    int one = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        return -1;
    }

    struct ip_mreq group;
    group.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    group.imr_interface.s_addr = inet_addr("127.0.0.1");
    if(setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
    {
        return -1;
    }

    return sockfd;
}
