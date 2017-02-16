//  ############################################
//  Source file : Thread.cpp
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-5-28 14:21:53
//  Update      : 2008-5-28 14:21:55
//  Copyright(C): chen min, Linkage.
//  ############################################

#include "Thread.h"
#include "Mdb_Exception.h"

//##ModelId=483BCA270128
void Thread::setPriority(ThreadPriority pri)
{
	#if defined(PRI_OTHER_MIN) && defined(PRI_OTHER_MAX)
        struct sched_param sp;
        sp.sched_priority = pri == THR_PRI_LOW ? IPRI_OTHER_MIN : PRI_OTHER_MAX;
        pthread_setschedparam(m_thread, SCHED_OTHER, &sp);
	#endif
}

//##ModelId=483BCA5E035B
void Thread::start(P_THREAD_FUNC pFunc,void *arg)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
#if !defined(__linux__)
        pthread_attr_setstacksize(&attr, ThreadStackSize);
#endif    
	int status = 0;
    if( (status=pthread_create(&m_thread, &attr, pFunc, arg)) != 0 )
    {
    	printf("%d %s\n", status, strerror(status));
    	throw Mdb_Exception(__FILE__, __LINE__, "创建线程失败!");		
    }
    pthread_attr_destroy(&attr);
}

//##ModelId=483BCA6D02EE
void Thread::join()
{
	pthread_join(m_thread, NULL);
}

//##ModelId=483BCA7B0186
void Thread::detach()
{
	pthread_detach(m_thread);
}

