// Mutex.cpp: implementation of the CMutex class.
//
//////////////////////////////////////////////////////////////////////

#include "mwci/CMutex.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMutex::CMutex()
{
m_IsReady=false;
#ifdef _WIN32

	if((m_mut = CreateMutex(NULL, FALSE, NULL))!=NULL)
		m_IsReady=true;
#else
	if(0 == pthread_mutex_init(&m_mut, NULL))
		m_IsReady=true;
#endif
}

CMutex::~CMutex()
{
if(m_IsReady)
	{
	m_IsReady=false;
	#ifdef _WIN32
		CloseHandle(m_mut);
	#else
		pthread_mutex_destroy(&m_mut);
	#endif
	}
}

bool CMutex::wait()
{
if(!m_IsReady)
	return false;

///我们等呀等,等到出错,或者获取了信号量
#ifdef _WIN32
	if(WaitForSingleObject( m_mut, INFINITE )==WAIT_FAILED)
		return false;
#else
	if(0 != pthread_mutex_lock(&m_mut))
		return false;
#endif
return true;
}

bool CMutex::release()
{
if(!m_IsReady)
	return false;

///没有什么了,就是释放了而已.
#ifdef _WIN32
	if(ReleaseMutex(m_mut)==0)
		return false;
#else
	if(0 != pthread_mutex_unlock(&m_mut))
		return false;
#endif
return true;
}
