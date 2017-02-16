#include "PO_HomeAreaCodeFix.h"

bool PO_HomeAreaCodeFix::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_HomeAreaCodeFix::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_phoneSeg=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_exchangeCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_beginTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_endTime=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_areaCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_officeCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_userType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_groupId=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_calledType=tmpstr;

	return true;
}

PO_HomeAreaCodeFix::PO_HomeAreaCodeFix(const PO_HomeAreaCodeFix& right)
{
	*this=right;
}

void PO_HomeAreaCodeFix::operator=(const PO_HomeAreaCodeFix& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_HomeAreaCodeFix &left,const PO_HomeAreaCodeFix& right)
{
	return (left.recordStruct.m_phoneSeg==right.recordStruct.m_phoneSeg);
}

int operator<(const PO_HomeAreaCodeFix &left,const PO_HomeAreaCodeFix& right)
{
	return (left.recordStruct.m_phoneSeg>right.recordStruct.m_phoneSeg);
}

ostream& operator<<(ostream& os,const PO_HomeAreaCodeFix& po)
{
	os<<po.recordStruct.m_phoneSeg<<" , "<<
			po.recordStruct.m_exchangeCode<<" , "<<
			po.recordStruct.m_beginTime<<" , "<<
			po.recordStruct.m_endTime<<" , "<<
			po.recordStruct.m_areaCode<<" , "<<
			po.recordStruct.m_officeCode<<" , "<<
			po.recordStruct.m_userType<<" , "<<
			po.recordStruct.m_groupId<<" , "<<
			po.recordStruct.m_calledType;
	return os;
}

PO_Property PO_HomeAreaCodeFix::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_HomeAreaCodeFix",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select a.AREA_CODE||a.PHONE_SEG, a.AREA_CODE||a.EXCHANGE_CODE, a.BEGIN_TIME, a.END_TIME, a.AREA_CODE, b.CITY_CODE, a.ASP||a.SERV_TYPE||a.HOME_TYPE,b.GROUP_ID,a.CALLED_TYPE from TD_FIXPHONE_SEG a, TD_NATIONAL_FEE_CODE b where trim(a.AREA_CODE)=trim(b.AREA_CODE)",
		9,"select count(*) from TD_FIXPHONE_SEG a, TD_NATIONAL_FEE_CODE b where trim(a.AREA_CODE)=trim(b.AREA_CODE)"};
