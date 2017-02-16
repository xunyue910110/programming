#include "PO_BillingCycle.h"

bool PO_BillingCycle::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}

bool PO_BillingCycle::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	//add by gaojf 2009-2-13 18:41 
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_dbNo=atoi(tmpstr.c_str());
	/////////////////////////////////
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_cycleId=tmpstr;
	
	//limeng 2009-09-21
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_eparchyCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_beginDate=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_endDate=tmpstr;
	
	return true;
}

PO_BillingCycle::PO_BillingCycle(const PO_BillingCycle& right)
{
	*this=right;
}

void PO_BillingCycle::operator=(const PO_BillingCycle& right)
{
	m_record=right.m_record;
}

int operator==(const PO_BillingCycle &left,const PO_BillingCycle& right)
{
  if (left.m_record.m_dbNo != right.m_record.m_dbNo)
  {
    return false;
  }
  //limeng 2009-09-21
  if (left.m_record.m_eparchyCode != right.m_record.m_eparchyCode)
  {
    return false;
  }
	return ((left.m_record.m_beginDate >= right.m_record.m_beginDate &&
	         left.m_record.m_beginDate <= right.m_record.m_endDate) || 
	        (left.m_record.m_beginDate <= right.m_record.m_beginDate &&
	         left.m_record.m_endDate   >= right.m_record.m_beginDate));
}

int operator<(const PO_BillingCycle &left,const PO_BillingCycle& right)
{
	if (left.m_record.m_dbNo < right.m_record.m_dbNo ) return true;
	if (left.m_record.m_dbNo > right.m_record.m_dbNo ) return false;
	//limeng 2009-09-21
	if (left.m_record.m_eparchyCode < right.m_record.m_eparchyCode ) return true;
	if (left.m_record.m_eparchyCode > right.m_record.m_eparchyCode ) return false;

	if (left.m_record.m_beginDate < right.m_record.m_beginDate &&
	    left.m_record.m_endDate   < right.m_record.m_endDate )
	{
		return true;
	}

	if (left.m_record.m_beginDate > right.m_record.m_beginDate &&
	    left.m_record.m_endDate   > right.m_record.m_endDate )
	{
		return false;
	}	
	
	return false;
}

ostream& operator<<(ostream& os,const PO_BillingCycle& po)
{
	os<<po.m_record.m_dbNo     <<" , "<<
	    po.m_record.m_cycleId  <<" , "<<
	    po.m_record.m_eparchyCode  <<" , "<<
			po.m_record.m_beginDate<<" , "<<
			po.m_record.m_endDate;
			
	return os;
}

PO_Property PO_BillingCycle::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_BillingCycle",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select db_no,cycle_id,eparchy_code,to_char(cyc_start_time,'YYYYMMDD'), \
		        to_char(cyc_end_time,'YYYYMMDD')  from vw_td_b_cycle_eparchy",
		5,"select count(*) from vw_td_b_cycle_eparchy"};

