#include "PO_B_Object.h"

bool PO_B_Object::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_B_Object::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_objectId=atoi(tmpstr.c_str());
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_attrType=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_attrId=atoi(tmpstr.c_str());
	
	return true;
}

PO_B_Object::PO_B_Object(const PO_B_Object& right)
{
	*this=right;
}

void PO_B_Object::operator=(const PO_B_Object& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_B_Object &left, const PO_B_Object& right)
{
	return left.recordStruct.m_objectId==right.recordStruct.m_objectId;
}

int operator<(const PO_B_Object &left,const PO_B_Object& right)
{
	return (left.recordStruct.m_objectId < right.recordStruct.m_objectId);
}

ostream& operator<<(ostream& os,const PO_B_Object& po)
{
	os<<po.recordStruct.m_objectId<<" , "<<
			po.recordStruct.m_attrType<<" , "<<
			po.recordStruct.m_attrId;
	return os;
}


PO_Property PO_B_Object::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_B_Object",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select OBJECT_ID, ATTR_TYPE, ATTR_ID from TD_B_OBJECT",
		3,"select count(*) from TD_B_OBJECT"};

