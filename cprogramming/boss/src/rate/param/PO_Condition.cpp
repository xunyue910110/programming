#include "PO_Condition.h"


bool PO_Condition::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_Condition::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_id = atoi(tmpstr.c_str());
	
	m_record.m_cond = 0;			
	return true;
}


PO_Condition::PO_Condition(const PO_Condition& right)
{
	*this = right;
}


void PO_Condition::operator = (const PO_Condition& right)
{
	m_record = right.m_record;
}


int operator == (const PO_Condition &left,const PO_Condition& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_Condition &left,const PO_Condition& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_Condition& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_Condition::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_Condition","","","",
    "SELECT COND_ID FROM TD_B_COND",1,"SELECT count(*) FROM TD_B_COND"};

