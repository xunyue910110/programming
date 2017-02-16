
 
#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "Primitive.h"


/**
 * \class Mutex Mutex.h
 * \brief 互斥原语 
 * 同步互斥对象 waitFor 锁定互斥 reset 复位互斥对象
 */


class Mutex : public virtual Primitive { 
  public:
    /**
     * Lock mutex. Only one session can lock mutex each moment of time.
     * 锁互斥. 同一个时刻只能有一格回话锁定该互斥对象.
     */
    void lock() { 
        priorityLock(DEFAULT_RANK);
    }

    /**
     * 带有超时的锁互斥
     */
    bool lock(unsigned timeout) {
        return priorityLock(DEFAULT_RANK, timeout);
    }
       
    virtual void priorityLock(int rank) = 0;
    
    virtual bool priorityLock(int rank, unsigned timeout) = 0;

    /**
     * 释放该互斥
     */ 
    virtual void unlock() = 0;
    /**
     * 返回原语类型
     */
    PrimitiveType getType() { 
	return IPC_MUTEX;
    }
};

#endif
