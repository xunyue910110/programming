// Mutex.h: interface for the CMutex class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _LINKAGE_MWCI_MUTEX_H_
#define _LINKAGE_MWCI_MUTEX_H_
#ifdef _WIN32
	#include <windows.h>
#else
	#include <pthread.h>
#endif
class CMutex  
{
public:
	///释放一个获得的信号量
	bool release();

	///请求获得一个信号量
	bool wait();
	
	///构造,创建信号量
	CMutex();
	
	///释放信号量
	virtual ~CMutex();
private:
	///标记是否成功构造
	volatile bool m_IsReady;
	
	///存放互斥信号量
	#ifdef _WIN32
		HANDLE m_mut;
	#else
		pthread_mutex_t m_mut ;
	#endif

};

#endif // #ifndef _LINKAGE_MWCI_MUTEX_H_
