#ifndef __SQUEUE_H__
#define __SQUEUE_H__

#include "Primitive.h"

/**
 * \class Queue Queue.h
 * \brief Queue 队列原语
 *
 * 队列原语. 提供回话间按照队列机制进行通信.
 *
 */  

class Queue : public virtual Primitive { 
  public:
    /**
     * 从队列中取元素
     */
    void get(Object& objDesc) {
        priorityGet(DEFAULT_RANK, objDesc);
    }

    /**
     * 取元素
     */
    void* get() { 
	return priorityGet(DEFAULT_RANK);
    }

    /**
     * 从队列中取元素，超时返回
     */
    bool get(Object& objDesc, unsigned timeout) { 
        return priorityGet(DEFAULT_RANK, objDesc, timeout);
    }

    void* get(unsigned timeout) {
        return priorityGet(DEFAULT_RANK, timeout);
    }
    
    virtual void priorityGet(int rank, Object& objDesc) = 0;

    void* priorityGet(int rank) { 
	Object objDesc;
	priorityGet(rank, objDesc);
	return (void*)objDesc.data;
    }

    virtual bool priorityGet(int rank, Object& objDesc, unsigned timeout) = 0;

    void* priorityGet(int rank, unsigned timeout) {
	Object objDesc;
	return priorityGet(rank, objDesc, timeout) ? (void*)objDesc.data : NULL;
    }
    
    /**
     * 向队列中存入元素
     */
    virtual void put(Object const& objDesc) = 0;

    /**
     * 向队列中存入指定大小的内存数据
     */
    virtual void put(void const* obj, size_t objSize) = 0;

    /**
     * 通过广播通形式写入数据
     */
    virtual void broadcast(Object const& objDesc) = 0;

    virtual void broadcast(void const* obj, size_t objSize) = 0;

    PrimitiveType getType() { 
	return IPC_QUEUE;
    }
};


#endif
