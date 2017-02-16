#include "ObjectCdr.h"
#include "base/exceptions.h"
#include "RateGlobalInfo.h"

extern RateGlobalInfo g_globalInfo;
PCDR * ObjectCdr::m_cdr = &g_globalInfo.m_pCdr;

bool ObjectCdr::getObjectValue(T_OBJECT_ATTR &value)
{
	if (m_key>-1)
	{
		value = (*m_cdr)[m_key];
	}
	else
	{
		THROW_C_P3(BusinessException, -1, "清单对象错误,非法的下标值!"
    	"object_id=%d,attr_id=%d,key=%d", m_id, m_attrId, m_key);
	}
	return true;
}

bool ObjectCdr::getObjectValue(long &value)
{	
	if (m_key>-1)
	{
		value = (Integer) (*m_cdr)[m_key];
	}
	else
	{
		THROW_C_P3(BusinessException, -1, "清单对象错误,非法的下标值!"
    	"object_id=%d,attr_id=%d,key=%d", m_id, m_attrId, m_key);
	}
	return true;
}

void ObjectCdr::dump(char *prefix)
{
	char myPrefix[256];	
	strcpy(myPrefix, prefix);	
	strcat(myPrefix,"  ");
	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"---- ObjectCdr : " <<m_id<<" ----" <<endl;	
	ObjectBase::dump(myPrefix);
	cout <<myPrefix<<"m_key        = "<< m_key <<endl;	
	cout <<myPrefix<<"}" <<endl;
}
