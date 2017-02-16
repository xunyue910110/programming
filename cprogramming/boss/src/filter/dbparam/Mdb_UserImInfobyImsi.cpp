#include "Mdb_UserImInfobyImsi.h"

bool Mdb_UserImInfobyImsi::getProperty(Mdb_Property &property) const
{
	property=m_property;
	return true;
}

bool Mdb_UserImInfobyImsi::convert(Session* session)
{
	int t_length,t_intValue;
	long t_longValue;
  char t_strTmp[64+1];

 	session->getColumnValue("user_id"       ,&t_longValue,t_length);m_record.m_userId=t_longValue;

 	session->getColumnValue("net_type_code"  ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_net_type_code = t_strTmp;

	session->getColumnValue("product_id"    ,&t_intValue,t_length);m_record.m_productId=t_intValue;

 	session->getColumnValue("brand_code"    ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_brand_code = t_strTmp;

 	session->getColumnValue("logic_phone"   ,t_strTmp,t_length);
 	         t_strTmp[t_length]=0;m_record.m_logic_phone = t_strTmp;

	session->getColumnValue("start_date"    ,&t_longValue,t_length);m_record.m_beginTime=t_longValue;

	session->getColumnValue("end_date"      ,&t_longValue,t_length);m_record.m_endTime=t_longValue;
 	return true;
}

void Mdb_UserImInfobyImsi::setKeyParams(InputParams *pInputParamsseIndex) const
{
	pInputParamsseIndex->setValue(VAR_TYPE_VSTR,(void *)&m_record.m_phyical_phone);
}

void Mdb_UserImInfobyImsi::setOtherParams(InputParams *pInputParamsseOther) const
{

}

Mdb_UserImInfobyImsi::Mdb_UserImInfobyImsi(const Mdb_UserImInfobyImsi& right)
{
	*this=right;
}

void Mdb_UserImInfobyImsi::operator=(const Mdb_UserImInfobyImsi& right)
{
	m_record=right.m_record;
}

ostream& operator<<(ostream& os,const Mdb_UserImInfobyImsi &po)
{
	os << po.m_record.m_userId         << " , "
	   << po.m_record.m_net_type_code  << " , "
	   << po.m_record.m_productId      << " , "
	   << po.m_record.m_brand_code     << " , "
	   << po.m_record.m_logic_phone    << " , "
	   << po.m_record.m_phyical_phone  << " , "
	   << po.m_record.m_beginTime      << " , "
	   << po.m_record.m_endTime;
	return os;
}

bool operator<(const Mdb_UserImInfobyImsi& left,const Mdb_UserImInfobyImsi& right)
{
	return (left.m_record.m_beginTime > right.m_record.m_beginTime);
}

//string 表名;string 索引名;int 索引字段数;string 其它条件语句;int 其它条件参数个数
Mdb_Property Mdb_UserImInfobyImsi::m_property={"tf_f_user_importinfo","idx_tf_f_user_importinfo_phyical_phone",1,"",0};
