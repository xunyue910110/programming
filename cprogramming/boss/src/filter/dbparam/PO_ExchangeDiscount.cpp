#include "PO_ExchangeDiscount.h"

bool PO_ExchangeDiscount::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_ExchangeDiscount::convert(const StringVector &argname)
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
	recordStruct.m_beginPhone_a=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_endPhone_a=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_ServType=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_beginPhone_b=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	recordStruct.m_endPhone_b=tmpstr;

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

PO_ExchangeDiscount::PO_ExchangeDiscount(const PO_ExchangeDiscount& right)
{
	*this=right;
}

void PO_ExchangeDiscount::operator=(const PO_ExchangeDiscount& right)
{
	recordStruct=right.recordStruct;
}

int operator==(const PO_ExchangeDiscount &left,const PO_ExchangeDiscount& right)
{
	return ((left.recordStruct.m_beginPhone_a>=right.recordStruct.m_beginPhone_a)&&
			(left.recordStruct.m_endPhone_a<=right.recordStruct.m_endPhone_a))||
			((left.recordStruct.m_beginPhone_a<=right.recordStruct.m_beginPhone_a)&&
			(left.recordStruct.m_endPhone_a>=right.recordStruct.m_endPhone_a));

	//add by dph
	/*if(((left.recordStruct.m_beginPhone_a>=right.recordStruct.m_beginPhone_a)&&
		(left.recordStruct.m_endPhone_a<=right.recordStruct.m_endPhone_a))||
		((left.recordStruct.m_beginPhone_a<=right.recordStruct.m_beginPhone_a)&&
		(left.recordStruct.m_endPhone_a>=right.recordStruct.m_endPhone_a)))
	{
		if(left.recordStruct.m_beginPhone_b == "**" ||
			left.recordStruct.m_beginPhone_b.substr(4,1) == "*" ||
			((left.recordStruct.m_beginPhone_b>=right.recordStruct.m_beginPhone_b)&&
			(left.recordStruct.m_endPhone_b<=right.recordStruct.m_endPhone_b))||
			((left.recordStruct.m_beginPhone_b<=right.recordStruct.m_beginPhone_b)&&
			(left.recordStruct.m_endPhone_b>=right.recordStruct.m_endPhone_b)))
		{
			return 1;
		}

		return 0;
	}*/

	return 0;
	//end of add by dph
}

int operator<(const PO_ExchangeDiscount &left,const PO_ExchangeDiscount& right)
{
	return (left.recordStruct.m_beginPhone_a<right.recordStruct.m_beginPhone_a)&&
			(left.recordStruct.m_endPhone_a<right.recordStruct.m_endPhone_a);
}

ostream& operator<<(ostream& os,const PO_ExchangeDiscount& po)
{
	os<<po.recordStruct.m_msc          <<" , "<<
	    po.recordStruct.m_beginPhone_a <<" , "<<
      po.recordStruct.m_endPhone_a   <<" , "<<
      po.recordStruct.m_ServType     <<" , "<<
	    po.recordStruct.m_beginPhone_b <<" , "<<
      po.recordStruct.m_endPhone_b   <<" , "<<
      po.recordStruct.m_beginTime    <<" , "<<
			po.recordStruct.m_endTime;
	return os;
}


PO_Property PO_ExchangeDiscount::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_ExchangeDiscount",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select MSC, AREA_CODE_A||BEGIN_PHONE_A, AREA_CODE_A||END_PHONE_A, SERV_TYPE, AREA_CODE_B||BEGIN_PHONE_B, AREA_CODE_B||END_PHONE_B, BEGIN_TIME, END_TIME from TD_EXCHANGE_DISCOUNT",
		8,"select count(*) from TD_EXCHANGE_DISCOUNT"};
