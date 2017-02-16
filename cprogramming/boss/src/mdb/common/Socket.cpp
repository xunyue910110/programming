//  ############################################
//  Source file : Socket.cpp
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-5-28 14:43:30
//  Update      : 2008-5-28 14:43:32
//  Copyright(C): chen min, Linkage.
//  ############################################

#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Socket.h"
#include "Mdb_Exception.h"

//##ModelId=483BB6E70148
int Socket::read(void* buf, int min_size, int max_size)
{
    if( m_fp == 0) 
        throw Mdb_Exception(__FILE__, __LINE__, "socket尚未建立连接!");	

    char *ptr;  
    int size = 0;
    while (size < min_size)
    {
        int rc;
        ptr = (char*)buf;
        while ((rc = ::read(m_fp, ptr + size, max_size - size)) < 0
               && errno == EINTR);
        if (rc <= 0) 
        {
        	throw Mdb_Exception(__FILE__, __LINE__, "套接字read错: %s",strerror(errno) );
        } 
        else 
        {
            size += rc;
        }
    } 

    return (int)size;
}

//##ModelId=483BB6EA02DE
bool Socket::write(void const* buf, size_t size)
{
    if( m_fp == 0) 
        throw Mdb_Exception(__FILE__, __LINE__, "socket尚未建立连接!");	
 
    do 
   	{
        ssize_t rc;
        while ((rc = ::write(m_fp, buf, size)) < 0 && errno == EINTR);
        if (rc <= 0) 
        {
            throw Mdb_Exception(__FILE__, __LINE__, "套接字write错：%s",strerror(errno));	
        } 
        else
        {
            buf = (char*)buf + rc;
            size -= rc;
        }
    } while (size != 0);

    return true;	
}

//##ModelId=483BB6F4000F
int Socket::readLine(void *buf, int maxlen)
{
	int n,rc;
	char c,*ptr;    
	
    if( m_fp == 0) 
        throw Mdb_Exception(__FILE__, __LINE__, "socket尚未建立连接!");	
    
	ptr=(char *)buf; 	
	for(n=1;n<maxlen;n++)
	{
		if((rc=::read(m_fp,&c,1))==1) 
		{
			*ptr++=c;     
			if(c=='\n') 
			{ 	
				break;
			}
			// newline is stored,like fgets() 
		} 
		else if(rc==0) 
		{
			if(n==1)
				return(0); /* EOF,no data read */
			else
				break;/* EOF,some data was read*/
		} 
		else 
		{
			throw Mdb_Exception(__FILE__, __LINE__, "套接字read错:%s",strerror(errno));
		}
	}
	*ptr=0; 

	return(n);	
}

//##ModelId=483BC065005D
bool Socket::startUpListen(char const* hostname, int port)
{
	if( m_fp!=0 )
		throw Mdb_Exception(__FILE__, __LINE__, "socket建立的连接尚未断开!");	
	
	sockaddr_in sock_inet;
    sock_inet.sin_family = AF_INET;
    if (hostname != NULL && *hostname != '\0' && strcmp(hostname, "localhost") != 0) 
    {
        struct hostent* hp;  
        if ((hp = gethostbyname(hostname)) == NULL
            || hp->h_addrtype != AF_INET)
        {
            throw Mdb_Exception(__FILE__, __LINE__, "gethostbyname错误!");
        }
        memcpy(&sock_inet.sin_addr, hp->h_addr,
               sizeof sock_inet.sin_addr);
    } 
    else 
    {
        sock_inet.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    
    sock_inet.sin_port = htons(port);
    if ((m_fp = socket(sock_inet.sin_family, SOCK_STREAM, 0)) > 0) 
    {
	    int i = 1;
	    setsockopt(m_fp, SOL_SOCKET, SO_REUSEADDR, (char*)&i, sizeof(int));

    	if( (bind(m_fp, (sockaddr*)&sock_inet, sizeof(sockaddr_in)) < 0)
    		||  (listen(m_fp, MAX_LISTEN_QUEUE_SIZE) < 0)
    	  )
	    {
	    	this->close();
			throw Mdb_Exception(__FILE__, __LINE__, "bind或listen错误：%s",strerror(errno));	
	    }
	}    
	else
	{
		throw Mdb_Exception(__FILE__, __LINE__, "socket建立失败：%s",strerror(errno));	
	}
	
    return true;
}

//##ModelId=483BC09A03A9
Socket *Socket::accept()
{
    int s=0;

    if( m_fp == 0) 
        throw Mdb_Exception(__FILE__, __LINE__, "socket尚未建立连接!");	

    s = ::accept(m_fp, NULL, NULL ) ;

    if (s < 0) 
    {
    	if( errno == EINTR)
    		return NULL;
    	throw Mdb_Exception(__FILE__, __LINE__, "套接字accept错误：%s",strerror(errno));	
    }
    else 
    {
        static struct linger l = {1, LINGER_TIME};
        int enabled = 1;
        if( (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&enabled,sizeof enabled) != 0)
        	||(setsockopt(s, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof l) != 0)
          ) 
        {
            ::close(s);
            throw Mdb_Exception(__FILE__, __LINE__, "setsockopt错误：%s",strerror(errno));	
        }

        return new Socket(s);
    }
}

//##ModelId=483BC6B302AF
bool Socket::connect(char const* hostname, 
	                     const  int port )
{
	if( m_fp!=0 )
		throw Mdb_Exception(__FILE__, __LINE__, "socket建立的连接尚未断开!");	

	sockaddr_in sock_inet;
    struct hostent* hp;  
    int iConnectResult,iSetSockoptResult;

    if ((hp=gethostbyname(hostname)) == NULL || hp->h_addrtype != AF_INET)
    {
        throw Mdb_Exception(__FILE__, __LINE__, "gethostbyname错误!");
    }
    sock_inet.sin_family = AF_INET;
    sock_inet.sin_port = htons(port);
    
	memcpy(&sock_inet.sin_addr, hp->h_addr,sizeof sock_inet.sin_addr);
	
	if ((m_fp = ::socket(sock_inet.sin_family, SOCK_STREAM, 0)) > 0) 
	{
		iConnectResult = ::connect(m_fp,(sockaddr*)&sock_inet, sizeof(sock_inet));
	    if (iConnectResult == 0) 
	    {
	        int enabled = 1;
	        static struct linger l = {1, LINGER_TIME};
	        if( (setsockopt(m_fp, IPPROTO_TCP, TCP_NODELAY,
	                       (char*)&enabled, sizeof enabled) != 0)
	             ||(setsockopt(m_fp, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof l) != 0)
	             
	          )
		    {
				this->close();
				throw Mdb_Exception(__FILE__, __LINE__, "setsockopt错误：%s",strerror(errno));
		    }
	    } 
	    else 
	    {
			this->close();
			throw Mdb_Exception(__FILE__, __LINE__, "connect错误：%s",strerror(errno));
	    }
	}
	else
	{
		throw Mdb_Exception(__FILE__, __LINE__, "socket建立失败：%s",strerror(errno));	
	}

    return true;
}

//##ModelId=483BC6BF00BB
void Socket::get_peer_name(string &sAddr)
{
	if (m_fp == 0) 
	{
        throw Mdb_Exception(__FILE__, __LINE__, "socket尚未建立连接!");	
    }
    
    sockaddr_in insock;
    socklen_t len = sizeof(insock);// add by chenm 2009-5-21

#ifdef AIX
    if (getpeername(m_fp,  (struct sockaddr*)&insock, &len) != 0)
#else
    if (getpeername(m_fp,  (struct sockaddr*)&insock, (int *)&len) != 0) //add int * by chenzm for hp-ux
#endif
    {
        throw Mdb_Exception(__FILE__, __LINE__, "getpeername错!");	
    }
    char* cAddr = inet_ntoa(insock.sin_addr);
    if (cAddr == NULL) 
    {
        throw Mdb_Exception(__FILE__, __LINE__, "inet_ntoa错!");	
    }
    sAddr = cAddr;
    
    return;
}

bool Socket::close()
{
	if( m_fp != 0)
	{
    	if (::close(m_fp) != 0) 
    	{
			throw Mdb_Exception(__FILE__, __LINE__, "close 套接字失败:%s!",strerror(errno));		
        }
        m_fp = 0;
    }
    return true;
}

// 通知客户端,不要再发包了,但并不会释放m_fp,如果要释放,还是要调用close函数
bool Socket::shutdown()
{
    if (m_fp != 0) 
    {
        int rc = ::shutdown(m_fp, 2);
    }
    
    return true;
}
