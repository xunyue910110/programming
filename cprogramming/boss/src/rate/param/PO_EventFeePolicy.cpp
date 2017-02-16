#include "PO_EventFeePolicy.h"


bool PO_EventFeePolicy::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}

bool PO_EventFeePolicy::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_id = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_feeId = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_priceId = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_rateTime = tmpstr[0];	
	
	m_record.m_next = 0;
	m_record.m_price = 0;
			
	return true;
}


PO_EventFeePolicy::PO_EventFeePolicy(const PO_EventFeePolicy& right)
{
	*this = right;
}


void PO_EventFeePolicy::operator = (const PO_EventFeePolicy& right)
{
	m_record = right.m_record;
}


int operator == (const PO_EventFeePolicy &left,const PO_EventFeePolicy& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_EventFeePolicy &left,const PO_EventFeePolicy& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_EventFeePolicy& po)
{	
	return os;
}


PO_Property PO_EventFeePolicy::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_EventFeePolicy","","","",
    "SELECT EVENT_FEEPOLICY_ID,FEE_ID,PRICE_ID,RATE_TIME "    
    "FROM TD_B_EVENT_FEEPOLICY",4,
    "SELECT count(*) FROM TD_B_EVENT_FEEPOLICY"};

