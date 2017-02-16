#include "PO_VoiceMagzine.h"

bool PO_VoiceMagzine::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_VoiceMagzine::convert(const StringVector &argname)
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

PO_VoiceMagzine::PO_VoiceMagzine(const PO_VoiceMagzine& right)
{
	*this=right;
}

void PO_VoiceMagzine::operator=(const PO_VoiceMagzine& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_VoiceMagzine &left,const PO_VoiceMagzine& right)
{
	return 1;
}

int operator<(const PO_VoiceMagzine &left,const PO_VoiceMagzine& right)
{
	if (left.recordStruct.m_phone<right.recordStruct.m_phone) return false;
	return true;
}

ostream& operator<<(ostream& os,const PO_VoiceMagzine& po)
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

//modify luoyb

PO_Property PO_VoiceMagzine::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_VoiceMagzine",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select Match_Type,PREFIX_TYPE,PHONE,AREA_CODE,GROUP_ID,BEGIN_TIME,END_TIME from TD_VOICE_MAGZINE",
		7,"select count(*) from TD_VOICE_MAGZINE"};
