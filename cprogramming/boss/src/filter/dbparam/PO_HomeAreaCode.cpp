#include "PO_HomeAreaCode.h"

bool PO_HomeAreaCode::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_HomeAreaCode::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m123=tmpstr;
	
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
	recordStruct.userType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.groupId=tmpstr;

	return true;
}

PO_HomeAreaCode::PO_HomeAreaCode(const PO_HomeAreaCode& right)
{
	*this=right;
}

void PO_HomeAreaCode::operator=(const PO_HomeAreaCode& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_HomeAreaCode &left,const PO_HomeAreaCode& right)
{
	return left.recordStruct.m123==right.recordStruct.m123;
}

int operator<(const PO_HomeAreaCode &left,const PO_HomeAreaCode& right)
{
	return (left.recordStruct.m123<right.recordStruct.m123);
}

ostream& operator<<(ostream& os,const PO_HomeAreaCode& po)
{
	os<<po.recordStruct.m123<<" , "<<
			po.recordStruct.beginTime<<" , "<<
			po.recordStruct.endTime<<" , "<<
			po.recordStruct.feeCode<<" , "<<
			po.recordStruct.userType<<" , "<<
			po.recordStruct.groupId;
	return os;
}

PO_Property PO_HomeAreaCode::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_HomeAreaCode",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select a.m123, a.BEGIN_TIME, a.END_TIME, a.area_code, a.asp||a.serv_type||a.home_type,b.group_id from td_msc a, td_national_fee_code b where trim(a.area_code)=trim(b.area_code)",
		6,"select count(*) from td_msc a, td_national_fee_code b where trim(a.area_code)=trim(b.area_code)"};
