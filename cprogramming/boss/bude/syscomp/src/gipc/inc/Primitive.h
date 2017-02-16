#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

#include "Object.h"

class Session;

/**
 * \class Primitive Primitive.h
 * \brief 抽象的同步原语
 * 同步原语基本类
 * waitFor，reset方法的语义有相应的特定原语描述
 */

class  Primitive { 
  public:
  
    /**
     * 原语类型
     */
    enum PrimitiveType { 
	 IPC_MUTEX, 
	 IPC_EVENT, 
	 IPC_SEMAPHORE, 
	 IPC_BARRIER, 
	 IPC_LOCK, 
	 IPC_SHARED_MEMORY, 
	 IPC_QUEUE 
    };


    enum Ranks { 
        DEFAULT_RANK = 0
    };

    /**
     * 得到原语类型
     */
    virtual PrimitiveType getType() = 0;

   /**
    * 取得类型名
    */       
    char const* getTypeName(); 

    /**
     * 获取原语名. 同类原语名必须唯一，不同原语类型可以有相同的名
     */
    virtual char const* getName() = 0;
    

   /**
    * 该原语是否存在
    */ 
    virtual bool alreadyExists() = 0;


    /**
     * 等待直到原语状态转换（由特定的原语规定语义）
     */
    void waitFor() { 
        priorityWait(DEFAULT_RANK);
    }


    /**
     * 等待直到原语状态转换或超时（由特定的原语规定语义）
     * \param timeout 超时设置(毫秒)
     * \return 超时返回 false
     */
    bool waitFor(unsigned timeout) { 
        return priorityWait(DEFAULT_RANK, timeout);
    }

    virtual void priorityWait(int rank) = 0;


    virtual bool priorityWait(int rank, unsigned timeout) = 0;


    /**
     * 原语复位/释放
     */
    virtual void reset() = 0;

    /**
     * 关闭原语 对原语引用计数实施减操作直到为零销毁该原语
     */
    virtual void close() = 0;

    /**
     * 为容器元素分配内存(queue,shared memory)
     * 该方法可以有继承类重写
     */
    virtual void* allocate(size_t size); 

    /**
     * 清楚操作
     */
    virtual void deallocate(void* obj, size_t size=0);

    static void deallocator(Object const& objDesc);

    virtual ~Primitive() {}
};

#endif    


