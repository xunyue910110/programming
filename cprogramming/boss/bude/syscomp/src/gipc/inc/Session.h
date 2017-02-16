#ifndef __SESSION_H__
#define __SESSION_H__

#include "Exception.h"
#include "Lock.h"
#include "Mutex.h"
#include "Queue.h"
#include "Barrier.h"
#include "Semaphore.h"
#include "SharedMemory.h"


/**
 * \class Session Session.h
 * \brief 会话类. 应用通过会话该对象创建,使用各种源于对象.
 *
 * 跨网络IPC会话 该会话可以是远程会话也可以是通过TCP/IPsocket连接的远程会话
 * 远程会话同步分布不同进程状态，对象。通过本机（客户端）stub将客户请求发送到服务端。
 * 本地会话同步统一进程的不同线程。
 *
 * 会话提供了创建同步对象的方法。
 * 
 * 这里仅对Session的功能点抽象 
 *
 */


class Session { 
   public:
    /**
     * 创建返回信号灯同步对象
     * initCount 是初始的计数值.
     */
    virtual Semaphore* createSemaphore(char const* name, int initCount)=0;
    virtual Semaphore* openSemaphore(char const* name)=0;
   

    /**
     * 创建返回互斥同步对象
     */
    virtual Mutex* createMutex(char const* name, bool locked)=0;
    virtual Mutex* openMutex(char const* name)=0;

    
    /**
     * 创建返回队列对象
     */
    virtual Queue* createQueue(char const* name)=0;
    virtual Queue* openQueue(char const* name)=0;



    /**
     * 创建返回共享内存同步对象
     */
    virtual SharedMemory* createSharedMemory(char const* name, Object const& objDesc)=0;
    virtual SharedMemory* createSharedMemory(char const* name, void const* obj, size_t objSize) = 0;
    virtual SharedMemory* openSharedMemory(char const* name)=0;
    

    /**
     * 创建返回锁对象
     */
    virtual Lock* createLock(char const* name)=0;
    virtual Lock* openLock(char const* name)=0;



    /**
     * 创建返回堵塞对象
     */
    virtual Barrier* createBarrier(char const* name, int nSessions)=0;
    virtual Barrier* openBarrier(char const* name)=0;


    /**
     * 关闭会话
     * 释放所有锁关闭打开的原语对象
     */
    virtual void close()=0;
    virtual void shutdownServer()=0;
    virtual void showServerInfo(char* buf, size_t bufSize)=0;


    /**
     * 取得会话ID标识
     * 唯一标识出服务端SessionImpl对象(全局唯一)
     */
    virtual void getSessionID(char* buf, size_t bufSize)=0; 


    /**
     * 检查是否为本地会话
     */
    virtual bool isLocal()=0;


    //连接是否正常(作为客户)，服务端始终OK 
    virtual bool isOk()=0;
    
    /**
     *  返回错误信息
     */
    virtual void getErrorText(char* buf, size_t bufSize)=0;

    virtual ~Session() {}
};

#endif





// -----------------------------------------------------------------------------
// 下列为 doxygen 自动化文档服务的部分


/**
 *  \mainpage GIPC 跨进程通信组件
 *  
 *  \section sec_intro 综述
 *  进程间通信（Interprocess Communication）即IPC，是指两个或两个以上进程以及两个或者两个以上线程之间进行通信联系。进程间通信组件包括一些服务封装，为应用者提供进程间共享系统资源，同步协作，协议通信，数据交互的功能，使客户端可以方便的在本地（服务端）或远程创建使用并管理这些原语，而不必关心他们的实现。
 *
 *  通过封装IPC底层调用的一般过程，向调用者提供几个固定的接口，使应用免于细碎的底层工作；另一方面便于系统之间的移植，达到最大限度的代码复用，提高应用开发速度。
 *
 *  该组件实现一些同步通信的标准原语如Semaphore、FIFO Queue、ShareMemory、Mutex、Lock、Barrier，主要提供以下几种常用的IPC机制手段：
 *
 *  的实现类图如下：
 *  \image HTML  relationships.jpg
 *
 *  \section sec_impl_intro 实现
 *  通过客户/服务的手段
 * 
 *  \section sec_apply  引用
 *  GIPC组件将以静态库(.a)形式发布，静态库名字为libgipc.a。使用GIPC组件的应用需要做以下工作：
 *  \arg 包含头文件gipc.h (#include "gipc.h")
 *  \arg 编译时连接libgipc.a ( -lgipc )
 *  
 *  其中gipc.h 中有几个参数会影响编译过程：
 * 
 */  



/**
 *  \example Test.h  
 *  测试应用依赖的头文件
 */


/**
 *  \example Barrier.Chat.cpp  
 *  应用Barrier通信原语实现队列广播样例程序
 */

/**
 *  \example Barrier.Chat.cpp  
 *  应用Barrier通信原语实现队列广播样例程序
 */
 
/**
 *  \example ExLock.Fifo.cpp
 *  联合独占锁实现队列功能 样例程序
 */
 
/**
 *  \example Mutex.Fifo.cpp
 *  联合Mutex互斥源于实现队列通信功能 样例程序
 */
 
/**
 *  \example Queue.Fifo.cpp
 *  应用Queue队列原语实现通信功能 样例程序
 */

/**
 *  \example Semaphore.Fifo.cpp
 *  联合Queue队列原语应用信号灯实现通信功能 样例程序
 */

/**
 *  \example SharedMemory.Fifo.cpp
 *  实现共享内存通信功能的 样例程序
 */

/**
 *  \example ShLock.Fifo.cpp
 *  锁原语Lock通信功能的 样例程序
 */
  
    