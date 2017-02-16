#include "PO_HomeAreaCodeI.h"

bool PO_HomeAreaCodeI::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_HomeAreaCodeI::convert(const StringVector &argname)
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
	
		//Added by Sunam 2005-11-3
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.groupIdEx=tmpstr;
	
	// added by yaoh  2007-7-3
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.groupIdEx1=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.groupIdEx2=tmpstr;	
	
	//Added By Sunam 2007-12-7 10:43
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.groupIdGprs =tmpstr;	

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.userType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.nation_code=tmpstr;

	return true;
}

PO_HomeAreaCodeI::PO_HomeAreaCodeI(const PO_HomeAreaCodeI& right)
{
	*this=right;
}

void PO_HomeAreaCodeI::operator=(const PO_HomeAreaCodeI& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_HomeAreaCodeI &left,const PO_HomeAreaCodeI& right)
{
	return left.recordStruct.feeCode==right.recordStruct.feeCode;
}

int operator<(const PO_HomeAreaCodeI &left,const PO_HomeAreaCodeI& right)
{
	return (left.recordStruct.feeCode<right.recordStruct.feeCode);
}

ostream& operator<<(ostream& os,const PO_HomeAreaCodeI& po)
{
	os<<po.recordStruct.feeCode<<" , "<<
			po.recordStruct.beginTime<<" , "<<
			po.recordStruct.endTime<<" , "<<
			po.recordStruct.groupId<<" , "<<
			po.recordStruct.groupIdEx<<" , "<<
			po.recordStruct.groupIdEx1<<" , "<<
			po.recordStruct.groupIdEx2<<" , "<<
			po.recordStruct.groupIdGprs<<" , "<<
			po.recordStruct.userType<<" , "<<
			po.recordStruct.nation_code;
	return os;
}


PO_Property PO_HomeAreaCodeI::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_HomeAreaCodeI",USERNAME_1,PASSWORD_1,SERVNAME_1,
		//Modified by Sunam 2005-11-3
		//"select NATION_AREA_CODE, BEGIN_TIME, END_TIME, GROUP_ID, asp||serv_type||home_type from TD_INTER_FEE_CODE",
		"select NATION_AREA_CODE, BEGIN_TIME, END_TIME, GROUP_ID, GROUP_ID_EX, GROUP_ID_EX1, GROUP_ID_EX2, GROUP_ID_GPRS, asp||serv_type||home_type, NATION_CODE from TD_INTER_FEE_CODE",
		//5,"select count(*) from TD_INTER_FEE_CODE"};
		10,"select count(*) from TD_INTER_FEE_CODE"};


