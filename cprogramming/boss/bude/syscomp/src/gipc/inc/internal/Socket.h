#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <stddef.h>

#include <time.h>

//#define DEFAULT_CONNECT_MAX_ATTEMPTS 100
//#define DEFAULT_RECONNECT_TIMEOUT    1
//#define DEFAULT_LISTEN_QUEUE_SIZE    5
//#define LINGER_TIME                  10

//
// Abstract socket interface
// 抽象SOCKET接口类 
//

const int          DEFAULT_CONNECT_MAX_ATTEMPTS =100 ;
const int          DEFAULT_RECONNECT_TIMEOUT    =1   ;
const int          DEFAULT_LISTEN_QUEUE_SIZE    =5   ;
const int          LINGER_TIME                  =10  ;

class   Socket {
  public:
 
    bool              read(void* buf, size_t size) {
               return read(buf, size, size) == (int)size;
    }
    virtual int       read(void* buf, size_t min_size, size_t max_size) = 0;
    virtual bool      write(void const* buf, size_t size) = 0;

    virtual bool      is_ok() = 0;
    virtual void      get_error_text(char* buf, size_t buf_size) = 0;

    //
    // 该方法服务接受客户端连接时调用
    //
    virtual Socket*   accept() = 0;

    //
    // 阻止对socket进一步的读写操作
    //
    virtual bool      shutdown() = 0;
    
    //
    // 关闭socket
    //
    virtual bool      close() = 0;

    //
    // 取得socket对端名 new char[]创建  
    //
    virtual char*     get_peer_name() = 0;

    //
    // 创建连接到服务端socket
    // 
    static Socket*  connect(char const* host, int port,
           int max_attempts = DEFAULT_CONNECT_MAX_ATTEMPTS,
           time_t timeout = DEFAULT_RECONNECT_TIMEOUT);

    //
    // 服务端创建打开socket端口用于监听服务
    // @param host 主机地址
    // @param port 端口
    // @return 创建Socket指针
    // 
    static Socket*  create(char const* host, int port,
           int listen_queue_size = DEFAULT_LISTEN_QUEUE_SIZE);


    virtual ~Socket() {}
    Socket() { state = ss_close; }


    int   errcode;     // error code of last failed operation


  protected:
    enum { ss_open, ss_shutdown, ss_close } state;
};

#endif



