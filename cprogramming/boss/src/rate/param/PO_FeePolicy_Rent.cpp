#include "PO_FeePolicy_Rent.h"

bool PO_FeePolicy_Rent::getProperty(PO_Property &property)
{
	property=m_property;
	return true;
}


bool PO_FeePolicy_Rent::convert(const StringVector &argname)
{
	int i=0;
	string tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_tpId=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_roleCode=tmpstr;

	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_eventType=tmpstr;
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_monthRent=atol(tmpstr.c_str());
	
	//modified by gaojf 2009-2-18 12:33
	/*
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_halfRent=atol(tmpstr.c_str());
	*/
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	if(strlen(tmpstr.c_str())>0)
	{
	  m_record.m_compMethod=tmpstr.c_str()[0];
	}else
	{
	  m_record.m_compMethod='\0';
	}
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_sumBillId=atoi(tmpstr.c_str());
	
	tmpstr=*(argname.begin()+i++);
	tmpstr.erase(0,tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1,tmpstr.npos);
	m_record.m_billId=atoi(tmpstr.c_str());

	return true;
}

PO_FeePolicy_Rent::PO_FeePolicy_Rent(const PO_FeePolicy_Rent& right)
{
	*this=right;
}

void PO_FeePolicy_Rent::operator=(const PO_FeePolicy_Rent& right)
{
	m_record=right.m_record;
}

int operator==(const PO_FeePolicy_Rent &left,const PO_FeePolicy_Rent& right)
{
	return (left.m_record.m_tpId      == right.m_record.m_tpId );
  /*
	return (left.m_record.m_tpId      == right.m_record.m_tpId &&
			    left.m_record.m_roleCode  == right.m_record.m_roleCode &&
			    left.m_record.m_eventType == right.m_record.m_eventType );
    */
}

int operator<(const PO_FeePolicy_Rent &left,const PO_FeePolicy_Rent& right)
{
	if (left.m_record.m_tpId < right.m_record.m_tpId)
	{
		return true;
	}
	if (left.m_record.m_tpId > right.m_record.m_tpId)
	{
		return false;
	}
	/*
	if (left.m_record.m_roleCode < right.m_record.m_roleCode)
	{
		return true;
	}
	if (left.m_record.m_roleCode > right.m_record.m_roleCode)
	{
		return false;
	}
	if (left.m_record.m_eventType < right.m_record.m_eventType)
	{
		return true;
	}
	if (left.m_record.m_eventType > right.m_record.m_eventType)
	{
		return false;
	}
	*/
	return false;
}

ostream& operator<<(ostream& os,const PO_FeePolicy_Rent& po)
{
	os<<po.m_record.m_tpId     <<" , "<<
			po.m_record.m_roleCode <<" , "<<
			po.m_record.m_eventType<<" , "<<
			po.m_record.m_monthRent<<" , "<<
			//po.m_record.m_halfRent <<" , "<<
			po.m_record.m_compMethod <<" , "<<
			po.m_record.m_sumBillId<<" , "<<
			po.m_record.m_billId;
			
	return os;
}

/*
PO_Property PO_FeePolicy_Rent::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_FeePolicy_Rent",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select feepolicy_id,effect_role_code,event_type_id, \
		 month_rent,half_rent,sum_item_id,bill_item_id  from TD_B_FEEPOLICY_RENT",
		7,"select count(*) from TD_B_FEEPOLICY_RENT"};
*/
PO_Property PO_FeePolicy_Rent::m_property={FROM_DATABASE,BY_FLAG,
		"","PO_FeePolicy_Rent",USERNAME_1,PASSWORD_1,SERVNAME_1,
		"select feepolicy_id,effect_role_code,event_type_id, \
		 month_rent,comp_method,sum_item_id,bill_item_id  from TD_B_FEEPOLICY_RENT",
		7,"select count(*) from TD_B_FEEPOLICY_RENT"};

