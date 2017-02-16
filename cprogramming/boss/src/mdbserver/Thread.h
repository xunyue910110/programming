//  ############################################
//  Source file : Thread.h
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-5-28 14:21:47
//  Update      : 2008-5-28 14:21:50
//  Copyright(C): chen min, Linkage.
//  ############################################

#ifndef THREAD_H_HEADER_INCLUDED_B7C36051
#define THREAD_H_HEADER_INCLUDED_B7C36051

//##ModelId=483BB4D6033C
#include <pthread.h>

typedef void *(*P_THREAD_FUNC)(void *) ;
const size_t ThreadStackSize = 1024*1024; 

enum ThreadPriority 
{
    THR_PRI_LOW,
    THR_PRI_HIGH
};

class Thread
{
  public:
	Thread()
	{			
	}
	//##ModelId=483BCA270128
	void setPriority(ThreadPriority pri);

	//##ModelId=483BCA5E035B
	void start(P_THREAD_FUNC pFunc,void *arg);

	//##ModelId=483BCA6D02EE
	void join();

	//##ModelId=483BCA7B0186
	void detach();

  private:
	//##ModelId=483BCA3F0000
	pthread_t m_thread;
};



#endif /* THREAD_H_HEADER_INCLUDED_B7C36051 */
