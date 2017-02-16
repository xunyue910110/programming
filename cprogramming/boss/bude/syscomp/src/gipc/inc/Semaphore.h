#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include "Primitive.h"


/**
 * \class Semaphore Semaphore.h
 * \brief 信号灯原语
 *
 * 信号灯操作 waitFor等到信号灯计数器大于零
 *            reset 信号灯计数器复位成零
 */
class Semaphore : public virtual Primitive { 
  public:
    /**
     * 信号灯计数器加操作 +count
     */
    virtual void signal(int count=1) = 0;
    /**
     * 返回原语类型
     */
    PrimitiveType getType() { 
	return IPC_SEMAPHORE;
    }
};

#endif

