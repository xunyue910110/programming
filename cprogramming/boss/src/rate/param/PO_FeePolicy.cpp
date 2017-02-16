#include "PO_FeePolicy.h"


bool PO_FeePolicy::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_FeePolicy::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_id = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_source = tmpstr[0];

	m_record.m_comp = 0;
			
	return true;
}


PO_FeePolicy::PO_FeePolicy(const PO_FeePolicy& right)
{
	*this = right;
}


void PO_FeePolicy::operator = (const PO_FeePolicy& right)
{
	m_record = right.m_record;
}


int operator == (const PO_FeePolicy &left,const PO_FeePolicy& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_FeePolicy &left,const PO_FeePolicy& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_FeePolicy& po)
{
	os<< po.m_record.m_id		  <<" , "<<
		 po.m_record.m_source     <<" , "<<
		 po.m_record.m_comp; 

	return os;
}


PO_Property PO_FeePolicy::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_FeePolicy","","","",
    "SELECT FEEPOLICY_ID,FEEPOLICY_SOURCE FROM TD_B_FEEPOLICY",
    2,"SELECT count(*) FROM TD_B_FEEPOLICY"};

