// Copyright (C) 2001-2003
// William E. Kempf
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  William E. Kempf makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.
#include "xtime.hpp"
#include "mutex.hpp"
#include "debug.hpp"
#include "timeconv.inl"

#if defined(WIN32)
 #include <windows.h>
 #include <time.h>
#else
 #include <pthread.h>
 #include <errno.h>
#endif

#include <assert.h>

namespace archy {

#if defined(WIN32)

mutex::mutex(const char* mutexName, bool ctor)throw(thread_resource_error)
{
    m_mutex = reinterpret_cast<void*>(CreateMutex(0, 0, mutexName));
    if (!m_mutex)
        throw thread_resource_error();
}

mutex::~mutex()
{
    int res = 0;
    res = CloseHandle(reinterpret_cast<HANDLE>(m_mutex));
    assert(res);
}

void mutex::do_lock()
{
    int res = 0;
    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), INFINITE);

    assert(res == WAIT_OBJECT_0);
}

bool mutex::do_trylock()
{
    unsigned int res = 0;
    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), 0);
    assert(res != WAIT_FAILED && res != WAIT_ABANDONED);
    return res == WAIT_OBJECT_0;
}


void mutex::do_unlock()
{
    int res = 0;
    res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
    assert(res);
}

void mutex::do_lock(cv_state&)
{
    do_lock();
}

void mutex::do_unlock(cv_state&)
{
    do_unlock();
}
#else


mutex::mutex(const char* mutexName, bool ctor)throw(thread_resource_error)
{    
    if (!ctor)
    {
        return;
    }
    int res = 0;
	pthread_mutexattr_t mattr;

	if (mutexName != 0)
	{
		pthread_mutexattr_init(&mattr);
		pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);

	}

    res = pthread_mutex_init(&m_mutex, (mutexName != 0 ? (&mattr) : 0));

    if (res != 0)
        throw thread_resource_error();

	if (mutexName != 0)
	{
		pthread_mutexattr_destroy(&mattr);
	}
	
}

mutex::~mutex()
{
    int res = 0;
    res = pthread_mutex_destroy(&m_mutex);
    assert(res == 0);

}

void mutex::do_lock()
{
    int res = 0;
    res = pthread_mutex_lock(&m_mutex);
    assert(res == 0);
}

bool mutex::do_trylock()
{
    int res = 0;
    res = pthread_mutex_trylock(&m_mutex);
    if (res == EDEADLK) throw lock_error();
    assert(res == 0 || res == EBUSY);
    return res == 0;
 
}

void mutex::do_unlock()
{
    int res = 0;

    res = pthread_mutex_unlock(&m_mutex);
    assert(res == 0);
}

void mutex::do_lock(cv_state&)
{
}

void mutex::do_unlock(cv_state& state)
{
    state.pmutex = &m_mutex;
}
#endif

} // namespace archy

