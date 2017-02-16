#include "Mdb_AdcECuserNum.h"

bool Mdb_AdcECuserNum::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_AdcECuserNum::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[64+1];
	
 	session->getColumnValue("USER_NUM"          ,&t_longValue,t_length); m_record.m_userNum  = t_longValue;
 	session->getColumnValue("EA_USER_ID"        ,&t_longValue,t_length); m_record.m_ecUserId  = t_longValue;
 	session->getColumnValue("BIZ_CODE"          ,&m_record.m_bizCode ,t_length);
 	return true;
}

void Mdb_AdcECuserNum::setKeyParams(InputParams *pInputParamsseIndex) const
{
	long t_userId = m_record.m_ecUserId.value();
	pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_userId);	
	
	char t_strTmp[20+1];
	sprintf(t_strTmp,"%s",m_record.m_bizCode.c_str());
	pInputParamsseIndex->setValue(VAR_TYPE_VSTR,(void *)&t_strTmp);	
}

void Mdb_AdcECuserNum::setOtherParams(InputParams *pInputParamsseOther) const
{
	
}

Mdb_AdcECuserNum::Mdb_AdcECuserNum(const Mdb_AdcECuserNum& right)
{
	*this=right;
}

void Mdb_AdcECuserNum::operator=(const Mdb_AdcECuserNum& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_AdcECuserNum &po)
{
	os << po.m_record.m_ecUserId      << " , "
	   << po.m_record.m_userNum      << " , "
	   << po.m_record.m_bizCode;
	return os;
}

Mdb_Property Mdb_AdcECuserNum::m_property={"TF_F_USER_GRPMBMP_SUB_NUMBER","IDX_TF_F_USER_GRPMBMP_SUB_NUMBER",1,"",0};
