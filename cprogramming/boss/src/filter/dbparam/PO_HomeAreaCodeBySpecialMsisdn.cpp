#include "PO_HomeAreaCodeBySpecialMsisdn.h"

bool PO_HomeAreaCodeBySpecialMsisdn::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_HomeAreaCodeBySpecialMsisdn::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_beginMsisdn=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_endMsisdn=tmpstr;
	
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
	recordStruct.m_feeCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_provCode=tmpstr;

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

PO_HomeAreaCodeBySpecialMsisdn::PO_HomeAreaCodeBySpecialMsisdn(const PO_HomeAreaCodeBySpecialMsisdn& right)
{
	*this=right;
}

void PO_HomeAreaCodeBySpecialMsisdn::operator=(const PO_HomeAreaCodeBySpecialMsisdn& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_HomeAreaCodeBySpecialMsisdn &left,const PO_HomeAreaCodeBySpecialMsisdn& right)
{
	return ((left.recordStruct.m_beginMsisdn>=right.recordStruct.m_beginMsisdn)&&
			(left.recordStruct.m_endMsisdn<=right.recordStruct.m_endMsisdn))||
			((left.recordStruct.m_beginMsisdn<=right.recordStruct.m_beginMsisdn)&&
			(left.recordStruct.m_endMsisdn>=right.recordStruct.m_endMsisdn));
}

int operator<(const PO_HomeAreaCodeBySpecialMsisdn &left,const PO_HomeAreaCodeBySpecialMsisdn& right)
{
	return (left.recordStruct.m_beginMsisdn<right.recordStruct.m_beginMsisdn)&&
			(left.recordStruct.m_endMsisdn<right.recordStruct.m_endMsisdn);
}

ostream& operator<<(ostream& os,const PO_HomeAreaCodeBySpecialMsisdn& po)
{
	os<<po.recordStruct.m_beginMsisdn<<" , "<<
			po.recordStruct.m_endMsisdn<<" , "<<
			po.recordStruct.m_beginTime<<" , "<<
			po.recordStruct.m_endTime<<" , "<<
			po.recordStruct.m_feeCode<<" , "<<
			po.recordStruct.m_provCode<<" , "<<
			po.recordStruct.m_userType<<" , "<<
			po.recordStruct.m_groupId<<" , "<<
			po.recordStruct.m_calledType;
	return os;
}


PO_Property PO_HomeAreaCodeBySpecialMsisdn::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_HomeAreaCodeBySpecialMsisdn",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select a.BEGIN_MSISDN, a.END_MSISDN, a.BEGIN_TIME, a.END_TIME, a.area_code, a.PROV_CODE, a.asp||a.serv_type||a.home_type,b.group_id,a.called_type from TD_SPECIAL_MSISDN a, td_national_fee_code b where trim(a.area_code)=trim(b.area_code)",
		9,"select count(*) from TD_SPECIAL_MSISDN a, td_national_fee_code b where trim(a.area_code)=trim(b.area_code)"};
