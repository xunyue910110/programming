#ifndef __IPCPARAM_H_
#define __IPCPARAM_H_

#include <string>

#include "IpcLocal.h"
#include "IIpcParam.h"
#include "IpcParamdef.h"

using std::string;
using namespace budeipc;

/// 编译控制宏定义
//#define OFF_PAR_TRACE		// 参数维护跟踪日志开关


/// 参数宏定义
#define CPAR_IPCKEY_   IPCINI::intValue("IPC_KEY") //2934567
#define CPAR_SHMSIZE_  IPCINI::intValue("IPC_SHMSIZE") //400*1024*1024 // 大于PAR_SHMSIZE_ + PAR_SHMBASE_

#define CPAR_LCKCNT_   IPCINI::intValue("IPC_LCKCNT")  //300

///
#define PAR_SHMBASE_   IPCINI::intValue("IPC_SHMBASE") //0

/// size of segment-head, index, records and records-ext;
#define PAR_SIZE_HEAD IPCINI::intValue("IPC_PAR_HEAD") //2*1024;
#define PAR_SIZE_IND  IPCINI::intValue("IPC_PAR_IND") //500*1024;
#define PAR_SIZE_REC  IPCINI::intValue("IPC_PAR_REC") //100*1024*1024;
#define PAR_SIZE_RECX IPCINI::intValue("IPC_PAR_RECX") //100*1024*1024;
#define PAR_SIZE_EXT  IPCINI::intValue("IPC_PAR_EXT") //100*1024*1024;

const int PAR_SHMSIZE_ = PAR_SIZE_HEAD + PAR_SIZE_IND + PAR_SIZE_REC + PAR_SIZE_RECX + PAR_SIZE_EXT;

/// base of index, records and records-ext
const int PAR_BASE_IND  = PAR_SIZE_HEAD;
const int PAR_BASE_REC1 = PAR_BASE_IND + PAR_SIZE_IND;
const int PAR_BASE_REC2 = PAR_BASE_REC1 + PAR_SIZE_REC;
const int PAR_BASE_EXT  = PAR_BASE_REC2 + PAR_SIZE_RECX;

///
#define HEAD_LOCKID_   1	// 参数segment head 锁
#define INDEX_LOCKID_  2	// 参数索引段访问&维护锁

///
#define DEFAULT_DATE_TIME  "19700101000000"
#define SQLBY_CPARAM_CONFIG "CParamConfig"		// 参数配置信息访问SQL_REF
const int defRefrshInterval = 600;	// 缺省参数刷新时间间隔(分钟)

/**
 *
 */
namespace budeipc {
	class IpcShm;
	class IpcLock;
}
 
/**
 *
 */
class IpcParam : public IIpcParam {
public:

	///
	IpcParam();
	virtual ~IpcParam();
	
	///
	// return index of key
	virtual int load(const string& key);
	// return par-record count of key
	virtual int refresh(const string& key);
	
	virtual int status(ostream* osptr = &cout);
	virtual int dump(const string& key, ostream* osptr = &cout);

	///
	virtual TSegHead headinfo(int outflag = 0, ostream* osptr = &cout);
	virtual TParIndex keyinfo(const string& key, int outflag = 0, ostream* osptr = &cout);

	virtual int recCnt(const string& key);
	virtual int recCopy(const string& key, TParRecord** pp);
	
	///
	virtual string getTrans(const string& key, const string& val1);
	virtual string getTrans(const string& key, const string& val1, const string& val2);
	virtual string getTrans(const string& key, const string& val1, const string& val2, const string& val3);
	
	virtual int beginLock2(const string& key);
	virtual int endLock2(int lockid);
	virtual string getTrans2(const string& val1);
	virtual string getTrans2(const string& val1, const string& val2);
	virtual string getTrans2(const string& val1, const string& val2, const string& val3);
	
protected:
	///
	void readSegHead();

	// return seq of key 
	int findKey(const string& key);
	
	bool isUpdated();
	bool isRefreshTime();
	//
	int __refresh(const string& key);
	
	int reload(const string& key, int nreccnt);
		
	///
	int appendExt(TParRecord& rec);
	
	int __getTrans(const string& key, TParRecord& rec);
	int __getTrans2(TParRecord& rec);
	
private:

	budeipc::IpcShm& _ipcshm;
	budeipc::IpcLock& _ipclck;

	char* m_hshm_at;
	char* m_hshm;
	
	//
	TParIndex* m_hind;
	
	//
	TSegHead m_seghead;
	
	//
	TParIndex m_parindex;
	
	// for circle case
	TParRecord* m_hrec2;
	TParRecord* m_hext2;

};


#endif
