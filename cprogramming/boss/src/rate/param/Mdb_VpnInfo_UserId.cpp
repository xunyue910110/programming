#include "Mdb_VpnInfo_UserId.h"

bool Mdb_VpnInfo_UserId::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_VpnInfo_UserId::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strValue[64+1];
 	session->getColumnValue("USER_ID"           ,&t_longValue,t_length);m_record.m_userId      =t_longValue;
 	session->getColumnValue("MEMBER_ROLE_CODE"  ,&t_intValue ,t_length);m_record.m_memberType  =t_intValue;
 	session->getColumnValue("MEMBER_ROLE_TYPE"  ,&t_strValue ,t_length);
 	                        t_strValue[t_length]=0;m_record.m_memberIdType=t_strValue;
 	session->getColumnValue("MEMBER_ROLE_ID"    ,&t_longValue,t_length);m_record.m_memberId    =t_longValue;
 	session->getColumnValue("MEMBER_ROLE_NUMBER",&t_strValue ,t_length);
 	                        t_strValue[t_length]=0;m_record.m_memberNumber=t_strValue;
 	session->getColumnValue("START_DATE"        ,&t_longValue,t_length);m_record.m_beginTime   =t_longValue;
 	session->getColumnValue("END_DATE"          ,&t_longValue,t_length);m_record.m_endTime     =t_longValue;
 	  session->getColumnValue("RELATION_TYPE_CODE"  ,&t_strValue ,t_length);
 	                        t_strValue[t_length]=0;m_record.m_relation_type=t_strValue;
 	return true;
}

void Mdb_VpnInfo_UserId::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_longValue;
  t_longValue = m_record.m_memberId.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_longValue);
}

void Mdb_VpnInfo_UserId::setOtherParams(InputParams *pInputParamsseOther) const
{
  char t_strValue[64+1];
  sprintf(t_strValue,"%s",m_record.m_memberIdType.c_str());
  pInputParamsseOther->setValue(VAR_TYPE_VSTR,(void *)t_strValue);	
}

Mdb_VpnInfo_UserId::Mdb_VpnInfo_UserId(const Mdb_VpnInfo_UserId& right)
{
	*this=right;
}

void Mdb_VpnInfo_UserId::operator=(const Mdb_VpnInfo_UserId& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_VpnInfo_UserId &po)
{
	os << po.m_record.m_userId      << " , "
	   << po.m_record.m_memberType  << " , "
	   << po.m_record.m_memberIdType<< " , "
	   << po.m_record.m_memberId    << " , "
	   << po.m_record.m_memberNumber<< " , "
	   << po.m_record.m_beginTime   << " , "
	   << po.m_record.m_endTime     << " , "
	   << po.m_record.m_lastMemberType<<" , "
	   << po.m_record.m_lastVpnId;
	return os;
}

Mdb_Property Mdb_VpnInfo_UserId::m_property={"TF_F_USER_MEMBER",
                              "IDX_TF_F_USER_MEMBER_USERID",1,"member_role_type = :0",1};


Mdb_VpnInfo_UserId& Mdb_VpnInfo_UserId::operator=( const Mdb_VpnInfo_Number& r_right)
{
	m_record.m_userId         = r_right.m_record.m_userId;      
	m_record.m_memberType     = r_right.m_record.m_memberType;  
	m_record.m_memberIdType   = r_right.m_record.m_memberIdType;
	m_record.m_memberId       = r_right.m_record.m_memberId;    
	m_record.m_memberNumber   = r_right.m_record.m_memberNumber;
	m_record.m_beginTime      = r_right.m_record.m_beginTime;   
	m_record.m_endTime        = r_right.m_record.m_endTime;    
	m_record.m_lastMemberType = r_right.m_record.m_lastMemberType; 
	m_record.m_lastVpnId      = r_right.m_record.m_lastVpnId; 
  return *this;
}

