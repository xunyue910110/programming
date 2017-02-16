//  ############################################
//  Source file : Socket.h
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-5-28 14:43:45
//  Update      : 2008-5-28 14:43:48
//  Copyright(C): chen min, Linkage.
//  ############################################

#ifndef SOCKET_H_HEADER_INCLUDED_B7C3119A
#define SOCKET_H_HEADER_INCLUDED_B7C3119A

#include <stddef.h>
#include <string>
#include "config-all.h"

USING_NAMESPACE(std)

const int MAX_LISTEN_QUEUE_SIZE = 20;
const int LINGER_TIME            =10;

//##ModelId=483BB50102FD
class Socket
{
  public:
	Socket()
	{
		m_fp = 0;	
	}
	
	~Socket()
	{
		this->close();	
	}
	
	//##ModelId=483BB6E70148
	int read(void* buf, int min_size, int max_size);

	//##ModelId=483BB6EA02DE
	bool write(void const* buf, size_t size);

	//##ModelId=483BB6F4000F
	int readLine(void *buf, int maxlen);

	//##ModelId=483BC065005D
	// server端使用
	bool startUpListen(char const* hostname, int port);

	//##ModelId=483BC09A03A9
	Socket *accept();

	//##ModelId=483BC6B302AF
	// client端使用
	bool connect(char const* hostname, 
	                     const  int port);

	//##ModelId=483BC6BF00BB
	void get_peer_name(string &sAddr);
	
	bool close();
	bool shutdown();
	
  private:
  	Socket(int fp)
	{
		m_fp = fp;	
	}

  private:
	//##ModelId=483BBCEF03D8
	int m_fp;
};



#endif /* SOCKET_H_HEADER_INCLUDED_B7C3119A */
