// 描述: 线程封装类
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 
// 版本: V1.0.0 
// 版权: 南京联创科技股份有限公司
//////////////////////////////////////////////////////////////////////

#ifndef ARCHY_THREAD_HPP_HEADER
#define ARCHY_THREAD_HPP_HEADER
#include <list>
#include "mutex.hpp"
#include "noncopyable.hpp"
#include "thread_exceptions.hpp"

#ifndef WIN32
#   include <pthread.h>
#   include "condition.hpp"
#endif

namespace archy{

extern "C"
#if defined(WIN32)
    unsigned __stdcall thread_proxy(void* param);
#else
    void* thread_proxy(void* param);
#endif


struct xtime;

class thread : private noncopyable
{
friend 
#if defined(WIN32)
    unsigned __stdcall thread_proxy(void* param)
#else
    void* thread_proxy(void* param)
#endif
    ;

protected:
    bool terminated;
    bool free_onterminate;

    virtual void run() = 0;
    virtual void final(){}
    virtual void initial(){}
public:
    // stack有实现，pri没有实现
    thread(bool freeOnTerminate = false, int stack = 0, int pri = 0);
    virtual ~thread();

    bool operator==(const thread& other) const;
    bool operator!=(const thread& other) const;
    
 
    void start()throw (thread_resource_error);
    void terminate();
    void detach();
    bool running(); // query whether the thread is in running or not
    void join();

    static void sleep(const xtime& xt);
    static void sleep(int seconds);
    static void yield();

private:
    int m_pri;
    int m_stack;
    
#if defined(WIN32)
    void* m_thread;
    unsigned int m_id;
#else
    pthread_t m_thread;
    pthread_attr_t m_attr;

#endif

    mutex m_mutex;
    bool m_joinable;
};


} // namespace archy


#endif // ARCHY_THREAD_HPP_HEADER
