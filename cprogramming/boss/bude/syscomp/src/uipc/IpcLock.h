#ifndef __IPC_LOCK_H_
#define __IPC_LOCK_H_

#include <sys/sem.h>

namespace budeipc {
	 
/**
 *  对一个资源提供一组锁（一个共享锁、一个排他锁），并由三个信号量实现：
 *  锁标记：标记这组锁是否被占用，通过申请可以占用一组锁；
 *  排他锁：实现排他锁，用于资源更新；
 *  共享锁：实现共享锁，用于资源查询；
 *  
 *  一个key 创建一个信号量集，一个信号量集实现一个锁池（多组锁）
 */	
using namespace std;

const int _SemCntPerLock = 4;

class IpcLock;
typedef auto_ptr<IpcLock> PIpcLock;

class IpcLock {
	
public:

	union semun {
		int					val;
		struct semid_ds*	buf;
		ushort*				array;
	};
	
	/// 管理进程引用：创建锁池（信号量集、多组锁资源）、删除锁池（信号量集）
	static int create(int key, int lockcnt);
	static void remove(int key);


	/// 处理进程引用：锁池是否已经创建、获取一个访问接口
	static int isExist(int key);
	static IpcLock* getLock(int key);


	/// 以下为Lock 类接口
	
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
	virtual void xunlock(int lockid) = 0;
	
	//! 共享锁解锁
	virtual void sunlock(int lockid) = 0;
	
	//! 互斥锁解锁
	virtual void sxunlock(int lockid) = 0;

	//! 判断排他锁是否加锁
	virtual int isxlock(int lockid) = 0;

	//! 判断共享锁是否加锁
	virtual int isslock(int lockid) = 0;
	
};

} // namespace budeipc

#endif

