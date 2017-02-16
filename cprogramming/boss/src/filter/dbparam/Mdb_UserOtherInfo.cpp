#include "Mdb_UserOtherInfo.h"

bool Mdb_UserOtherInfo::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserOtherInfo::convert(Session* session)
{
	int t_length,t_intValue;
	long t_longValue;
	char t_strTmp[64+1];

	session->getColumnValue("otherinfo_type",t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_otherinfo_type = t_strTmp;

 	session->getColumnValue("attr_value1"   ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_attr_value1 = t_strTmp;

 	session->getColumnValue("attr_value2"   ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_attr_value2 = t_strTmp;

 	session->getColumnValue("attr_value3"   ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_attr_value3 = t_strTmp;

	session->getColumnValue("start_date"    ,&t_longValue,t_length);m_record.m_beginTime=t_longValue;

	session->getColumnValue("end_date"      ,&t_longValue,t_length);m_record.m_endTime=t_longValue;
 	return true;
}

void Mdb_UserOtherInfo::setKeyParams(InputParams *pInputParamsseIndex) const
{
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&m_record.m_userId);
}

void Mdb_UserOtherInfo::setOtherParams(InputParams *pInputParamsseOther) const
{

}

Mdb_UserOtherInfo::Mdb_UserOtherInfo(const Mdb_UserOtherInfo& right)
{
	*this=right;
}

void Mdb_UserOtherInfo::operator=(const Mdb_UserOtherInfo& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_UserOtherInfo &po)
{
	os << po.m_record.m_userId        << " , "
	   << po.m_record.m_otherinfo_type<< " , "
	   << po.m_record.m_attr_value1   << " , "
	   << po.m_record.m_attr_value2   << " , "
	   << po.m_record.m_attr_value3   << " , "
	   << po.m_record.m_beginTime     << " , "
	   << po.m_record.m_endTime;
	return os;
}

//string 表名;string 索引名;int 索引字段数;string 其它条件语句;int 其它条件参数个数
Mdb_Property Mdb_UserOtherInfo::m_property={"tf_f_user_otherinfo","idx_tf_f_user_otherinfo_user_id",1,"",0};
