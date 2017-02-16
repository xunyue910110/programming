#include "ObjectUserParam.h"
#include "RateGlobalInfo.h"
#include "base/exceptions.h"

ObjectUserParam::ObjectUserParam(){}
ObjectUserParam::~ObjectUserParam(){}

void ObjectUserParam::dump(char *prefix)
{
	m_globalInfo->m_idInfo.init(m_globalInfo->m_cycParam.m_beginTime,m_globalInfo->m_cycParam.m_endTime,m_policyInfo->m_rateTpInfo->m_eventType);
	cout << " Value : " << m_globalInfo->m_idInfo.getParamValue(m_paramId) <<endl;
}

bool ObjectUserParam::getObjectValue(T_OBJECT_ATTR &value)
{
	m_globalInfo->m_idInfo.init(m_globalInfo->m_cycParam.m_beginTime,m_globalInfo->m_cycParam.m_endTime,m_policyInfo->m_rateTpInfo->m_eventType);
	value = m_globalInfo->m_idInfo.getParamValue(m_paramId);
	return true;
}

bool ObjectUserParam::getObjectValue(long &value)
{
	const char *paramValue;
	m_globalInfo->m_idInfo.init(m_globalInfo->m_cycParam.m_beginTime,m_globalInfo->m_cycParam.m_endTime,m_policyInfo->m_rateTpInfo->m_eventType);
	paramValue = m_globalInfo->m_idInfo.getParamValue(m_paramId);
	if(paramValue == NULL)
	{
		paramValue = "-1";
	}
	
	value = atol(paramValue);
	return true;
}

void ObjectUserParam::setParamId(const long &paramId)
{
	m_paramId = paramId;
}
