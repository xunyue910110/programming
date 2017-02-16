#include "DBPassOper.h"
#include "udbi/udbi.h"
#include "des/des.h"
#include "base/exceptions.h"
#include "base/SysParam.h"
#include "base/Properties.h"
#include "log4cpp/Logger.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>

using namespace UDBI;
using namespace std;
using namespace log4cpp;

static SysParam*       m_theSysParam=0;
static Logger& logger = Logger::getLogger("gboss.bude.passmgr");
	
JConnect *pconn;

class DBPassOperFromDB:public DBPassOper
{
	private:		
		string strconfigFile;
		string dbsid; 
		string dbuser;
		string dbpass;
		string decodePass;
		//是否本进程自己的jdb环境
		bool ownerjdb;		
	protected:
		bool init();
	public:
		DBPassOperFromDB();
		virtual ~DBPassOperFromDB();

		bool GetDBPass(DBPass& dbPass, const string &strDBCode, const string &strDBUserName);	
};

DBPassOperFromDB::DBPassOperFromDB()
{
	ownerjdb = false;
	init();
}

DBPassOperFromDB::~DBPassOperFromDB()
{
	logger.info("DBPassOperFromDB end\n");
	if (pconn != NULL)
			pconn->DisConnect();
	if(ownerjdb)
		jdb::jdeinit();
}

bool DBPassOperFromDB::init()
{
	bool result = false;
	logger.info("DBPassOperFromDB::init Beginning...\n");
	//get system param from configure file
  char *ptmGetenv,pathName[50];
  ptmGetenv = getenv("WORK_HOME");
  if (ptmGetenv == 0)
  {
      strconfigFile="DBPass.cfg" ;
  } 
  else
  {
       sprintf(pathName,"%s/etc/DBPass.cfg",ptmGetenv);
       strconfigFile=pathName;
  }
  
  if ((m_theSysParam = new SysParam()) == 0)
  {
     logger.alert("Init  For Class SysParam Error..");
     THROW(BaseException, "DBPassOperFromDB::init异常：Init  For Class SysParam Error..");
     return false;
  }
  m_theSysParam->initialize((char *)strconfigFile.c_str());
  
  string  section = "\\SYSTEM";
      
  if (!m_theSysParam->getValue(section, "USERNAME", dbuser)) {
      logger.alert("从配置文件中获取数据库用户名时出错。");
      THROW(BaseException, "DBPassOperFromDB::init异常：从配置文件中获取数据库用户名时出错。");
      return false;
  }
 
  if (!m_theSysParam->getValue(section, "PASSWORD", dbpass)) {
      logger.alert("从配置文件中获取数据库密码时出错。");
      THROW(BaseException, "DBPassOperFromDB::init异常：从配置文件中获取数据库密码时出错。");
      return false;
  }
 
  if (!m_theSysParam->getValue(section, "SID", dbsid)) {
      logger.alert("从配置文件中获取数据库SID时出错。");
      THROW(BaseException, "DBPassOperFromDB::init异常：从配置文件中获取数据库密码时出错。");
      return false;
  }
	
	delete m_theSysParam;
	

	if(DES::decrypt(dbpass, decodePass)!=0)
  {
		logger.alert("DBPassOperFromDB pass decrypt Error(dbpass)");
		THROW(BaseException, "DBPassOperFromDB::init异常：DBPassOperFromDB pass decrypt Error(dbpass)");
		return false;
	}
	
	logger.info("DBPassOperFromDB::init End...\n");
	return true;
}
	
bool DBPassOperFromDB::GetDBPass(DBPass& dbPass, const string &strDBCode, const string &strDBUserName)
{
	bool result = false;
	string strsql;
	
	logger.info("DBPassOperFromDB::GetDBPass Beginning...\n");

		//connect to DataBase
	logger.info("DBPassOperFromDB::GetDBPass connect database...\n");
	try
	{
		if ( jdb::getInitMode() == INIT_NO )
		{
			jdb::jinit(dbsid, dbuser, decodePass);
			ownerjdb = true;
		}
		pconn = jdb::getConnect();
	}
	catch (Exception &er)
	{
		logger.alert("DBPassOperFromDB::GetDBPass Error:[%s]",er.getMessage().c_str());
		return false;
	}
	
	
	strsql = "SELECT a.Databasecode DBCODE, b.hostip DBHOST	, b.servicename DBSERVICENAME, a.username DBUSER, a.password DBPASS ";
	strsql = strsql + "FROM td_s_dblogin a, td_s_dbinfo b ";
	strsql = strsql + "WHERE a.Databasecode=b.databasecode AND a.Databasecode=:dbcode AND a.username=:dbuser";
	
	logger.info("DBPassOperFromDB::GetDBPass SQL:%s\n",strsql.c_str());

	JTable *pTab = pconn->getJTable();
	try
	{	
		pTab->AddSql(strsql);
		pTab->SetParam(":dbcode", strDBCode);
		pTab->SetParam(":dbuser", strDBUserName);
		
		pTab->ExecSelect();

		while(pTab->Fetch())
		{
			dbPass.Code 			= pTab->GetString("DBCODE") ;     
			dbPass.Host 			= pTab->GetString("DBHOST");     
			dbPass.ServerName = pTab->GetString("DBSERVICENAME");
			dbPass.UserName 	= pTab->GetString("DBUSER");
			dbPass.Passwd 		= pTab->GetString("DBPASS");
		} 

		if(DES::decrypt(dbPass.Passwd, dbPass.DecPasswd)!=0)
		{
			logger.alert("DBPassOperFromDB pass decrypt Error");
			return false;
		}
			
		pTab->Clear();
		result = true;
	}
	catch(Exception &er)
	{
		pTab->Clear();
		logger.alert("DBPassOperFromDB::GetDBPass Error:[%d][%s]",er.getCode(),er.getMessage().c_str());
		return false;
	}

	logger.info("DBPassOperFromDB::GetDBPass End...\n"); 
	return result;
}

/////////////////////////////////////////////////////////////////////
DBPassOper* DBPassOper::getOper(const string& opt)
{
	if( opt == "Default" )
	{
		return new DBPassOperFromDB();
	}
	else if( opt == "DB" )
	{
		return new DBPassOperFromDB();
	}
	else
	{
		return new DBPassOperFromDB();
	}
}

////////////////////////////////////////////////////////////////////////
extern "C" bool GetDBPass(DBPass& dbpass, const string &strDBCode, const string &strDBUserName)
{
	bool result;
	result = false;
	
  LOG_TRACE(logger, "进入GetDBPass函数");
  
  DBPassOper* dbPassOper;
  try
  {
  	dbPassOper = DBPassOper::getOper();
  	if(dbPassOper->GetDBPass(dbpass, strDBCode, strDBUserName))
  		result  = true;
  }
  catch(Exception &e)
  {
      logger.alert(e);
      return false;
  }
  delete dbPassOper;
  
  LOG_TRACE(logger, "退出GetDBPass函数");
  return result;
}
