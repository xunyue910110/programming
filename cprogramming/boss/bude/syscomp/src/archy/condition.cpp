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

#include "condition.hpp"
#include "xtime.hpp"

#include "thread_exceptions.hpp"
#include "debug.hpp"
#include "timeconv.inl"

#if defined(WIN32)
#   ifndef NOMINMAX
#      define NOMINMAX
#   endif
#   include <windows.h>
#else
#   include <errno.h>
#endif
#include <assert.h>

namespace archy {

namespace detail {

#if defined(WIN32)
condition_impl::condition_impl(const char* name/* = 0*/, bool ctor /*= true*/)
{
    if (ctor) 
    {
        m_gone = 0;
        m_blocked = 0;
        m_waiting = 0;
    }
    std::string strName;

    if (name == 0 )
    {
        m_mutex = reinterpret_cast<void*>(CreateMutex(0, 0, 0));
        m_gate = reinterpret_cast<void*>(CreateSemaphore(0, 1, 1, 0));
        m_queue = reinterpret_cast<void*>(CreateSemaphore(0, 0,
                                                         static_cast<long>(static_cast<unsigned long>(-1)/2)/*std::numeric_limits<long>::max()*/,
                                                          0));

    }
    else
    {
        strName = std::string("cond_") + name;
        m_mutex = reinterpret_cast<void*>(CreateMutex(0, 0, strName.c_str()));

        strName = std::string("gate_") + name;
        m_gate = reinterpret_cast<void*>(CreateSemaphore(0, 1, 1, strName.c_str()));

        strName = std::string("queu_") + name;
        m_queue = reinterpret_cast<void*>(CreateSemaphore(0, 0,
                                                         static_cast<long>(static_cast<unsigned long>(-1)/2)/*std::numeric_limits<long>::max()*/,
                                                          strName.c_str()));

    }

    if (!m_gate || !m_queue || !m_mutex)
        {
            int res = 0;
            if (m_gate)
                {
                    res = CloseHandle(reinterpret_cast<HANDLE>(m_gate));
                    assert(res);
                }
            if (m_queue)
                {
                    res = CloseHandle(reinterpret_cast<HANDLE>(m_queue));
                    assert(res);
                }
            if (m_mutex)
                {
                    res = CloseHandle(reinterpret_cast<HANDLE>(m_mutex));
                    assert(res);
                }
            throw thread_resource_error();
        }
}

condition_impl::~condition_impl()
{
    int res = 0;
    res = CloseHandle(reinterpret_cast<HANDLE>(m_gate));
    assert(res);
    res = CloseHandle(reinterpret_cast<HANDLE>(m_queue));
    assert(res);
    res = CloseHandle(reinterpret_cast<HANDLE>(m_mutex));
    assert(res);
}

void condition_impl::notify_one()
{
    unsigned signals = 0;

    int res = 0;
    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), INFINITE);
    assert(res == WAIT_OBJECT_0);

    if (m_waiting != 0) // the m_gate is already closed
        {
            if (m_blocked == 0)
                {
                    res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
                    assert(res);
                    return;
                }

            ++m_waiting;
            --m_blocked;
            signals = 1;
        }
    else
        {
            res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_gate), INFINITE);
            assert(res == WAIT_OBJECT_0);
            if (m_blocked > m_gone)
                {
                    if (m_gone != 0)
                        {
                            m_blocked -= m_gone;
                            m_gone = 0;
                        }
                    signals = m_waiting = 1;
                    --m_blocked;
                }
            else
                {
                    res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
                    assert(res);
                }
        }

    res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
    assert(res);

    if (signals)
        {
            res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_queue), signals, 0);
            assert(res);
        }
}

void condition_impl::notify_all()
{
    unsigned signals = 0;

    int res = 0;
    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), INFINITE);
    assert(res == WAIT_OBJECT_0);

    if (m_waiting != 0) // the m_gate is already closed
        {
            if (m_blocked == 0)
                {
                    res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
                    assert(res);
                    return;
                }

            m_waiting += (signals = m_blocked);
            m_blocked = 0;
        }
    else
        {
            res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_gate), INFINITE);
            assert(res == WAIT_OBJECT_0);
            if (m_blocked > m_gone)
                {
                    if (m_gone != 0)
                        {
                            m_blocked -= m_gone;
                            m_gone = 0;
                        }
                    signals = m_waiting = m_blocked;
                    m_blocked = 0;
                }
            else
                {
                    res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
                    assert(res);
                }
        }

    res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
    assert(res);

    if (signals)
        {
            res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_queue), signals,
                                   0);
            assert(res);
        }
}

void condition_impl::enter_wait()
{
    int res = 0;
    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_gate), INFINITE);
    assert(res == WAIT_OBJECT_0);
    ++m_blocked;
    res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
    assert(res);
}

void condition_impl::do_wait()
{
    int res = 0;
    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_queue), INFINITE);
    assert(res == WAIT_OBJECT_0);

    unsigned was_waiting=0;
    unsigned was_gone=0;

    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), INFINITE);
    assert(res == WAIT_OBJECT_0);
    was_waiting = m_waiting;
    was_gone = m_gone;
    if (was_waiting != 0)
        {
            if (--m_waiting == 0)
                {
                    if (m_blocked != 0)
                        {
                            res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1,
                                                   0); // open m_gate
                            assert(res);
                            was_waiting = 0;
                        }
                    else if (m_gone != 0)
                        m_gone = 0;
                }
        }
    else if (++m_gone == (/*std::numeric_limits<unsigned>::max()*/static_cast<unsigned>(-1) / 2))
        {
            // timeout occured, normalize the m_gone count
            // this may occur if many calls to wait with a timeout are made and
            // no call to notify_* is made
            res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_gate), INFINITE);
            assert(res == WAIT_OBJECT_0);
            m_blocked -= m_gone;
            res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
            assert(res);
            m_gone = 0;
        }
    res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
    assert(res);

    if (was_waiting == 1)
        {
            for (/**/ ; was_gone; --was_gone)
                {
                    // better now than spurious later
                    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_queue),
                                              INFINITE);
                    assert(res == WAIT_OBJECT_0);
                }
            res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
            assert(res);
        }
}

bool condition_impl::do_timed_wait(const xtime& xt)
{
    bool ret = false;
    unsigned int res = 0;

    for (;;)
        {
            int milliseconds;
            to_duration(xt, milliseconds);

            res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_queue),
                                      milliseconds);
            assert(res != WAIT_FAILED && res != WAIT_ABANDONED);
            ret = (res == WAIT_OBJECT_0);

            if (res == WAIT_TIMEOUT)
                {
                    xtime cur;
                    xtime_get(&cur, TIME_UTC);
                    if (xtime_cmp(xt, cur) > 0)
                        continue;
                }

            break;
        }

    unsigned was_waiting=0;
    unsigned was_gone=0;

    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_mutex), INFINITE);
    assert(res == WAIT_OBJECT_0);
    was_waiting = m_waiting;
    was_gone = m_gone;
    if (was_waiting != 0)
        {
            if (!ret) // timeout
                {
                    if (m_blocked != 0)
                        --m_blocked;
                    else
                        ++m_gone; // count spurious wakeups
                }
            if (--m_waiting == 0)
                {
                    if (m_blocked != 0)
                        {
                            res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1,
                                                   0); // open m_gate
                            assert(res);
                            was_waiting = 0;
                        }
                    else if (m_gone != 0)
                        m_gone = 0;
                }
        }
    else if (++m_gone == (/*std::numeric_limits<unsigned>::max()*/static_cast<unsigned>(-1) / 2))
        {
            // timeout occured, normalize the m_gone count
            // this may occur if many calls to wait with a timeout are made and
            // no call to notify_* is made
            res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_gate), INFINITE);
            assert(res == WAIT_OBJECT_0);
            m_blocked -= m_gone;
            res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
            assert(res);
            m_gone = 0;
        }
    res = ReleaseMutex(reinterpret_cast<HANDLE>(m_mutex));
    assert(res);

    if (was_waiting == 1)
        {
            for (/**/ ; was_gone; --was_gone)
                {
                    // better now than spurious later
                    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_queue),
                                              INFINITE);
                    assert(res ==  WAIT_OBJECT_0);
                }
            res = ReleaseSemaphore(reinterpret_cast<HANDLE>(m_gate), 1, 0);
            assert(res);
        }

    return ret;
}
#else
condition_impl::condition_impl(const char* name/* = 0*/, bool ctor /*= true*/)
{
    if (!ctor)
    {
        return;
    }
    
    int res = 0;

	pthread_condattr_t cattr;

	if (name != 0)
	{
		pthread_condattr_init(&cattr);
		pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
	}

    res = pthread_cond_init(&m_condition, (name != 0 ? (&cattr) : 0));
    if (res != 0)
        throw thread_resource_error();

	if (name != 0)
	{
		pthread_condattr_destroy(&cattr);
	}

}

condition_impl::~condition_impl()
{
    int res = 0;
    res = pthread_cond_destroy(&m_condition);
    assert(res == 0);
}

void condition_impl::notify_one()
{
    int res = 0;
    res = pthread_cond_signal(&m_condition);
    assert(res == 0);
}

void condition_impl::notify_all()
{
    int res = 0;
    res = pthread_cond_broadcast(&m_condition);
    assert(res == 0);
}

void condition_impl::do_wait(pthread_mutex_t* pmutex)
{
    int res = 0;
    res = pthread_cond_wait(&m_condition, pmutex);
    assert(res == 0);
}

bool condition_impl::do_timed_wait(const xtime& xt, pthread_mutex_t* pmutex)
{
    timespec ts;
    to_timespec(xt, ts);

    int res = 0;
    res = pthread_cond_timedwait(&m_condition, pmutex, &ts);
    assert(res == 0 || res == ETIMEDOUT);

    return res != ETIMEDOUT;
}
#endif

} // namespace detail

} // namespace archy

