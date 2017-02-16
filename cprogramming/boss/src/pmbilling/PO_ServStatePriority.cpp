#include "PO_ServStatePriority.h"


bool PO_ServStatePriority::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_ServStatePriority::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_serviceId=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_servStateCode=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_priority=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_startDate=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_endDate=tmpstr;

	return true;
}

PO_ServStatePriority::PO_ServStatePriority(const PO_ServStatePriority& right)
{
	*this=right;
}

void PO_ServStatePriority::operator=(const PO_ServStatePriority& right)
{
	m_record=right.m_record;
}

int operator==(const PO_ServStatePriority &left,const PO_ServStatePriority& right)
{
    return (left.m_record.m_serviceId == right.m_record.m_serviceId);
}

int operator<(const PO_ServStatePriority &left,const PO_ServStatePriority& right)
{
    return (left.m_record.m_serviceId < right.m_record.m_serviceId);
}

ostream& operator<<(ostream& os,const PO_ServStatePriority& po)
{
	os<<po.m_record.m_serviceId	<<" , "<<
		po.m_record.m_servStateCode		<<" , "<<
		po.m_record.m_priority		<<" , "<<
		po.m_record.m_startDate		<<" , "<<
		po.m_record.m_endDate; 

	return os;
}


PO_Property PO_ServStatePriority::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_ServStatePriority","","","","SELECT SERVICE_ID,STATE_CODE, \
    PRIORITY,to_char(START_DATE,'yyyymmddhh24miss'),\
    to_char(END_DATE,'yyyymmddhh24miss') FROM TD_S_SERVICESTATE",
    5,"SELECT count(*) FROM TD_S_SERVICESTATE"};
