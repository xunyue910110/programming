#include "utils.hpp"

#ifndef WIN32
#   include <unistd.h>
#   include "pthread.h"
#   ifdef __linux__
#       include <sys/select.h>
#    endif
#endif

namespace archy {


int getCurrentThreadId()
{
		int pid;
#ifdef  WIN32				
		pid = GetCurrentThreadId();
#else
		pid = pthread_self();
#endif  
		return pid;

}

int getCurrentProcessId()
{
    int pid;

#ifdef  WIN32				
    pid = GetCurrentProcessId();
#else
	pid = getpid();
#endif  

    return pid;
}


#ifndef WIN32
void	select_sleep(int iSec,int iMicroSec)
{
	struct timeval tvTime;
	tvTime.tv_sec = iSec;
	tvTime.tv_usec = iMicroSec*1000;				
	
	::select(0,0,0,0,&tvTime);
	
}
#endif

void Sleeping(int mSec)
{
#ifndef WIN32
	select_sleep(mSec/1000, mSec - (mSec/1000)*1000);
#else 
	::Sleep(mSec);
#endif
}

} // namespace


