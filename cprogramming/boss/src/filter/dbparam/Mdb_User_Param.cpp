#include "Mdb_User_Param.h"

bool Mdb_User_Param::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_User_Param::convert(Session* session)
{
	int t_length,t_intValue;
	long t_longValue;
	char t_strTmp[50+1];

 	session->getColumnValue("param_id"  ,&t_intValue,t_length);m_record.m_paramId = t_intValue;

  session->getColumnValue("param_value"  ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_paramValue = t_strTmp;

  session->getColumnValue("start_date" ,&t_longValue,t_length);m_record.m_beginTime = t_longValue;

 	session->getColumnValue("end_date"   ,&t_longValue,t_length);m_record.m_endTime = t_longValue;
 	return true;
}

void Mdb_User_Param::setKeyParams(InputParams *pInputParamsseIndex) const
{
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&m_record.m_userId);
}

void Mdb_User_Param::setOtherParams(InputParams *pInputParamsseOther) const
{
}

Mdb_User_Param::Mdb_User_Param(const Mdb_User_Param& right)
{
	*this=right;
}

void Mdb_User_Param::operator=(const Mdb_User_Param& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_User_Param &po)
{
	os << po.m_record.m_userId        << " , "
	   << po.m_record.m_paramId       << " , "
	   << po.m_record.m_paramValue    << " , "
	   << po.m_record.m_beginTime     << " , "
	   << po.m_record.m_endTime;
	return os;
}

//string 表名;string 索引名;int 索引字段数;string 其它条件语句;int 其它条件参数个数
Mdb_Property Mdb_User_Param::m_property={"tf_f_user_param","idx_tf_f_user_param_user_id",1,"",0};
