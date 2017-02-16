#include "PO_RateEventExclusiveRela.h"


bool PO_RateEventExclusiveRela::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}

bool PO_RateEventExclusiveRela::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_EFeepolicyID = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_ExclusivedEFID = atoi(tmpstr.c_str());
				
	return true;
}


PO_RateEventExclusiveRela::PO_RateEventExclusiveRela(const PO_RateEventExclusiveRela& right)
{
	*this = right;
}


void PO_RateEventExclusiveRela::operator = (const PO_RateEventExclusiveRela& right)
{
	m_record = right.m_record;
}


int operator == (const PO_RateEventExclusiveRela &left,const PO_RateEventExclusiveRela& right)
{
   return (left.m_record.m_EFeepolicyID == right.m_record.m_EFeepolicyID);
}


int operator < (const PO_RateEventExclusiveRela &left,const PO_RateEventExclusiveRela& right)
{
	return (left.m_record.m_EFeepolicyID < right.m_record.m_EFeepolicyID);

}


ostream& operator << (ostream& os,const PO_RateEventExclusiveRela& po)
{	
	os<<po.m_record.m_EFeepolicyID<<" , "<<
	    po.m_record.m_ExclusivedEFID;
	    
	return os;
}


PO_Property PO_RateEventExclusiveRela::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_DefaultFeePolicy","","","",
    "SELECT event_feepolicy_id,exclusived_efid FROM TD_B_EVENT_EXCLUSIVE",2,
    "SELECT count(*) FROM TD_B_EVENT_EXCLUSIVE"};

