#ifndef PARAMMANAGER_H_INCLUDED_C46A585A
#define PARAMMANAGER_H_INCLUDED_C46A585A

#include "base/config-all.h"

#include "dbparam/PO_Common.h"
#include "dbparam/ParamContainerBase.h"
#include "dbparam/ParamContainer.h"
#include "dbparam/ParamContainerNext.h"
#include "dbparam/ParamContainerChild.h"
#include "dbi/dbi.h"

#include <stdio.h>
#include <unistd.h>
#include <list>
#include <sys/stat.h>

#define GET_DB_FLAG		1
#define GET_DB_TIME		2
#define GET_FILE_FLAG	4
#define GET_FIRST		8
#define GET_FILE_TIME	16

#define DEFAULTINTERVAL	600

#define RC_BASE			100
#define RM_BASE			string("Error message:")
#define RC_GENERAL		(RC_BASE+1)
#define RM_GENERAL		RM_BASE+"general error occurred."
#define RC_CONN_FAIL	(RC_BASE+2)
#define RM_CONN_FAIL	RM_BASE+"dbinterface.connect() failed."
#define RC_BIND_FAIL	(RC_BASE+3)
#define RM_BIND_FAIL	RM_BASE+"dbinterface.dataBind() failed."
#define RC_EXEC_FAIL	(RC_BASE+4)
#define RM_EXEC_FAIL	RM_BASE+"dbinterface.executeSql() failed."
#define RC_PREP_FAIL	(RC_BASE+5)
#define RM_PREP_FAIL	RM_BASE+"ParamContainer.prepareSize() failed."
#define RC_SYSC_FAIL	(RC_BASE+6)
#define RM_SYSC_FAIL	RM_BASE+"system() failed."
#define RC_PIPE_FAIL	(RC_BASE+7)
#define RM_PIPE_FAIL	RM_BASE+"popen() failed."
#define RC_DATI_FAIL	(RC_BASE+8)
#define RM_DATI_FAIL	RM_BASE+"dataIn() failed."

//char *truepath(const char *path);


class ParamManager
{
  public:
    bool initialization();

	~ParamManager();
	
	virtual bool addParamObject();

    bool end();

    bool needUpdate();

    virtual bool getData();

    bool update(const int mode=-1);

	bool setUpdateInterval(const int second);

    bool dump(ostream &os);

	int errorCode();
	string errorMessage();

	void setHostName(const string hostName);
	void setAppName(const string appName);
	void setChannelNo(const int channelNo);
	// 从应用框架中得到数据库联接信息
	void setDbConnInfo(const string &s_userName,const string &s_passWord,
                       const string &s_servName);
    //modified by shandy 20060712
    char *truepath(const char *path);
    //added by shandy 20060712
    //获取文件类型的参数的最小的更新时间
    time_t getUpdateMinTime();
  private:
    //added by shandy 20060712
    //获取文件修改时间
    //@param fname 输入的文件名，含路径信息
    time_t getFileModifyTime(const char* fname);    
  protected:
	DbInterface dbinterface;

    list<ParamContainerBase*> m_paramContainerList;
    list<ParamContainerBase*>::iterator m_paramContainerListItr;
	
	string updateTimeStamp;
	string lastUpdateTimeStamp;
	time_t updateIntervalTime;
	time_t lastUpdateTime;

	int getMode;

	int m_errorCode;
	string m_errorMessage;

	string m_hostName;
	string m_appName;
	int m_channelNo;

	string m_userName;
	string m_passWord;
	string m_servName;
	int connected;
    char truepathBuffer[500];
};


#endif /* PARAMMANAGER_H_INCLUDED_C46A585A */

