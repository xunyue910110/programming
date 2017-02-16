#include "mwci/CLocalNameServiceImpl.h"

//##ModelId=41907A1D03C5
bool CLocalNameServiceImpl::ErasureNaming(CNaming cNaming,int iIndex )
{
	if(m_iState!=LNS_READY)
		return false;


	int iRet=m_pLocalNaming->DelNameSvc(cNaming.m_strLogicName,iIndex);


	if ( iRet!= LNS_OK)
		return false;
	else
		return true;
}

//##ModelId=41907A6D0009
bool CLocalNameServiceImpl::SubmitNaming(CNaming cNaming, int iIndex)
{
	if(m_iState!=LNS_READY)
		return false;

	int iRet=m_pLocalNaming->SetNameSvc(cNaming.m_strLogicName,cNaming,iIndex);

	if( iRet!= LNS_OK)
		return false;
	else
		return true;
}

//##ModelId=41908148011B
bool CLocalNameServiceImpl::ListNaming(vector<CNaming> &vtheNaming)
{
	if(m_iState!=LNS_READY)
		return false;

	int iRet=m_pLocalNaming->ListNaming(vtheNaming);

	if( iRet!= LNS_OK)
		return false;
	else
		return true;
}

//##ModelId=41918C6700A6
bool CLocalNameServiceImpl::AddNaming(CNaming cNaming)
{
	if(m_iState!=LNS_READY)
		return false;

	int iRet=m_pLocalNaming->AddNameSvc(cNaming.m_strLogicName, cNaming);

	if(iRet != LNS_OK)
		return false;
	else
		return true;
}

//##ModelId=4191BB8701F7
bool CLocalNameServiceImpl::SetCfgFile(string strFileName)
{
	//if already created, release it first
	if(m_iState==LNS_READY)
		{
			delete this->m_pLocalNaming;
		}

	this->m_pLocalNaming=new LocalParseN(strFileName.c_str());

	if (this->m_pLocalNaming== NULL)
		return false;
	if (m_pLocalNaming->IsReady() == false)
		{
			delete this->m_pLocalNaming;
			return false;
		}

	m_iState=LNS_READY;
	return true;
}

//##ModelId=4194AEBC0381
bool CLocalNameServiceImpl::GetContext(CContext& theContext)
{
	if(m_iState!=LNS_READY)
		return false;
	int iRet=m_pLocalNaming->GetLocalCI(theContext);
	if (iRet != LNS_OK)
		return false;
	return true;
}

//##ModelId=4194AF1603E4
bool CLocalNameServiceImpl::GetUptTime(STD tm &theTime)
{
	if(m_iState!=LNS_READY)
		return false;
	CContext theContext;
	int iRet=GetContext(theContext);
	if (iRet != LNS_OK)
		return false;
	theTime= theContext.m_tmNamingUptTime;
	return true;
}

//##ModelId=419567C90089
bool CLocalNameServiceImpl::LookUpNaming(CNaming& theNaming, string & strLogicalName, int iIndex)
{
	int iRet;
	if(m_iState!=LNS_READY)
		return false;
	iRet=m_pLocalNaming->GetNameSvc(strLogicalName, theNaming, iIndex);
	if (iRet != LNS_OK)
		return false;
	return true;
}

//##ModelId=419567C9010B
bool CLocalNameServiceImpl::LookUpNaming(CNaming& theNaming, int iIndex)
{
	int iRet;
	if(m_iState!=LNS_READY)
		return false;
	iRet=m_pLocalNaming->GetNameSvc( m_LogicName, theNaming,  iIndex);
	if (iRet != LNS_OK)
		return false;
	return true;
}


//##ModelId=419567C90133
bool CLocalNameServiceImpl::SetLogicalName(string strLogicalName)
{
	m_LogicName=strLogicalName;
	return true;
}

//##ModelId=4195695C039F
string CLocalNameServiceImpl::GetNamingSrvInfo()
{
	CNaming theNaming;
	string theString;

	if(m_iState!=LNS_READY)
		return theString;

	int iRet=m_pLocalNaming->GetRemoteNS(theNaming);
	if (iRet != LNS_OK)
		return theString;

	return (theNaming.m_strMachineIP+":"+theNaming.m_strMachinePort);
}

//##ModelId=41957F180364
bool CLocalNameServiceImpl::IsNeedLocalService()
{
	if(m_iState!=LNS_READY)
		return false;
	CContext theLocCI;
	if (m_pLocalNaming->GetLocalCI(theLocCI)!=LNS_OK)
		return false;
	return theLocCI.m_bNeedLocSvc;
}

//##ModelId=41B6AF0D0042
CLocalNameServiceImpl::CLocalNameServiceImpl()
{
	m_pLocalNaming=NULL;
	m_iState=0;
}


//##ModelId=41B6AF0D0074
CLocalNameServiceImpl::~CLocalNameServiceImpl()
{
	if (m_pLocalNaming)
		delete m_pLocalNaming;
}

//##ModelId=41BEA5CA0124
void CLocalNameServiceImpl::Save()
{
	if (m_pLocalNaming)
		m_pLocalNaming->SaveToFile();
}

//##ModelId=41BFAF9C01ED
void CLocalNameServiceImpl::EmptyNaming()
{
	if (m_pLocalNaming)
		m_pLocalNaming->DeleteAllNaming();
}

//##ModelId=41C77DBE00C7
 bool CLocalNameServiceImpl::SetContext(const CContext& theContext)
 {
	if (m_pLocalNaming)
		if(m_pLocalNaming->SetLocalCI(theContext)!=LNS_OK)
			return false;
	return false;
 }
