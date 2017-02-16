#ifndef __LISTENER__H__
#define __LISTENER__H__

#include "Thread.h"


class Socket;
class Server;

class Listener {
	public:
		Listener(Server * server); 
		~Listener();
		int setDebug(int & debugLevel);
		 
		static void  thread_proc acceptThread(void*); 

		void  listen(); 
		int   start(const int &port);
		int   stop();
		
		char* dump(char* buf);

		void setHostName(const string & hostName);  

  public:
    Server* m_server;
		bool    m_running; //服务状态
		Socket* m_comm;                 
		int     m_port;     
		int     m_debug;
		
		string m_hostName;      
		
		Thread  m_thread;
};

#endif //__LISTENER__H__



