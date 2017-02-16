#include "IpcParam.h"

#include <iostream>

#include "udbi/udbi.h"

//#include "budeipc.h"
#include "IpcLocal.h"
#include "IpcLock.h"
#include "IpcShm.h"

#include "ParamHelper.h"

using std::cout;
using std::endl;
	
using namespace UDBI;
using namespace budeipc;


#ifdef OFF_PAR_TRACE
#define __PAR_TRACE(msg) 
#define __PAR_TRACE_P1(msg, p1)
#define __PAR_TRACE_P2(msg, p1, p2)
#else
#define __PAR_TRACE(msg) \
		readSegHead(); \
		if (m_seghead.trace == 1) { \
			debug2File(IPC_TRACE, __FILE__, __LINE__, msg); \
		} 
#define __PAR_TRACE_P1(msg, p1) \
		readSegHead(); \
		if (m_seghead.trace == 1) { \
			debug2File(IPC_TRACE, __FILE__, __LINE__, msg, p1); \
		} 
#define __PAR_TRACE_P2(msg, p1, p2) \
		readSegHead(); \
		if (m_seghead.trace == 1) { \
			debug2File(IPC_TRACE, __FILE__, __LINE__, msg, p1, p2); \
		} 
#endif


namespace {
	inline void StrToUpper(string& s)  
	{  
		for(int i=0; i<s.length(); ++i)
		{  
			s[i] = toupper(s[i]);		
		}  
	}   
	
	/**
	 *  class __DAO, helper class
	 */
	class __DAO {
	public:
		//!
		__DAO():_ptab(0) 
		{
			JConnect* lpcon = jdb::getConnect();
			_ptab = lpcon->CreateJTable("TD_S_CPARAM");            
		}
	
		//!
		~__DAO() 
		{
			delete _ptab;
		}
		
		string GetSql(void)
		{
			return _ptab->GetSql();
		}
	    void SetParam(const string& par, const string& val, short* isNull=0)
	    {
	    	_ptab->SetParam(par, val, isNull);
	    }
	    string GetString(const string& fldName, bool* ifNull=0)
	    {
	    	return _ptab->GetString(fldName, ifNull);
	    }
	    int GetInt(const string& fldName, bool* ifNull=0, const int nullValue=-1)
	    {
	        return _ptab->GetInt(fldName, ifNull, nullValue);
	    }
	
		//! 清空函数
		void Clear() { _ptab->Clear(); }
	    int ExecSelect(const string& selBy="")
	    {
	    	return _ptab->ExecSelect(selBy);
	    }    
	    bool Fetch() { return _ptab->Fetch(); }
	    int jcount(const string &selBy)
	    {
	        string sstmt;
	        JSQL::getSQL(sstmt, "TD_S_CPARAM", selBy);
	        
	        string stmt("select count(*) J_COUNT from ( ");
	        stmt += sstmt;
	        stmt += " ) ";
	        
	        _ptab->AddSql(stmt);        
	        _ptab->ExecSelect();    
	        if (_ptab->Fetch()) {
	            return _ptab->GetInt("J_COUNT");
	        }
	
	        //
	        return 0;
	    }
	
	private:
		JTable* _ptab;
		
	};  // class __DAO
	
	
	/**
	 * class __PRECORD, helper class
	 */
	class __PRECORD {
	public:
		__PRECORD(int size)
		{
			precord = new TParRecord[size+1];
			memset(precord, 0, sizeof(TParRecord)*(size+1));
			reccnt = size;
		}
		~__PRECORD()
		{
			delete [] precord;
			precord = NULL;
			reccnt = 0;
		}
		int getFromDb(const string& key)
		{
		    __DAO dao;
		
		    dao.SetParam(":VKEY", key);
		    dao.ExecSelect(key);
		
		    int i=0, _cvalCnt=0;
		    string s1, s2, s3(NULL_VALUE3), s4;
		    
		    string ss = dao.GetSql();
		    StrToUpper(ss);
		    if (ss.find(" VALUE3") != string::npos)
		    {
		    	_cvalCnt = 3;
		    }
		    
		    while ( dao.Fetch() )
		    {
		        s1=dao.GetString("VALUE1");
		        s2=dao.GetString("VALUE2");
		        if ( _cvalCnt > 0 ) 
		            s3 = dao.GetString("VALUE3");
		        s4=dao.GetString("VRESULT");
		        
		        strncpy((precord+i)->value1, s1.c_str(), CODE_LEN_MAX);
		        strncpy((precord+i)->value2, s2.c_str(), CODE_LEN_MAX);
		        strncpy((precord+i)->value3, s3.c_str(), CODE_LEN_MAX);
		        strncpy((precord+i)->sresult, s4.c_str(), NAME_LEN_MAX);
	
		        ++i;
		    }
		
			// sorting ...
			int tmpRecCnt = (reccnt>=i) ? i : reccnt;
			_ParamHelper::quicksort(precord, 0, tmpRecCnt-1);
				
		    //
		    return tmpRecCnt;
		}
		
		//
		int reccnt;
		TParRecord* precord;
		
	}; // __PRECORD
	
} // unnamed namespace	



/**
 * class IIpcParam's realization
 */
void IIpcParam::initialize()
{
	IpcLock::create(CPAR_IPCKEY_, CPAR_LCKCNT_);
	IpcShm::create(CPAR_IPCKEY_, CPAR_SHMSIZE_);

	///	
	IpcLock& lck = IpcLock::getLock(CPAR_IPCKEY_);
	lck.rqstLock( HEAD_LOCKID_ );
	lck.rqstLock( INDEX_LOCKID_ );
	
	///
	lck.xlock(HEAD_LOCKID_, 0);
	
	IpcShm& shm = IpcShm::getShm(CPAR_IPCKEY_);
	char* hshmat = shm.atbuf();
	char* hshm = hshmat + PAR_SHMBASE_;
	memset(hshm, 0, PAR_SHMSIZE_);

	TSegHead seghead;
	memset(&seghead, 0, sizeof(TSegHead));
	
	seghead.shmbase = PAR_SHMBASE_;
	seghead.shmsize = PAR_SHMSIZE_;
	seghead.indbase = PAR_BASE_IND;
	seghead.parcnt = 0;
	seghead.recbase1 = PAR_BASE_REC1;
	seghead.recbase2 = PAR_BASE_REC2;
	seghead.recsize = 0;
	seghead.extbase = PAR_BASE_EXT;
	seghead.extsize = 0;
	seghead.trace = 1;

	memcpy(hshm, &seghead, sizeof(TSegHead));
	
	shm.dtbuf(hshmat);
	lck.xunlock(HEAD_LOCKID_);
}

void IIpcParam::uninitialize()
{
	IpcShm::remove(CPAR_IPCKEY_);
	IpcLock::remove(CPAR_IPCKEY_);
}

IIpcParam& IIpcParam::getIpcParam()
{
	static IpcParam ipcparam;
	return ipcparam;
}


/**
 *  class IpcParam's realization
 */
IpcParam::IpcParam()
try
:_ipcshm(IpcShm::getShm(CPAR_IPCKEY_)), _ipclck(IpcLock::getLock(CPAR_IPCKEY_)), m_hshm(0), m_hind(0)	
{
	m_hshm_at = _ipcshm.atbuf();
	m_hshm = m_hshm_at + PAR_SHMBASE_;

	//	
	readSegHead();
	m_hind = (TParIndex*)(m_hshm+m_seghead.indbase);
} 
catch ( Exception& er)
{
	debug2File(IPC_TRACE, __FILE__, __LINE__, "IpcParam::IpcParam() exception!!!!! (%s)", er.toString().c_str()); \
}

IpcParam::~IpcParam()
{ 
	_ipcshm.dtbuf(m_hshm_at);
}

///// verify whether key is empty outside.
int IpcParam::load(const string& key)
{
	__PAR_TRACE_P1("Loading %s ...", key.c_str());

	/// 
	int reccnt = 0;

	_ipclck.sxlock(HEAD_LOCKID_, 0);
	
	try
	{
		////
		int idx = findKey(key);
		if (idx >= 0) 
		{
			_ipclck.sxunlock(HEAD_LOCKID_);
			__PAR_TRACE_P2("Load %s end: key has loaded(%d).", key.c_str(), idx);
			return idx;
		}
	
		//
		int rfInterval = defRefrshInterval;
	    string updatedTime(DEFAULT_DATE_TIME);

	    __DAO dao;
	    dao.SetParam(":VKEY", key);
		reccnt = dao.jcount(key);
		if ( reccnt <= 0 ) 
		{
			_ipclck.sxunlock(HEAD_LOCKID_);
			__PAR_TRACE_P1("Load %s failure: no-data-found.", key.c_str());
			return -1;
		}

	    dao.ExecSelect(SQLBY_CPARAM_CONFIG);
	    if ( dao.Fetch() ) 
	    {
	    	rfInterval = dao.GetInt("REFRESH_INTERVAL");
	        updatedTime = dao.GetString("UPDATED_TIME");
	    }
		
		__PRECORD prec(reccnt);
		if (prec.getFromDb(key) <= 0) {
			_ipclck.sxunlock(HEAD_LOCKID_);
			__PAR_TRACE_P1("Load %s failure: get-from-db error or no-data-found.", key.c_str());
			return -1;
		}
		
		int capacity = reccnt*1.4;
	
		// for index-write: parsize, recsize;
		readSegHead();

		/// write records		
		if ( (m_seghead.recsize+capacity)*sizeof(TParRecord) >= PAR_SIZE_REC )
		{
			_ipclck.sxunlock(HEAD_LOCKID_);
			__PAR_TRACE_P1("Load %s failure: no enough segment space.", key.c_str());
			return -1;	
		}

		// load using base1(m_hrec1)
		// load using no-lock
		TParRecord* _hrec = (TParRecord*)(m_hshm + m_seghead.recbase1);
		_hrec += m_seghead.recsize;
	    memset(_hrec, 0, sizeof(TParRecord)*capacity);
		memcpy(_hrec, prec.precord, sizeof(TParRecord)*reccnt);	

		// base2 clear
		_hrec = (TParRecord*)(m_hshm + m_seghead.recbase2);
		_hrec += m_seghead.recsize;
		memset(_hrec, 0, sizeof(TParRecord)*capacity);

		/// write index
		memset(&m_parindex, 0, sizeof(TParIndex));
		strncpy(m_parindex.skey, key.c_str(), NAME_LEN_MAX);
		m_parindex.sequence = m_seghead.parcnt;
		m_parindex.reccnt = reccnt;
		m_parindex.extcnt = 0;
		m_parindex.recoff = m_seghead.recsize;
		// wfj: cancel append support
		m_parindex.extoff = 0;		// re-use, for case of refresh & re-load.
		m_parindex.lockid = _ipclck.rqstLock();
		m_parindex.recflag = 1;
		m_parindex.capacity = capacity;
		m_parindex.rinterval = rfInterval; // 60;		//?
		m_parindex.lrtime = time(NULL);
		strcpy(m_parindex.sutime, updatedTime.c_str());
		
		// not to use lock, because should no read-request for new-index data
		memcpy(m_hind+m_seghead.parcnt, &m_parindex, sizeof(TParIndex));
		
		/// update segment head
		m_seghead.parcnt += 1;
		m_seghead.recsize += capacity;
		
		//writeSegHead();
		_ipclck.xlock(HEAD_LOCKID_, 0);
		memcpy(m_hshm, &m_seghead, sizeof(TSegHead));
		_ipclck.xunlock(HEAD_LOCKID_);	
	
		_ipclck.sxunlock(HEAD_LOCKID_);

		__PAR_TRACE_P2("Loaded %s(%d).", key.c_str(), reccnt);
		
		///
		return m_seghead.parcnt-1;	
	} catch ( Exception& er) {
		_ipclck.sxunlock(HEAD_LOCKID_);
		__PAR_TRACE_P2("Load %s failure: %s.", key.c_str(), er.toString().c_str());
		return -1;
		//throw;
	}
}

///// verify whether key is empty outside.
int IpcParam::reload(const string& key, int nreccnt)
{
	__PAR_TRACE_P1("ReLoading %s ...", key.c_str());

	/// 
	try
	{
		_ipclck.sxlock(HEAD_LOCKID_, 0);
	} catch ( Exception& er) {
		__PAR_TRACE_P2("ReLoad %s failure: %s.", key.c_str(), er.toString().c_str());
		return -1;
	} 
	
	//|- for clear memory-before	
	TParIndex  tmpParIndex;
	memset(&tmpParIndex, 0, sizeof(TParIndex));
	memcpy(&tmpParIndex, &m_parindex, sizeof(TParIndex));

	int reccnt = 0;

	try
	{
		///
		__PRECORD prec(nreccnt);
		if (prec.getFromDb(key) <= 0) {
			_ipclck.sxunlock(HEAD_LOCKID_);
			__PAR_TRACE_P1("ReLoad %s failure: get-from-db error or no-data-found.", key.c_str());
			return -1;
		}
		
		int capacity = nreccnt*1.4;
	
		// for index-write: parsize, recsize;
		readSegHead();

		/// segment-capacity check
		if ( (m_seghead.recsize+capacity)*sizeof(TParRecord) >= PAR_SIZE_REC )
		{
			_ipclck.sxunlock(HEAD_LOCKID_);
			__PAR_TRACE_P1("ReLoad %s failure: no enough segment space.", key.c_str());
			return -1;	
		}

		// reload using base1(m_hrec1)
		// reload using no-lock
		TParRecord* _hrec = (TParRecord*)(m_hshm + m_seghead.recbase1);
		_hrec += m_seghead.recsize;
	    memset(_hrec, 0, sizeof(TParRecord)*capacity);
		memcpy(_hrec, prec.precord, sizeof(TParRecord)*nreccnt);	
		
		// base2 clear
		_hrec = (TParRecord*)(m_hshm + m_seghead.recbase2);
		_hrec += m_seghead.recsize;
		memset(_hrec, 0, sizeof(TParRecord)*capacity);

		/// write index
		m_parindex.reccnt = nreccnt;
		m_parindex.recoff = m_seghead.recsize;
		m_parindex.recflag = 1;
		m_parindex.capacity = capacity;

		// reload, should add index-lock
		_ipclck.xlock(INDEX_LOCKID_, 0);
		memcpy(m_hind+m_parindex.sequence, &m_parindex, sizeof(TParIndex));
		_ipclck.xunlock(INDEX_LOCKID_);
		
		/// update segment head
		m_seghead.recsize += capacity;
		
		//writeSegHead(), should add head-lock
		_ipclck.xlock(HEAD_LOCKID_, 0);
		memcpy(m_hshm, &m_seghead, sizeof(TSegHead));
		_ipclck.xunlock(HEAD_LOCKID_);	
	
		///
		_ipclck.sxunlock(HEAD_LOCKID_);

		//|- clear memory-before
		TParRecord* _tmphrec = 0;
		_tmphrec = (TParRecord*)(m_hshm + m_seghead.recbase1);
		_tmphrec += tmpParIndex.recoff;
		memset(_tmphrec, 0, tmpParIndex.capacity);
		
		_tmphrec = (TParRecord*)(m_hshm + m_seghead.recbase2);
		_tmphrec += tmpParIndex.recoff;
		memset(_tmphrec, 0, tmpParIndex.capacity);

		///
		__PAR_TRACE_P2("ReLoaded %s(%d).", key.c_str(), nreccnt);
		return nreccnt;	
	} catch ( Exception& er) {
		_ipclck.sxunlock(HEAD_LOCKID_);
		__PAR_TRACE_P2("ReLoad %s failure: %s.", key.c_str(), er.toString().c_str());
		return -1;
	} 
}

/// inter-use: estimate refresh-interval
int IpcParam::__refresh(const string& key)
{
	__PAR_TRACE_P1("Refreshing %s ... ", key.c_str());	

	////
	int idx = findKey(key);
	if ( idx < 0 )
	{
		__PAR_TRACE_P1("Refresh %s failure: key is unloaded.", key.c_str());
		return -1;
	}

	//// beginning refresh
	if (_ipclck.sxlock(m_parindex.lockid, IPC_NOWAIT) < 0)
	{
		__PAR_TRACE_P1("Refresh %s exit, key is refreshing.", key.c_str());
		return -1;
	}

	try
	{
		//
		memcpy(&m_parindex, (m_hind+idx), sizeof(TParIndex));	
		
		if ( !isRefreshTime() ) 
		{
			_ipclck.sxunlock(m_parindex.lockid);
			__PAR_TRACE_P1("Refresh %s exit, not refresh time.", key.c_str());
			return 0;			
		}
		
		///	
		__DAO dao;
		dao.SetParam(":VKEY", key);
		int reccnt = dao.jcount(key);
		if ( reccnt <= 0 ) 
		{
			_ipclck.sxunlock(m_parindex.lockid);
			__PAR_TRACE_P1("Refresh %s failure: no-data-found.", key.c_str());
			return 0;
		}

		///
		int rfInterval = defRefrshInterval;
		time_t rfTime=time(NULL);
	    string updatedTime(DEFAULT_DATE_TIME);

	    dao.ExecSelect(SQLBY_CPARAM_CONFIG);
	    if ( dao.Fetch() ) 
	    {
	    	rfInterval = dao.GetInt("REFRESH_INTERVAL");
	        updatedTime = dao.GetString("UPDATED_TIME");
	    }
		
		// update m_parindex
		m_parindex.reccnt = reccnt;
		m_parindex.lrtime = rfTime;
		m_parindex.rinterval = rfInterval; // 60;		//?
		strcpy(m_parindex.sutime, updatedTime.c_str());
		
		///
		if (reccnt > m_parindex.capacity) 
		{
			if (reload(key, reccnt) < 0)
			{
				_ipclck.sxunlock(m_parindex.lockid);
				__PAR_TRACE_P1("Refresh %s failure: reload error.", key.c_str());
				return -1;
			}
						
			//
			_ipclck.sxunlock(m_parindex.lockid);
			__PAR_TRACE_P2("Refreshed[reload] %s(%d).", key.c_str(), reccnt);
			return reccnt;		
		}
			
		__PRECORD prec(reccnt);
		if (prec.getFromDb(key) <= 0)
		{
			_ipclck.sxunlock(m_parindex.lockid);
			__PAR_TRACE_P1("Refresh %s failure: get-from-db error or no-data-found.", key.c_str());
			return -1;	
		}
	
		/// write par-records
		TParRecord* _hrec = 0;
		if (m_parindex.recflag == 1)
		{
			_hrec = (TParRecord*)(m_hshm + m_seghead.recbase2);
			m_parindex.recflag = 2;
		}
		else
		{
			_hrec = (TParRecord*)(m_hshm + m_seghead.recbase1);
			m_parindex.recflag = 1;
		}
		_hrec += m_parindex.recoff;
	
		_ipclck.xlock(m_parindex.lockid, 0);
		memcpy(_hrec, prec.precord, sizeof(TParRecord)*reccnt);	
		_ipclck.xunlock(m_parindex.lockid);

		/// refresh par-index
		_ipclck.xlock(INDEX_LOCKID_, 0);
		memcpy(m_hind+idx, &m_parindex, sizeof(TParIndex));
		_ipclck.xunlock(INDEX_LOCKID_);
		
		///
		_ipclck.sxunlock(m_parindex.lockid);
		__PAR_TRACE_P2("Refreshed %s(%d).", key.c_str(), reccnt);
		return reccnt;	
	} catch ( Exception& er) {
		_ipclck.sxunlock(m_parindex.lockid);
		__PAR_TRACE_P2("Refresh %s failure: %s.", key.c_str(), er.toString().c_str());
		return -1;
		//throw;
	} 
}

/// Implementation of interface, force refresh
int IpcParam::refresh(const string& key)
{
	__PAR_TRACE_P1("Refreshing %s ... ", key.c_str());	

	////
	int idx = findKey(key);
	if ( idx < 0 )
	{
		__PAR_TRACE_P1("Refresh %s failure: key is unloaded.", key.c_str());
		return -1;
	}

	//// beginning refresh
	if (_ipclck.sxlock(m_parindex.lockid, IPC_NOWAIT) < 0)
	{
		__PAR_TRACE_P1("Refresh %s exit, key is refreshing.", key.c_str());
		return -1;
	}

	try
	{
		//
		memcpy(&m_parindex, (m_hind+idx), sizeof(TParIndex));	
		
		//// 强制刷新实现, 删掉刷新时间间隔的判断
				
		///	
		__DAO dao;
		dao.SetParam(":VKEY", key);
		int reccnt = dao.jcount(key);
		if ( reccnt <= 0 ) 
		{
			_ipclck.sxunlock(m_parindex.lockid);
			__PAR_TRACE_P1("Refresh %s failure: no-data-found.", key.c_str());
			return 0;
		}

		///
		int rfInterval = defRefrshInterval;
		time_t rfTime=time(NULL);
	    string updatedTime(DEFAULT_DATE_TIME);

	    dao.ExecSelect(SQLBY_CPARAM_CONFIG);
	    if ( dao.Fetch() ) 
	    {
	    	rfInterval = dao.GetInt("REFRESH_INTERVAL");
	        updatedTime = dao.GetString("UPDATED_TIME");
	    }
		
		// update m_parindex
		m_parindex.reccnt = reccnt;
		m_parindex.lrtime = rfTime;
		m_parindex.rinterval = rfInterval; // 60;		//?
		strcpy(m_parindex.sutime, updatedTime.c_str());
		
		///
		if (reccnt > m_parindex.capacity) 
		{
			if (reload(key, reccnt) < 0)
			{
				_ipclck.sxunlock(m_parindex.lockid);
				__PAR_TRACE_P1("Refresh %s failure: reload error.", key.c_str());
				return -1;
			}
						
			//
			_ipclck.sxunlock(m_parindex.lockid);
			__PAR_TRACE_P2("Refreshed[reload] %s(%d).", key.c_str(), reccnt);
			return reccnt;		
		}
			
		__PRECORD prec(reccnt);
		if (prec.getFromDb(key) <= 0)
		{
			_ipclck.sxunlock(m_parindex.lockid);
			__PAR_TRACE_P1("Refresh %s failure: get-from-db error or no-data-found.", key.c_str());
			return -1;	
		}
	
		/// write par-records
		TParRecord* _hrec = 0;
		if (m_parindex.recflag == 1)
		{
			_hrec = (TParRecord*)(m_hshm + m_seghead.recbase2);
			m_parindex.recflag = 2;
		}
		else
		{
			_hrec = (TParRecord*)(m_hshm + m_seghead.recbase1);
			m_parindex.recflag = 1;
		}
		_hrec += m_parindex.recoff;
	
		_ipclck.xlock(m_parindex.lockid, 0);
		memcpy(_hrec, prec.precord, sizeof(TParRecord)*reccnt);	
		_ipclck.xunlock(m_parindex.lockid);

		/// refresh par-index
		_ipclck.xlock(INDEX_LOCKID_, 0);
		memcpy(m_hind+idx, &m_parindex, sizeof(TParIndex));
		_ipclck.xunlock(INDEX_LOCKID_);
		
		///
		_ipclck.sxunlock(m_parindex.lockid);
		__PAR_TRACE_P2("Refreshed %s(%d).", key.c_str(), reccnt);
		return reccnt;	
	} catch ( Exception& er) {
		_ipclck.sxunlock(m_parindex.lockid);
		__PAR_TRACE_P2("Refresh %s failure: %s.", key.c_str(), er.toString().c_str());
		return -1;
		//throw;
	} 
}

int IpcParam::recCnt(const string& key)
{
	if (findKey(key) < 0) 
	{
		return -1;
	}
	
	//
	return m_parindex.reccnt;	
}

int IpcParam::recCopy(const string& key, TParRecord** pp)
{
	if ( findKey(key) < 0 ) {
		__PAR_TRACE_P1("Key: %s not existed.", key.c_str());
		return -1;
	}
	
	TParRecord *_hrec=0, *_hrecext=0;
	if (m_parindex.recflag == 1)
	{
		_hrec = (TParRecord*)(m_hshm + m_seghead.recbase1);
	}
	else
	{
		_hrec = (TParRecord*)(m_hshm + m_seghead.recbase2);
	}
	_hrec += m_parindex.recoff;
	
	//_hrecext = (TParRecord*)(m_hshm + m_seghead.extbase);
	//_hrecext += m_parindex.extoff;
	
	//
	memcpy(*pp, _hrec, sizeof(TParRecord)*m_parindex.reccnt);
	//memcpy(*pp+m_parindex.reccnt, _hrecext, sizeof(TParRecord)*m_parindex.extcnt);
	
	///
	return m_parindex.reccnt;
}

///
string IpcParam::getTrans(const string& key, const string& val1)
{
	///
	TParRecord parRec;
	memset(&parRec, 0, sizeof(TParRecord));
	strncpy(parRec.value1, val1.c_str(), CODE_LEN_MAX);
	strcpy(parRec.value2, NULL_VALUE2);
	strcpy(parRec.value3, NULL_VALUE3);
	
	__getTrans(key, parRec);	
	
	//
	return parRec.sresult;
}

string IpcParam::getTrans(const string& key, const string& val1, const string& val2)
{
	///
	TParRecord parRec;
	memset(&parRec, 0, sizeof(TParRecord));
	strncpy(parRec.value1, val1.c_str(), CODE_LEN_MAX);
	strncpy(parRec.value2, val2.c_str(), CODE_LEN_MAX);
	strcpy(parRec.value3, NULL_VALUE3);

	__getTrans(key, parRec);	
	
	//
	return parRec.sresult;
}

string IpcParam::getTrans(const string& key, const string& val1, const string& val2, const string& val3)
{
	///
	TParRecord parRec;
	memset(&parRec, 0, sizeof(TParRecord));
	strncpy(parRec.value1, val1.c_str(), CODE_LEN_MAX);
	strncpy(parRec.value2, val2.c_str(), CODE_LEN_MAX);
	strncpy(parRec.value3, val3.c_str(), CODE_LEN_MAX);
	
	__getTrans(key, parRec);	
	
	//
	return parRec.sresult;
}


////
void IpcParam::readSegHead()
{
	_ipclck.slock(HEAD_LOCKID_, 0);
	memset(&m_seghead, 0, sizeof(TSegHead));
	memcpy(&m_seghead, m_hshm, sizeof(TSegHead));
	_ipclck.sunlock(HEAD_LOCKID_);
}

int IpcParam::findKey(const string& key)
{
	/// read and get index	
	int idx = 0;	
	memset(&m_parindex, 0, sizeof(TParIndex));

	_ipclck.slock(INDEX_LOCKID_, 0);

	/// for index-reading: parsize;
	readSegHead();

	for (idx=0; idx<m_seghead.parcnt; idx++)
	{
		if (strcmp((m_hind+idx)->skey, key.c_str()) == 0)
		{
			memcpy(&m_parindex, (m_hind+idx), sizeof(TParIndex));
			
			break;
		}
	}

	_ipclck.sunlock(INDEX_LOCKID_);
	
	return (idx>=m_seghead.parcnt) ? -1 : idx;
}


/// default is true
/// DEFAULT_DATE_TIME or large than last updated time is true
/// Configed but not DEFAULT_DATE_TIME is false
bool IpcParam::isUpdated()
{
    __DAO dao;
    dao.SetParam(":VKEY", m_parindex.skey);
    dao.ExecSelect(SQLBY_CPARAM_CONFIG);

    if ( dao.Fetch() ) {
        string updatedTime = dao.GetString("UPDATED_TIME");

	    if ( (strcmp(updatedTime.c_str(), DEFAULT_DATE_TIME)==0) ||  
	       (strcmp(updatedTime.c_str(), m_parindex.sutime)>0) )
	    {
	        return true;
	    }
	    else
	    {
	    	return false;
	    }
    }    
    
    //
    //return false;
    return true;
}

/// < 0  is false, no-refresh for ever
/// == 0 is true, refresh real-time
/// elapsed time large than interval is true, refresh
bool IpcParam::isRefreshTime()
{
	if (m_parindex.rinterval < 0) 
	{
		return false;
	} 
	else if (m_parindex.rinterval == 0)
	{
		return true;
	}
		
	int curTime = time(NULL);
	
    int elapsedMins = (curTime - m_parindex.lrtime)/60;
    
    return (elapsedMins >= m_parindex.rinterval) ? true : false;
}


/// 索引的index在load之后是固化的，不会改变
int IpcParam::appendExt(TParRecord& rec)
{
	__PAR_TRACE_P1("Appending %s ...", m_parindex.skey);

	_ipclck.sxlock(m_parindex.lockid, 0);
	
	try
	{
		//有sxlock()保证，不可能有对本索引数据的写操作，所以不用加共享锁
		memcpy(&m_parindex, (m_hind+m_parindex.sequence), sizeof(TParIndex));
				
		///
		TParRecord* _hrecext = (TParRecord*)(m_hshm + m_seghead.extbase);
		_hrecext += m_parindex.extoff;
		
		//_ipclck.xlock(m_parindex.lockid, 0);
		memcpy(_hrecext+m_parindex.extcnt, &rec, sizeof(TParRecord));	
		//_ipclck.xunlock(m_parindex.lockid);

		///
		m_parindex.extcnt += 1;
		
		_ipclck.xlock(INDEX_LOCKID_, 0);
		memcpy(m_hind+m_parindex.sequence, &m_parindex, sizeof(TParIndex));
		_ipclck.xunlock(INDEX_LOCKID_);
		
		///
		_ipclck.sxunlock(m_parindex.lockid);
		
		char _trcMsg[512]="";
		sprintf(_trcMsg, "%s(%s,%s,%s)", m_parindex.skey, rec.value1, rec.value2, rec.value3);
		__PAR_TRACE_P1("Appended %s.", _trcMsg);

		return m_parindex.extcnt-1;
	} catch	(Exception& er) {
		_ipclck.sxunlock(m_parindex.lockid);
		__PAR_TRACE_P2("Append %s failure: %s.", m_parindex.skey, er.toString().c_str());
		//return -1;
		throw;
	}
	
}

int IpcParam::__getTrans(const string& key, TParRecord& rec)
{
	/// 如还未加载则加载，如加载失败则退出
	if ( (findKey(key) < 0) && (load(key) < 0) ) {
		__PAR_TRACE_P1("Key: %s not existed.", key.c_str());
		return -1;
	}

	/// 主动刷新
	if ( isRefreshTime() && isUpdated() ) 
	{
		__refresh(key);
	}

	TParRecord *_hrec=0;
	if (m_parindex.recflag == 1) {
		_hrec = (TParRecord*)(m_hshm + m_seghead.recbase1);
	} else {
		_hrec = (TParRecord*)(m_hshm + m_seghead.recbase2);
	}
	_hrec += m_parindex.recoff;
	
	///
	strcpy(rec.sresult, "");	
	if (_hrec == NULL) return -1;

	/// 
	_ipclck.slock(m_parindex.lockid);
	int pos = _ParamHelper::binsrch(_hrec, m_parindex.reccnt, rec);
	_ipclck.sunlock(m_parindex.lockid);
	
	///
	if (pos < 0 && isRefreshTime() && __refresh(key)>0)
	{
		if (m_parindex.recflag == 1) {
			_hrec = (TParRecord*)(m_hshm + m_seghead.recbase1);
		} else {
			_hrec = (TParRecord*)(m_hshm + m_seghead.recbase2);
		}
		_hrec += m_parindex.recoff;

		_ipclck.slock(m_parindex.lockid, 0);
		pos = _ParamHelper::binsrch(_hrec, m_parindex.reccnt, rec);
		_ipclck.sunlock(m_parindex.lockid);
	}
	
	//
	return pos;	
}

///◆ mode-2
int IpcParam::beginLock2(const string& key)
{
	if ( (findKey(key) < 0) && (load(key) < 0) )
	{
		__PAR_TRACE_P1("Key: %s not existed.", key.c_str());
		return -1;
	}

	/// 主动刷新
	if ( isRefreshTime() && isUpdated() ) 
	{
		__refresh(key);
	}
		
	//_ipclck.slock(m_parindex.lockid);

	if (m_parindex.recflag == 1) {
		m_hrec2 = (TParRecord*)(m_hshm + m_seghead.recbase1);
	} else {
		m_hrec2 = (TParRecord*)(m_hshm + m_seghead.recbase2);
	}
	m_hrec2 += m_parindex.recoff;

	//m_hext2 = (TParRecord*)(m_hshm + m_seghead.extbase);
	//m_hext2 += m_parindex.extoff;

	//
	return m_parindex.lockid;
}

int IpcParam::endLock2(int lockid)
{
	//_ipclck.sunlock(lockid);

	return 0;		
}
	
string IpcParam::getTrans2(const string& val1)
{
	///
	TParRecord parRec;
	memset(&parRec, 0, sizeof(TParRecord));
	strncpy(parRec.value1, val1.c_str(), CODE_LEN_MAX);
	strcpy(parRec.value2, NULL_VALUE2);
	strcpy(parRec.value3, NULL_VALUE3);
	
	__getTrans2(parRec);	
	
	//
	return parRec.sresult;
}

string IpcParam::getTrans2(const string& val1, const string& val2)
{
	///
	TParRecord parRec;
	memset(&parRec, 0, sizeof(TParRecord));
	strncpy(parRec.value1, val1.c_str(), CODE_LEN_MAX);
	strncpy(parRec.value2, val2.c_str(), CODE_LEN_MAX);
	strcpy(parRec.value3, NULL_VALUE3);

	__getTrans2(parRec);	
	
	//
	return parRec.sresult;
}

string IpcParam::getTrans2(const string& val1, const string& val2, const string& val3)
{
	///
	TParRecord parRec;
	memset(&parRec, 0, sizeof(TParRecord));
	strncpy(parRec.value1, val1.c_str(), CODE_LEN_MAX);
	strncpy(parRec.value2, val2.c_str(), CODE_LEN_MAX);
	strncpy(parRec.value3, val3.c_str(), CODE_LEN_MAX);
	
	__getTrans2(parRec);	
	
	//
	return parRec.sresult;
}

int IpcParam::__getTrans2(TParRecord& rec)
{
	strcpy(rec.sresult, "");
	
	//
	if (m_hrec2 == NULL) return -1;
	
	_ipclck.slock(m_parindex.lockid);
	int pos = _ParamHelper::binsrch(m_hrec2, m_parindex.reccnt, rec);	
	_ipclck.sunlock(m_parindex.lockid);
	
	//
	return pos;	
}


///◆ 维护功能
int IpcParam::status(ostream* osptr)
{
    time_t _ttnow;
    struct timeval  _tvnow;

    time(&_ttnow);
    gettimeofday(&_tvnow, NULL);

    struct tm* _tmnow = localtime(&_ttnow);
    char _snow[50];
    sprintf(_snow, "%4d年%02d月%02d日  %02d:%02d:%02d",
             _tmnow->tm_year+1900, _tmnow->tm_mon+1, _tmnow->tm_mday,
             _tmnow->tm_hour, _tmnow->tm_min, _tmnow->tm_sec);
	
	//
	readSegHead();
	
	*osptr << "\n======================================================" << endl;
	*osptr << "GeneratedTime: " << _snow << endl;
	*osptr << "\nSegment(Base: " << m_seghead.shmbase << "; Size: " << m_seghead.shmsize << "):";
	*osptr << "\n\tindbase : " << m_seghead.indbase << ";";
	*osptr << "\n\tparsize : " << m_seghead.parcnt << ";";
	*osptr << "\n\trecbase1: " << m_seghead.recbase1 << ";";
	*osptr << "\n\trecbase2: " << m_seghead.recbase2 << ";";
	*osptr << "\n\trecsize : " << m_seghead.recsize << ";";
	*osptr << "\n\textbase : " << m_seghead.extbase << ";";
	*osptr << "\n\textsize : " << m_seghead.extsize << ";";
	*osptr << "\n\tresvn1  : "  << m_seghead.resvn1  << ";";
	*osptr << "\n\tresvn2  : "  << m_seghead.resvn2  << ";";
	*osptr << "\n\tresvc1  : "  << m_seghead.resvc1  << ";";
	*osptr << "\n======================================================" << endl;
	*osptr << endl;
	
	
	TParIndex* _tmpindex = new TParIndex[m_seghead.parcnt+2];
	try
	{	
		_ipclck.slock(INDEX_LOCKID_, 0);
		memcpy(_tmpindex, m_hind, sizeof(TParIndex)*m_seghead.parcnt);	
		_ipclck.sunlock(INDEX_LOCKID_);
	
		char smsg[200];
		for (int i=0; i<m_seghead.parcnt; ++i)
		{
			*osptr << "\nKEY: " << _tmpindex[i].skey << "(" << _tmpindex[i].sequence << ")";
			*osptr << "; RecOff: " << _tmpindex[i].recoff;
			*osptr << "; RecCnt: " << _tmpindex[i].reccnt; 
			*osptr << "; ExtOff: " << _tmpindex[i].extoff;
			*osptr << "; ExtCnt: " << _tmpindex[i].extcnt << endl;	
			*osptr << "======================================================================" << endl;
	
			sprintf(smsg, "\t lockid: %2d; lrtime: %d; rinterval: %d, capacity: %d",
				  _tmpindex[i].lockid, _tmpindex[i].lrtime, _tmpindex[i].rinterval, _tmpindex[i].capacity);
			*osptr << smsg << endl;
			sprintf(smsg, "\trecflag: %2d; sutime: %s",
				 _tmpindex[i].recflag, _tmpindex[i].sutime);
			*osptr << smsg << endl;
			
			*osptr << endl;		
		}
	} catch (Exception& er) {
		*osptr << "Status failure: " << er.toString() << endl;
		delete [] _tmpindex;
		return -1;
	}
			
	///
	delete [] _tmpindex;	
	return m_parindex.reccnt;
	
}

int IpcParam::dump(const string& key, ostream* osptr)
{
	///
	if ( findKey(key) < 0 ) {
		*osptr << "dump failure[findKey(key)]." << endl;
		return -2;
	}
	
	TParRecord *_hrec, *_hrecext;
	if (m_parindex.recflag == 1)
	{
		_hrec = (TParRecord*)(m_hshm + m_seghead.recbase1);
	}
	else
	{
		_hrec = (TParRecord*)(m_hshm + m_seghead.recbase2);
	}
	_hrec += m_parindex.recoff;
	
	_hrecext = (TParRecord*)(m_hshm + m_seghead.extbase);
	_hrecext += m_parindex.extoff;

	///
	_ipclck.slock(m_parindex.lockid);
	
	*osptr << "\n======================================================" << endl;
	*osptr << "\nKEY: " << key << "; RecCnt: " << m_parindex.reccnt; 
	*osptr << ", ExtCnt: " << m_parindex.extcnt << endl;	
	*osptr << "======================================================" << endl;
	
	*osptr << "\n\tsequence , " << m_parindex.sequence << ";";
	*osptr << "\n\tlockid   , " << m_parindex.lockid   << ";";
	*osptr << "\n\trecflag  , " << m_parindex.recflag  << ";";
	*osptr << "\n\treccnt   , " << m_parindex.reccnt   << ";";
	*osptr << "\n\trecoff   , " << m_parindex.recoff   << ";";
	*osptr << "\n\textcnt   , " << m_parindex.extcnt   << ";";
	*osptr << "\n\textoff   , " << m_parindex.extoff   << ";";
	*osptr << "\n\tcapacity , " << m_parindex.capacity << ";";
	*osptr << "\n\trinterval, " << m_parindex.rinterval << ";";
	*osptr << "\n\tlrtime   , " << m_parindex.lrtime   << ";";
	*osptr << "\n\tsutime   , " << m_parindex.sutime   << ";";
	*osptr << "\n\tresvn1   , " << m_parindex.resvn1   << ";";
	*osptr << "\n\tresvc1   , " << m_parindex.resvc1   << ";";
	*osptr << endl;

	*osptr << "\n\t---------------------------------------------------------------------" << endl;
	for (int i=0; i<m_parindex.reccnt; i++)
	{
		*osptr << "\tV1:"   << (_hrec+i)->value1;
		*osptr << "\t\tV2:" << (_hrec+i)->value2;
		*osptr  << "\t\tV3:"<< (_hrec+i)->value3;
		
		*osptr << "\tRESULT:" << (_hrec+i)->sresult << endl;
	}	
	
	*osptr << "\n\t---------------------------------------------------------------------" << endl;
	for (int i=0; i<m_parindex.extcnt; i++)
	{
		*osptr << "\tV1:"   << (_hrecext+i)->value1;
		*osptr << "\t\tV2:" << (_hrecext+i)->value2;
		*osptr  << "\t\tV3:"<< (_hrecext+i)->value3;
		
		*osptr << "\tRESULT:" << (_hrecext+i)->sresult << endl;
	}	
	
	_ipclck.sunlock(m_parindex.lockid);
	
	//
	return m_parindex.reccnt;			
}

////
TSegHead IpcParam::headinfo(int outflag, ostream* osptr)
{
	//
	readSegHead();
	
	if ( outflag > 0 )
	{
	    time_t _ttnow;
	    struct timeval  _tvnow;
	
	    time(&_ttnow);
	    gettimeofday(&_tvnow, NULL);
	
	    struct tm* _tmnow = localtime(&_ttnow);
	    char _snow[50];
	    sprintf(_snow, "%4d年%02d月%02d日  %02d:%02d:%02d",
	             _tmnow->tm_year+1900, _tmnow->tm_mon+1, _tmnow->tm_mday,
	             _tmnow->tm_hour, _tmnow->tm_min, _tmnow->tm_sec);
		
		*osptr << "\n======================================================" << endl;
		*osptr << "GeneratedTime: " << _snow << endl;
		
		*osptr << "\nIPC Information";
		*osptr << "(KEY: " << CPAR_IPCKEY_ << "):";
		*osptr << "\n\tIPC_SHMSIZE : " << CPAR_SHMSIZE_ ;
		*osptr << "\n\tIPC_LCKCNT  : " << CPAR_LCKCNT_  ;
		*osptr << "\n\tIPC_SHMBASE : " << PAR_SHMBASE_;
		*osptr << "\n\tIPC_PAR_HEAD: " << PAR_SIZE_HEAD ;
		*osptr << "\n\tIPC_PAR_IND : " << PAR_SIZE_IND ;
		*osptr << "\n\tIPC_PAR_REC : " << PAR_SIZE_REC ;
		*osptr << "\n\tIPC_PAR_EXT : " << PAR_SIZE_EXT ;
		*osptr << endl;
		
		*osptr << "\nCParam's Segment Information";
		*osptr << "(Base: " << m_seghead.shmbase << "; Size: " << m_seghead.shmsize << "):";
		*osptr << "\n\tindbase : " << m_seghead.indbase << ";";
		*osptr << "\n\tparsize : " << m_seghead.parcnt << ";";
		*osptr << "\n\trecbase1: " << m_seghead.recbase1 << ";";
		*osptr << "\n\trecbase2: " << m_seghead.recbase2 << ";";
		*osptr << "\n\trecsize : " << m_seghead.recsize << ";";
		*osptr << "\n\textbase : " << m_seghead.extbase << ";";
		*osptr << "\n\textsize : " << m_seghead.extsize << ";";
		*osptr << "\n\ttrace   : " << m_seghead.trace << ";";
		*osptr << "\n\tresvn1  : "  << m_seghead.resvn1  << ";";
		*osptr << "\n\tresvn2  : "  << m_seghead.resvn2  << ";";
		*osptr << "\n\tresvc1  : "  << m_seghead.resvc1  << ";";
		*osptr << "\n======================================================" << endl;
		*osptr << endl;
	}
		
	return m_seghead;
}

TParIndex IpcParam::keyinfo(const string& key, int outflag, ostream* osptr)
{
	//
	if (findKey(key) < 0) 
	{
		return m_parindex;
	}

	if ( outflag > 0 )
	{	
	    time_t _ttnow;
	    struct timeval  _tvnow;
	
	    time(&_ttnow);
	    gettimeofday(&_tvnow, NULL);
	
	    struct tm* _tmnow = localtime(&_ttnow);
	    char _snow[50];
	    sprintf(_snow, "%4d年%02d月%02d日  %02d:%02d:%02d",
	             _tmnow->tm_year+1900, _tmnow->tm_mon+1, _tmnow->tm_mday,
	             _tmnow->tm_hour, _tmnow->tm_min, _tmnow->tm_sec);
		
		*osptr << "\n======================================================" << endl;
		*osptr << "GeneratedTime: " << _snow << endl;
	
		char smsg[200];
		*osptr << "\nKEY: " << m_parindex.skey << "(" << m_parindex.sequence << ")";
		*osptr << "; RecOff: " << m_parindex.recoff;
		*osptr << "; RecCnt: " << m_parindex.reccnt; 
		*osptr << "; ExtOff: " << m_parindex.extoff;
		*osptr << "; ExtCnt: " << m_parindex.extcnt << endl;	
		*osptr << "======================================================================" << endl;
	
		sprintf(smsg, "\t lockid: %2d; lrtime: %d; rinterval: %d, capacity: %d",
			  m_parindex.lockid, m_parindex.lrtime, m_parindex.rinterval,m_parinde