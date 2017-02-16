#include "PO_ObjectUser.h"


bool PO_ObjectUser::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_ObjectUser::convert(const StringVector &argname)
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
	m_record.m_attrType = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_attrId = atoi(tmpstr.c_str());		
	
	return true;
}


PO_ObjectUser::PO_ObjectUser(const PO_ObjectUser &right)
{
	*this = right;
}


void PO_ObjectUser::operator = (const PO_ObjectUser &right)
{
	m_record = right.m_record;
}


int operator == (const PO_ObjectUser &left,const PO_ObjectUser& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_ObjectUser &left,const PO_ObjectUser& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_ObjectUser& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_ObjectUser::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_ObjectUser","","","",
    "SELECT OBJECT_ID,ATTR_TYPE,ATTR_ID "
    "FROM TD_B_OBJECT WHERE ATTR_TYPE = '3'",3,
    "SELECT count(*) FROM TD_B_OBJECT WHERE ATTR_TYPE = '3'"};
