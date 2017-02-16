#include "Mdb_UserServ.h"

bool Mdb_UserServ::getProperty(Mdb_Property &property) const
{
    property=m_property;
    return true;
}

bool Mdb_UserServ::convert(Session* session)
{
	int  t_length,t_intValue;
	long t_longValue;
	char t_strTmp[64+1];
	session->getColumnValue("SERV_INS_ID",&t_longValue,t_length);m_record.m_servInsId = t_longValue;
    session->getColumnValue("USER_ID",&t_longValue,t_length);m_record.m_userId = t_longValue;
    session->getColumnValue("SERV_ID",&m_record.m_servId,t_length);
    session->getColumnValue("MAIN_TAG",t_strTmp,t_length);m_record.m_mainTag   = t_strTmp[0];
    session->getColumnValue("START_DATE",&t_longValue,t_length);m_record.m_startDate = t_longValue;
    session->getColumnValue("END_DATE",&t_longValue,t_length);m_record.m_endDate = t_longValue;

    return true;
}

void Mdb_UserServ::setKeyParams(InputParams *pInputParamsseIndex) const
{
	long t_longValue;
	t_longValue = m_record.m_userId.value();
    pInputParamsseIndex->setValue(VAR_TYPE_LONG,(void *)&t_longValue);
}

void Mdb_UserServ::setOtherParams(InputParams *pInputParamsseOther) const
{

}

Mdb_UserServ::Mdb_UserServ(const Mdb_UserServ& right)
{
    *this=right;
}

void Mdb_UserServ::operator=(const Mdb_UserServ& right)
{
    m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_UserServ &po)
{
    os  << po.m_record.m_servInsId << " , "
    	<< po.m_record.m_userId << " , "
        << po.m_record.m_servId << " , "
        << po.m_record.m_mainTag << " , "
        << po.m_record.m_startDate << " , "
        << po.m_record.m_endDate ;
    return os;
}

Mdb_Property Mdb_UserServ::m_property={"TF_F_USER_SERV",
                              "IDX_TF_F_USER_SERV",1,"",0};

