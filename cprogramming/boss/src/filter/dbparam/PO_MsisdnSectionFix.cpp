#include "PO_MsisdnSectionFix.h"

bool PO_MsisdnSectionFix::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_MsisdnSectionFix::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_areaCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_msisdnSeg=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_sectionCode=tmpstr;

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

PO_MsisdnSectionFix::PO_MsisdnSectionFix(const PO_MsisdnSectionFix& right)
{
	*this=right;
}

void PO_MsisdnSectionFix::operator=(const PO_MsisdnSectionFix& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_MsisdnSectionFix &left,const PO_MsisdnSectionFix& right)
{
	return (left.recordStruct.m_msisdnSeg==right.recordStruct.m_msisdnSeg &&
	       left.recordStruct.m_areaCode==right.recordStruct.m_areaCode);
}

int operator<(const PO_MsisdnSectionFix &left,const PO_MsisdnSectionFix& right)
{
	if(left.recordStruct.m_msisdnSeg == right.recordStruct.m_msisdnSeg)
	{
		return (left.recordStruct.m_areaCode<right.recordStruct.m_areaCode);
	}
	else
	{
		return (left.recordStruct.m_msisdnSeg > right.recordStruct.m_msisdnSeg);
	}
}

ostream& operator<<(ostream& os,const PO_MsisdnSectionFix& po)
{
	os<<po.recordStruct.m_areaCode <<" , "<<
	    po.recordStruct.m_msisdnSeg<<" , "<<
      po.recordStruct.m_sectionCode<<" , "<<
      po.recordStruct.m_beginTime<<" , "<<
			po.recordStruct.m_endTime;
	return os;
}


PO_Property PO_MsisdnSectionFix::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_MsisdnSectionFix",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select AREA_CODE, MSISDN_SEG, SECTION_CODE, BEGIN_TIME, END_TIME from TD_MSISDN_SECTION",
		5,"select count(*) from TD_MSISDN_SECTION"};
