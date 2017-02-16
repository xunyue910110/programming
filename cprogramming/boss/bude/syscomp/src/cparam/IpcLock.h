#ifndef __IPC_LOCK_H_
#define __IPC_LOCK_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

///
namespace budeipc {
	 
	/**
	 *  对一个资源提供一组锁（一个互斥锁、一个共享锁、一个排他锁），并由三个信号量实现：
	 *  互斥锁：请求锁，用于数据刷新进程同步;
	 *  共享锁：实现共享锁，用于数据读;
	 *  排他锁：实现排他锁，用于数据写;
	 *  
	 *  一个key 创建一个信号量集，一个信号量集实现一个锁池（多组锁）
	 */	
	
	/**
	 * class IpcLock definition
	 */	
	class IpcLock {
	public:

		/// semaphore create and remove
		static int create(int key, int lockcnt);
		static void remove(int key);
	
		/// 
		static int isExist(int key);
		static IpcLock& getLock(int key);
	
	
		/// following is IpcLock's interface
		
		//!
		virtual ~IpcLock() {}
		virtual int getLockId(int idx) = 0;
		virtual long getkey() = 0;
		virtual long getipcid() = 0;
		virtual int getLockcnt() = 0;
		
		// 申请锁资源：随机申请、或申请指定锁
		virtual int rqstLock() = 0;
		virtual void rqstLock(int lockid) = 0;	
					
		// 释放锁资源
		virtual void rlseLock(int lockid) = 0;
		
		// KILL 锁
		virtual void kill(int lockid) = 0;
		
		//! 指定锁状态输出
		virtual void status(int lockid) = 0;
		
		//! 锁池状态输出
		virtual void status() = 0;
			
		//! 排他锁加锁
		virtual int xlock(int lockid, int flag=0) = 0;
		
		//! 共享锁加锁
		virtual int slock(int lockid, int flag=0) = 0;
		
		//! 互斥锁加锁
		virtual int sxlock(int lockid, int flag=0) = 0;
	
		//! 排他锁解锁
		virtual int xunlock(int lockid) = 0;
		
		//! 共享锁解锁
		virtual int sunlock(int lockid) = 0;
		
		//! 互斥锁解锁
		virtual int sxunlock(int lockid) = 0;
	
		//! 判断排他锁是否加锁
		virtual int isxlock(int lockid) = 0;
	
		//! 判断共享锁是否加锁
		virtual int isslock(int lockid) = 0;
		
	};

} // namespace budeipc

#endif

