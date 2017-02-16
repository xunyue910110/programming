/*
* Copyright (c) 2003,Linkage All rights reserved.
*
*@文件说明:使用UDP发送消息
*@文件标识:暂无
*@摘    要:UDP发送类
*/
#include "base/config-all.h"

#include <stdexcept>
#include <sys/socket.h>
#include <cerrno>
#include <netinet/in.h>

USING_NAMESPACE(std)


///抛出异常
class UdpSenderException : public runtime_error
{
public:
    UdpSenderException() : runtime_error(strerror(errno)){ ; }
};


///将运行信息发送到服务端
class UdpSender
{
public:
    UdpSender();
    ~UdpSender();

    ///建立UDP句柄，绑定信息
    void open(char *address, int port);
    ///向服务端发送消息
    void send(void *data, size_t size);
    ///关闭UDP句柄
    void close();
private:
    int m_sock;
    struct sockaddr_in m_client, m_server;
};



