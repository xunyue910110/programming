#ifndef __BARRIER_H__
#define __BARRIER_H__

#include "Primitive.h"


/**
 * \class Barrier Barrier.h
 * \brief 堵塞原语类
 *
 * 通过调用 \c waitFor 方法,回话线程被堵塞,直道到达指定数量等待回话.
 * 调用 \c reset 方法 唤醒全部的被堵塞回话.
 * 
 */


class Barrier : public virtual Primitive { 
    /**
     * 返回原语类型
     */
    PrimitiveType getType() { 
	return IPC_BARRIER;
    }
};

#endif
