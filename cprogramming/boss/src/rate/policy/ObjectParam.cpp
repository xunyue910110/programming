#include "ObjectParam.h"
#include "RateGlobalInfo.h"
#include "base/exceptions.h"

ObjectParam::ObjectParam(){}
ObjectParam::~ObjectParam(){}

void ObjectParam::dump(char *prefix)
{
	cout << " Value : " << m_policyInfo->m_rateTpInfo->getParamValue(m_paramId) <<endl;
}

bool ObjectParam::getObjectValue(T_OBJECT_ATTR &value)
{
	value = m_policyInfo->m_rateTpInfo->getParamValue(m_paramId);
	return true;
}

bool ObjectParam::getObjectValue(long &value)
{
	const char *paramValue;
	paramValue = m_policyInfo->m_rateTpInfo->getParamValue(m_paramId);
	value = atol(paramValue);
	return true;
}

void ObjectParam::setParamId(const long &paramId)
{
	m_paramId = paramId;
}
