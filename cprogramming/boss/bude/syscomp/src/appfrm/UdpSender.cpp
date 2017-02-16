
#include "UdpSender.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>



void UdpSender::open(char *address, int port)
{    	
    ///建立UDP句柄
    m_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_sock < 0)
    {
        throw UdpSender();
    }

    struct hostent *host;
    struct in_addr addr;

    ///取服务端地址
    addr.s_addr = inet_addr(address);
    if (addr.s_addr == -1)
    {
        host = gethostbyname(address);
        if (host != NULL)
        {
            addr = *(struct in_addr *) *host->h_addr_list;	
        }
        else
        {
            throw UdpSenderException();
        }
    }

    ///设置服务端信息
    memset((char *) &m_server, 0, sizeof(m_server));
    m_server.sin_family = AF_INET;
    m_server.sin_addr.s_addr = addr.s_addr;
    m_server.sin_port = port;

    ///设置客户端信息
    memset((char *) &m_client, 0, sizeof(m_client));
    m_client.sin_family = AF_INET;
    m_client.sin_addr.s_addr = htonl(INADDR_ANY);
    m_client.sin_port = htons(0);

    ///绑定客户端信息
    if (bind(m_sock, reinterpret_cast <struct sockaddr *> (&m_client),
            sizeof(m_client)) < 0) 
    {
        throw UdpSenderException();
    }
}

void UdpSender::close()
{
    if(m_sock != -1)
    {	
        ::close(m_sock);
        m_sock = -1;
    }
}

UdpSender::UdpSender() : m_sock(-1)
{
}

UdpSender::~UdpSender()
{
    close();
}

void UdpSender::send(void *data, size_t size)
{
    sendto(m_sock, data, size, 0, 
           reinterpret_cast <struct sockaddr *> (&m_server), sizeof(m_server)); 	
}
