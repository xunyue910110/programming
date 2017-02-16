//////////////////////////////////////////////////////////////////////
// 
// 类名: tcp_socket & tcp_listener
// 描述: tcp封装类
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-15 21:11 
// 版本: V2.0.0 
// 版权: 南京联创科技股份有限公司
// 在tcp_client::write方法中，加入了等待套接口可写的操作。
//////////////////////////////////////////////////////////////////////
#include "tcp.hpp"
#include <assert.h>
#include <time.h>
#include <errno.h>
#ifndef WIN32
#include <fcntl.h>
#endif

#ifdef OS_WIN
init_WSA::init_WSA()
{
	//-initialize OS socket resources!
	WSADATA	wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		abort();
	}
};

init_WSA::~init_WSA() 
{ 
	WSACleanup(); 
} 

init_WSA init_wsa;
#endif

namespace archy {

#ifndef WIN32
int tcp_socket::connect(const char *strServer, int iPort, int iTimeoutSecs)
{
	unsigned long           lFtpAddr;

	m_sAddr.sin_family = AF_INET;
	m_sAddr.sin_port = htons(iPort);

	/* 设置FTP server Internet 地址：
	首先，判断是否是 格式  "133.96.168.1"
	否则，判断是否是主机名  如："sjz1" */

    m_iSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_iSocket == INVALID_SOCKET)	{
		return -1;
	}

	lFtpAddr = inet_addr(strServer);
#ifndef INADDR_NONE
#define INADDR_NONE (in_addr_t)0xffffffff
#endif
	if(lFtpAddr == INADDR_NONE){
                //  Internet host information structure
	        struct hostent  *lpHostEnt;
		if (!(lpHostEnt = gethostbyname(strServer))){
//			perror("gethostbyname");
			return -1;
		}
		m_sAddr.sin_addr = *((struct in_addr *)*lpHostEnt->h_addr_list);
	}
	else  {
		m_sAddr.sin_addr=*((struct in_addr *)&lFtpAddr);
   }
        
	int flags = fcntl(m_iSocket, F_GETFL, 0);
	fcntl(m_iSocket, F_SETFL, flags | O_NONBLOCK);
	
	int n;
	int error = 0;
	if ( (n = ::connect(m_iSocket, 
		reinterpret_cast<struct sockaddr *>(&m_sAddr), 
		sizeof(m_sAddr) )) < 0)
		if (errno != EINPROGRESS)
			return(-1);

	/* Do whatever we want while the connect is taking place. */

	if (n == 0)
		goto done;	/* connect completed immediately */

	fd_set rset,wset;
	FD_ZERO(&rset);
	FD_SET(m_iSocket, &rset);
	wset = rset;
	struct timeval	tval;
	tval.tv_sec = iTimeoutSecs;
	tval.tv_usec = 0;

	if ( (n = ::select(m_iSocket+1, &rset, &wset, NULL,
					 iTimeoutSecs ? &tval : NULL)) == 0) {
		::closesocket(m_iSocket);		/* timeout */
		errno = ETIMEDOUT;
		return(-1);
	}

	if (FD_ISSET(m_iSocket, &rset) || FD_ISSET(m_iSocket, &wset)) {
//		int len = sizeof(error);
//		if (getsockopt(m_iSocket, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
//			return(-1);			/* Solaris pending error */
	} else
		return -1;

done:
	fcntl(m_iSocket, F_SETFL, flags);	/* restore file status flags */
	if (error) {
		::closesocket(m_iSocket);		/* just in case */
		errno = error;
		return(-1);
	}
	return(0);
}

#else 
int tcp_socket::connect(const char *strServer, int iPort, int iTimeoutSecs)
{
	unsigned long           lFtpAddr;

	m_sAddr.sin_family = AF_INET;
	m_sAddr.sin_port = htons(iPort);

	/* 设置FTP server Internet 地址：
	首先，判断是否是 格式  "133.96.168.1"
	否则，判断是否是主机名  如："sjz1" */

    m_iSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_iSocket == INVALID_SOCKET)	{
		return -1;
	}

	lFtpAddr = inet_addr(strServer);
#ifndef INADDR_NONE
#define INADDR_NONE (in_addr_t)0xffffffff
#endif
	if(lFtpAddr == INADDR_NONE){
                //  Internet host information structure
	        struct hostent  *lpHostEnt;
		if (!(lpHostEnt = gethostbyname(strServer))){
//			perror("gethostbyname");
			return -1;
		}
		m_sAddr.sin_addr = *((struct in_addr *)*lpHostEnt->h_addr_list);
	}
	else  {
		m_sAddr.sin_addr=*((struct in_addr *)&lFtpAddr);
   }
 
    ULONG NonBlk = 1;
    int Ret;
    DWORD Err;

    // Set to Non-blocking mode
    ioctlsocket(m_iSocket, FIONBIO, &NonBlk);


     // This call will return immediately, coz our socket is non-blocking
	Ret = ::connect(m_iSocket, (const sockaddr*)&m_sAddr, sizeof(m_sAddr));

     // If connected, it will return 0, or error
    if (Ret == SOCKET_ERROR)
    {
           Err = WSAGetLastError();
           // Check if the error was WSAEWOULDBLOCK, where we'll wait.
           if (Err == WSAEWOULDBLOCK)
          {
                  printf("\nConnect() returned WSAEWOULDBLOCK. Need to Wait..");
                  fd_set         Write, Err;
                  TIMEVAL      Timeout;

                  FD_ZERO(&Write);
                  FD_ZERO(&Err);
                  FD_SET(m_iSocket, &Write);
                  FD_SET(m_iSocket, &Err);

                  Timeout.tv_sec  = iTimeoutSecs;
                  Timeout.tv_usec = 0; // your timeout

                  Ret = select (0,                // ignored
                                      NULL,           // read,
                                      &Write,        // Write Check
                                      &Err,            // Error check
                                      &Timeout);

                  if (Ret == 0)
                          printf("\nConnect Timeout (%d Sec).", iTimeoutSecs);
                  else
                  {
                         if (FD_ISSET(m_iSocket, &Write))
                         {
                                  printf("\nConnected...");
                                  return 0;
                         }
                         if (FD_ISSET(m_iSocket, &Err))
                         {
                                  printf("\nSelect() Error.");
                                  return -1;
                         }
                   }
            }
            else
                    printf("\nConnect Error %d", WSAGetLastError());
      }
      else
      {
               printf("\nWoooo!! got connected with NO Waiting!!");
               return 0;
       }
       return -1;
}

#endif

int tcp_socket::connect(const char *strServer,int iPort, bool bIsUdp )
{

	unsigned long           lFtpAddr;

	m_sAddr.sin_family = AF_INET;
	m_sAddr.sin_port = htons(iPort);

	/* 设置FTP server Internet 地址：
	首先，判断是否是 格式  "133.96.168.1"
	否则，判断是否是主机名  如："sjz1" */

        m_iSocket = ::socket(AF_INET, ( bIsUdp ? (SOCK_DGRAM) : (SOCK_STREAM)), 0);
	if (m_iSocket == INVALID_SOCKET)	{
		return -1;
	}

	lFtpAddr = inet_addr(strServer);
#ifndef INADDR_NONE
#define INADDR_NONE (in_addr_t)0xffffffff
#endif
	if(lFtpAddr == INADDR_NONE){
                //  Internet host information structure
	        struct hostent  *lpHostEnt;
		if (!(lpHostEnt = gethostbyname(strServer))){
//			perror("gethostbyname");
			return -1;
		}
		m_sAddr.sin_addr = *((struct in_addr *)*lpHostEnt->h_addr_list);
	}
	else  {
		m_sAddr.sin_addr=*((struct in_addr *)&lFtpAddr);
        }

	int ret = ::connect(m_iSocket, reinterpret_cast<struct sockaddr *>(&m_sAddr), sizeof(m_sAddr) );
        if( ret < 0){
		::closesocket(m_iSocket);
		return -1;
	}
        return 0;
}

tcp_socket::tcp_socket()
:m_iSocket(INVALID_SOCKET)
,m_autoclose(true)
{
}

tcp_socket::tcp_socket(SOCKET iSocketfd)
:m_autoclose(true)
,m_iSocket(iSocketfd)
{
//	printf("Ctor:%ld\n",  m_iSocket);
}

tcp_socket::tcp_socket(SOCKET iSocket, struct sockaddr_in addr)
:m_autoclose(true)
,m_iSocket(iSocket)
,m_sAddr(addr)
{
//	printf("Ctor:%ld\n",  m_iSocket);

}



tcp_socket::~tcp_socket()
{	
//	printf("Dtor:%ld\n",  m_iSocket);
	if( m_autoclose && m_iSocket != INVALID_SOCKET ) {
//		printf("Close:%ld\n",  m_iSocket);
		::closesocket(m_iSocket);
	}
}

int tcp_socket::readn(char* strBuf, int iLen, long iTime)
{
	int i = 0;
	int iNum(0);
	time_t timer(::time(0));
	while(1) {
		iNum = this->read(strBuf + i, 1, iTime);
		if( iNum <= 0 ) { break; }
		i++;
		if( i == iLen ) { break; }
		if( ::difftime(time(0), timer) > iTime )
			return 0;
	}
	if( iNum < 0 )
		return -1;
	return i;
}


int tcp_socket::read(char *strBuf, int iLen, long iTime)
{
	fd_set iRset;

	FD_ZERO(&iRset);
	FD_SET(m_iSocket,&iRset);
	
	struct timeval	sTimeout;
	sTimeout.tv_sec = iTime;
	sTimeout.tv_usec = 0;

	int fd = ::select(m_iSocket+1,&iRset,NULL,NULL,&sTimeout);
	if(fd < 0) {
//		perror("select");
		return -1;
	}

	/* m_iSocket可读 */
	if(FD_ISSET(m_iSocket,&iRset)) {
		/*接收到的实际字节数*/
		int iReadLen = (int)recv(m_iSocket, strBuf, iLen, 0);
		if(iReadLen >= 0) {
			strBuf[iReadLen] = 0;
			return iReadLen;
		}
		else{
			return -1;
		}
	}
	
	return 0;
}

int tcp_socket::write(const char *strBuf,int iLength, long iTime)
{
	fd_set iRset;

	FD_ZERO(&iRset);
	FD_SET(m_iSocket,&iRset);
	
	struct timeval	sTimeout;
	sTimeout.tv_sec = iTime;
	sTimeout.tv_usec = 0;

	int fd = ::select(m_iSocket+1, NULL, &iRset, NULL,&sTimeout);
	if(fd < 0) {
//		perror("select");
		return -1;
	}

	/* m_iSocket可写 */
	if(FD_ISSET(m_iSocket,&iRset)) {
		/*接收到的实际字节数*/
		int iReadLen = (int)send(m_iSocket, strBuf, iLength, 0);
		return iReadLen;
	}
	
	return 0;
//	return (int)send(m_iSocket, (char*)strBuf, iLength, 0);
}

int tcp_socket::writen(const void* vptr, int n)
{
	size_t nleft;
	int nwritten;
	const char *ptr;
	ptr = (const char *)vptr;
	nleft = n;
	while( nleft > 0 ) {
		if( (nwritten = send(m_iSocket, ptr, nleft, 0) ) <= 0 ) {
			return -1; // error
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}

int tcp_socket::close(void)
{
	::closesocket(m_iSocket);
	m_iSocket = INVALID_SOCKET;
	return 0;
}

tcp_listener::tcp_listener()
	:m_iSocket(INVALID_SOCKET)
{}


int tcp_listener::initial(int iPort)
{
	int option = 1;
	/*创建TCP协议的字节流套接字*/
	m_iSocket = socket(AF_INET,SOCK_STREAM,0);
	if( m_iSocket == INVALID_SOCKET ) {
		return -1;
	}
	/* 将服务器地址清0 */
	memset(&m_sAddr, 0, sizeof(m_sAddr));
	m_sAddr.sin_family = AF_INET;
	m_sAddr.sin_addr.s_addr = INADDR_ANY;
	/* 使用服务端口号 */
	m_sAddr.sin_port = htons(iPort);
	
	setsockopt ( m_iSocket, SOL_SOCKET, SO_REUSEADDR,(char*) &option,sizeof( option ) ) ;

	/*将本地主机(服务器)的地址捆扎到创建的套接字上 */
	if(bind(m_iSocket, reinterpret_cast<struct sockaddr *>(&m_sAddr), (int)sizeof(m_sAddr))<0)
	{ 
		return -1;
	} 
	/*建立长度为5的监听队列,从套接字上收听连接请求*/
	if(listen(m_iSocket, 5)<0)
	{ 
		return -1; 
	}

	return 0;
}

int tcp_listener::accept(tcp_socket& tcp, long iTimeout)
{
	/* 设定的定时时间 */
	struct timeval	sTimeout;
	sTimeout.tv_sec = iTimeout;
	sTimeout.tv_usec = 0;

	fd_set iRset;
	FD_ZERO(&iRset);
	FD_SET(m_iSocket,&iRset);
	
	int fd = ::select(m_iSocket+1,&iRset,NULL,NULL,&sTimeout);
	if(fd < 0)
		return -1;
	
	
	/*工作端口监测，是否有工作端口请求到来*/
	if(FD_ISSET(m_iSocket,&iRset)) {
		#if defined(_AIX) || defined(__linux__) || defined(__sun__)
			socklen_t iNamelen;
		#else
			int iNamelen;
		#endif
		SOCKET so(INVALID_SOCKET);
		struct sockaddr_in addr;
		iNamelen = sizeof(addr);
		so = ::accept(m_iSocket, reinterpret_cast<struct sockaddr *>(&addr) ,&iNamelen);
		tcp.set_addr(addr);
		tcp.set_socket(so);
        return 1;
	}
	return 0;
	
}


int tcp_listener::close(void)
{
	::closesocket(m_iSocket);
	m_iSocket = INVALID_SOCKET;
	return 0;
}

tcp_listener::~tcp_listener(void)
{	
	if( m_iSocket != INVALID_SOCKET)
	::closesocket(m_iSocket);
}





tcp_socket::tcp_socket(const tcp_socket &so)
:m_autoclose(true)
{
	m_iSocket = so.get_socket();
	m_sAddr = so.get_addr();
}

tcp_socket& tcp_socket::operator = (const tcp_socket &so)
{
	if( this != &so ) {
		if( m_autoclose )
			tcp_socket::close();
		m_iSocket = so.get_socket();
		m_sAddr = so.get_addr();
	}
	return *this;
}

} // namespace archy
