#ifndef __LOCK_H__
#define __LOCK_H__

#include "Primitive.h"

/**
 * \class Lock  Lock.h
 * \brief 锁原语.设置独占或共享锁,该锁不支持嵌套
 *
 * waitFor 设置独占锁
 * reset 移除全部的锁
 */ 

class Lock : public virtual Primitive { 
  public:
    /**
     * 设置独占锁.其他回话不能再设置独占或共享.
     */       
    void exclusiveLock() { 
        priorityExclusiveLock(DEFAULT_RANK);
    }

    /**
     * 设置超时独占锁.其他回话不能再设置独占或共享.
     * \param timeout 超时(毫秒)
     */       
    bool exclusiveLock(unsigned timeout) { 
        return priorityExclusiveLock(DEFAULT_RANK, timeout);
    }

    /**
     * 设置共享锁
     */
    void sharedLock() { 
        prioritySharedLock(DEFAULT_RANK);
    }

    /**
     * 超时共享锁
     */
    bool sharedLock(unsigned timeout) { 
        return prioritySharedLock(DEFAULT_RANK, timeout);
    }

    virtual void priorityExclusiveLock(int rank) = 0;

    virtual bool priorityExclusiveLock(int rank, unsigned timeout) = 0;

    virtual void prioritySharedLock(int rank) = 0;

    virtual bool prioritySharedLock(int rank, unsigned timeout) = 0;

    /**
     * 解除全部锁
     */
    virtual void unlock() = 0;

    /**
     * 获得原语类型
     */
    PrimitiveType getType() { 
	return IPC_LOCK;
    }
};

#endif




