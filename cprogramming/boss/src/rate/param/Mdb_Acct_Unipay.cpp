#include "Mdb_Acct_Unipay.h"

bool Mdb_Acct_Unipay::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_Acct_Unipay::convert(Session* session)
{
	int  t_length;
	long t_longValue;
	char t_strValue[64+1];
 	session->getColumnValue("ACCT_ID"       ,&t_longValue ,t_length); m_record.m_acctId = t_longValue;
 	session->getColumnValue("UNIPAY_PHONE"  ,t_strValue,t_length);t_strValue[t_length]=0;
 	                                  m_record.m_msisdn = t_strValue;
 	session->getColumnValue("AREA_TYPE"     ,t_strValue,t_length);t_strValue[t_length]=0;
 	                                  m_record.m_areaType = t_strValue[0];
 	session->getColumnValue("HOME_FALG"     ,t_strValue,t_length);t_strValue[t_length]=0;
 	                                  m_record.m_homeTag  = t_strValue[0];
 	return true;
}

void Mdb_Acct_Unipay::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_acctId = m_record.m_acctId.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_acctId);
}

void Mdb_Acct_Unipay::setOtherParams(InputParams *pInputParamsseOther) const
{
}

Mdb_Acct_Unipay::Mdb_Acct_Unipay(const Mdb_Acct_Unipay& right)
{
	*this=right;
}

void Mdb_Acct_Unipay::operator=(const Mdb_Acct_Unipay& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_Acct_Unipay &po)
{
	os << po.m_record.m_acctId<<" , "
	   << po.m_record.m_msisdn<<" , "
	   << po.m_record.m_areaType<<" , "        
	   << po.m_record.m_homeTag;
	   
	return os;        
}                   
                    
Mdb_Property Mdb_Acct_Unipay::m_property={"TF_F_ACCT_UNIPAY","IDX_TF_F_ACCT_UNIPAY",1,
    "",0};
