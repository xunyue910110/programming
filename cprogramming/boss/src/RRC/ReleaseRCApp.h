//  ############################################
//  Source file : ReleaseRCApp.h
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-5-28 14:21:47
//  Update      : 2008-5-28 14:21:50
//  Copyright(C): chen min, Linkage.
//  ############################################

#ifndef RELEASERCAPP_H_INCLUDED_C4353D06
#define RELEASERCAPP_H_INCLUDED_C4353D06

#include "config-all.h"

#include "appfrm/Application.h"
#include "base/Directory.h"
#include "log4cpp/Logger.h"
#include "Session.h"
#include "DataBase.h"
#include "MemManager.h"
#include "InstanceFactory.h"

#include <string>
USING_NAMESPACE(std)

//##ModelId=3BCAA26200AF
class ReleaseRCApp : public FileApplication
{
	public:
		ReleaseRCApp()
		{
			m_pDB      = NULL;
			m_pSession = NULL;	
			m_pMemMgr  = NULL;
		}
		virtual ~ReleaseRCApp(){};
	
	protected:
		//##ModelId=3BCAA280003A
		virtual bool initialization();
		
		//##ModelId=3BCAA28000C6
		virtual bool loopProcess();
		
		//##ModelId=3BCAA28000B2
		virtual bool end();
	
	private:
		DataBase   *m_pDB;
		Session    *m_pSession;
		MemManager *m_pMemMgr;
		int         m_iSleepTime;
		T_NAMEDEF   m_dbName;
		// 获取进程PID的命令--各平台上可能不尽相同，因此配置实现
    	string m_sCommand;

	private:
		// 写入版本信息
		void setMdbServerVersion();
		void run();
		bool isPidExists(const int pid);
		void rollBack(SessionInfo &sessionInfo);

};



#endif /* RELEASERC_H_INCLUDED_C4353D06 */
