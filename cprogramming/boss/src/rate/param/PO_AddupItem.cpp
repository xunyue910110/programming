#include "PO_AddupItem.h"


bool PO_AddupItem::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_AddupItem::convert(const StringVector &argname)
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
	m_record.m_elemType = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_elemId = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_elemValueSrc = tmpstr[0];	
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_cycleRuleId = atoi(tmpstr.c_str());
		
	m_record.m_cycleRule = 0;
	
	return true;
}


PO_AddupItem::PO_AddupItem(const PO_AddupItem& right)
{
	*this = right;
}


void PO_AddupItem::operator = (const PO_AddupItem& right)
{
	m_record = right.m_record;
}


int operator == (const PO_AddupItem &left,const PO_AddupItem& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_AddupItem &left,const PO_AddupItem& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_AddupItem& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_AddupItem::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_AddupItem","","","",
    "SELECT ADDUP_ITEM_CODE,ELEM_TYPE,ELEM_ID,ELEM_VALUE_SRC,"
    "CYCLE_RULE_ID FROM TD_B_ADDUPITEM",5,
    "SELECT count(*) FROM TD_B_ADDUPITEM"};

