#include "PO_FeepolicyMutexRela.h"


bool PO_FeepolicyMutexRela::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}

bool PO_FeepolicyMutexRela::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_highpriorId= tmpstr.c_str();

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_lowpriorId = tmpstr.c_str();
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_mutexType = tmpstr[0];
				
	return true;
}


PO_FeepolicyMutexRela::PO_FeepolicyMutexRela(const PO_FeepolicyMutexRela& right)
{
	*this = right;
}


void PO_FeepolicyMutexRela::operator = (const PO_FeepolicyMutexRela& right)
{
	m_record = right.m_record;
}


int operator == (const PO_FeepolicyMutexRela &left,const PO_FeepolicyMutexRela& right)
{
	if (right.m_record.m_highpriorId==0)
   return (left.m_record.m_lowpriorId == right.m_record.m_lowpriorId);
  if (right.m_record.m_lowpriorId==0)
   return (left.m_record.m_highpriorId == right.m_record.m_highpriorId);	
   
   return  (left.m_record.m_lowpriorId == right.m_record.m_lowpriorId&&
            left.m_record.m_highpriorId == right.m_record.m_highpriorId);
  
}


int operator < (const PO_FeepolicyMutexRela &left,const PO_FeepolicyMutexRela& right)
{
    if (left.m_record.m_lowpriorId == right.m_record.m_lowpriorId)
    {
        return (left.m_record.m_highpriorId < right.m_record.m_highpriorId);
    }
    else
{
	return (left.m_record.m_lowpriorId < right.m_record.m_lowpriorId);
    }
}


ostream& operator << (ostream& os,const PO_FeepolicyMutexRela& po)
{	
	os<<po.m_record.m_lowpriorId<<" , "<<
	    po.m_record.m_highpriorId<<" , "<<
	    po.m_record.m_mutexType;
	    
	return os;
}


PO_Property PO_FeepolicyMutexRela::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_DefaultFeePolicy","","","",
    "SELECT highprior_feepolicy_id,lowprior_feepolicy_id,mutex_type FROM TD_B_FEEPOLICY_MUTUX_RELA",3,
    "SELECT count(*) FROM TD_B_FEEPOLICY_MUTUX_RELA"};

