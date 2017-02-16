#include "PO_HomeAreaCodeC.h"

bool PO_HomeAreaCodeC::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_HomeAreaCodeC::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.feeCode=tmpstr;

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
	recordStruct.groupId=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.userType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.cityCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.provCode=tmpstr;

	return true;
}

PO_HomeAreaCodeC::PO_HomeAreaCodeC(const PO_HomeAreaCodeC& right)
{
	*this=right;
}

void PO_HomeAreaCodeC::operator=(const PO_HomeAreaCodeC& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_HomeAreaCodeC &left,const PO_HomeAreaCodeC& right)
{
	return left.recordStruct.cityCode==right.recordStruct.cityCode;
}

int operator<(const PO_HomeAreaCodeC &left,const PO_HomeAreaCodeC& right)
{
	return (left.recordStruct.cityCode<right.recordStruct.cityCode);
}

ostream& operator<<(ostream& os,const PO_HomeAreaCodeC& po)
{
	os<<po.recordStruct.feeCode<<" , "<<
			po.recordStruct.beginTime<<" , "<<
			po.recordStruct.endTime<<" , "<<
			po.recordStruct.groupId<<" , "<<
			po.recordStruct.userType<<" , "<<
			po.recordStruct.cityCode<<" , "<<
			po.recordStruct.provCode;
	return os;
}


PO_Property PO_HomeAreaCodeC::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_HomeAreaCodeC",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select AREA_CODE, BEGIN_TIME, END_TIME, GROUP_ID, asp||serv_type||home_type,CITY_CODE,PROV_CODE from TD_NATIONAL_FEE_CODE",
		7,"select count(*) from TD_NATIONAL_FEE_CODE"};

