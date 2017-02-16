//  ############################################
//  Source file : MdbServerApp.cpp
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-5-28 14:21:47
//  Update      : 2008-5-28 14:21:50
//  Copyright(C): chen min, Linkage.
//  ############################################

#include "MdbServerApp.h"
#include "base/SysParam.h"
#include "appfrm/ErrorHandle.h"
#include "appfrm/VersionInfo.h"
#include "Mdb_Exception.h"
#include "Logger.h"

#include <assert.h>
#include <unistd.h>

MdbServerApp g_application;
Application * const theApp = &g_application;

//##ModelId=3BCAA280003A
bool MdbServerApp::initialization()
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
	string root,sPath,sParamName,sParamValue,sDbName;
	int iPort,iMaxConnect;

	root=SECTDELIM+string(m_name);
	sprintf(channelNo,"%d",m_channelNo);
	sPath = root + SECTDELIM + channelNo ;
	
	sParamName="local_mdb_name";
	if (!m_theSysParam->getValue(sPath, sParamName, sParamValue)) 
	{
		m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
		return false;
	}
	sDbName = sParamValue;
	
	sParamName="listen_port";
	if (!m_theSysParam->getValue(sPath, sParamName, sParamValue)) 
	{
		m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
		return false;
	}
	iPort = atoi( sParamValue.c_str() );

	sParamName="max_connect_num";
	if (!m_theSysParam->getValue(sPath, sParamName, sParamValue)) 
	{
		m_theErrorMessages->insert(EAPPFRM_SYSVAL,sPath + SECTDELIM + sParamValue);
		return false;
	}
	iMaxConnect = atoi( sParamValue.c_str() );

	try
	{
		m_pMdbServer = new MdbServer();
		m_pMdbServer->initialize(sDbName.c_str(),iPort,iMaxConnect);
		// add by chenm 2009-5-21 21:20:00
		Logger *pLogger = Logger::getInstance();
		pLogger->setChannelNo(m_channelNo);
		// over 2009-5-21 21:20:04	
	}
	catch(LINKAGE_MDB::Mdb_Exception e)
	{
		m_theErrorMessages->insert(EAPPFRM_SYSVAL,e.GetString());
		if(m_pMdbServer!=NULL)
			delete m_pMdbServer;
		return false;	
	}

	return true;
}


//##ModelId=3BCAA28000B2
bool MdbServerApp::end()
{
	m_pMdbServer->end();
	
    if(m_pMdbServer!=0)
      delete m_pMdbServer;
    
    return Application::end();
}

//##ModelId=3BCAA28000C6
bool MdbServerApp::loopProcess()
{
	try
	{
		m_pMdbServer->run();	
	}
	catch(LINKAGE_MDB::Mdb_Exception e)
	{
		e.toString();
		m_theErrorMessages->insert(EAPPFRM_SYSVAL,e.GetString());
	}
	m_stop = true;
    return true;
}

//-------------------------------------------------------------------------//
// TODO: Add your owner member function here
//-------------------------------------------------------------------------//
void MdbServerApp::setMdbServerVersion()
{
	VersionInfo theVersionInfo;
	
	theVersionInfo.m_date   = "2006/05/18";
	theVersionInfo.m_main   = 1;
	theVersionInfo.m_mini   = 2;
	theVersionInfo.m_patch  = 5;
	theVersionInfo.m_comment= "update pp-log for SOX";
	
	setVersion(theVersionInfo);
	
	return;	
}


