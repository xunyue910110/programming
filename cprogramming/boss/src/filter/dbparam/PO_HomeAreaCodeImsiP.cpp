#include "PO_HomeAreaCodeImsiP.h"

bool PO_HomeAreaCodeImsiP::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_HomeAreaCodeImsiP::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.beginImsi=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.endImsi=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.beginTime=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.endTime=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.feeCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.officeCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.userType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.groupId=tmpstr;

	return true;
}

PO_HomeAreaCodeImsiP::PO_HomeAreaCodeImsiP(const PO_HomeAreaCodeImsiP& right)
{
	*this=right;
}

void PO_HomeAreaCodeImsiP::operator=(const PO_HomeAreaCodeImsiP& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_HomeAreaCodeImsiP &left,const PO_HomeAreaCodeImsiP& right)
{
	return ((left.recordStruct.beginImsi>=right.recordStruct.beginImsi)&&
			(left.recordStruct.endImsi<=right.recordStruct.endImsi))||
			((left.recordStruct.beginImsi<=right.recordStruct.beginImsi)&&
			(left.recordStruct.endImsi>=right.recordStruct.endImsi));
}

int operator<(const PO_HomeAreaCodeImsiP &left,const PO_HomeAreaCodeImsiP& right)
{
	return (left.recordStruct.beginImsi<right.recordStruct.beginImsi)&&
			(left.recordStruct.endImsi<right.recordStruct.endImsi);
}

ostream& operator<<(ostream& os,const PO_HomeAreaCodeImsiP& po)
{
	os<<po.recordStruct.beginImsi<<" , "<<
			po.recordStruct.endImsi<<" , "<<
			po.recordStruct.beginTime<<" , "<<
			po.recordStruct.endTime<<" , "<<
			po.recordStruct.feeCode<<" , "<<
			po.recordStruct.officeCode<<" , "<<
			po.recordStruct.userType<<" , "<<
			po.recordStruct.groupId;
	return os;
}


PO_Property PO_HomeAreaCodeImsiP::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_HomeAreaCodeImsiP",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select a.BEGIN_IMSI, a.END_IMSI, a.BEGIN_TIME, a.END_TIME, a.area_code, a.OFFICE_CODE, a.asp||a.serv_type||a.home_type,b.group_id from TD_IMSIP a, td_national_fee_code b where trim(a.area_code)=trim(b.area_code)",
		8,"select count(*) from TD_IMSIP a, td_national_fee_code b where trim(a.area_code)=trim(b.area_code)"};
