//////////////////////////////////////////////////////////////////////
// 
// 类名: thread
// 描述: 线程封装类
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 
// 版本: V1.0.0 
// 版权: 南京联创科技股份有限公司
// 1.0.1 在构造函数中添加pthread_setconcurrency
//////////////////////////////////////////////////////////////////////
#include "thread.hpp"
#include "debug.hpp"

#if defined(WIN32)
#ifndef _MT
#define _MT
#endif
  #include <windows.h>
  #include <process.h>
// extern "C" unsigned long _beginthreadex( void *security, unsigned stack_size,
// unsigned ( __stdcall *start_address )( void * ), void *arglist,
// unsigned initflag, unsigned *thrdaddr );
#endif

#include "debug.hpp"
#include "utils.hpp"

namespace archy {

#if defined(WIN32)
unsigned __stdcall thread_proxy(void* param)
#else
void* thread_proxy(void* param)
#endif
{
	try
	{
	    thread* th = static_cast<thread*>(param);
		
		th->m_mutex.do_lock();
		th->terminated = false;
		th->m_mutex.do_unlock();
		th->initial();
		th->run();
		th->final();
		th->m_mutex.do_lock();
		th->terminated = true;
		th->m_mutex.do_unlock();
		if( th->free_onterminate ) {
			th->detach();
			delete th;
		}
	}
	catch (...)
	{
        ARCHY_TRACE("thread %d found exception and exiting...\n", getCurrentThreadId());
        abort();
	}
	return 0;
}



thread::thread(bool freeOnTerminate/* = false */,
	int stack /*= 0*/, int pri/* = 0*/)
    : terminated(false), m_joinable(true)
#if defined(WIN32)
   ,m_thread(0)
#else
   ,m_thread(0)
#endif  
   ,m_pri(pri)
   ,m_stack(stack)
   ,free_onterminate(freeOnTerminate)
{
#ifndef WIN32

#ifdef OS_SUN
	pthread_setconcurrency(100);
#endif // OS_SUN

	pthread_attr_init(&m_attr);
	if(stack) {
  #ifdef	PTHREAD_STACK_MIN
		pthread_attr_setstacksize(&m_attr, 
			stack <= PTHREAD_STACK_MIN ? PTHREAD_STACK_MIN : stack);
  #else
		pthread_attr_setstacksize(&m_attr, stack);
  #endif
	}
#endif
}

bool thread::running()
{
	return !terminated;
}

void thread::terminate()
{
	scoped_lock lock(m_mutex);
	terminated = true;
}

void thread::start()throw (thread_resource_error)
{
#if defined(WIN32)
    m_thread = reinterpret_cast<void*>(_beginthreadex(0, m_stack, &thread_proxy,
                                           this, 0, &m_id));
    if (!m_thread)
        throw thread_resource_error();
#else
    int res = 0;
    res = pthread_create(&m_thread, &m_attr, &thread_proxy, this);
    if (res != 0) {
        fprintf(stderr, "pthread_create:%d\n", res);
        throw thread_resource_error();
    }
#endif
}

void thread::detach()
{
    if (m_joinable)
    {
#if defined(WIN32)
        int res = 0;
        res = CloseHandle(reinterpret_cast<HANDLE>(m_thread));
        ARCHY_ASSERT(res);
#else
        pthread_detach(m_thread);
#endif
	m_joinable = false;
    }
	
}

thread::~thread()
{
	if( !terminated) {
	     this->detach();
#if defined(ARCHY_HAS_PTHREADS)	
	 pthread_attr_destroy(&m_attr);
#endif 
	}
}

bool thread::operator==(const thread& other) const
{
#if defined(WIN32)
    return other.m_id == m_id;
#else
    return pthread_equal(m_thread, other.m_thread) != 0;
#endif
}

bool thread::operator!=(const thread& other) const
{
    return !operator==(other);
}

void thread::join()
{
    int res = 0;
    if( m_joinable ) {
	#if defined(WIN32)
	    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_thread), INFINITE);
	    ARCHY_ASSERT(res == WAIT_OBJECT_0);
	    res = CloseHandle(reinterpret_cast<HANDLE>(m_thread));
	    ARCHY_ASSERT(res);
	#else
	    res = pthread_join(m_thread, 0);
	    ARCHY_ASSERT(res == 0);
	#endif
	    // This isn't a race condition since any race that could occur would
	    // have us in undefined behavior territory any way.
	    m_joinable = false;
    }
}


} // namespace archy

