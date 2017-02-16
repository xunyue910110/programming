#ifndef __SHARED_MEMORY_H__
#define __SHARED_MEMORY_H__

#include "Primitive.h"


/**
 * \class SharedMemory SharedMemory.h
 * \brief 共享内存服务原语 SharedMemory . 
 *
 * 该原语在服务端创建一段内存对象用于客户端存取
 * 数据.  放入共享内存的数据通过 Object 对象封装成共享数据实体.
 *
 */

class SharedMemory : public virtual Primitive { 
  public:

    /**
     * 向共享内存中存入数据对象 Object 
     */
    virtual void set(Object const& objDesc) = 0;

    /**
     * 向共享内存中存入数据,某一大小的数据 obj
     */
    virtual void set(void const* obj, size_t objSize) = 0; 

    /**
     * 从共享内存中取数据对象 Object 
     */
    virtual void get(Object& objDesc) = 0;

    inline void* get() { 
	Object objDesc;
	get(objDesc);
	return (void*)objDesc.data;
    }
    /**
     * 返回原语类型
     */
    PrimitiveType getType() { 
	return IPC_SHARED_MEMORY;
    }
};

#endif
