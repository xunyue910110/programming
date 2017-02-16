#include "PO_ObjectAddup.h"


bool PO_ObjectAddup::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_ObjectAddup::convert(const StringVector &argname)
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
	m_record.m_roleCode = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_attrType = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_attrId = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_attrSource = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_startNum = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_endNum = atoi(tmpstr.c_str());
	
	
	return true;
}


PO_ObjectAddup::PO_ObjectAddup(const PO_ObjectAddup &right)
{
	*this = right;
}


void PO_ObjectAddup::operator = (const PO_ObjectAddup &right)
{
	m_record = right.m_record;
}


int operator == (const PO_ObjectAddup &left,const PO_ObjectAddup& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_ObjectAddup &left,const PO_ObjectAddup& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_ObjectAddup& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_ObjectAddup::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_ObjectAddup","","","",
    "SELECT OBJECT_ID,ROLE_ID,ATTR_TYPE,ATTR_ID,VALUE_SRC,"
    "START_NUM,END_NUM FROM TD_B_OBJECT WHERE ATTR_TYPE in ('1','4','5')",7,
    "SELECT count(*) FROM TD_B_OBJECT WHERE ATTR_TYPE in ('1','4','5')"};
