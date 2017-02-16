#include "ReplaceParamMgr.h"
#include "GlobalInfo.h"


bool ReplaceParamMgr::initialize(GlobalInfo* pGlobalInfo) 
{
	m_globalInfo = pGlobalInfo;
	return true;
}

bool ReplaceParamMgr::replaceParam(char* pParam,AttributeValue& attributeValue)
{

	long lkey = m_globalInfo->m_discntId  * 100 + 
		        m_globalInfo->m_eventType * 10 + 
		        atoi(pParam);

	if(!m_globalInfo->getParamValue(lkey,attributeValue) )
	{
		//error deal
		return false;
	}
	return true;
}

bool ReplaceParamMgr::replaceParam(char* pParam,int& value) 
{
	long lkey = m_globalInfo->m_discntId  * 100 + 
		        m_globalInfo->m_eventType * 10 + 
		        atoi(pParam);

	if(!m_globalInfo->getParamValue(lkey,value) )
	{  
		//error deal
		return false;
	}
	return true;
}
