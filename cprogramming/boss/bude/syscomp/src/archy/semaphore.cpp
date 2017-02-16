#include "semaphore.hpp"

#ifndef ARCHY_HAS_WINTHREADS
  #ifdef	ARCHY_SYSV_SEMAPHORES

   extern "C" {
   #include <sys/types.h>
   #include <sys/ipc.h>
   #include <sys/sem.h>
   };
   
   #include <cerrno>
  
  #endif

#endif

namespace archy {

#ifndef ARCHY_HAS_WINTHREADS
#ifdef	ARCHY_SYSV_SEMAPHORES


semaphore::semaphore(int resource)throw (thread_resource_error)
{
        union semun
        {
                int val; 
                struct semid_ds *buf; 
                unsigned short *array; 
        } arg;


	if((m_semaphore = semget(IPC_PRIVATE, 1, 0644 | IPC_CREAT)) == -1)
	{
		throw thread_resource_error();
	}

	arg.val = resource;
	if(semctl(m_semaphore, 0, SETVAL, arg) == -1)
	{
		throw thread_resource_error();
	}
}

semaphore::~semaphore()
{
	semctl(m_semaphore, 0, IPC_RMID);
}

bool semaphore::wait(void)
{
	struct sembuf ops[] = {{0, -1, 0}};
	
	semop(m_semaphore, ops, 1);
	return true;
}

bool semaphore::trywait(void)
{
	struct sembuf ops[] = {{0, -1, IPC_NOWAIT}};
	
	return (semop(m_semaphore, ops, 1) == EAGAIN) ? false : true;
}

void semaphore::post(void)
{
	struct sembuf ops[] = {{0, 1, 0}};

	semop(m_semaphore, ops, 1);
}


#else // ARCHY_SYSV_SEMAPHORES

semaphore::semaphore(size_t resource) throw (thread_resource_error)
{
	if(sem_init(&m_semaphore, 0, resource))
	{
		throw thread_resource_error();
	}
}

semaphore::~semaphore()
{
	sem_destroy(&m_semaphore);
}

bool semaphore::wait(void)
{
	sem_wait(&m_semaphore);
	return true;
}

void semaphore::post(void)
{
	sem_post(&m_semaphore);
}

bool semaphore::trywait(void)
{
	return (sem_trywait(&m_semaphore) == 0) ? true : false;
}


#endif //ARCHY_SYSV_SEMAPHORES

#else // ARCHY_HAS_WINTHREADS

semaphore::semaphore(int resource)throw (thread_resource_error)
{
	m_semObject = ::CreateSemaphore((LPSECURITY_ATTRIBUTES)NULL, resource, MAX_SEM_VALUE, (LPCTSTR)NULL);
	if( m_semObject == NULL )
		throw thread_resource_error();
}	

semaphore::~semaphore()
{
	if(m_semObject)::CloseHandle(m_semObject);
}

bool semaphore::wait(void)
{
    return  ::WaitForSingleObject(m_semObject, INFINITE) != WAIT_FAILED  ;
}

bool semaphore::trywait(void)
{
    return  ::WaitForSingleObject(m_semObject, 0) != WAIT_FAILED  ;
}

void semaphore::post(void)
{
	::ReleaseSemaphore(m_semObject, 1, (LPLONG)NULL);
}


#endif //ARCHY_HAS_WINTHREADS


}


