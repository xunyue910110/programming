//  ############################################
//  Source file : ReleaseRCApp.cpp
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-5-28 14:21:47
//  Update      : 2008-5-28 14:21:50
//  Copyright(C): chen min, Linkage.
//  ############################################

#include "ReleaseRCApp.h"
#include "base/SysParam.h"
#include "appfrm/ErrorHandle.h"
#include "appfrm/VersionInfo.h"
#include "Mdb_Exception.h"
#include "Table.h"
#include <sys/errno.h>

#include <assert.h>
#include <unistd.h>

ReleaseRCApp g_application;
Application * const theApp = &g_application;

//##ModelId=3BCAA280003A
bool ReleaseRCApp::initialization()
{
    setoptstr("c:");
   
    // 设置版本信息
    setMdbServerVersion();
   	
    // normally, call standard initialization first
    if (!Application::initialization())
   		return false;

#ifndef _DEBUG_
    set_asdaemon(true);
    set_onlyone(true);
    set_runinbkg(true);
#endif
	//-----------------------------------------------------------------------//
	// ADD YOUR INITIALIZATION CODE HERE
	//-----------------------------------------------------------------------//
	char channelNo[4];
	string root,sPath,sParamName,sParamValue;

	root=SECTDELIM+string(m_name);
	sprintf(channelNo,"%d",m_channelNo);
	sPath = root + SECTDELIM + channelNo ;
	
	sParamName="local_mdb_name";
	if (!m_theSysParam->getValue(sPath, sParamName, sParamValue)) 
	{
		m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
		return false;
	}
	strcpy(m_dbName,sParamValue.c_str());
	
	sParamName="sleep_time";
	if (!m_theSysParam->getValue(sPath, sParamName, sParamValue)) 
	{
		m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
		return false;
	}
	m_iSleepTime = atoi( sParamValue.c_str() );
	
	sParamName="COMMAND";
	if (!m_theSysParam->getValue(sPath, sParamName, m_sCommand)) 
	{
		m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
		return false;
	}
	m_sCommand = "ps -ef | grep %d | grep -v grep | awk '{print $2}'";
	
	try
	{
		m_pDB      = InstanceFactory::getDBInstance(MDB,m_dbName,"",0);
		m_pSession = m_pDB->connect();
		m_pMemMgr  = m_pDB->getMemMgrInstance(); 
	}
	catch(LINKAGE_MDB::Mdb_Exception e)
	{
		if( m_pDB != NULL )
		{
			if( m_pSession != NULL )
			{
				m_pDB->disconnect(m_pSession);
			}
			delete m_pDB;
		}
		m_theErrorMessages->insert(EAPPFRM_SYSVAL,e.GetString());
		return false;	
	}

	return true;
}


//##ModelId=3BCAA28000B2
bool ReleaseRCApp::end()
{
	
    if( m_pDB != NULL )
	{
		if( m_pSession != NULL )
		{
			m_pDB->disconnect(m_pSession);
		}
		delete m_pDB;
	}
    
    return Application::end();
}

//##ModelId=3BCAA28000C6
bool ReleaseRCApp::loopProcess()
{
	try
	{
		this->run();	
	}
	catch(LINKAGE_MDB::Mdb_Exception &e)
	{
		e.toString();
		m_theErrorMessages->insert(EAPPFRM_SYSVAL,e.GetString());
	}
	sleep(m_iSleepTime);
    return true;
}

//-------------------------------------------------------------------------//
// TODO: Add your owner member function here
//-------------------------------------------------------------------------//
void ReleaseRCApp::setMdbServerVersion()
{
	VersionInfo theVersionInfo;
	
	theVersionInfo.m_date   = "2009-2-10";
	theVersionInfo.m_main   = 1;
	theVersionInfo.m_mini   = 2;
	theVersionInfo.m_patch  = 5;
	theVersionInfo.m_comment= "create new release";
	
	setVersion(theVersionInfo);
	
	return;	
}

// 删除进程不在的session信息,同时,检查Lock_manager表中,是否有相应的记录锁,如果有,则删除释放之
void ReleaseRCApp::run()
{
	vector<SessionInfo> sessionInfos;
    vector<SessionInfo>::iterator itr;
     
    m_pMemMgr->getSessionInfos(sessionInfos);
 
	for(itr=sessionInfos.begin();itr!=sessionInfos.end();++itr)
    {
    	if(getpid()==itr->m_pId)
    	{
    	    //是当前进程自身
    	    continue;
    	}
    	if(isPidExists(itr->m_pId))
    	{
    	    //进程仍然活着，不能杀掉他的session
    	    continue;
    	}
    	
    	// 对于异常退出的session,进行回滚操作(主要是清除lockmanager表里的记录锁)
        #ifdef _DEBUG_
          cout << "rollBack " << itr->m_pId << endl;
        #endif
        
    	this->rollBack(*itr);
    	
    	#ifdef _DEBUG_
          cout << "rollBacked " << endl;
        #endif
        
        
        #ifdef _DEBUG_
          cout << "unRegisterSession " << itr->m_pId << endl;
        #endif
        
    	m_pMemMgr->unRegisterSession(*itr);
    	
        #ifdef _DEBUG_
          cout << "unRegisterSessioned " << endl;
        #endif
	}     
}

bool ReleaseRCApp::isPidExists(const int iPid)
{
    char cmd[200];
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, m_sCommand.c_str(), iPid);
    
    FILE * handle;
    
    if((handle = popen(cmd,"r")) == NULL)
    {
        throw Mdb_Exception(__FILE__, __LINE__,"pipe open error:%s",strerror(errno));
    }
	fflush(handle);

	
	bool flag=false;
	char line[300];
    memset(line, 0, sizeof(line));
    
	while(fgets(line, sizeof(line), handle) != (char *)NULL)
	{
		if(atoi(line)==iPid)
		{
		    flag = true;
		    break;
		}
		memset(line, 0, sizeof(line));
	}
	
    if( pclose(handle) < 0)
    {
        throw Mdb_Exception(__FILE__, __LINE__,"pipe close error:%s",strerror(errno));
    }
    
	return flag;	  
}  

void ReleaseRCApp::rollBack(SessionInfo &sessionInfo)
{
	vector<TableDef>    tableDefs;
	m_pMemMgr->getTableDefList(tableDefs);	
	
	Table *pTable;
	for(int i=0;i<tableDefs.size();++i)
	{
		// 对于非lock表 进行回滚操作
		if(strncmp(tableDefs[i].m_tableName,"LOCK",4)!=0)
		{
            #ifdef _DEBUG_
              cout << "rollBack table= " << tableDefs[i] << endl;
            #endif
			pTable = new Table(m_dbName,tableDefs[i].m_tableName,m_pMemMgr);
			pTable->initialize();
			pTable->rollback(sessionInfo.m_sessionId);	
			delete pTable;
            #ifdef _DEBUG_
              cout << "table " << tableDefs[i] << "rollBacked" << endl;
            #endif
		}
	}
}
