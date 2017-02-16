#include "PO_NumberConvert.h"

bool PO_NumberConvert::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_NumberConvert::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_msc=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_phone=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_bill_type=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_billNumber=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_ServType=tmpstr;

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

PO_NumberConvert::PO_NumberConvert(const PO_NumberConvert& right)
{
	*this=right;
}

void PO_NumberConvert::operator=(const PO_NumberConvert& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_NumberConvert &left,const PO_NumberConvert& right)
{
	return (left.recordStruct.m_phone==right.recordStruct.m_phone);
}

int operator<(const PO_NumberConvert &left,const PO_NumberConvert& right)
{
	return (left.recordStruct.m_phone>right.recordStruct.m_phone);
}

ostream& operator<<(ostream& os,const PO_NumberConvert& po)
{
	os<<po.recordStruct.m_msc          <<" , "<<
	    po.recordStruct.m_phone        <<" , "<<
      po.recordStruct.m_ServType     <<" , "<<
	    po.recordStruct.m_bill_type    <<" , "<<
      po.recordStruct.m_billNumber   <<" , "<<
      po.recordStruct.m_beginTime    <<" , "<<
			po.recordStruct.m_endTime;
	return os;
}


PO_Property PO_NumberConvert::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_NumberConvert",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select MSC, AREA_CODE||ORG_PHONE, BILL_TYPE, BILL_NUMBER, SERV_TYPE, BEGIN_TIME, END_TIME from TD_NUMBER_CONVERT",
		7,"select count(*) from TD_NUMBER_CONVERT"};
