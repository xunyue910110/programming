#include "PO_HomeAreaCodeP.h"

bool PO_HomeAreaCodeP::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_HomeAreaCodeP::convert(const StringVector &argname)
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
	recordStruct.m_officeCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_userType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_groupId=tmpstr;
	
	//Added by Sunam 2005-12-13 
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_calledType=tmpstr;

	return true;
}

PO_HomeAreaCodeP::PO_HomeAreaCodeP(const PO_HomeAreaCodeP& right)
{
	*this=right;
}

void PO_HomeAreaCodeP::operator=(const PO_HomeAreaCodeP& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_HomeAreaCodeP &left,const PO_HomeAreaCodeP& right)
{
	return ((left.recordStruct.m_beginMsisdn>=right.recordStruct.m_beginMsisdn)&&
			(left.recordStruct.m_endMsisdn<=right.recordStruct.m_endMsisdn))||
			((left.recordStruct.m_beginMsisdn<=right.recordStruct.m_beginMsisdn)&&
			(left.recordStruct.m_endMsisdn>=right.recordStruct.m_endMsisdn));
}

int operator<(const PO_HomeAreaCodeP &left,const PO_HomeAreaCodeP& right)
{
	return (left.recordStruct.m_beginMsisdn<right.recordStruct.m_beginMsisdn)&&
			(left.recordStruct.m_endMsisdn<right.recordStruct.m_endMsisdn);
}

ostream& operator<<(ostream& os,const PO_HomeAreaCodeP& po)
{
	os<<po.recordStruct.m_beginMsisdn<<" , "<<
			po.recordStruct.m_endMsisdn<<" , "<<
			po.recordStruct.m_beginTime<<" , "<<
			po.recordStruct.m_endTime<<" , "<<
			po.recordStruct.m_feeCode<<" , "<<
			po.recordStruct.m_officeCode<<" , "<<
			po.recordStruct.m_userType<<" , "<<
			po.recordStruct.m_groupId<<" , "<<
			//Added by Sunam 2005-12-13
			po.recordStruct.m_calledType;
	return os;
}

//Modified by Sunam 2005-12-13
/*
PO_Property PO_HomeAreaCodeP::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_HomeAreaCodeP",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select a.BEGIN_MSISDN, a.END_MSISDN, a.BEGIN_TIME, a.END_TIME, a.area_code, a.OFFICE_CODE, a.asp||a.serv_type||a.home_type,b.group_id from TD_PROV_MSISDN a, td_national_fee_code b where trim(a.area_code)=trim(b.area_code)",
		8,"select count(*) from TD_PROV_MSISDN a, td_national_fee_code b where trim(a.area_code)=trim(b.area_code)"};
*/
PO_Property PO_HomeAreaCodeP::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_HomeAreaCodeP",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select a.BEGIN_MSISDN, a.END_MSISDN, a.BEGIN_TIME, a.END_TIME, a.area_code, a.OFFICE_CODE, a.asp||a.serv_type||a.home_type,b.group_id,a.called_type from TD_PROV_MSISDN a, td_national_fee_code b where trim(a.area_code)=trim(b.area_code)",
		9,"select count(*) from TD_PROV_MSISDN a, td_national_fee_code b where trim(a.area_code)=trim(b.area_code)"};
