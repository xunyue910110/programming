#include "Mdb_UserServState.h"

bool Mdb_UserServState::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserServState::convert(Session* session)
{
	int t_length,t_intValue;
	long t_longValue;
  char t_strTmp[20+1];

 	session->getColumnValue("main_tag"    ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_main_tag = t_strTmp;

 	session->getColumnValue("serv_state_code",t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_state = t_strTmp;

	session->getColumnValue("start_date"    ,&t_longValue,t_length);m_record.m_beginTime=t_longValue;

	session->getColumnValue("end_date"      ,&t_longValue,t_length);m_record.m_endTime=t_longValue;
 	return true;
}

void Mdb_UserServState::setKeyParams(InputParams *pInputParamsseIndex) const
{
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&m_record.m_userId);
}

void Mdb_UserServState::setOtherParams(InputParams *pInputParamsseOther) const
{

}

Mdb_UserServState::Mdb_UserServState(const Mdb_UserServState& right)
{
	*this=right;
}

void Mdb_UserServState::operator=(const Mdb_UserServState& right)
{
	m_record=right.m_record;
}

int operator<(const Mdb_UserServState &left,const Mdb_UserServState& right)
{
	return (left.m_record.m_beginTime>right.m_record.m_beginTime);
}

ostream& operator<<(ostream& os,const Mdb_UserServState &po)
{
	os << po.m_record.m_userId        << " , "
	   << po.m_record.m_main_tag      << " , "
	   << po.m_record.m_state         << " , "
	   << po.m_record.m_stategroup    << " , "
	   << po.m_record.m_beginTime     << " , "
	   << po.m_record.m_endTime;
	return os;
}

//string 表名;string 索引名;int 索引字段数;string 其它条件语句;int 其它条件参数个数
Mdb_Property Mdb_UserServState::m_property={"tf_f_user_servstate","idx_tf_f_user_servstate",1,"",0};
