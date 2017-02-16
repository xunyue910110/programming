#ifndef __CRITICAL_SECTION_H__
#define __CRITICAL_SECTION_H__


#include <pthread.h>

//代码临界区定义类
class CriticalSection {
    friend class SessionImpl;
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

