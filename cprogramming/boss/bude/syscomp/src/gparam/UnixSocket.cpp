#include "UnixSocket.h"
//__CYGWIN__ added by cy 2004.11.26
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__linux__) || defined(__CYGWIN__)
#include <sys/ioctl.h>
#else
#include <stropts.h>
#endif

#include <fcntl.h>
#include <sys/time.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#if defined(_AIX)
#include <strings.h>
#endif  /* _AIX */
#include <stddef.h>
#include <assert.h>
#include <errno.h>

extern "C" {
#include <netdb.h>
}


//设置忽略SIGPIPE信号
///* write on a pipe with no one to read it */
class UnixSocketLibrary {
  public:
    UnixSocketLibrary() {
            static struct sigaction sigpipe_ignore;
            sigpipe_ignore.sa_handler = SIG_IGN;
            sigaction(SIGPIPE, &sigpipe_ignore, NULL);
    }
};

static UnixSocketLibrary unisock_lib;


bool UnixSocket::open(char const* hostname, int port, int listen_queue_size)
{
    sockaddr_in sock_inet;
    sock_inet.sin_family = AF_INET;
    if (hostname != NULL && *hostname != '\0' && strcmp(hostname, "localhost") != 0) {
        struct hostent* hp;  // entry in hosts table
        if ((hp = gethostbyname(hostname)) == NULL
            || hp->h_addrtype != AF_INET)
        {
            errcode = bad_address;
            return false;
        }
        memcpy(&sock_inet.sin_addr, hp->h_addr,
               sizeof sock_inet.sin_addr);
    } else {
        sock_inet.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    
    sock_inet.sin_port = htons(port);
    if ((fd = socket(sock_inet.sin_family, SOCK_STREAM, 0)) < 0) {
        errcode = errno;
        return false;
    }
    int on = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof on);

    if (bind(fd, (sockaddr*)&sock_inet, sizeof(sockaddr_in)) < 0) {
        errcode = errno;
        ::close(fd);
        return false;
    }
    
    if (listen(fd, listen_queue_size) < 0) {
        errcode = errno;
        ::close(fd);
        return false;
    }
    errcode = ok;
    state = ss_open;
    return true;
}

char* UnixSocket::get_peer_name()
{
    if (state != ss_open) {
        errcode = not_opened;
        return NULL;
    }
    struct sockaddr_in insock;

//socklen_t 兼容类型
#if defined(__linux__) || (defined(__FreeBSD__) && __FreeBSD__ > 3) || defined(__hpux__) || defined(_AIX43) || defined(__OpenBSD__)
    socklen_t len = sizeof(insock);
#elif defined(_AIX41)
    size_t len = sizeof(insock);
#else
    int len = sizeof(insock);
#endif
    if (getpeername(fd, (struct sockaddr*)&insock, &len) != 0) {
        errcode = errno;
        return NULL;
    }
    char* addr = inet_ntoa(insock.sin_addr);
    if (addr == NULL) {
        errcode = errno;
        return NULL;
    }
    char* addr_copy = new char[strlen(addr)+1];
    strcpy(addr_copy, addr);
    errcode = ok;
    return addr_copy;
}

bool  UnixSocket::is_ok()
{
    return errcode == ok;
}

//返回错误描述
void UnixSocket::get_error_text(char* buf, size_t buf_size)
{
    char* msg;
    switch(errcode) {
      case ok:
        msg = (char *)"ok";
        break;
      case not_opened:
        msg = (char *)"socket not opened";
        break;
      case bad_address:
        msg = (char *)"bad address";
        break;
      case connection_failed:
        msg = (char *)"exceed limit of attempts of connection to server";
        break;
      case broken_pipe:
        msg = (char *)"connection is broken";
        break;
      case invalid_access_mode:
        msg = (char *)"invalid access mode";
        break;
      default:
//#ifdef UNICODE 
//        mbstowcs(buf, strerror(errcode), buf_size);
//       return;
//#else
        msg = strerror(errcode);
//#endif
    }
    memset(buf,0,buf_size);
    strncpy(buf, msg, buf_size);
}


Socket* UnixSocket::accept()
{
    int s;

    if (state != ss_open) {
        errcode = not_opened;
        return NULL;
    }

    while((s = ::accept(fd, NULL, NULL )) < 0 && errno == EINTR);

    if (s < 0) {
        errcode = errno;
        return NULL;
    } else if (state != ss_open) {
        errcode = not_opened;
        return NULL;
    } else {
        static struct linger l = {1, LINGER_TIME};
        int enabled = 1;
        if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&enabled,
                       sizeof enabled) != 0)
        {
            errcode = errno;
            ::close(s);
            return NULL;
        }
        if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof l) != 0) {
            errcode = errno;
            ::close(s);
            return NULL;
        }
        errcode = ok;
        return new UnixSocket(s);
    }
}


bool UnixSocket::connect(char const* hostname, int port, int max_attempts, time_t timeout)
{
    sockaddr_in sock_inet;
    struct hostent* hp;  // entry in hosts table
    
    int rc;

    if ((hp=gethostbyname(hostname)) == NULL || hp->h_addrtype != AF_INET)
    {
        errcode = bad_address;
        return false;
    }
    sock_inet.sin_family = AF_INET;
    sock_inet.sin_port = htons(port);
    
    while (true) {
        for (int i = 0; hp->h_addr_list[i] != NULL; i++) 
        {
            memcpy(&sock_inet.sin_addr, hp->h_addr_list[i],sizeof sock_inet.sin_addr);
            
            if ((fd = socket(sock_inet.sin_family, SOCK_STREAM, 0)) < 0) {
                errcode = errno;
                return false;
            }
            do {
                rc = ::connect(fd,(sockaddr*)&sock_inet, sizeof(sock_inet));
            } while (rc < 0 && errno == EINTR);
            
            if (rc < 0) {
                errcode = errno;
                ::close(fd);
                if (errcode != ENOENT && errcode != ECONNREFUSED) {
                    return false;
                }
            } else {
                int enabled = 1;
                static struct linger l = {1, LINGER_TIME};
                if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,
                               (char*)&enabled, sizeof enabled) != 0)
                {
                    errcode = errno;
                    ::close(fd);
                    return false;
                }
                if (setsockopt(fd, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof l) != 0) { 
                    errcode = errno;
                    ::close(fd);
                    return false; 
                }
                errcode = ok;
                state = ss_open;
                return true;
            }
        }
        if (--max_attempts > 0) {
            sleep(timeout);
        } else {
            break;
        }
    }
    errcode = connection_failed;
    return false;
}


int UnixSocket::read(void* buf, size_t min_size, size_t max_size)
{
    char *ptr;  
    size_t size = 0;
    if (state != ss_open) {
        errcode = not_opened;
        return -1;
    }
    do {
        ssize_t rc;
        ptr = (char*)buf;
        while ((rc = ::read(fd, ptr + size, max_size - size)) < 0
               && errno == EINTR);
        if (rc < 0) {
            errcode = errno;
            return -1;
        } else if (rc == 0) {
            errcode = broken_pipe;
            return -1;
        } else {
            size += rc;
        }
    } while (size < min_size);

    return (int)size;
}

int UnixSocket::readline(void *buf, int maxlen)
{
	int n,rc;
	char c,*ptr;    
	
	if (state != ss_open) {
        errcode = not_opened;
        return -1;
    }
    
	errcode = 0;
	ptr=(char *)buf; 	
	for(n=1;n<maxlen;n++)
	{
		if((rc=::read(fd,&c,1))==1) {
			*ptr++=c;     
			if(c=='\n') { 	
				break;
			}
			// newline is stored,like fgets() 
		} else if(rc==0) {
			if(n==1)
				return(0); /* EOF,no data read */
			else
				break;/* EOF,some data was read*/
		} else {
			errcode = errno;
			return(-1);/* error,errno set by read()*/
		}
	}
	*ptr=0; 

	return(n);
}  

bool UnixSocket::write(void const* buf, size_t size)
{
    if (state != ss_open) {
        errcode = not_opened;
        return false;
    }  
    do {
        ssize_t rc;
        while ((rc = ::write(fd, buf, size)) < 0 && errno == EINTR);
        if (rc < 0) {
            errcode = errno;
            return false;
        } else if (rc == 0) {
            errcode = broken_pipe;
            return false;
        } else {
            buf = (char*)buf + rc;
            size -= rc;
        }
    } while (size != 0);



    //
    // errcode is not assigned 'ok' value beacuse write function
    // can be called in parallel with other socket operations, so
    // we want to preserve old error code here.
    //
    return true;
}


bool UnixSocket::close()
{
    if (state != ss_close) {
        state = ss_close;
        if (::close(fd) == 0) {
            errcode = ok;
            return true;
        } else {
            errcode = errno;
            return false;
        }
    }
    errcode = ok;
    return true;
}

bool UnixSocket::shutdown()
{
    if (state == ss_open) 
    {
        state = ss_shutdown;
        int rc = ::shutdown(fd, 2);
        if (rc != 0) 
        {
            errcode = errno;
            return false;
        }
    }
    return true;
}

UnixSocket::~UnixSocket()
{
    close();
}


UnixSocket::UnixSocket(int new_fd)
{
    fd = new_fd;
    state = ss_open;
    errcode = ok;
}

Socket* Socket::create(char const* hostname, int port, int listen_queue_size)
{
    UnixSocket* sock = new UnixSocket();
    sock->open(hostname, port, listen_queue_size);
    return sock;
}

Socket* Socket::connect(char const* hostname, int port,
                            int max_attempts,
                            time_t timeout)
{
    UnixSocket* sock = new UnixSocket();
    if ( sock->connect(hostname, port, max_attempts, timeout)==false )
    {
        delete sock;
        //cout<<" Socket::connect error"<<endl;
        sock=0;
    }
    return sock;
}


