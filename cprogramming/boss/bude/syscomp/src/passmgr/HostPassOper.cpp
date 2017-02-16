#include "HostPassOper.h"
#include "base/exceptions.h"
#include "base/SysParam.h"
#include "base/Properties.h"
#include "base/StringUtil.h"
#include "log4cpp/Logger.h"
#include "archy/file.hpp"
#include "des/des.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>

using namespace std;
using namespace log4cpp;
using namespace archy;

static Logger& logger = Logger::getLogger("gboss.bude.passmgr");

class HostPassOperFromFS:public HostPassOper
{
	private:		
		string strconfigFile;
		file passFile;
	protected:
		bool init();
	public:
		HostPassOperFromFS();
		~HostPassOperFromFS();
		/**
     * 根据主机名及用户名获取主机访问信息
     * @param pass 					输出主机对象的引用
     * @param strDBCode 		输入主机名
     * @param strDBUserName 输入主机用户名
     * @return: true 成功 false 失败
     */
		bool GetHostPass(HostInfo& hostInfo, const string &strHostName, const string &strHostUserName);	
};

HostPassOperFromFS::HostPassOperFromFS()
{
	init();
}

HostPassOperFromFS::~HostPassOperFromFS()
{
	logger.info("HostPassOperFromFS end~~~~~~~~~~~~~~~~\n");
}

bool HostPassOperFromFS::init()
{
	bool result = false;
	logger.info("HostPassOperFromFS::init Beginning...\n");

  char *ptmGetenv,pathName[50];
  ptmGetenv = getenv("WORK_HOME");
  if (ptmGetenv == 0)
  {
      strconfigFile="HostPass.cfg" ;
  } 
  else
  {
       sprintf(pathName,"%s/etc/HostPass.cfg",ptmGetenv);
       strconfigFile=pathName;
  }
	
	result = true;

	logger.info("HostPassOperFromFS::init End...\n");
	return result;
}
	
bool HostPassOperFromFS::GetHostPass(HostInfo& hostInfo, const string &strHostName, const string &strHostUserName)
{
	bool result = false;
	logger.info("HostPassOperFromFS::GetHostPass Beginning...\n");
	
	try
	{
		if(passFile.open(strconfigFile, "r")==-1)
		{
			logger.alert("HostPassOperFromFS::GetHostPass 打开配置文件出错...\n");
			return false;
		}
		
		string line;	
		while(passFile.read_line(line)==1)
		{
			vector<string> vPartInfo;
	  	vPartInfo.clear();
	  	StringUtil::split(line,"	",vPartInfo, true);
	
  		if(vPartInfo[0]==strHostName & vPartInfo[2]==strHostUserName)
  		{
  			hostInfo.Name 		= vPartInfo[0];
  			hostInfo.Ip   		= vPartInfo[1];
  			hostInfo.UserName = vPartInfo[2];
  			hostInfo.Passwd		= vPartInfo[3];
  			hostInfo.Remark		= vPartInfo[4];
  			
  			if(DES::decrypt(hostInfo.Passwd, hostInfo.DecPasswd)!=0)
		    {
			    logger.alert("HostPassOperFromFS pass decrypt Error");
			    return false;
		    }

				result = true;
				break;
  		}
	  }
	  
	  passFile.close();
	}
	catch(Exception &er)
	{
		passFile.close();
		logger.alert("HostPassOperFromFS::GetHostPass Error:[%s]",er.getMessage().c_str());
		return false;
	}
	
	logger.info("HostPassOperFromFS::GetHostPass End...\n"); 
	return result;
}


/////////////////////////////////////////////////////////////////////
HostPassOper* HostPassOper::getOper(const string& opt)
{
	if( opt == "Default" )
	{
		return new HostPassOperFromFS();
	}
	else if( opt == "FS" )
	{
		return new HostPassOperFromFS();
	}
	else
	{
		return new HostPassOperFromFS();
	}
}

////////////////////////////////////////////////////////////////////////
extern "C" bool GetHostPass(HostInfo& hostInfo, const string &strHostName, const string &strHostUserName)
{
	bool result;
	result = false;
	
  LOG_TRACE(logger, "进入GetHostPass函数");
  
  HostPassOper* hostPassOper;
  try
  {
  	hostPassOper = HostPassOper::getOper();
  	if(hostPassOper->GetHostPass(hostInfo, strHostName, strHostUserName))
  		result  = true;
  }
  catch(Exception &e)
  {
      logger.alert(e);
      return false;
  }
  delete hostPassOper;
  
  LOG_TRACE(logger, "退出GetHostPass函数");
  return result;
}