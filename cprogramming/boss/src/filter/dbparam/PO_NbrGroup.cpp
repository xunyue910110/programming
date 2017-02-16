#include "PO_NbrGroup.h"

bool PO_NbrGroup::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_NbrGroup::convert(const StringVector &argname)
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
	recordStruct.m_beginPhone=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_endPhone=tmpstr;

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

PO_NbrGroup::PO_NbrGroup(const PO_NbrGroup& right)
{
	*this=right;
}

void PO_NbrGroup::operator=(const PO_NbrGroup& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_NbrGroup &left,const PO_NbrGroup& right)
{
	return ((left.recordStruct.m_beginPhone>=right.recordStruct.m_beginPhone)&&
			(left.recordStruct.m_endPhone<=right.recordStruct.m_endPhone))||
			((left.recordStruct.m_beginPhone<=right.recordStruct.m_beginPhone)&&
			(left.recordStruct.m_endPhone>=right.recordStruct.m_endPhone));
}

int operator<(const PO_NbrGroup &left,const PO_NbrGroup& right)
{
	return (left.recordStruct.m_beginPhone<right.recordStruct.m_beginPhone)&&
			(left.recordStruct.m_endPhone<right.recordStruct.m_endPhone);
}

ostream& operator<<(ostream& os,const PO_NbrGroup& po)
{
	os<<po.recordStruct.m_msc          <<" , "<<
	    po.recordStruct.m_beginPhone   <<" , "<<
      po.recordStruct.m_endPhone     <<" , "<<
      po.recordStruct.m_ServType     <<" , "<<
	    po.recordStruct.m_bill_type    <<" , "<<
      po.recordStruct.m_billNumber   <<" , "<<
      po.recordStruct.m_beginTime    <<" , "<<
			po.recordStruct.m_endTime;
	return os;
}


PO_Property PO_NbrGroup::m_property={FROM_DATABASE,BY_TIME,
		"","PO_NbrGroup",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select MSC, AREA_CODE||BEGIN_PHONE, AREA_CODE||END_PHONE, BILL_TYPE, BILL_NUMBER, SERV_TYPE, BEGIN_TIME, END_TIME from TD_NBR_GROUP",
		8,"select count(*) from TD_NBR_GROUP"};
