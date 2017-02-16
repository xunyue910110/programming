#include "PO_Object.h"


bool PO_Object::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_Object::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_id = atoi(tmpstr.c_str());
	
	m_record.m_objectBase = 0;			
	return true;
}


PO_Object::PO_Object(const PO_Object& right)
{
	*this = right;
}


void PO_Object::operator = (const PO_Object& right)
{
	m_record = right.m_record;
}


int operator == (const PO_Object &left,const PO_Object& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_Object &left,const PO_Object& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_Object& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_Object::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_Object","","","",
    "SELECT OBJECT_ID FROM TD_B_OBJECT",1,"SELECT count(*) FROM TD_B_OBJECT"};

