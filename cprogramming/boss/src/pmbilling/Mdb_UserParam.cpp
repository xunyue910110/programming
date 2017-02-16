#include "Mdb_UserParam.h"

bool Mdb_UserParam::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserParam::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strValue[128+1];
	
 	session->getColumnValue("USER_ID"     ,&t_longValue,t_length);m_record.m_userId=t_longValue;
 	session->getColumnValue("PARAM_ID"   ,&t_intValue,t_length); m_record.m_paramId=t_intValue;
 	session->getColumnValue("PARAM_VALUE",&t_strValue,t_length); 
 	           t_strValue[t_length]=0;m_record.m_value=t_strValue;
 	session->getColumnValue("START_DATE"   ,&t_longValue,t_length);m_record.m_beginTime=t_longValue;
 	session->getColumnValue("END_DATE"     ,&t_longValue,t_length);m_record.m_endTime=t_longValue;
 	return true;
}

void Mdb_UserParam::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_longValue;
  t_longValue = m_record.m_userId.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_longValue);
}

void Mdb_UserParam::setOtherParams(InputParams *pInputParamsseOther) const
{
	
}

Mdb_UserParam::Mdb_UserParam(const Mdb_UserParam& right)
{
	*this=right;
}

void Mdb_UserParam::operator=(const Mdb_UserParam& right)
{
	m_record=right.m_record;
}

//Added by Malone 2009-11-28 13:19 start
int operator<(const Mdb_UserParam &left,const Mdb_UserParam& right)
{
	if ( left.m_record.m_paramId == right.m_record.m_paramId )
	{
			//参数ID相同时，根据开始逆向排序，获取时取排好序后的第一条
			return (left.m_record.m_beginTime>right.m_record.m_beginTime);
	}
	else
	{
			return (left.m_record.m_paramId < right.m_record.m_paramId);
	}
}
//Added by Malone 2009-11-28 13:19 end

ostream& operator<<(ostream& os,const Mdb_UserParam &po)
{
	os << po.m_record.m_userId    << " , "
	   << po.m_record.m_paramId  << " , "
	   << po.m_record.m_value    << " , "
	   << po.m_record.m_beginTime  << " , "
	   << po.m_record.m_endTime;
	return os;
}

Mdb_Property Mdb_UserParam::m_property={"TF_F_USER_PARAM",
                              "IDX_TF_F_USER_PARAM_USER_ID",1,"",0};
