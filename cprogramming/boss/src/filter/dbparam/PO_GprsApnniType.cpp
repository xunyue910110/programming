#include "PO_GprsApnniType.h"

bool PO_GprsApnniType::getProperty(PO_Property &property)
{
    property = m_property;
    return true;
}


bool PO_GprsApnniType::convert(const StringVector &argname)
{
	int    i = 0;
	string tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_apnGroup = tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_apnType = tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_priority = atoi(tmpstr.c_str());
	
	return true;
}

PO_GprsApnniType::PO_GprsApnniType(const PO_GprsApnniType& right)
{
    *this = right;
}

void PO_GprsApnniType::operator=(const PO_GprsApnniType& right)
{
    m_record = right.m_record;
}

int operator==(const PO_GprsApnniType &left,const PO_GprsApnniType& right)
{
	return (left.m_record.m_apnGroup==right.m_record.m_apnGroup);
}

int operator<(const PO_GprsApnniType &left,const PO_GprsApnniType& right)
{
	if(left.m_record.m_apnGroup<right.m_record.m_apnGroup)
		return true;
	return false;
}

ostream& operator<<(ostream& os,const PO_GprsApnniType& po)
{
    os<<po.m_record.m_apnGroup <<" , "
      <<po.m_record.m_apnType  <<" , "
      <<po.m_record.m_priority   ;
    return os;
}


PO_Property PO_GprsApnniType::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_GprsApnniType",USERNAME_1,PASSWORD_1,SERVNAME_1,
    "select Apnni_group,Apnni_type,priority from TD_APNNI_GROUP_TYPE",
    3,"select count(*) from TD_APNNI_GROUP_TYPE"};

