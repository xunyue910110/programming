#include "IpcLocal.h"
#include "IpcShm.h"

using namespace budeipc;

/**
 *  每个实例仅维护一个共享内存段，每个共享内存段上仅维护
 *  一个排他锁和多个（可限定、可不限定）共享锁
 *  加锁失败立即返回
 *
 *  锁控制在进程内实现
 */
class IpcShmImpl: public IpcShm {
public:
	//! 构造函数
	explicit IpcShmImpl(key_t key, int lockid):m_key(key), m_size(0)
	{
		if ((m_shmid = shmget(key, 0, 0)) == -1) {
			IPCTHROW_C(EIpcException, errno, "访问共享内存段错误 ");
		}
		
		struct shmid_ds _buf;
		if (shmctl(m_shmid, IPC_STAT, &_buf) == -1) {
			IPCTHROW_C(EIpcException, errno, "读取共享内存段信息错误 ");
		}
		m_size = _buf.shm_segsz;
		
		if ((m_pbuf = (char*)shmat(m_shmid, 0, 0)) == (char*)-1) {
			IPCTHROW_C(EIpcException, errno, "映射共享内存段错误 ");
		}
	}

	//! 析构函数	
	virtual ~IpcShmImpl() {
		shmdt(m_pbuf);
	}
	
	//! 元数据访问
	virtual char* getbuf() { return m_pbuf; }
	
	virtual long getkey() { return m_key; }
	
	virtual long getipcid() { return m_shmid; }
	
	virtual int getsize() { return m_size; }
	
	/**
	 *  原子操作：保证操作一致性
	 *  访问、挂接、引用共享内存段，加排他锁；
	 *  写数据;
	 *  解锁，释放共享内存段的引用
	 */
	virtual int write(const char* inbuf, int buflen)
	{
		char* buf = m_pbuf;
		memset(buf, 0, m_size);
		memcpy(buf, inbuf, buflen);
		
		return buflen;
	}
	
	/**
	 *  原子操作：保证操作一致性
	 *  访问、挂接、引用共享内存段，加共享锁；
	 *  读数据；
	 *  解锁，释放共享内存段的引用
	 */
	virtual int read(char* outbuf, int buflen)
	{
		char* buf = m_pbuf;
		memcpy(outbuf, buf, buflen);
		
		return 0;
	}
	
private:

	int m_key;
	
	int m_shmid;

	long m_size;
	
	char* m_pbuf;
	
};
	

/// ========================================================================
//! 创建共享内存段
int IpcShm::create(int key, int size)
{
	int  shmid;

	if ((shmid = shmget(key, size, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
		IPCTHROW_C(EIpcException, errno, "创建共享内存段错误 ");
	}
	
	return shmid;
}

//! 移除共享内存段
void IpcShm::remove(int key)
{
	int shmid;
	
	if ((shmid = shmget(key, 0, 0)) != -1) {
		shmctl(shmid, IPC_RMID, (struct shmid_ds*)0);	
	}
}

//! 判断指定key 之IPC共享内存段资源是否已经存在
int IpcShm::isExist(int key)
{
	int shmid;
	
	if ((shmid=shmget(key,10,IPC_CREAT|IPC_EXCL|0600)) == -1) {
		return 1;
	}
	shmctl(shmid, IPC_RMID, (struct shmid_ds*)0);
	
	return 0;
}

//! 获取一个共享内存访问接口
IpcShm* IpcShm::getShm(int key)
{
	return new IpcShmImpl(key, 1);
}

