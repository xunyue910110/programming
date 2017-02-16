#include "PO_AspByMsisdnHeader.h"

bool PO_AspByMsisdnHeader::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_AspByMsisdnHeader::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_msisdnSeg = tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_asp = tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_tdType = tmpstr[0];

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_beginTime = tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_endTime = tmpstr;

	return true;
}

PO_AspByMsisdnHeader::PO_AspByMsisdnHeader(const PO_AspByMsisdnHeader& right)
{
	*this=right;
}

void PO_AspByMsisdnHeader::operator=(const PO_AspByMsisdnHeader& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_AspByMsisdnHeader &left,const PO_AspByMsisdnHeader& right)
{
	return (left.recordStruct.m_msisdnSeg == right.recordStruct.m_msisdnSeg);
}

int operator<(const PO_AspByMsisdnHeader &left,const PO_AspByMsisdnHeader& right)
{
	return (left.recordStruct.m_msisdnSeg < right.recordStruct.m_msisdnSeg);
}

ostream& operator<<(ostream& os,const PO_AspByMsisdnHeader& po)
{
	os<<po.recordStruct.m_msisdnSeg<<" , "<<
		po.recordStruct.m_asp<<" , "<<
		po.recordStruct.m_tdType<<" , "<<
		po.recordStruct.m_beginTime<<" , "<<
		po.recordStruct.m_endTime;
	return os;
}


PO_Property PO_AspByMsisdnHeader::m_property={FROM_DATABASE,BY_FLAG,"",
		"PO_AspByMsisdnHeader",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"SELECT MSISDN_SEG,ASP,TD_TYPE,BEGIN_TIME,END_TIME FROM TD_MOBILE_SEG",
		5,"SELECT COUNT(*) FROM TD_MOBILE_SEG"};
