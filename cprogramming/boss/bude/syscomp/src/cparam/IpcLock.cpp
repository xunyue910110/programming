#include "IpcLock.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

//#include "budeipc.h"
#include "IpcLocal.h"

			  			  
namespace budeipc {

	/**
	 * semphores per lock 
	 */
	const int _SemCntPerLock = 4;

	///	semaphore union struct 
	union semun {
		int					val;
		struct semid_ds*	buf;
		ushort*				array;
	};
		

	/**
	 * class IpcLockImpll definition
	 */
	class IpcLockImpl : public IpcLock {
	public:
		
		//! 构造函数
		explicit IpcLockImpl(int key) : m_key(key), m_semid(0), m_semcnt(0)
		{
			if ((m_semid = semget(m_key, 0, 0)) == -1) {
				IPCTHROW_C(IpcException, errno, "访问信号量集错误 ");
			}
			
			struct semid_ds sem_buf;
			union semun arg;
			arg.buf = &sem_buf;
			if (semctl(m_semid, 0, IPC_STAT, arg) == -1) {
				IPCTHROW_C(IpcException, errno, "读取信号量集信息错误 ");
			}
			m_semcnt = sem_buf.sem_nsems;
		}		
	
		//! 析构函数
		virtual ~IpcLockImpl() {}
	
		//! 返回指定索引值对于的锁标识
		virtual int getLockId(int idx) { return (idx-1)*_SemCntPerLock + 1; }
		
		virtual long getkey() { return m_key; }
		
		virtual long getipcid() { return m_semid; }
		
		virtual int getLockcnt() { return (m_semcnt-1)/_SemCntPerLock; }
		
		
		/**
		 *  申占锁资源：随机占有一个锁资源，返回锁标识
		 *  成功返回0
		 *  异常：EAGAIN，已无锁资源并IPC_NOWAIT; 等;
		 *  r (第0个信号量，资源个数，计数信号量）
		 *  b, b（锁占用标志位，二元信号量）
		 *  ※ 锁资源请求位因有跨进程生效要求，故不能指定SEM_UNDO 标志！！！
		 */
		virtual int rqstLock()
		{
			struct sembuf LKCMD_HASLOCK = {0, -1, IPC_NOWAIT};
			struct sembuf LKCMD_RQST[2] = { {0, 0, IPC_NOWAIT}, {0, 1, 0} };
	
			if (semop(m_semid, &LKCMD_HASLOCK, 1) == -1) {
				IPCTHROW_C(IpcException, errno, "申请锁资源错误 ");
			}
			
			int i; /// i, 0, 1, 2, 3, 4, ... ; 1, 5, 9, 13, 17, ...;
			for (i=0; i<(m_semcnt-1)/_SemCntPerLock; i++) {  
				LKCMD_RQST[0].sem_num = 1+i*_SemCntPerLock;
				LKCMD_RQST[1].sem_num = 1+i*_SemCntPerLock;
				if (semop(m_semid, LKCMD_RQST, 2) != -1) {
					break;
				}
			}
			
			if (i == (m_semcnt-1)/_SemCntPerLock) 
				IPCTHROW(IpcException, "申请锁资源错误，没有空闲锁！ ");
			else
				return i+1;   // 占锁成功，返回锁id
		}
					
		//! 申请锁资源：占有指定锁标识对应的锁资源			
		virtual void rqstLock(int lockid)
		{
			struct sembuf LKCMD_HASLOCK = {0, -1, IPC_NOWAIT};
			struct sembuf LKCMD_RQST[2] = { {0, 0, IPC_NOWAIT}, {0, 1, 0} };
	
			if (semop(m_semid, &LKCMD_HASLOCK, 1) == -1) {
				IPCTHROW_C(IpcException, errno, "申请锁资源错误 ");
			}
			
			int __lockid = (lockid-1)*_SemCntPerLock+1;
			LKCMD_RQST[0].sem_num = __lockid;
			LKCMD_RQST[1].sem_num = __lockid;
			if (semop(m_semid, LKCMD_RQST, 2) == -1) {
				IPCTHROW_C(IpcException, errno, "申请锁资源错误 ");
			}
		}
					
		/**
		 *  释放锁资源: b, r 
		 *  成功返回0，失败返回-1（EAGAIN，已释放并IPC_NOWAIT）
		 *  ※ 锁资源请求位因有跨进程生效要求，故不能指定SEM_UNDO 标志！！！
		 */
		virtual void rlseLock(int lockid)
		{
			int __lockid = (lockid-1)*_SemCntPerLock+1;
			
			// 先初始化锁值再释放
			union semun arg;
			arg.val = 0;
			if (semctl(m_semid, __lockid+1, SETVAL, arg) == -1) {
				IPCTHROW_C(IpcException, errno, "释放锁资源错误 ");
			}
			if (semctl(m_semid, __lockid+2, SETVAL, arg) == -1) {
				IPCTHROW_C(IpcException, errno, "释放锁资源错误 ");
			}
			if (semctl(m_semid, __lockid+3, SETVAL, arg) == -1) {
				IPCTHROW_C(IpcException, errno, "释放锁资源错误 ");
			}
			
			struct sembuf LKCMD_RLSE[2] = { {0, -1, IPC_NOWAIT}, {0, 1, 0} };
			LKCMD_RLSE[0].sem_num = __lockid;
			LKCMD_RLSE[1].sem_num = 0;
			if (semop(m_semid, LKCMD_RLSE, 2) == -1) {
				IPCTHROW_C(IpcException, errno, "释放锁资源错误 ");
			}
		}
		
		//! KILL 锁
		virtual void kill(int lockid)
		{
			int __lockid = (lockid-1)*_SemCntPerLock+1;
			
			union semun arg;
			arg.val = 0;
			
			if (semctl(m_semid, __lockid+1, SETVAL, arg) == -1)  {
				IPCTHROW_C(IpcException, errno, "释放锁资源错误 ");
			}
			if (semctl(m_semid, __lockid+2, SETVAL, arg) == -1) {
				IPCTHROW_C(IpcException, errno, "KILL锁错误 ");
			}
			if (semctl(m_semid, __lockid+3, SETVAL, arg) == -1) {
				IPCTHROW_C(IpcException, errno, "KILL锁错误 ");
			}
		}		
		
		//! 指定锁状态输出
		virtual void status(int lockid)
		{
			int val=-999;
			int __lockid = (lockid-1)*_SemCntPerLock+1;
		
			printf("\tlck_id lck_flag x_lock s_lock xx_lock\n");
			printf("\t------ -------- ------ ------ -------\n");

			int ival0=-999, ival1=-999, ival2=-999, ival3=-999;
			ival0 = semctl(m_semid, __lockid, GETVAL, 0);
			ival1 = semctl(m_semid, __lockid+1, GETVAL, 0);
			ival2 = semctl(m_semid, __lockid+2, GETVAL, 0);
			ival3 = semctl(m_semid, __lockid+3, GETVAL, 0);

			printf("\t%6d %8d %6d %6d %7d \n", lockid, ival0, ival1, ival2, ival3);
		}
		
		//! 信号量集状态输出
		virtual void status()
		{
			int ival=-999, ival0=-999, ival1=-999, ival2=-999, ival3=-999;
			
			printf("\nsemid: %d, semkey: %d, semcnt: %d", m_semid, m_key, m_semcnt);
			
			ival = semctl(m_semid, 0, GETVAL, 0);
			printf(", remain_locks: %d.\n", ival);
			
			printf("\tlck_id lck_flag x_lock s_lock xx_lock\n");
			printf("\t------ -------- ------ ------ -------\n");
		
			for (int i=0; i<(m_semcnt/_SemCntPerLock); i++) {
				int x = i*_SemCntPerLock+1;

				ival0 = semctl(m_semid, x, GETVAL, 0);
				ival1 = semctl(m_semid, x+1, GETVAL, 0);
				ival2 = semctl(m_semid, x+2, GETVAL, 0);
				ival3 = semctl(m_semid, x+3, GETVAL, 0);

				printf("\t%6d %8d %6d %6d %7d \n", i+1, ival0, ival1, ival2, ival3);
			}
		}

		/**
		 *  排他锁加锁: x, x, s
		 *  成功返回0，失败返回-1（EAGAIN，有锁并IPC_NOWAIT）
		 *  异常：EINTR，进程被终止; EIDRM，信号量被删除; 等;
		 *  ※加锁时指定SEM_UNDO 标志，防止进程异常退出时产生死锁
		 */ 
		virtual int xlock(int lockid, int flag)
		{
			struct sembuf  LKCMD_XLOCK[3] = { {0, 0, 0}, {0, 1, SEM_UNDO}, {0, 0, 0} };
	
			int __lockid = (lockid-1)*_SemCntPerLock+1;
			
			LKCMD_XLOCK[0].sem_num = __lockid+1;	// 排他锁判断
			LKCMD_XLOCK[0].sem_flg = LKCMD_XLOCK[0].sem_flg | flag;
			LKCMD_XLOCK[1].sem_num = __lockid+1;	// 排他锁加锁
			LKCMD_XLOCK[2].sem_num = __lockid+2;	// 共享锁判断
			LKCMD_XLOCK[2].sem_flg = LKCMD_XLOCK[2].sem_flg | flag;
			
			if (semop(m_semid, LKCMD_XLOCK, 3) == -1) {
				if (errno == EAGAIN) {
					return -1;
				} else {
					IPCTHROW_C(IpcException, errno, "排他锁加锁错误 ");
				}
			}
			
			return 0;
		}				
		
		/**
		 *  共享锁加锁：x, s
		 *  成功返回0，失败返回-1（EAGAIN，有锁并IPC_NOWAIT）
		 *  异常：EINTR，进程被终止; EIDRM，信号量被删除; 等;
		 *  ※加锁时指定SEM_UNDO 标志，防止进程异常退出时产生死锁
		 */
		virtual int slock(int lockid, int flag)
		{
			struct sembuf LKCMD_SLOCK[2] = { {0, 0, 0}, {0, 1, SEM_UNDO} };
	
			int __lockid = (lockid-1)*_SemCntPerLock+1;
			
			LKCMD_SLOCK[0].sem_num = __lockid+1;		// 排他锁判断
			LKCMD_SLOCK[0].sem_flg = LKCMD_SLOCK[0].sem_flg | flag;
			LKCMD_SLOCK[1].sem_num = __lockid+2;		// 共享锁加锁
			
			if (semop(m_semid, LKCMD_SLOCK, 2) == -1) {
				if (errno == EAGAIN) {
					return -1;
				} else {
					IPCTHROW_C(IpcException, errno, "共享锁加锁错误");
				}
			}
			
			return 0;
		}		
	
		/**
		 *  互斥锁加锁：sx, sx
		 *  成功返回0，失败返回-1（EAGAIN，有锁并IPC_NOWAIT）
		 *  异常：EINTR，进程被终止; EIDRM，信号量被删除; 等;
		 *  ※加锁时指定SEM_UNDO 标志，防止进程异常退出时产生死锁
		 */
		virtual int sxlock(int lockid, int flag)
		{
			struct sembuf LKCMD_SXLOCK[2] = {{0, 0, 0}, {0, 1, SEM_UNDO} };
			
			int __lockid = (lockid-1)*_SemCntPerLock+1;
			
			LKCMD_SXLOCK[0].sem_num = __lockid+3;
			LKCMD_SXLOCK[0].sem_flg = LKCMD_SXLOCK[0].sem_flg | flag;
			LKCMD_SXLOCK[1].sem_num = __lockid+3;
			
			if (semop(m_semid, LKCMD_SXLOCK, 2) == -1) {
				if (errno == EAGAIN) {
					return -1;
				} else {
					IPCTHROW_C(IpcException, errno, "互斥锁加锁错误");
				}
			}
			
			return 0;
		}
		
		/**
		 *  排他锁解锁: x
		 *  成功返回0
		 *  异常：EAGAIN，锁已释放并IPC_NOWAIT
		 *  ※加锁时指定SEM_UNDO 标志，防止进程异常退出时产生死锁
		 */
		virtual int xunlock(int lockid)
		{
			struct sembuf LKCMD_UNLOCK = {0, -1, IPC_NOWAIT|SEM_UNDO};
			
			int __lockid = (lockid-1)*_SemCntPerLock+1;
			LKCMD_UNLOCK.sem_num = __lockid+1;	// 释放排他锁
			
			return semop(m_semid, &LKCMD_UNLOCK, 1);
		}
		
		/**
		 *  共享锁解锁: s
		 *  成功返回0
		 *  异常：EAGAIN，锁已释放并IPC_NOWAIT		 
		 *  ※加锁时指定SEM_UNDO 标志，防止进程异常退出时产生死锁
		 */
		virtual int sunlock(int lockid)
		{
			struct sembuf LKCMD_UNLOCK = {0, -1, IPC_NOWAIT|SEM_UNDO};
	
			int __lockid = (lockid-1)*_SemCntPerLock+1;
			LKCMD_UNLOCK.sem_num = __lockid+2;	// 释放共享锁
			
			return semop(m_semid, &LKCMD_UNLOCK, 1);
		}
		
		/**
		 *  共享锁解锁: sx
		 *  成功返回0
		 *  异常：EAGAIN，锁已释放并IPC_NOWAIT		 
		 *  ※加锁时指定SEM_UNDO 标志，防止进程异常退出时产生死锁
		 */
		virtual int sxunlock(int lockid)
		{
			struct sembuf LKCMD_UNLOCK = {0, -1, IPC_NOWAIT|SEM_UNDO};
			
			int __lockid = (lockid-1)*_SemCntPerLock+1;
			LKCMD_UNLOCK.sem_num = __lockid+3;  // 释放互斥锁
			
			return semop(m_semid, &LKCMD_UNLOCK, 1);
		}
		
		/**
		 *  判断排他锁是否加锁: x
		 *  成功: 0，未加锁; 1，已加锁;
		 *  异常：EINTR，进程被终止; EIDRM，信号量被删除; 等;
		 */
		virtual int isxlock(int lockid)
		{
			struct sembuf LKCMD_ISLOCK = {0, 0, IPC_NOWAIT};
	
			int __lockid = (lockid-1)*_SemCntPerLock+1;
			LKCMD_ISLOCK.sem_num = __lockid+1;	// 排他锁是否加锁
			
			if (semop(m_semid, &LKCMD_ISLOCK, 1) == -1) {
				if (errno == EAGAIN) {
					return 1;
				} else {
					IPCTHROW_C(IpcException, errno, "判排他锁错误");
				}
			}
			
			return 0;
		}
		
		/**
		 *  判断共享锁是否加锁: s
		 *  成功: 0，未加锁; 1，已加锁;
		 *  异常：EINTR，进程被终止; EIDRM，信号量被删除; 等;
		 */
		virtual int isslock(int lockid)
		{
			struct sembuf LKCMD_ISLOCK = {0, 0, IPC_NOWAIT};
	
			int __lockid = (lockid-1)*_SemCntPerLock+1;
			LKCMD_ISLOCK.sem_num = __lockid+2;	// 共享锁是否加锁
			
			if (semop(m_semid, &LKCMD_ISLOCK, 1) == -1) {
				if (errno == EAGAIN) {
					return 1;
				} else {
					IPCTHROW_C(IpcException, errno, "判共享锁错误");
				}
			}
			
			return 0;
		}
			
	private:
	
		int m_key;
		
		int m_semid;
		
		int m_semcnt;
		
	};
	
	
	/**
	 *  创建锁资源  3*lockcnt+1个信号量；第0个用于锁资源占用（可以表示个数）；
	 *  其后3个一组，表示一组锁（一个排他锁、一个共享锁、一个锁占用）；
	 *  初始化信号量，第0个信号量用于标识锁资源个数占用一个锁，该值减1，释放一个锁该值加1；
	 *  如锁资源已全部被占用（值为0），则申请不到新锁；
	 */
	int IpcLock::create(int key, int lockcnt)
	{
		int semid, semcnt = lockcnt*_SemCntPerLock + 1;
		
		if ((semid = semget(key, semcnt, IPC_CREAT | IPC_EXCL | 0660)) == -1) {
			IPCTHROW_C(IpcException, errno, "创建信号量集错误 ");
		}
	
		ushort* sem_init = new ushort[semcnt];
		sem_init[0] = lockcnt;
		//sem_init[1] = 1;	// 共享内存段头信息锁
		
		for (int i=1; i<semcnt; i++) {
			sem_init[i] = 0;
		}
		
		union semun arg;
		arg.array = sem_init;
		if (semctl(semid, 0, SETALL, arg) == -1) {
			semctl(semid, 0, IPC_RMID, (struct semid_ds*)0);
			delete [] sem_init;
	
			IPCTHROW_C(IpcException, errno, "初始化信号量集错误 ");
		}
		delete [] sem_init;
		
		return semid;
	}		
	
	//! 删除锁资源
	void IpcLock::remove(int key)
	{
		int semid;
		
		if ((semid = semget(key, 0, 0)) != -1) {
			semctl(semid, 0, IPC_RMID, (struct semid_ds*)0);
		}
	}
	
	//! 判断指定key 之IPC信号量集资源是否已存在
	int IpcLock::isExist(int key)
	{
		int semid;
		
		if ((semid=semget(key, 1, IPC_CREAT|IPC_EXCL|0660)) == -1) {
			return 1;
		}
		semctl(semid, 0, IPC_RMID, (struct semid_ds*)0);
			
		return 0;
	}
	
	//! 获取一个锁访问接口
	IpcLock& IpcLock::getLock(int key)
	{
		static IpcLockImpl stlock(key);
		
		return stlock;
	}

} // namespace budeipc
