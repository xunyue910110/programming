#include "Mdb_VpnInfo_Number.h"

bool Mdb_VpnInfo_Number::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_VpnInfo_Number::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strValue[64+1];
 	session->getColumnValue("USER_ID"           ,&t_longValue,t_length);m_record.m_userId=t_longValue;
 	session->getColumnValue("MEMBER_ROLE_CODE"  ,&t_intValue ,t_length);m_record.m_memberType=t_intValue;
 	session->getColumnValue("MEMBER_ROLE_TYPE"  ,&t_strValue ,t_length);
 	                        t_strValue[t_length]=0;m_record.m_memberIdType=t_strValue;
 	session->getColumnValue("MEMBER_ROLE_ID"    ,&t_longValue,t_length);m_record.m_memberId=t_longValue;
 	session->getColumnValue("MEMBER_ROLE_NUMBER",&t_strValue ,t_length);
 	                        t_strValue[t_length]=0;m_record.m_memberNumber=t_strValue;
 	session->getColumnValue("START_DATE"        ,&t_longValue,t_length);m_record.m_beginTime=t_longValue;
 	session->getColumnValue("END_DATE"          ,&t_longValue,t_length);m_record.m_endTime=t_longValue;
 	  session->getColumnValue("RELATION_TYPE_CODE"  ,&t_strValue ,t_length);
 	                        t_strValue[t_length]=0;m_record.m_relation_type=t_strValue;
 	return true;
}

void Mdb_VpnInfo_Number::setKeyParams(InputParams *pInputParamsseIndex) const
{
  char t_strValue[64+1];
  sprintf(t_strValue,"%s",m_record.m_memberNumber.c_str());
	pInputParamsseIndex->setValue(VAR_TYPE_VSTR,(void *)t_strValue);
}

void Mdb_VpnInfo_Number::setOtherParams(InputParams *pInputParamsseOther) const
{
  char t_strValue[64+1];
  sprintf(t_strValue,"%s",m_record.m_memberIdType.c_str());
  pInputParamsseOther->setValue(VAR_TYPE_VSTR,(void *)t_strValue);	
 	
}

Mdb_VpnInfo_Number::Mdb_VpnInfo_Number(const Mdb_VpnInfo_Number& right)
{
	*this=right;
}

void Mdb_VpnInfo_Number::operator=(const Mdb_VpnInfo_Number& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_VpnInfo_Number &po)
{
	os << po.m_record.m_userId      << " , "
	   << po.m_record.m_memberType  << " , "
	   << po.m_record.m_memberIdType<< " , "
	   << po.m_record.m_memberId    << " , "
	   << po.m_record.m_memberNumber<< " , "
	   << po.m_record.m_beginTime   << " , "
	   << po.m_record.m_endTime     << " , "
	   << po.m_record.m_lastMemberType << " , "
	   << po.m_record.m_lastVpnId;
	return os;
}

Mdb_Property Mdb_VpnInfo_Number::m_property={"TF_F_USER_MEMBER",
                              "IDX_TF_F_USER_MEMBER_NUMBER",1,"member_role_type = :0",1};



