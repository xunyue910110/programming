//  ############################################
//  Source file : RemoteDB.h
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-6-5 15:34:08
//  Update      : 2008-6-5 15:34:09
//  Copyright(C): chen min, Linkage.
//  ############################################

#ifndef REMOTEDB_H_HEADER_INCLUDED_B7E061C8
#define REMOTEDB_H_HEADER_INCLUDED_B7E061C8

class Session;
class TableDef;
class Socket;

#include "MdbConstDef.h"
#include "DataBase.h"


class RemoteDB:public DataBase
{
	public:
		RemoteDB(const char *dbName,const char *hostName,const int port);
		virtual ~RemoteDB();
		
		virtual Session* connect();
		virtual bool disconnect(Session *&session);
		
	private:
		int m_port;
		char m_hostname[20];
		Socket *m_pSocket;
};

#endif
