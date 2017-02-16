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

#ifndef BOOST_CONDITION_WEK070601_HPP
#define BOOST_CONDITION_WEK070601_HPP

#include "thread_exceptions.hpp"
#include "noncopyable.hpp"
#include "mutex.hpp"

#ifndef WIN32
#   include <pthread.h>
#endif

namespace archy {

struct xtime;

namespace detail {

class condition_impl : private noncopyable
{
    friend class condition;

public:
    condition_impl(const char* name = 0, bool ctor = true);
    ~condition_impl();

    void notify_one();
    void notify_all();

#if defined(WIN32)
    void enter_wait();
    void do_wait();
    bool do_timed_wait(const xtime& xt);
#else
    void do_wait(pthread_mutex_t* pmutex);
    bool do_timed_wait(const xtime& xt, pthread_mutex_t* pmutex);
#endif

#if defined(WIN32)
    void* m_gate;
    void* m_queue;
    void* m_mutex;
    unsigned m_gone;  // # threads that timed out and never made it to m_queue
    unsigned long m_blocked; // # threads blocked on the condition
    unsigned m_waiting; // # threads no longer waiting for the condition but
                        // still waiting to be removed from m_queue
#else
    pthread_cond_t m_condition;
#endif
};

} // namespace detail

class condition : private noncopyable
{
public:
    condition(const char* name = 0, bool ctor = true) 
        : m_impl(name, ctor)
    { }
    
    ~condition() { }

    void notify_one() { m_impl.notify_one(); }
    void notify_all() { m_impl.notify_all(); }

    void wait(mutex& lock)
    {
        do_wait(lock);
    }
    
    bool timed_wait(mutex& lock, const xtime& xt)
    {
        return do_timed_wait(lock, xt);
    }
private:
    detail::condition_impl m_impl;

    void do_wait(mutex& mutex)
    {
#if defined(WIN32) 
        m_impl.enter_wait();
#endif

		mutex::cv_state state;
        mutex.do_unlock(state);

#if defined(WIN32)
        m_impl.do_wait();
#else
        m_impl.do_wait(state.pmutex);
#endif

		mutex.do_lock(state);
    }

    bool do_timed_wait(mutex& mutex, const xtime& xt)
    {
#if (defined(WIN32) )
        m_impl.enter_wait();
#endif

		mutex::cv_state state;
		mutex.do_unlock(state);

        bool ret = false;

#if defined(WIN32)
        ret = m_impl.do_timed_wait(xt);
#else
        ret = m_impl.do_timed_wait(xt, state.pmutex);
#endif
		mutex.do_lock(state);

        return ret;
    }
};

} // namespace boost


#endif // BOOST_CONDITION_WEK070601_HPP
