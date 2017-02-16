#if !defined(ARCHY_SEMAPHORE_CLASS_HEADER_INCLUDE_RUIEURQORUQO908U01841)
#define ARCHY_SEMAPHORE_CLASS_HEADER_INCLUDE_RUIEURQORUQO908U01841

#include "thread_exceptions.hpp"
#include "noncopyable.hpp"

#if defined(ARCHY_HAS_PTHREADS)
#include <pthread.h>
#else
#include <windows.h>
#define	MAX_SEM_VALUE	1000000
#endif 

namespace archy {

class semaphore:private noncopyable
{
private:
#ifndef ARCHY_HAS_WINTHREADS
  #ifdef	ARCHY_SYSV_SEMAPHORES
	int m_semaphore;
  #else
	sem_t m_semaphore;
  #endif
#else // ARCHY_HAS_WINTHREADS
	HANDLE	m_semObject;
#endif // !ARCHY_HAS_WINTHREADS

public:
	/**
	 * The initial value of the semaphore can be specified.  An initial
	 * value is often used When used to lock a finite resource or to 
	 * specify the maximum number of thread instances that can access a 
	 * specified resource.
	 * 
	 * @param resource specify initial resource count or 0 default.
	 */
	semaphore(int resource = 0)throw (thread_resource_error);

	/**
	 * Destroying a semaphore also removes any system resources
	 * associated with it.  If a semaphore has threads currently waiting
	 * on it, those threads will all continue when a semaphore is
	 * destroyed.
	 */
	virtual ~semaphore();

	/**
	 * Wait is used to keep a thread held until the semaphore counter
	 * is greater than 0.  If the current thread is held, then another
	 * thread must increment the semaphore.  Once the thread is accepted, 
	 * the semaphore is automatically decremented, and the thread 
	 * continues execution.
	 * 
	 * The pthread semaphore object does not support a timed "wait", and
	 * hence to maintain consistancy, neither the posix nor win32 source
	 * trees support "timed" semaphore objects.
	 * 
	 * @see #Post
	 */
	bool wait(void);

	/**
	 * TryWait is a non-blocking variant of Wait. If the semaphore counter
	 * is greater than 0, then the thread is accepted and the semaphore
	 * counter is decreased. If the semaphore counter is 0 TryWait returns
	 * immediately with false.
	 *
	 * @return true if thread is accepted otherwise false
	 *
	 * @see #Wait
	 * @see #Post
	 */
	bool trywait(void);

	/**
	 * Posting to a semaphore increments its current value and releases
	 * the first thread waiting for the semaphore if it is currently at
	 * 0.  Interestingly, there is no support to increment a semaphore by
	 * any value greater than 1 to release multiple waiting threads in
	 * either pthread or the win32 API.  Hence, if one wants to release
	 * a semaphore to enable multiple threads to execute, one must perform
	 * multiple post operations.
	 * 
	 * @see #Wait
	 */
	void post(void);

};

} // namespace archy

#endif // ARCHY_SEMAPHORE_CLASS_HEADER_INCLUDE_RUIEURQORUQO908U01841
