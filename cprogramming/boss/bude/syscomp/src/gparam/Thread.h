#ifndef __THREAD_H__
#define __THREAD_H__

#include "Config.h"

#define thread_proc

#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>




const size_t ThreadStackSize = 1024*1024; 


class Thread {
    pthread_t thread;

  public:
    typedef void (thread_proc* thread_proc_t)(void*);
    
    //启动线程
    //thread_proc_t f 线程调用的函数
    // function to be excuted by the newly created pthread.
    void start(thread_proc_t f, void* arg) 
    {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
#if !defined(__linux__)
        pthread_attr_setstacksize(&attr, ThreadStackSize);
#endif
#if defined(_AIX41)
        // At AIX 4.1, 4.2 threads are by default created detached
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_UNDETACHED);
#endif
        //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//added by cy 2004.03.24
        pthread_create(&thread, &attr, (void*(*)(void*))f, arg);
        pthread_attr_destroy(&attr);
    }

    enum ThreadPriority {
        THR_PRI_LOW,
        THR_PRI_HIGH
    };

    //设置线程优先级别
    void setPriority(ThreadPriority pri) {
#if defined(PRI_OTHER_MIN) && defined(PRI_OTHER_MAX)
        struct sched_param sp;
        sp.sched_priority = pri == THR_PRI_LOW ? IPRI_OTHER_MIN : PRI_OTHER_MAX;
        pthread_setschedparam(thread, SCHED_OTHER, &sp);
#endif
    }
   
    //等待线程终止
    void join() {
        void* result;
        pthread_join(thread, &result);
    }

    //强制回收线程占用的系统资源
    void detach() {
        pthread_detach(thread);
    }
};

//线程范围的公共变量
class CurrentThread {
  public:
    static void  setThreadData(void* obj) {
        pthread_setspecific(key, obj);
    }
    static void* getThreadData() {
        return pthread_getspecific(key);
    }
    CurrentThread() {
        pthread_key_create(&key, NULL);
    }

    ~CurrentThread() {
        pthread_key_delete(key);
    }
  private:
    static pthread_key_t key;
};

/*-----------------------------------------------------------------------------
 */

//代码临界区定义类
class CriticalSection {
  public:
    CriticalSection() {
        pthread_mutex_init(&mutex, NULL);
    }
    ~CriticalSection() {
        pthread_mutex_destroy(&mutex);
    }
    
    //进入临界区
    void enter() {
        pthread_mutex_lock(&mutex); 
    }
    
    //临界区离开
    void leave() {
        pthread_mutex_unlock(&mutex);
    }
  protected:
    pthread_mutex_t mutex;
};


//临界区自动管理类。
//一般声明为本地变量 对变量作用域范围内加锁
class CriticalSectionTrigger { 
    CriticalSection& cs;
  public:
    CriticalSectionTrigger(CriticalSection& aCS) 
      : cs(aCS)    
    {
        cs.enter();
    }
    ~CriticalSectionTrigger() { 
        cs.leave();
    }
};

#define RIPC_CS(cs) CriticalSectionTrigger __RIPCCS(cs)



#endif




