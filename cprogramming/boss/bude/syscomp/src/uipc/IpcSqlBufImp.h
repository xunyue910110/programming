#ifndef __IPC_SQL_BUF_IMP_
#define __IPC_SQL_BUF_IMP_

#include "budeipc.h"
#include "IpcLock.h"
#include "IpcShm.h"

#include "IpcSqlBuf.h"

using namespace budeipc;

//const int _SQL_IPCKEY = 234567;			   ///move to IpcSqlBuf.h
const int _SQL_SEG_SIZE = __STMTSIZE*__MAXSTMTCNT+1024*1024;	/// 1024*1024与__STMTOFF一致
const int _SQL_SHM_SIZE = 100+2*_SQL_SEG_SIZE; ///100>sizeof(TSqlShmHead)

const int _SQL_LOCK_CNT = 3;
const int _SQLLOCKID = 1;
const int _SEGLOCKID = 2;

/**
 *  SQL语句共享内存头信息
 */
struct TSqlShmHead {
	long segbase;
	long segflag;
	long resvn1, resvn2;
	char resvc1[20];
};

/**
 *  SQL语句缓存段头信息
 */
struct TSqlSegHead {
	long sqlcnt;
	long indoff;
	long stmtoff;
};

/**
 *  SQL语句索引结构
 */
struct TSqlIndex {
	char skey[100];
	long offset;
	long stmtlen;
};

/**
 *  SQL语句结构
 */
struct TSqlStmt {
	char skey[100];
	char stmt[__STMTSIZE];
};

/* 用于共享内存之SQL缓存, 同一共享内存缓存多种数据情况 */
//! sqlshm:| hhhhh  s1s1s1s1s1s1  s2s2s2s2s2s2s2s2
//! sqlseg:| s1:shshsh   SISISISI   STSTSTSTSTST
//! sqlseg:| s2:shshsh   SISISISI   STSTSTSTSTST
const int __SQLBASE = 0;
const int __SQLSEG1 = sizeof(TSqlShmHead);
const int __SQLSEG2 = __SQLSEG1+_SQL_SEG_SIZE;

const int __INDOFF = sizeof(TSqlSegHead);
const int __STMTOFF = 1024*1024;				

class IpcSqlBufImp : public IpcSqlBuf {
public:
	IpcSqlBufImp(int ipckey);
	~IpcSqlBufImp();
	
	virtual int load();	 
	virtual int reload();
	virtual int append(const string& tabName, const string& sqlRef);
	virtual int refresh(const string& tabName, const string& sqlRef);
	virtual int get(string& sqlstmt, const string& tabName, const string& sqlRef);
	
	virtual long getSegBase();
	virtual long getSqlCnt();
	
protected:
	void initshm();	
	void swapshm();
	
	int findsql(const string& tabref);
	int findsql(const string& tabref, string& strstmt);
	
	void addsql(TSqlStmt* psqlstmt);
	void appendsql(TSqlStmt* psqlstmt);
	void overridesql(int indidx, TSqlStmt* psqlstmt);					
		
private:
	PIpcShm  _shm;
	PIpcLock  _lock;
		
	TSqlShmHead  m_ShmHead;
	TSqlSegHead  m_SegHead;
	
	char* m_pshm;
	char* m_psegshm;
	char* m_pindshm;
	char* m_pstmtshm;
	
};


#endif
