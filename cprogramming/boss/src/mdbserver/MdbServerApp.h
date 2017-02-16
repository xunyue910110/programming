//  ############################################
//  Source file : MdbServerApp.h
//  Version     : 0.1.1
//  Language	   : ANSI C++
//  OS Platform : UNIX
//  Authors     : chen min
//  E_mail      : chenm@lianchuang.com
//  Create      : 2008-5-28 14:21:47
//  Update      : 2008-5-28 14:21:50
//  Copyright(C): chen min, Linkage.
//  ############################################

#ifndef MDBSERVERAPP_H_INCLUDED_C4353D06
#define MDBSERVERAPP_H_INCLUDED_C4353D06

#include "config-all.h"

#include "appfrm/Application.h"
#include "base/Directory.h"
#include "MdbServer.h"
#include "log4cpp/Logger.h"
#include "MdbServer.h"

#include <string>
USING_NAMESPACE(std)

//##ModelId=3BCAA26200AF
class MdbServerApp : public FileApplication
{
	public:
		virtual ~MdbServerApp(){};
	
	protected:
		//##ModelId=3BCAA280003A
		virtual bool initialization();
		
		//##ModelId=3BCAA28000C6
		virtual bool loopProcess();
		
		//##ModelId=3BCAA28000B2
		virtual bool end();
	
	private:
		MdbServer *m_pMdbServer;

	private:
		// 写入版本信息
		void setMdbServerVersion();

};



#endif /* MYAPPLICATION_H_INCLUDED_C4353D06 */
