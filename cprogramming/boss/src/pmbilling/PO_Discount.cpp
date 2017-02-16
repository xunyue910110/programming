#include "PO_Discount.h"


bool PO_Discount::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}
    

bool PO_Discount::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_discountId = atoi(tmpstr.c_str());

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_bdiscountId = atoi(tmpstr.c_str());

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_startDate = tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_endDate = tmpstr;
	
	//add by xuf begin 2009-8-28 20:01:05
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_specDealTag = tmpstr;
	
	//add by xuf begin 2009-8-28 20:01:05
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_ruleId = tmpstr;

  //add by taijq begin 2009-12-16
  tmpstr = *(argname.begin()+i++);
  tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
  tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
  m_record.m_discountValue = atol(tmpstr.c_str());
  //add by taijq end 2009-12-16

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	
	m_record.m_discountType = DiscountInfo::DISCNT_TYPE(atoi(tmpstr.c_str()));
			
	return true;
}


PO_Discount::PO_Discount(const PO_Discount& right)
{
	*this = right;
}


void PO_Discount::operator=(const PO_Discount& right)
{
	m_record = right.m_record;
}


int operator==(const PO_Discount &left,const PO_Discount& right)
{
    return (left.m_record.m_discountId == right.m_record.m_discountId);
}


int operator<(const PO_Discount &left,const PO_Discount& right)
{
    return (left.m_record.m_discountId < right.m_record.m_discountId);
}


ostream& operator<<(ostream& os,const PO_Discount& po)
{
	os<< po.m_record.m_discountId   <<" , "<<
		 po.m_record.m_bdiscountId	<<" , "<<
		 po.m_record.m_startDate    <<" , "<<
		 po.m_record.m_endDate      <<" , "<<
		 po.m_record.m_specDealTag  <<" , "<<
		 po.m_record.m_ruleId      <<" , "<<
		 po.m_record.m_discountValue   <<" , "<<  //add by taijq 2009-12-16
		 po.m_record.m_discountType; 

	return os;
}


PO_Property PO_Discount::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_Discount","","","",
    "SELECT DISCNT_CODE,nvl(B_DISCNT_CODE,0),    \
    to_char(START_DATE,'yyyymmddhh24miss'),                \
    to_char(END_DATE,'yyyymmddhh24miss'),nvl(RSRV_STR1,0),nvl(RSRV_STR2,0),nvl(RSRV_STR3,0),RSRV_STR5 FROM TD_B_DISCNT",
    8,"SELECT count(*) FROM TD_B_DISCNT"};
