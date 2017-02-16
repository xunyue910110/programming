//  ############################################
//  Source file : SocketSession.h
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-4-23
//  Update      : 2008-4-23
//  Copyright(C): chen min, Linkage.
//  ############################################

class Socket;
class Session;
class DataBase;
class InputParams;

#ifndef SOCKETSESSION_H_HEADER_INCLUDED_B7C32D7E
#define SOCKETSESSION_H_HEADER_INCLUDED_B7C32D7E

#include <map>
#include <vector>
#include <string>

#include "config-all.h"

#include "MdbConstDef.h"

USING_NAMESPACE(std)

const int CONNECT_LOG_TYPE=0;
const int DISCONNECT_LOG_TYPE=1;

class SocketSession
{
  public:
	SocketSession();
	
	//##ModelId=483BB6C1001F
	void run();

	//##ModelId=483BC1490196
	bool end();

	//##ModelId=483BC225031C
	bool isActive();

	//断开本地内存数据库连接
	void disconnect();

	//连接本地内存数据库
	void connect(const char *cDBName);
	
	void setActive();
	
	void setSocket(Socket * pSocket);

  	//add by chenyong --2009/2/15号 by chenm 2009-5-21 改成public方法
  	//------------------------------------------------------------------
  	void rollBack();
  	//------------------------------------------------------------------

	
  private:
	//##ModelId=483BDE3703A9
	void dealRequest(char *cRecvline,const int iReceiveBytes,SocketSession *pSocketSession);

  	void quit(const char *cRecvline,int iStrLenth);
  	void insert(const char *cRecvline,int iStrLenth);
  	void update(const char *cRecvline,int iStrLenth);
  	void select(const char *cRecvline,int iStrLenth);
  	void deleteRec(const char *cRecvline,int iStrLenth);
  	void begin(const char *cRecvline,int iStrLenth);
  	void commit(const char *cRecvline,int iStrLenth);
  	void rollBack(const char *cRecvline,int iStrLenth);
  	//add by chenyong --2009/2/15号
  	//------------------------------------------------------------------
  	//void rollBack();
  	//------------------------------------------------------------------
  	void getTableDesc(const char *cRecvline,int iStrLenth);
  	void getIndexDesc(const char *cRecvline,int iStrLenth);
  	void getAllTableName(const char *cRecvline,int iStrLenth);
  	void getAllIndexName(const char *cRecvline,int iStrLenth);
  	void selectw(const char *cRecvline,int iStrLenth); //为windows控制台增加,对于数据量过大时,应支持分页查询
  	void getTableRowCount(const char *cRecvline,int iStrLenth);
  	void getRowByRowNextRec(const char *cRecvline,int iStrLenth); // add by chenm 2009-7-17 for rowbyrow
  	
  	void write2Client(const void *cMsg,size_t iSendSize=0);
  	
  	// add by chenm 2009-5-21 21:25:51
  	void writeServLog(const int iLogType);
  
  private:
	// 本地数据库连接会话
	//##ModelId=483BB667034B
	Session *m_session;
	//##ModelId=483BB8F703B9
	Socket *m_pWorkSocket;
	//##ModelId=483BC21000FA
	bool m_isActive;
	//##ModelId=483BCD0F00DA
	DataBase *m_db;
	// 是否是事务
	bool m_isInTransction;

	typedef map<string, void(SocketSession::*)(const char *,int)> FUNC_MAP;
	typedef FUNC_MAP::iterator  FUNC_MAP_ITR;
	
	// 函数指针容器
	FUNC_MAP m_fucMap;	
	
	string m_sClintName;
	
	char        m_cRowByRowExpr[128] ;                // by chenm 2009-7-17 17:15:42 
	char 	m_cRowByRowTableName[64];
	char 	m_cRowByRowIndexName[64];
	InputParams *m_pRowByRowOtherParams;
};

#endif /* SOCKETSESSION_H_HEADER_INCLUDED_B7C32D7E */
