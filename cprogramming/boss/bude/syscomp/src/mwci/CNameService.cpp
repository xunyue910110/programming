#include "mwci/CNameService.h"
#include "mwci/CMutex.h"

CMutex *gpMutex;
//##ModelId=4191CB2D023F
bool CNameService::BatChgNaming(const vector<CNaming>& vtheNamings,IMWCaller * theCaller)
{
	if (m_RemoteService)
		return m_RemoteService->BatChgNaming(vtheNamings,theCaller);
	return false;
}

//##ModelId=4191CC51031B
bool CNameService::IncreSyn(IMWCaller * theCaller)
{
	CContext theContext;
	vector<CNaming> vtheNaming(100);
	int i;
	string NamingSvcInfo;
	STD tm theUpdateTime;

	//Inital theUpdateTime the zero value
	memset(&theUpdateTime,0x00,sizeof(STD tm));

	vtheNaming.clear();

	if(!SetNamingSvcInfo())
		return false;

	if(!m_LocalService->GetContext(theContext))
		return false;

	if(!m_RemoteService->FindUptNaming(theContext.m_nContextID, theContext.m_tmNamingUptTime,vtheNaming,theCaller))
		return false;

	const int iAllResult=vtheNaming.size();

	//delete from local fisrt
	for (i=0;i<iAllResult;i++)
		m_LocalService->ErasureNaming(vtheNaming[i]);

	//add back to local
	for (i=0;i<iAllResult;i++)
		{
			m_LocalService->AddNaming(vtheNaming[i]);
			if (IsTMBiger(vtheNaming[i].m_tmReDeployTime,theUpdateTime))
				theUpdateTime=vtheNaming[i].m_tmReDeployTime;
		}

	//update the local "NAMINGUPTTIME"
	m_LocalService->GetContext(this->m_cContext);
	this->m_cContext.m_tmNamingUptTime=theUpdateTime;
	m_LocalService->SetContext(m_cContext);

	//write back to disk
	m_LocalService->Save();

	return true;

}

//##ModelId=4191CD2A0114
bool CNameService::FullSyn(IMWCaller * theCaller)
{
	CContext theContext;
	vector<CNaming> vtheNaming(512);
	int i;
	STD tm theUpdateTime;

	//Inital theUpdateTime the zero value
	memset(&theUpdateTime,0x00,sizeof(STD tm));

	vtheNaming.clear();

	if(!SetNamingSvcInfo())
		return false;

	if(!m_LocalService->GetContext(theContext))
		return false;

	if(!m_RemoteService->ListNaming(theContext.m_nContextID, vtheNaming,theCaller))
		return false;

	const int iAllResult=vtheNaming.size();

	//delete all local Naming
		m_LocalService->EmptyNaming();

	//add back to local
	for (i=0;i<iAllResult;i++)
	{
		m_LocalService->AddNaming(vtheNaming[i]);
		if (IsTMBiger(vtheNaming[i].m_tmReDeployTime,theUpdateTime))
			theUpdateTime=vtheNaming[i].m_tmReDeployTime;
	}

	//update the local "NAMINGUPTTIME"
	m_LocalService->GetContext(this->m_cContext);
	this->m_cContext.m_tmNamingUptTime=theUpdateTime;
	m_LocalService->SetContext(m_cContext);

	//write back to disk
	m_LocalService->Save();

	return true;

}

//##ModelId=4191E02C023E
string CNameService::GetService()
{
	//don't know what to do
	string a;
	return a;
}

//##ModelId=4191E0480338
string CNameService::GetServerInfo()
{
	return m_LocalService->GetNamingSrvInfo();
}

//##ModelId=4192099301B2
CNameService* CNameService::GetInstance()
{
	if (m_cNameService==NULL)
	{
		m_cNameService=new CNameService();
		char * pFilename=getenv(LNS_ENV_STR);
		if (pFilename)
			if(!m_cNameService->SetCfgFile(pFilename))
			{
				delete m_cNameService;
				return NULL;
			}
		return m_cNameService;
	}
	else
		return m_cNameService;
}

//##ModelId=419307CD0241
bool CNameService::SetContext(CContext cContext)
{
	m_cContext=cContext;
	return true;
}

//##ModelId=419307D50184
bool CNameService::SetLogicalName(const string & strLogicalName)
{
	m_strLogicalName=strLogicalName;
	return true;
}

//##ModelId=4194AA7B015E
CNaming CNameService::LookUpNaming(CContext cContext, string strLogicalName,IMWCaller* theCaller,bool NeedRemote,int iIdx)
{
	CNaming theNaming;
	theNaming.m_cSvcTypeCode=-1;

	if(m_bIfLookUpLocal&&!NeedRemote)
		if(m_LocalService->LookUpNaming(theNaming, strLogicalName,iIdx))
			return theNaming;

	if(NeedRemote)
	{
		m_RemoteService->SetNamingSrvInfo(m_LocalService->GetNamingSrvInfo());

		if (m_RemoteService->LookUpNaming(cContext.m_nContextID, strLogicalName, theNaming,theCaller,iIdx))
			return theNaming;
	}

	theNaming.m_cSvcTypeCode="-1";
	return theNaming;
}


//##ModelId=4194AA7B0172
bool CNameService::ReLoadNaming()
{
	//在当前模式下不需要
	return true;
}

//##ModelId=4194AA7B0190
bool CNameService::ErasureNaming(CNaming cNaming)
{
	return m_LocalService->ErasureNaming(cNaming);
}

//##ModelId=4194AA7B01A4
bool CNameService::SubmitNaming(CNaming cNaming)
{
	return m_LocalService->SubmitNaming(cNaming);
}

//##ModelId=4194AA7B01C2
bool CNameService::ListNaming(vector<CNaming>& theNamingS,IMWCaller * theCaller)
{
	if(!SetNamingSvcInfo())
		return false;

	return m_RemoteService->ListNaming(theNamingS,theCaller);
}

//##ModelId=4194AA7B01E0
bool CNameService::FindUptNaming(CContext cContext,STD tm tmUptTime, vector<CNaming>& theNamingS,IMWCaller * theCaller)
{
	return m_RemoteService->FindUptNaming(cContext.m_nContextID, tmUptTime, theNamingS,theCaller);
}

//##ModelId=4194AA7B024F
bool CNameService::AddNaming(CNaming cNaming)
{
	return true;
}

//##ModelId=4194AA7B026D
bool CNameService::SetCfgFile(string strFileName)
{
	if (!m_LocalService->SetCfgFile(strFileName))
		return false;
	return true;
}

//##ModelId=4194AD0202D1
CNaming CNameService::LookUpNaming(string strLogicalName,IMWCaller* theCaller,bool NeedRemote,int iIdx)
{
	CNaming theNaming;

	theNaming.m_cSvcTypeCode=-1;

	if(!m_LocalService->GetContext(m_cContext))
		return theNaming;

	return LookUpNaming(m_cContext,strLogicalName,theCaller,NeedRemote,iIdx);

}

//##ModelId=4194AD0202D1
CNaming CNameService::LookUpNaming(string strLogicalName)
{
	CNaming theNaming;
	m_DBService=CDBNameServiceImpl::GetInstance();
	m_DBService->LookUpNaming(theNaming, strLogicalName);
	return theNaming;
}

//##ModelId=4195656200AC
bool CNameService::FindUptNaming(STD tm tmUptTime, vector<CNaming>& theNamingS,IMWCaller * theCaller)
{
	if(!m_LocalService->GetContext(m_cContext))
		return false;

	if(!SetNamingSvcInfo())
		return false;

	return m_RemoteService->FindUptNaming(m_cContext.m_nContextID, tmUptTime, theNamingS,theCaller);
}

//##ModelId=41956562019D
//CNaming CNameService::LookUpNaming(int iIdx)
//{
//	CNaming theNaming;
//	theNaming.m_cSvcTypeCode=-1;
//
//	if(!m_LocalService->GetContext(m_cContext))
//		return theNaming;
//
//	return LookUpNaming(m_cContext,m_strLogicalName,iIdx);
//}

//##ModelId=419566C00382
void CNameService::SetIfLookUpLocal( bool tag)
{
	m_bIfLookUpLocal =tag;
}

//##ModelId=419566C002B1
CNameService *CNameService::m_cNameService= NULL;

//##ModelId=41BE68FD0391
CNameService::CNameService()
{
	m_bIfLookUpLocal = true;
 	m_bKnowNamingSvc=false;
	m_bKnowContext=false;

  m_LocalService=new CLocalNameServiceImpl();
	m_RemoteService=new CRemoteNameServiceImpl;
	m_DBService=NULL;
}

//##ModelId=41BE9C1B006B
CNameService::~CNameService()
{
	if (m_LocalService) delete m_LocalService;
	if (m_RemoteService) delete m_RemoteService;
	if (m_DBService) delete m_DBService;
}

bool CNameService::SetNamingSvcInfo()
{
	if (!m_bKnowNamingSvc)
	{
		string theNamingSvcInfo;
		theNamingSvcInfo=m_LocalService->GetNamingSrvInfo();
		if (theNamingSvcInfo.empty())
			return false;
		m_RemoteService->SetNamingSrvInfo(theNamingSvcInfo);
		m_bKnowNamingSvc=true;
	}
	return true;
}

//##ModelId=41C77DBE0185
bool CNameService::IsTMBiger(const STD tm& left,const STD tm& right)
{
	if (left.tm_year>right.tm_year) return true;
	if (left.tm_year<right.tm_year) return false;

	if (left.tm_mon>right.tm_mon) return true;
	if (left.tm_mon<right.tm_mon) return false;

	if (left.tm_mday>right.tm_mday) return true;
	if (left.tm_mday<right.tm_mday) return false;

	if (left.tm_hour>right.tm_hour) return true;
	if (left.tm_hour<right.tm_hour) return false;

	if (left.tm_min>right.tm_min) return true;
	if (left.tm_min<right.tm_min) return false;

	if (left.tm_sec>right.tm_sec) return true;
	if (left.tm_sec<right.tm_sec) return false;

	return false;
}
