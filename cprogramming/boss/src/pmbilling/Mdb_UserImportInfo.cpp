#include "Mdb_UserImportInfo.h"

bool Mdb_UserImportInfo::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserImportInfo::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[50+1];
	
 	session->getColumnValue("USER_ID",&t_longValue ,t_length);m_record.m_userId=t_longValue;
 	session->getColumnValue("PRODUCT_ID", &t_intValue    ,t_length);m_record.m_productId=t_intValue;
 	session->getColumnValue("BRAND_CODE",t_strTmp     ,t_length); 
 	  t_strTmp[t_length]=0; m_record.m_brandCode   = t_strTmp;
 	session->getColumnValue("LOGIC_PHONE",t_strTmp     ,t_length); 
 	  t_strTmp[t_length]=0; m_record.m_msisdn   = t_strTmp;
 	session->getColumnValue("NET_TYPE_CODE",t_strTmp     ,t_length); 
 	  t_strTmp[t_length]=0;m_record.m_netTypeCode   = t_strTmp;
 	session->getColumnValue("START_DATE" ,&t_longValue  ,t_length);m_record.m_startDate=t_longValue;
 	session->getColumnValue("END_DATE" ,&t_longValue  ,t_length);m_record.m_endDate=t_longValue;

 	return true;
}

void Mdb_UserImportInfo::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_id = m_record.m_userId.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_id);
}

void Mdb_UserImportInfo::setOtherParams(InputParams *pInputParamsseOther) const
{
  
}

Mdb_UserImportInfo::Mdb_UserImportInfo(const Mdb_UserImportInfo& right)
{
	*this=right;
}

void Mdb_UserImportInfo::operator=(const Mdb_UserImportInfo& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_UserImportInfo &po)
{
	os << po.m_record.m_userId        << " , "
	   << po.m_record.m_productId     << " , "
	   << po.m_record.m_brandCode       << " , "
	   << po.m_record.m_msisdn        << " , "
	   << po.m_record.m_netTypeCode	  <<" , "
	   << po.m_record.m_startDate << " , "
	   << po.m_record.m_endDate;
	return os;
}

bool operator>(const Mdb_UserImportInfo& left,const Mdb_UserImportInfo& right)
{
	return left.m_record > right.m_record;
}

Mdb_Property Mdb_UserImportInfo::m_property={"TF_F_USER_IMPORTINFO","idx_TF_F_USER_IMPORTINFO",1,
    "",0};
