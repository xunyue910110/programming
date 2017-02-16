#include "PO_EffectAddup.h"


bool PO_EffectAddup::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_EffectAddup::convert(const StringVector &argname)
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
	m_record.m_addupItemCode = atoi(tmpstr.c_str());	
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_addupRoleId = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	strncpy(m_record.m_addupUpper,tmpstr.c_str(),MIN_STR_LENGTH);
	m_record.m_addupUpper[MIN_STR_LENGTH] = '\0';
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_clearTag = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_clearCycleNum = atoi(tmpstr.c_str());
	
	m_record.m_next = 0;	
	m_record.m_addupItem = 0;
			
	return true;
}


PO_EffectAddup::PO_EffectAddup(const PO_EffectAddup& right)
{
	*this = right;
}


void PO_EffectAddup::operator = (const PO_EffectAddup& right)
{
	m_record = right.m_record;
}


bool operator == (const PO_EffectAddup &left,const PO_EffectAddup& right)
{
    return (left.m_record == right.m_record);
}


bool operator < (const PO_EffectAddup &left,const PO_EffectAddup& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_EffectAddup& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_EffectAddup::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_EffectAddup","","","",
    "SELECT ADDUP_ID,ORDER_NO,ADDUP_ITEM_CODE,ADDUP_ROLE_ID,"
    "ADDUP_UPPER,CLEAR_TAG,CLEAR_CYCLE_NUM FROM TD_B_ADDUP",
    7,"SELECT count(*) FROM TD_B_ADDUP"};

