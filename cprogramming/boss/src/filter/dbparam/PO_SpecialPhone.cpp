#include "PO_SpecialPhone.h"

bool PO_SpecialPhone::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_SpecialPhone::convert(const StringVector &argname)
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
	recordStruct.m_preFixType=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_condition=atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_msisdn=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_areacode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_priority=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_servType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_asp=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_homeType=tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_groupId = atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_length = atoi(tmpstr.c_str());

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_calledType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_isNoNeedTp = tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_telitem = tmpstr;

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

PO_SpecialPhone::PO_SpecialPhone(const PO_SpecialPhone& right)
{
	*this=right;
}

void PO_SpecialPhone::operator=(const PO_SpecialPhone& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_SpecialPhone &left,const PO_SpecialPhone& right)
{
	return 1;
}

int operator<(const PO_SpecialPhone &left,const PO_SpecialPhone& right)
{
	if (left.recordStruct.m_priority < right.recordStruct.m_priority)
	{
		return false;
	}
	else if (left.recordStruct.m_priority > right.recordStruct.m_priority)
	{
		return true;
	}
	if (left.recordStruct.m_msisdn < right.recordStruct.m_msisdn) return false;
	return true;
}

ostream& operator<<(ostream& os,const PO_SpecialPhone& po)
{
	os<<po.recordStruct.m_matchType<<" , "<<
	    po.recordStruct.m_preFixType<<" , "<<
	    po.recordStruct.m_condition<<" , "<<
	    po.recordStruct.m_msisdn<<" , "<<
	    po.recordStruct.m_areacode<<" , "<<
	    po.recordStruct.m_priority<<" , "<<
			po.recordStruct.m_servType<<" , "<<
			po.recordStruct.m_asp<<" , "<<
			po.recordStruct.m_homeType<<" , "<<
			po.recordStruct.m_groupId<<" , "<<
			po.recordStruct.m_length<<" , "<<
      po.recordStruct.m_calledType  <<" , "<<
			po.recordStruct.m_isNoNeedTp  <<" , "<<
			po.recordStruct.m_telitem  <<" , "<<
			po.recordStruct.m_beginTime<<" , "<<
			po.recordStruct.m_endTime;
	return os;
}

PO_Property PO_SpecialPhone::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_SpecialPhone",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select MATCH_TYPE,PREFIX_TYPE,CONDITION_ID,PHONE,AREA_CODE,PRIORITY,SERV_TYPE,ASP,HOME_TYPE,GROUP_ID,LENGTH,CALLED_TYPE,ISNONEEDTP,TELITEM_ID,BEGIN_TIME,END_TIME from TD_SPECIAL_PHONE WHERE ASP_LIMIT = '0' ",
		16,"select count(*) from TD_SPECIAL_PHONE"};


///继承类
bool PO_SpecialPhone_CNC::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

PO_Property PO_SpecialPhone_CNC::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_SpecialPhone_CNC",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select MATCH_TYPE,PREFIX_TYPE,CONDITION_ID,PHONE,AREA_CODE,PRIORITY,SERV_TYPE,ASP,HOME_TYPE,GROUP_ID,LENGTH,CALLED_TYPE,ISNONEEDTP,TELITEM_ID,BEGIN_TIME,END_TIME from TD_SPECIAL_PHONE WHERE ASP_LIMIT <> '0' ",
		16,"select count(*) from TD_SPECIAL_PHONE"};
