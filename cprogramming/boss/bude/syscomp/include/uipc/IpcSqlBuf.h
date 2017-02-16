#ifndef __IPC_SQL_BUF_
#define __IPC_SQL_BUF_

#include <memory>	/// auto_ptr

const int _SQL_IPCKEY = 2934567;

#define __STMTSIZE  4*1024
#define __MAXSTMTCNT  4000

class IpcSqlBuf;
typedef auto_ptr<IpcSqlBuf> PIpcSqlBuf;

class IpcSqlBuf {
public:
	static IpcSqlBuf* getSqlBuf(int ipckey);
	static void IpcCreate(int ipckey);
	static void IpcRemove(int ipckey);
	
	//! 析构函数
	~IpcSqlBuf() { }
	
	//! SQL加载函数, 返回加载SQL语句数量
	virtual int load() = 0;	 
	
	//! SQL重加载函数, 返回加载SQL语句数量
	virtual int reload() = 0;
	
	//! 单条SQL语句添加函数
	virtual int append(const string& tabName, const string& sqlRef) = 0;
	
	//! 单条SQL语句刷新函数
	virtual int refresh(const string& tabName, const string& sqlRef) = 0;
	
	//! SQL语句查找函数
	virtual int get(string& sqlstmt, const string& tabName, const string& sqlRef) = 0;
	
	//! 元数据
	virtual long getSegBase() = 0;
	virtual long getSqlCnt() = 0;
};

#endif
