#include "base/exceptions.h"
#include "mwci/CDBNameServiceImpl.h"
#include "mwci/LocalParseN.h"
#include "mwci/CContext.h"
#include "mwci/CNaming.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include "log4cpp/Logger.h"


using namespace std;
using namespace UDBI;
using namespace log4cpp;

static Logger& logger = Logger::getLogger("gboss.scom.mwci.CDBNameServiceImpl");

//##ModelId=41FA1BAE01C6
CDBNameServiceImpl::CDBNameServiceImpl(int iContextid)
{
	m_iContextid = iContextid;
	Init();
	ListNameing();
}

CDBNameServiceImpl::CDBNameServiceImpl()
{
	Init();
	ListNameing();
}

void CDBNameServiceImpl::setContextid(int iContextid)
{
	m_iContextid = iContextid;
}

//##ModelId=41FA1BAE01C6
bool CDBNameServiceImpl::LookUpNaming(CNaming& theNaming, const string &strLogicalName)
{
	bool result = false;
	CContext m_cContext;
	if(m_listNaming.size() != 0)
	{
		theEnd=m_listNaming.end();

		for (itr=m_listNaming.begin();itr!=theEnd;itr++)
		{
			m_cContext = itr->m_cContext;

			if(m_cContext.m_nContextID == m_iContextid && itr->m_strLogicName.compare(strLogicalName) == 0)
			{
		        theNaming.m_strLogicName   = itr->m_strLogicName;
				theNaming.m_strPhysicalName   = itr->m_strPhysicalName;
				theNaming.m_strMachineIP   = itr->m_strMachineIP;
				theNaming.m_strMachinePort = itr->m_strMachinePort;
				theNaming.m_strMachineName = itr->m_strMachineName;
				result = true;
				break;
			}
		}
	}

	return result;
}

//##ModelId=41FAF17D0188
bool CDBNameServiceImpl::Init(JConnect *pconn)
{
	if(pconn1 == NULL)
	{
		ifstream oTotalStream;
		string username,password,sid;

		logger.info("Welcome to CDBNameServiceImpl::Init\n");

		try
		{
			char *pFileName=getenv("NAMING_CONFIG");

			if (!pFileName)
			{
				logger.alert("Fail to get environment NAMING_CONFIG. Config file with DB USERNAME PASSWORD SID must be in the file.");
				return false;
			}

			oTotalStream.open(pFileName, ios::in);

			if (oTotalStream.rdstate() != ios::goodbit)
			{
				oTotalStream.close();
				logger.alert("Fail to open config file specify by NAMING_CONFIG (%s). ",pFileName);
				return false;
			}

			oTotalStream>>username>>password>>sid;
			logger.info("username:(%s);password(******);sid:(%s)",username.c_str(),sid.c_str());

			oTotalStream.close();
			//jdb::jinit(sid, username, password);
			//this->pconn1 = jdb::getConnect();
		}
		catch(...)
		{
			logger.alert("CDBNameServiceImpl::Init GetDBConnect Error!\n");
			return false;
		}
	}
	else
	{
		this->pconn1 = pconn1 ;
		return true;
	}
	return true;
}

//##ModelId=41FAF1A40332
bool CDBNameServiceImpl::ListNameing()
{
	bool result = false;
	string strsql;
	CContext m_cContext;
	logger.info("CDBNameServiceImpl::ListNameing Beginning...\n");

	strsql = "select context_id,logical_name,machine_ip,machine_port,machine_name,service_name,";
	strsql = strsql + "service_type_code,redeploy_time,redeploy_staff_id,redeploy_depart_id,remark from td_m_naming ";
	strsql = strsql + "where context_id > 9000 and context_id <10000";

	logger.info("CDBNameServiceImpl::ListNameing SQL:%s\n",strsql.c_str());

	try
	{
		JTable *pTab = jdb::getConnect()->CreateJTable();
		pTab->AddSql(strsql);
		pTab->ExecSelect();

		while(pTab->Fetch())
		{
			CNaming theNaming;
			CContext cContext;
			cContext.m_nContextID	   = pTab->GetInt("CONTEXT_ID");
			theNaming.m_cContext       = cContext;
			theNaming.m_strLogicName   = pTab->GetString("LOGICAL_NAME");
			theNaming.m_strMachineIP   = pTab->GetString("MACHINE_IP");
			theNaming.m_strMachinePort = pTab->GetString("MACHINE_PORT");
			theNaming.m_strMachineName = pTab->GetString("MACHINE_NAME");
			theNaming.m_strPhysicalName   = pTab->GetString("SERVICE_NAME");
			m_listNaming.push_back(theNaming);
		}

		delete pTab;
		result = true;
	}
	catch(Exception &er)
	{
		logger.alert("CDBNameServiceImpl::ListNameing Error:[%s]",er.getMessage().c_str());
		return false;
	}

	logger.info("CDBNameServiceImpl::ListNameing End...\n");
	return result;
}

//##ModelId=41FAF3020137
CDBNameServiceImpl* CDBNameServiceImpl::GetInstance(int iContextid)
{
	if(cdsi == NULL)
	{
		if(iContextid == 0){
			char * pFilename=getenv(LNS_ENV_STR);
			LocalParseN localParse(pFilename);
			CContext theLocCI;
			localParse.GetLocalCI(theLocCI);
			cdsi = new CDBNameServiceImpl(theLocCI.m_nContextID);
		}
		else
			cdsi = new CDBNameServiceImpl(iContextid);
	}
	return cdsi;
}

CDBNameServiceImpl *CDBNameServiceImpl::cdsi = NULL;
