//  ############################################
//  Source file : OdbcDB.h
//  Version     : 0.1.1
//  Language	: ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-9-19 14:28:19
//  Update      : 2008-9-19 14:28:21
//  Copyright(C): chen min, Linkage.
//  ############################################

// 封装标准ODBC接口,针对支持ODBC标准的第三方内存数据库

#ifndef ODBCDB_H_HEADER_INCLUDED_B7E061C8
#define ODBCDB_H_HEADER_INCLUDED_B7E061C8

class Session;
class TableDef;

#include "MdbConstDef.h"
#include "DataBase.h"


class OdbcDB:public DataBase
{
	public:
		OdbcDB(const char *dbName
						,const char *hostName
						,const int port
						,const char *dbUserName
						,const char *dbUserPwd
						,int iConnType=1 // add by chenm 2009-7-21 For altibase 1 : TCP/IP, 2 : UNIXDOMAIN, 3 : IPC
						);
		virtual ~OdbcDB();
		
		virtual Session* connect();
		virtual bool disconnect(Session *&session);
		
	private:
		int m_port;
		T_NAMEDEF m_hostname;
		T_NAMEDEF m_dbUserName;
		T_NAMEDEF m_dbUserPwd;
		int m_iConnType;	
};

#endif
