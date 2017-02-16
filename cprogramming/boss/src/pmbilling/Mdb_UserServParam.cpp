#include "Mdb_UserServParam.h"

bool Mdb_UserServParam::getProperty(Mdb_Property &property) const
{
    property=m_property;
    return true;
}

bool Mdb_UserServParam::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[100+1];
	
    session->getColumnValue("SERV_INS_ID",&t_longValue,t_length);m_record.m_servInsId = t_longValue;
    session->getColumnValue("SERV_PARAM_ID",&m_record.m_servParamId,t_length);
    session->getColumnValue("SERV_PARAM_VALUE",&t_strTmp,t_length);
     	           t_strTmp[t_length]=0;m_record.m_servParamValue=t_strTmp;
    session->getColumnValue("START_DATE",&t_longValue,t_length);m_record.m_startDate = t_longValue;
    session->getColumnValue("END_DATE",&t_longValue,t_length);m_record.m_endDate = t_longValue;

    return true;
}

void Mdb_UserServParam::setKeyParams(InputParams *pInputParamsseIndex) const
{
  long t_longValue;
  t_longValue = m_record.m_servInsId;
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_longValue);

}

void Mdb_UserServParam::setOtherParams(InputParams *pInputParamsseOther) const
{

}

Mdb_UserServParam::Mdb_UserServParam(const Mdb_UserServParam& right)
{
    *this=right;
}

void Mdb_UserServParam::operator=(const Mdb_UserServParam& right)
{
    m_record=right.m_record;
}

//Added by Malone 2009-11-28 13:19 start
int operator<(const Mdb_UserServParam &left,const Mdb_UserServParam& right)
{
	if ( left.m_record.m_servParamId == right.m_record.m_servParamId )
	{
			//参数ID相同时，根据开始逆向排序，获取时取排好序后的第一条
			return (left.m_record.m_startDate>right.m_record.m_startDate);
	}
	else
	{
			return (left.m_record.m_servParamId < right.m_record.m_servParamId);
	}
}
//Added by Malone 2009-11-28 13:19 end

ostream& operator<<(ostream& os,const Mdb_UserServParam &po)
{
    os  << po.m_record.m_servInsId << " , "
        << po.m_record.m_servParamId << " , "
        << po.m_record.m_servParamValue << " , "
        << po.m_record.m_startDate << " , "
        << po.m_record.m_endDate ;
    return os;
}

Mdb_Property Mdb_UserServParam::m_property={"TF_F_USER_SERVPARAM",
                              "IDX_TF_F_USER_SERVPARAM_INSID",1,"",0};

