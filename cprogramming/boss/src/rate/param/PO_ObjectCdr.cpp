#include "PO_ObjectCdr.h"


bool PO_ObjectCdr::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_ObjectCdr::convert(const StringVector &argname)
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


PO_ObjectCdr::PO_ObjectCdr(const PO_ObjectCdr &right)
{
	*this = right;
}


void PO_ObjectCdr::operator = (const PO_ObjectCdr &right)
{
	m_record = right.m_record;
}


int operator == (const PO_ObjectCdr &left,const PO_ObjectCdr& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_ObjectCdr &left,const PO_ObjectCdr& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_ObjectCdr& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_ObjectCdr::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_ObjectCdr","","","",
    "SELECT OBJECT_ID,ATTR_TYPE,ATTR_ID "
    "FROM TD_B_OBJECT WHERE ATTR_TYPE = '2'",3,
    "SELECT count(*) FROM TD_B_OBJECT WHERE ATTR_TYPE = '2'"};
