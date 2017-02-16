#include "PO_FixSpecialPhoneSection.h"

bool PO_FixSpecialPhoneSection::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_FixSpecialPhoneSection::convert(const StringVector &argname)
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
	recordStruct.m_sectionCode=tmpstr;

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

PO_FixSpecialPhoneSection::PO_FixSpecialPhoneSection(const PO_FixSpecialPhoneSection& right)
{
	*this=right;
}

void PO_FixSpecialPhoneSection::operator=(const PO_FixSpecialPhoneSection& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_FixSpecialPhoneSection &left,const PO_FixSpecialPhoneSection& right)
{
	return 1;
}

int operator<(const PO_FixSpecialPhoneSection &left,const PO_FixSpecialPhoneSection& right)
{
	if (left.recordStruct.m_phone<right.recordStruct.m_phone) return false;
	return true;
}

ostream& operator<<(ostream& os,const PO_FixSpecialPhoneSection& po)
{
	os<<  po.recordStruct.m_matchType     <<" , "<<
				po.recordStruct.m_prefixType    <<" , "<<
	      po.recordStruct.m_phone         <<" , "<<
	      po.recordStruct.m_areaCode      <<" , "<<
	      po.recordStruct.m_sectionCode   <<" , "<<
	      po.recordStruct.m_beginTime     <<" , "<<
	      po.recordStruct.m_endTime;
	return os;
}


PO_Property PO_FixSpecialPhoneSection::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_FixSpecialPhoneSection",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select MATCH_TYPE,PREFIX_TYPE,PHONE,AREA_CODE,SECTION_CODE,BEGIN_TIME,END_TIME from TD_FIX_SPECIALPHONE_SECTION ",
		7,"select count(*) from TD_FIX_SPECIALPHONE_SECTION"};

