#include "PO_SpecialPhoneGroup.h"

bool PO_SpecialPhoneGroup::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_SpecialPhoneGroup::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_matchType=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_prefixType=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_phone=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_areaCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_groupId=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_beginTime=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_endTime=tmpstr;

	return true;
}

PO_SpecialPhoneGroup::PO_SpecialPhoneGroup(const PO_SpecialPhoneGroup& right)
{
	*this=right;
}

void PO_SpecialPhoneGroup::operator=(const PO_SpecialPhoneGroup& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_SpecialPhoneGroup &left,const PO_SpecialPhoneGroup& right)
{
	return 1;
}

int operator<(const PO_SpecialPhoneGroup &left,const PO_SpecialPhoneGroup& right)
{
	if (left.recordStruct.m_phone<right.recordStruct.m_phone) return false;
	return true;
}

ostream& operator<<(ostream& os,const PO_SpecialPhoneGroup& po)
{
	os<<  po.recordStruct.m_matchType<<" , "<<
				po.recordStruct.m_prefixType<<" , "<<
	      po.recordStruct.m_phone<<" , "<<
	      po.recordStruct.m_areaCode<<" , "<<
	      po.recordStruct.m_groupId<<" , "<<
	      po.recordStruct.m_beginTime<<" , "<<
	      po.recordStruct.m_endTime;
	return os;
}


PO_Property PO_SpecialPhoneGroup::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_SpecialPhoneGroup",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select MATCH_TYPE,PREFIX_TYPE,PHONE,AREA_CODE,GROUP_ID,BEGIN_TIME,END_TIME from TD_SPECIAL_PHONE_GROUP WHERE ASP_LIMIT = '0' ",
		7,"select count(*) from TD_SPECIAL_PHONE_GROUP"};


///继承类
bool PO_SpecialPhoneGroup_CNC::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

PO_Property PO_SpecialPhoneGroup_CNC::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_SpecialPhoneGroup_CNC",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select MATCH_TYPE,PREFIX_TYPE,PHONE,AREA_CODE,GROUP_ID,BEGIN_TIME,END_TIME from TD_SPECIAL_PHONE_GROUP WHERE ASP_LIMIT <> '0' ",
		7,"select count(*) from TD_SPECIAL_PHONE_GROUP"};

