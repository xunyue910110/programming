#include "Mdb_ReUserInfo_All.h"
//2008-10-24 9:29:30
bool Mdb_ReUserInfo_All::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_ReUserInfo_All::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[40+1];
	
	session->getColumnValue("USER_ID"           ,&t_longValue ,t_length); m_record.m_userId = t_longValue;
 	session->getColumnValue("EPARCHY_CODE"      , t_strTmp    ,t_length); 
 	        t_strTmp[t_length]=0;m_record.m_eparchyCode= t_strTmp;
	session->getColumnValue("SERIAL_NUMBER"      , t_strTmp    ,t_length); 
 	        t_strTmp[t_length]=0;m_record.m_msisdn= t_strTmp;
	session->getColumnValue("OPEN_DATE"         ,&t_longValue,t_length); m_record.m_openDate = t_longValue;
 	
 	m_record.m_cycId=0;
    m_record.m_monthEndTag='0';
 	return true;
}

void Mdb_ReUserInfo_All::setKeyParams(InputParams *pInputParamsseIndex) const
{
	
}

void Mdb_ReUserInfo_All::setOtherParams(InputParams *pInputParamsseOther) const
{

}

Mdb_ReUserInfo_All::Mdb_ReUserInfo_All(const Mdb_ReUserInfo_All& right)
{
	*this=right;
}

void Mdb_ReUserInfo_All::operator=(const Mdb_ReUserInfo_All& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_ReUserInfo_All &po)
{
	os << po.m_record.m_userId           << " , "
	   << po.m_record.m_msisdn           << " , "
	   << po.m_record.m_cycId            << " , "
	   << po.m_record.m_monthEndTag      << " , "
	   << po.m_record.m_eparchyCode      << " , "
	   << po.m_record.m_openDate;
	   
	return os;
}

Mdb_Property Mdb_ReUserInfo_All::m_property={"TF_F_USER","idx_tf_f_user_userid",0,"",0};
