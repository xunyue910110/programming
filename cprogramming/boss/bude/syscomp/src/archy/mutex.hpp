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

// modified by archy 


#ifndef BOOST_MUTEX_WEK070601_HPP
#define BOOST_MUTEX_WEK070601_HPP


#include "noncopyable.hpp"
#include "thread_exceptions.hpp"


#ifndef WIN32
#   include <pthread.h>
#endif


namespace archy {

struct xtime;

class mutex : private noncopyable
{
	friend class condition;
public:

    mutex(const char* mutexName = 0, bool ctor = true)throw(thread_resource_error);
    ~mutex();

#if defined(WIN32)
    typedef void* cv_state;
#else
    struct cv_state
    {
        pthread_mutex_t* pmutex;
    };
#endif
    void do_lock();
    bool do_trylock();
    void do_unlock();
    void do_lock(cv_state& state);
    void do_unlock(cv_state& state);
private:

#if defined(WIN32)
    void* m_mutex;
#else
    pthread_mutex_t m_mutex;
#endif
};

class scoped_lock : private noncopyable
{
public:

    explicit scoped_lock(mutex& mx)
        : m_mutex(mx)
    {
        m_mutex.do_lock();
    }
    ~scoped_lock()
    {
        m_mutex.do_unlock();
    }
	operator mutex&(){
		return m_mutex;
	}
private:
	mutex& m_mutex;
};

} // namespace archy

#endif // BOOST_MUTEX_WEK070601_HPP
