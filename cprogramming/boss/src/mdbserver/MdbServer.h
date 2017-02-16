//  ############################################
//  Source file : MdbServer.h
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-4-23
//  Update      : 2008-4-23
//  Copyright(C): chen min, Linkage.
//  ############################################

class Thread;
class SocketSession;
class Socket;

#ifndef MDBSERVER_H_HEADER_INCLUDED_B7C3009F
#define MDBSERVER_H_HEADER_INCLUDED_B7C3009F

#include <vector>
#include <string>
#include "config-all.h"

USING_NAMESPACE(std)

//##ModelId=483BB46C01C5
class MdbServer
{
  public:
	MdbServer(){}
	~MdbServer();
	
	//##ModelId=483BCDBF038A
	void run();

	//##ModelId=483BCDCC0148
	bool initialize(const char *cDBName,int port,int iMaxConnect);
	
	bool end();


  private:	
	static void *runSocketSession(void* arg);
	
  private:
	//##ModelId=483BC28F0213
	SocketSession *getSession(Socket * pSocket);

	//##ModelId=483BB8DD03B9
	Socket *m_pListenSocket;
	//##ModelId=483BC2390177
	vector<SocketSession *> m_sessionPool;
	//##ModelId=483BC27E038A
	int m_maxConnect;
	//##ModelId=483BC48000AB
	Thread *m_pThread;
	//##ModelId=483BF8F70232
	int m_port;
	
	string m_dbName;
};



#endif /* MDBSERVER_H_HEADER_INCLUDED_B7C3009F */
