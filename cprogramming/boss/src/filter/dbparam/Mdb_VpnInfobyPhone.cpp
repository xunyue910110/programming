#include "Mdb_VpnInfobyPhone.h"

bool Mdb_VpnInfobyPhone::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_VpnInfobyPhone::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strValue[64+1];
 	session->getColumnValue("user_id"           ,&t_longValue,t_length);m_record.m_userId=t_longValue;
 	session->getColumnValue("member_role_code"  ,&t_intValue ,t_length);m_record.m_memberType=t_intValue;
 	session->getColumnValue("member_role_type"  ,&t_strValue ,t_length);
 	                        t_strValue[t_length]=0;m_record.m_memberIdType=t_strValue;
 	session->getColumnValue("member_role_id"    ,&t_longValue,t_length);m_record.m_memberId=t_longValue;
 	session->getColumnValue("member_role_number",&t_strValue ,t_length);
 	                        t_strValue[t_length]=0;m_record.m_memberNumber=t_strValue;
 	session->getColumnValue("start_date"        ,&t_longValue,t_length);m_record.m_beginTime=t_longValue;
 	session->getColumnValue("end_date"          ,&t_longValue,t_length);m_record.m_endTime=t_longValue;
 	return true;
}

void Mdb_VpnInfobyPhone::setKeyParams(InputParams *pInputParamsseIndex) const
{
	pInputParamsseIndex->setValue(VAR_TYPE_VSTR,(void *)&m_record.m_memberNumber);
}

void Mdb_VpnInfobyPhone::setOtherParams(InputParams *pInputParamsseOther) const
{
  pInputParamsseOther->setValue(VAR_TYPE_VSTR,(void *)&m_record.m_memberIdType);	
}

Mdb_VpnInfobyPhone::Mdb_VpnInfobyPhone(const Mdb_VpnInfobyPhone& right)
{
	*this=right;
}

void Mdb_VpnInfobyPhone::operator=(const Mdb_VpnInfobyPhone& right)
{
	m_record=right.m_record;
}

int operator<(const Mdb_VpnInfobyPhone &left,const Mdb_VpnInfobyPhone& right)
{
	return (left.m_record.m_beginTime>right.m_record.m_beginTime);
}

ostream& operator<<(ostream& os,const Mdb_VpnInfobyPhone &po)
{
	os << po.m_record.m_userId      << " , "
	   << po.m_record.m_memberType  << " , "
	   << po.m_record.m_memberIdType<< " , "
	   << po.m_record.m_memberId    << " , "
	   << po.m_record.m_memberNumber<< " , "
	   << po.m_record.m_beginTime   << " , "
	   << po.m_record.m_endTime     << " , "
	   << po.m_record.m_lastMemberType ;
	return os;
}

//string 表名;string 索引名;int 索引字段数;string 其它条件语句;int 其它条件参数个数
Mdb_Property Mdb_VpnInfobyPhone::m_property={"tf_f_user_member","idx_tf_f_user_member_number",1,"member_role_type = :0",1};
