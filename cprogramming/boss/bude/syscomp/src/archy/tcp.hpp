//////////////////////////////////////////////////////////////////////
// 
// 类名: tcp_client & tcp_listener
// 描述: tcp封装类
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-15 21:11 
// 版本: V2.0.0 
// 版权: 南京联创科技股份有限公司

//////////////////////////////////////////////////////////////////////


#ifndef	C_SocketClient__H
#define C_SocketClient__H
#include "archy_config.hpp"

#if defined(OS_WIN)
#include <winsock.h>
//#include <winsock2.h>
/**
 *  class init_WSA used to initalise windows sockets specfifc stuff : there is
 *  an MS - specific init sequence for Winsock 2 this class attempts to 
 *  initalise Winsock 2.2 - needed for non - blocking I/O. It will fall back 
 *  on 1.2 or lower if 2.0 or higher is not available,  but < 2.0 does not 
 *  support non - blocking I/o
 *  TO DO : might be an idea to have a method that reports version of 
 *  Winsock in use or a predicate to test if non - blocking is OK -- JFC
 */

class init_WSA
{
public:
	init_WSA();
	~init_WSA();
};
#else

#define closesocket close
typedef int SOCKET;
#define INVALID_SOCKET	-1

#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>

#endif // OS_WIN

namespace archy {
	
class tcp_socket
{
protected:
	struct sockaddr_in m_sAddr;
	SOCKET m_iSocket;
	
	bool m_autoclose;
public:
	tcp_socket(const tcp_socket& so);
	tcp_socket();
	tcp_socket(SOCKET iSocket);
	tcp_socket(SOCKET iSocket, struct sockaddr_in addr);
	tcp_socket& operator = (const tcp_socket& so);
	~tcp_socket();
	
	bool valid()const { return m_iSocket == INVALID_SOCKET; }
	
	// 在对象析构时是否自动关闭套接口, 默认是关闭
	void auto_close(bool autoclose = false)
	{ m_autoclose = autoclose; }
	
	int connect(const char *strServer, int iPort, int iTimeoutSecs);
	int connect(const char *strServer, int iPort, bool bIsUdp = false);

	int read(char *strBuf, int iLen, long iTime = 900);	
	int readn(char* strBuf, int iLen, long iTime = 900);

	int write(const char *strBuf, int iLen, long iTime = 900);
	int writen(const void* vptr, int n);

	int close();

	SOCKET get_socket()const { return m_iSocket; }
	void set_socket(SOCKET fd) { m_iSocket = fd; }
	
	struct sockaddr_in get_addr() const { return m_sAddr; }
	void set_addr(struct sockaddr_in addr) { m_sAddr = addr; }

	// 获取对端的ip地址
	const char* get_peer_addr()const { return inet_ntoa(m_sAddr.sin_addr); }

};

class tcp_listener{

private:
	struct sockaddr_in m_sAddr;
	SOCKET m_iSocket;
	
public:
	tcp_listener();
	~tcp_listener();

	int initial(int iPort);
	
	int accept(tcp_socket& tcp, long iTimeout = 900);

	int close(void);
	
	SOCKET get_socket()const { return m_iSocket; }
	void set_socket(SOCKET fd){ m_iSocket = fd; }
	
	struct sockaddr_in get_addr() const { return m_sAddr; }
	void set_addr(struct sockaddr_in addr) { m_sAddr = addr; }
	
};

} // namespace archy

#endif


