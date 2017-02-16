#include <stdio.h>
#include <string.h>

#include "udbi/udbi.h"

#include "IpcSqlBufImp.h"

using namespace UDBI;


static void wAllTrim(char sBuf[])
{
	int i,iFirstChar,iEndPos,iFirstPos;

	iEndPos=iFirstChar=iFirstPos=0;

	for(i=0;sBuf[i]!='\0';i++){
		if(sBuf[i]==' '){
			if(iFirstChar==0) iFirstPos++;
		}
		else{
			iEndPos=i;
			iFirstChar=1;
		}
	}

	for(i=iFirstPos;i<=iEndPos;i++)  sBuf[i-iFirstPos]=sBuf[i];
	sBuf[i-iFirstPos]='\0';
}

static int getSqlFromDb(TSqlStmt** ppstmt)
{
	int j=0;
	
	string strkey, strstmt;
	char sskey[100]="", sstmt[__STMTSIZE]="";
	
	JTable* jtab = jdb::getConnect()->getJTable();
	jtab->Clear();
	jtab->AddSql("SELECT ltrim(rtrim(tab_name||sql_ref)) sql_key, ltrim(rtrim(sql_stmt)) sql_stmt");
	jtab->AddSql("		  FROM code_code ");
	jtab->AddSql("		 ORDER BY dao_name, tab_name, sql_ref ");
	jtab->ExecSelect();
	
	while( jtab->Fetch() ) {
		strkey = jtab->GetString("SQL_KEY");
		strstmt = jtab->GetString("SQL_STMT");
		
		strcpy(sskey, strkey.c_str());
		strcpy(sstmt, strstmt.c_str());
		
		wAllTrim(sskey);
		wAllTrim(sstmt);	
		strcpy((*ppstmt+j)->skey, sskey);
		strcpy((*ppstmt+j)->stmt, sstmt);
		j++;
	}
		
	return j;
}

static int getSqlFromDb2(const string& tabName, const string& sqlRef, TSqlStmt* pstmt)
{
	char sskey[100]="", sstmt[__STMTSIZE]="";

	JTable* jtab = jdb::getConnect()->getJTable();
	jtab->Clear();
	jtab->AddSql("SELECT ltrim(rtrim(tab_name||sql_ref)) sql_key, ltrim(rtrim(sql_stmt)) sql_stmt");
	jtab->AddSql("		  FROM code_code ");
	jtab->AddSql("		 WHERE tab_name = :V1 and sql_ref = :V2 ");
	jtab->SetParam(":V1", tabName);
	jtab->SetParam(":V2", sqlRef);
	jtab->ExecSelect();
	
	if ( !(jtab->Fetch()) ) {
		return -1;
	} 
	
	{
		string strkey = jtab->GetString("SQL_KEY");
		string strstmt = jtab->GetString("SQL_STMT");
		
		strcpy(sskey, strkey.c_str());
		strcpy(sstmt, strstmt.c_str());
		
		wAllTrim(sskey);
		wAllTrim(sstmt);	
		strcpy(pstmt->skey, sskey);
		strcpy(pstmt->stmt, sstmt);		
	}
	
	return 1;
}

//!
IpcSqlBuf* IpcSqlBuf::getSqlBuf(int ipckey)
{
	return new IpcSqlBufImp(ipckey);
}

void IpcSqlBuf::IpcCreate(int ipckey)
{
	IpcLock::create(ipckey, _SQL_LOCK_CNT);
	IpcShm::create(ipckey, _SQL_SHM_SIZE);

	PIpcLock _lock( IpcLock::getLock(ipckey) );
	_lock->rqstLock(_SQLLOCKID);
	_lock->rqstLock(_SEGLOCKID);
	
	PIpcShm _shm( IpcShm::getShm(ipckey) );
	char* pshm = _shm->getbuf() + __SQLBASE;
	memset(pshm, 0, _SQL_SHM_SIZE);
}

void IpcSqlBuf::IpcRemove(int ipckey)
{
	PIpcLock _lock( IpcLock::getLock(ipckey) );
	_lock->rlseLock(_SQLLOCKID);
	_lock->rlseLock(_SEGLOCKID);
	
	IpcLock::remove(ipckey);
	IpcShm::remove(ipckey);
}	

/**
 *  构造函数
 */
IpcSqlBufImp::IpcSqlBufImp(int ipckey)
try
	: _shm( IpcShm::getShm(ipckey) ), 
	 _lock(IpcLock::getLock(ipckey)),
	 m_pshm(0), m_psegshm(0), m_pindshm(0), m_pstmtshm(0)
{ 
	m_pshm = _shm->getbuf() + __SQLBASE;
} 
catch (EIpcException& err) {
	throw;
}

/**
 *  析构函数
 */
IpcSqlBufImp::~IpcSqlBufImp()
{
	cout << "IpcSqlBufImp::~IpcSqlBufImp()" << endl;	
}

/**
 *  SQL共享内存挂接初始化，包括SEGMENT、INDEX、STMT的内存指针
 *  同时初始化SQL共享内存头信息、SQL语句缓存段头信息
 *
 *  ※每次预访问共享内存时都需要调用本函数做初始化
 */
 
void IpcSqlBufImp::initshm()
{
	_lock->slock(_SQLLOCKID);
	memset(&m_ShmHead, 0, sizeof(m_ShmHead));
	memcpy(&m_ShmHead, m_pshm, sizeof(m_ShmHead));
	_lock->sunlock(_SQLLOCKID);
	
	m_psegshm = m_pshm + m_ShmHead.segbase;
	_lock->slock(_SEGLOCKID);
	memset(&m_SegHead, 0, sizeof(m_SegHead));
	memcpy(&m_SegHead, m_psegshm, sizeof(m_SegHead));	
	_lock->sunlock(_SEGLOCKID);

	m_pindshm = m_psegshm + __INDOFF;
	m_pstmtshm = m_psegshm + __STMTOFF;
}

/**
 *  共享内存切换函数
 *  SQLBUF设计了两个语句缓存段，每次重新加载数据时要切换到另一个空闲
 *  缓存段上加载，本函数即是切换SEGMENT、INDEX、STMT的内存指针，同时
 *  初始化SQL共享内存头信息、SQL语句缓存段头信息
 *
 *  ※ __SQLSEG1、__SQLSEG2分别为第一、第二个缓存段的偏移量
 */
void IpcSqlBufImp::swapshm()
{
	_lock->slock(_SQLLOCKID);
	memset(&m_ShmHead, 0, sizeof(m_ShmHead));
	memcpy(&m_ShmHead, m_pshm, sizeof(m_ShmHead));
	_lock->sunlock(_SQLLOCKID);
	
	if (m_ShmHead.segflag == 1) {
		m_ShmHead.segflag = 2;
		m_ShmHead.segbase = __SQLSEG2;
	} else {
		m_ShmHead.segflag = 1;
		m_ShmHead.segbase = __SQLSEG1;
	}
	
	m_psegshm = m_pshm + m_ShmHead.segbase;
	m_pindshm = m_psegshm + __INDOFF;
	m_pstmtshm = m_psegshm + __STMTOFF;

	m_SegHead.sqlcnt = 0;
	m_SegHead.indoff = 0;
	m_SegHead.stmtoff = 0;

	//! 清空预加载数据的共享内存段
	_lock->xlock(_SEGLOCKID);
	memset(m_psegshm, 0, _SQL_SEG_SIZE);	
	memcpy(m_psegshm, &m_SegHead, sizeof(m_SegHead));		
	_lock->xunlock(_SEGLOCKID);
}	

/**
 *  获取当前SEGMENT的偏移
 */
long IpcSqlBufImp::getSegBase()
{
	initshm();
	
	return m_ShmHead.segbase;
}
/**
 *  获取当前缓存的SQL语句数量
 */
long IpcSqlBufImp::getSqlCnt()
{
	initshm();
	
	return m_SegHead.sqlcnt;
}

/**
 *  追加SQL语句函数
 *  将一个新的SQL语句追加到SQL语句缓存段的末尾
 *  ※ 此函数不刷新SQL语句缓存段头信息，主要用于SQL语句加载
 */
void IpcSqlBufImp::addsql(TSqlStmt* psqlstmt)
{
	int sqllen = strlen(psqlstmt->stmt);
	
	TSqlIndex  tsqlind;
	strcpy(tsqlind.skey, psqlstmt->skey);
	tsqlind.stmtlen = sqllen;
	tsqlind.offset = m_SegHead.stmtoff;
	
	//! 追加数据
	_lock->xlock(_SEGLOCKID);
	memcpy(m_pindshm+m_SegHead.indoff, &tsqlind, sizeof(tsqlind));
	memcpy(m_pstmtshm+m_SegHead.stmtoff, psqlstmt->stmt, sqllen);
	_lock->xunlock(_SEGLOCKID);
	
	m_SegHead.sqlcnt += 1;
	m_SegHead.indoff += sizeof(tsqlind);
	m_SegHead.stmtoff += sqllen;
}	

/**
 *  SQL语句追加函数
 *  将一个新的SQL语句追加到SQL语句缓存段的末尾
 *  ※ 此函数与addsql()的区别是同步刷新了SQL语句缓存段头信息
 *     故主要用于已加载后的单个SQL语句增加的情况
 */
void IpcSqlBufImp::appendsql(TSqlStmt* psqlstmt)
{
	int sqllen = strlen(psqlstmt->stmt);
	
	TSqlIndex  tsqlind;
	strcpy(tsqlind.skey, psqlstmt->skey);
	tsqlind.stmtlen = sqllen;
	tsqlind.offset = m_SegHead.stmtoff;
	
	//! 追加数据
	_lock->xlock(_SEGLOCKID);
	memcpy(m_pindshm+m_SegHead.indoff, &tsqlind, sizeof(tsqlind));
	memcpy(m_pstmtshm+m_SegHead.stmtoff, psqlstmt->stmt, sqllen);
	_lock->xunlock(_SEGLOCKID);
	
	m_SegHead.sqlcnt += 1;
	m_SegHead.indoff += sizeof(tsqlind);
	m_SegHead.stmtoff += sqllen;
	
	//! 更新SEGMENT头信息
	_lock->xlock(_SEGLOCKID);
	memcpy(m_psegshm, &m_SegHead, sizeof(m_SegHead));
	_lock->xunlock(_SEGLOCKID);
}	

/** 
 *  SQL语句改写/刷新函数
 *  对一个原来已经缓存了的SQL语句进行刷新，主要用于更新变化
 *  SQL语句刷新时放弃原来的SQL语句的内存，将语句缓存到语句区
 *  末尾，之后修改索引区对应该SQL的偏移量
 *  ※ 此函数用于已加载SQL语句的更新
 */
void IpcSqlBufImp::overridesql(int indidx, TSqlStmt* psqlstmt)
{
	int sqllen = strlen(psqlstmt->stmt);
	TSqlIndex* pind = (TSqlIndex*)m_pindshm;
	
	//! 写数据
	_lock->xlock(_SEGLOCKID);
	pind[indidx].stmtlen = sqllen;
	pind[indidx].offset = m_SegHead.stmtoff;
	memcpy(m_pstmtshm+m_SegHead.stmtoff, psqlstmt->stmt, sqllen);
	_lock->xunlock(_SEGLOCKID);
	
	m_SegHead.stmtoff += sqllen;

	//! 更新SEGMENT头信息	
	_lock->xlock(_SEGLOCKID);
	memcpy(m_psegshm, &m_SegHead, sizeof(m_SegHead));
	_lock->xunlock(_SEGLOCKID);
}

/**
 *  SQL语句查找函数
 *  查找指定条件的SQL语句是否缓存，如找到返回缓存位置，否则返回-1
 */
int IpcSqlBufImp::findsql(const string& tabref)
{
	//! 加共享锁
	if (_lock->slock(_SEGLOCKID, IPC_NOWAIT) < 0) {
		cout << "slock nowait" << endl;
		return -1;
	}

	int i=0;
	TSqlIndex* pind = (TSqlIndex*)m_pindshm;		
	for (i=0; i<m_SegHead.sqlcnt; i++) {
		if (strcmp(pind[i].skey, tabref.c_str()) == 0) {
			break;
		}
	}
	
	// 解共享锁
	_lock->sunlock(_SEGLOCKID);

	if (i == m_SegHead.sqlcnt) {
		return -1;
	}	
	
	return i;	
}

/**
 *  SQL语句查找函数
 *  查找指定条件的SQL语句是否缓存，如找到返回该SQL语句，同时返回
 *  缓存位置，查找失败返回-1
 */
int IpcSqlBufImp::findsql(const string& tabref, string& strstmt)
{
	//! 加共享锁
	if (_lock->slock(_SEGLOCKID, IPC_NOWAIT) < 0) {
		cout << "slock nowait" << endl;
		return -1;
	}

	int i=0;
	TSqlIndex* pind = (TSqlIndex*)m_pindshm;		
	for (i=0; i<m_SegHead.sqlcnt; i++) {
		if (strcmp(pind[i].skey, tabref.c_str()) == 0) {
			char sstmt[__STMTSIZE];
			memset(sstmt, 0, sizeof(sstmt));
			memcpy(&sstmt, m_pstmtshm+pind[i].offset, pind[i].stmtlen);
			strstmt = sstmt;
			
			break;
		}
	}
	
	// 解共享锁
	_lock->sunlock(_SEGLOCKID);

	if (i == m_SegHead.sqlcnt) {
		return -1;
	}	
	
	return i;	
}

/**
 *  SQL语句首次加载函数
 *  初始化SQL共享内存头信息，读取SQL语句，缓存SQL语句
 *  ※用于共享内存建立后的首次SQL语句加载
 */
int IpcSqlBufImp::load()
{
	//! 初始化SHM头信息
	m_ShmHead.segflag = 1;
	m_ShmHead.segbase = __SQLSEG1;
	
	_lock->xlock(_SQLLOCKID);
	memcpy(m_pshm, &m_ShmHead, sizeof(m_ShmHead));
	_lock->xunlock(_SQLLOCKID);

	//! 从数据库读取SQL到本地内存
	TSqlStmt* ptstmt = new TSqlStmt[__MAXSTMTCNT];
	memset(ptstmt, 0, sizeof(TSqlStmt)*__MAXSTMTCNT);
	int sqlcnt = getSqlFromDb(&ptstmt);
	if (sqlcnt <= 0) {
		delete [] ptstmt;
		return -1;
	}

	//! 加上互斥锁
	if (_lock->sxlock(_SQLLOCKID, IPC_NOWAIT)<0) {
		return -1;
	}
	
	{
		m_psegshm = m_pshm + m_ShmHead.segbase;

		m_SegHead.sqlcnt = 0;
		m_SegHead.indoff = 0;
		m_SegHead.stmtoff = 0;
	
		_lock->xlock(_SEGLOCKID);
		memset(m_psegshm, 0, _SQL_SEG_SIZE);
		memcpy(m_psegshm, &m_SegHead, sizeof(m_SegHead));		
		_lock->xunlock(_SEGLOCKID);
		
		m_pindshm = m_psegshm + __INDOFF;
		m_pstmtshm = m_psegshm + __STMTOFF;
		
		int i = 0;
		for (i=0; i< sqlcnt; i++) {
			cout << (ptstmt+i)->skey << endl;
			addsql(ptstmt+i);
		}
		
		_lock->xlock(_SEGLOCKID);
		memcpy(m_psegshm, &m_SegHead, sizeof(m_SegHead));
		_lock->xunlock(_SEGLOCKID);
	}
	_lock->sxunlock(_SQLLOCKID);
	
	delete [] ptstmt;
	
	return sqlcnt;
}	

/** 
 *  SQL语句重加载函数
 *  用于SQL语句的全量重加载
 */
int IpcSqlBufImp::reload()
{
	//! 从数据库读取SQL到本地内存
	TSqlStmt* ptstmt = new TSqlStmt[__MAXSTMTCNT];
	memset(ptstmt, 0, sizeof(TSqlStmt)*__MAXSTMTCNT);
	int sqlcnt = getSqlFromDb(&ptstmt);
	if (sqlcnt <= 0) {
		delete [] ptstmt;
		return -1;
	}

	//! 加上互斥锁
	if (_lock->sxlock(_SQLLOCKID, IPC_NOWAIT) < 0) {
		return -1;
	}	
	{
		//! 切换SEGMENT
		swapshm();
	
		//! 写数据
		for (int i=0; i< sqlcnt; i++) {
			addsql(ptstmt+i);
		}

		//! 更新SEGMENT头信息
		_lock->xlock(_SEGLOCKID);
		memcpy(m_psegshm, &m_SegHead, sizeof(m_SegHead));
		_lock->xunlock(_SEGLOCKID);
		
		//! 更新SHM头信息
		_lock->xlock(_SQLLOCKID);
		memcpy(m_pshm, &m_ShmHead, sizeof(m_ShmHead));
		_lock->xunlock(_SQLLOCKID);	
	}
	_lock->sxunlock(_SQLLOCKID);		
			
	delete [] ptstmt;
	
	return sqlcnt;
}
	
/**
 *  SQL语句追加接口，添加新的SQL语句
 */
int IpcSqlBufImp::append(const string& tabName, const string& sqlRef)
{
	initshm();
	
	TSqlStmt sqlStmt;
	if (getSqlFromDb2(tabName, sqlRef, &sqlStmt) < 0) {
		return -1;
	}

	appendsql(&sqlStmt);
	
	return 0;
}

/**
 *  SQL语句刷新接口，刷新指定条件的SQL语句
 */
int IpcSqlBufImp::refresh(const string& tabName, const string& sqlRef)
{
	initshm();
	
	TSqlStmt sqlStmt;
	if (getSqlFromDb2(tabName, sqlRef, &sqlStmt) < 0) {
		return -1;
	}
	
	string tmpTabRef(tabName);
	tmpTabRef += sqlRef;

	if (_lock->sxlock(_SEGLOCKID, IPC_NOWAIT) < 0) {
		return -1;
	}
	
	int iret = 0;
	if ((iret=findsql(tmpTabRef)) < 0) {
		appendsql(&sqlStmt);
	} else {
		overridesql(iret, &sqlStmt);
	}
	
	_lock->sxunlock(_SEGLOCKID);
	
	return 0;
}

/** 
 *  SQL语句获取函数，查找缓存返回匹配到的SQL语句，失败返回-1
 */
int IpcSqlBufImp::get(string& sqlstmt, const string& tabName, const string& sqlRef)
{
	initshm();
	
	string tmpTabRef(tabName);
	tmpTabRef += sqlRef;

	if (findsql(tmpTabRef, sqlstmt) < 0) {
		return -1;
	}
	
	return 0;
}
