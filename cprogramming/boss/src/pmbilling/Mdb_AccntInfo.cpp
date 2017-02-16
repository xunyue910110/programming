#include "Mdb_AccntInfo.h"
//2008-10-24 9:29:30
bool Mdb_AccntInfo::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_AccntInfo::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[20+1];
	
	m_record.m_idAttr[IdInfo::ID_TYPE_CODE] = (char)ACCT_ID_TYPE;
 	session->getColumnValue("ACCT_ID"           ,&t_longValue ,t_length); m_record.m_idAttr[IdInfo::ID_ATTR_ID] = t_longValue;
 	session->getColumnValue("EPARCHY_CODE"      , t_strTmp    ,t_length); 
 	          t_strTmp[t_length]=0;m_record.m_idAttr[IdInfo::ID_EPARCHY_CODE] = t_strTmp;
 	session->getColumnValue("CITY_CODE"         , t_strTmp    ,t_length); 
 	          t_strTmp[t_length]=0;m_record.m_idAttr[IdInfo::ID_CITY_CODE]    = t_strTmp;
 	session->getColumnValue("CUST_ID"           ,&t_longValue ,t_length); m_record.m_idAttr[IdInfo::ID_CUST_ID] = t_longValue;
 	session->getColumnValue("PAY_MODE_CODE"     , t_strTmp    ,t_length); 
 	          t_strTmp[t_length]=0;m_record.m_idAttr[IdInfo::ID_PAYMODE_CODE] = t_strTmp[0];
 	session->getColumnValue("SCORE_VALUE"       ,&t_longValue ,t_length); 
 	       m_record.m_idAttr[IdInfo::ID_SCORE_VALUE]  = t_longValue;
 	session->getColumnValue("CREDIT_VALUE"      ,&t_longValue,t_length); 
 	     m_record.m_idAttr[IdInfo::ID_CREDIT_VALUE] = t_longValue;
// 	session->getColumnValue("NET_TYPE_CODE" ,&t_longValue,t_length);
// 	    m_record.m_idAttr[IdInfo::ID_NETTYPE_CODE] = t_longValue;
 	session->getColumnValue("REMOVE_TAG"        , t_strTmp ,t_length);
 	          t_strTmp[t_length]=0;m_record.m_idAttr[IdInfo::ID_REMOVE_TAG] = t_strTmp[0];
 	session->getColumnValue("OPEN_DATE"         ,&t_longValue,t_length); m_record.m_idAttr[IdInfo::ID_OPEN_DATE]  = t_longValue;
 	session->getColumnValue("REMOVE_DATE"       ,&t_longValue,t_length); m_record.m_idAttr[IdInfo::ID_REMOVE_DATE]    = t_longValue;
 	return true;
}

void Mdb_AccntInfo::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_acctId = m_record.m_idAttr[IdInfo::ID_ATTR_ID].value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_acctId);
}

void Mdb_AccntInfo::setOtherParams(InputParams *pInputParamsseOther) const
{

}

Mdb_AccntInfo::Mdb_AccntInfo(const Mdb_AccntInfo& right)
{
	*this=right;
}

void Mdb_AccntInfo::operator=(const Mdb_AccntInfo& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_AccntInfo &po)
{
	os << po.m_record;
	return os;
}

Mdb_Property Mdb_AccntInfo::m_property={"TF_F_ACCOUNT","IDX_TF_F_ACCOUNT",1,"",0};
