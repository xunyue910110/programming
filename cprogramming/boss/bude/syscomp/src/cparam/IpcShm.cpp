#include "IpcShm.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//#include "budeipc.h"
#include "IpcLocal.h"


namespace budeipc {
	
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
				IPCTHROW_C(IpcException, errno, "访问共享内存段错误 ");
			}
			
			struct shmid_ds _buf;
			if (shmctl(m_shmid, IPC_STAT, &_buf) == -1) {
				IPCTHROW_C(IpcException, errno, "读取共享内存段信息错误 ");
			}
			m_size = _buf.shm_segsz;
		}
	
		//! 析构函数	
		virtual ~IpcShmImpl() {
			///
		}
		
		//! 元数据访问
		virtual char* atbuf() 
		{ 
			char* _pbuf;
			
			if ((_pbuf = (char*)shmat(m_shmid, 0, 0)) == (char*)-1) {
				IPCTHROW_C(IpcException, errno, "映射共享内存段错误 ");
			}
			
			return _pbuf; 
		}
		
		virtual void dtbuf(char* _pbuf)
		{
			shmdt(_pbuf);	
		}
		
		virtual long getkey() { return m_key; }
		
		virtual long getipcid() { return m_shmid; }
		
		virtual int getsize() { return m_size; }
				
	private:
	
		int m_key;
		
		int m_shmid;
	
		long m_size;
		
	};
		
	
	/// ========================================================================
	//! 创建共享内存段
	int IpcShm::create(int key, int size)
	{
		int  shmid;
	
		if ((shmid = shmget(key, size, IPC_CREAT | IPC_EXCL | 0660)) == -1) {
			IPCTHROW_C(IpcException, errno, "创建共享内存段错误 ");
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
		
		if ((shmid=shmget(key,10,IPC_CREAT|IPC_EXCL|0660)) == -1) {
			return 1;
		}
		shmctl(shmid, IPC_RMID, (struct shmid_ds*)0);
		
		return 0;
	}
	
	//! 获取一个共享内存访问接口
	IpcShm& IpcShm::getShm(int key)
	{
		static IpcShmImpl stshm(key, 1);
		
		return stshm;
	}
	
} // namespace budeipc
