#include "PO_CondRuleRela.h"


bool PO_CondRuleRela::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_CondRuleRela::convert(const StringVector &argname)
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
	m_record.m_orderNo = atoi(tmpstr.c_str());

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_andOrTag = tmpstr[0];

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_condId = atoi(tmpstr.c_str());	
	
	m_record.m_condRule = 0;
	m_record.m_next = 0;
	m_record.m_condType = '1';
			
	return true;
}


PO_CondRuleRela::PO_CondRuleRela(const PO_CondRuleRela& right)
{
	*this = right;
}


void PO_CondRuleRela::operator = (const PO_CondRuleRela& right)
{
	m_record = right.m_record;
}


int operator == (const PO_CondRuleRela &left,const PO_CondRuleRela& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_CondRuleRela &left,const PO_CondRuleRela& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_CondRuleRela& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_CondRuleRela::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_CondRuleRela","","","",
    "SELECT COND_ID,ORDER_NO,AND_OR_TAG,SUB_COND_ID FROM TD_B_COMPCOND",
    4,"SELECT count(*) FROM TD_B_COMPCOND"};

