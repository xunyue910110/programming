#include "PO_FeePolicyComp.h"


bool PO_FeePolicyComp::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}   
	
bool PO_FeePolicyComp::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;	

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_compId = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_id = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	strncpy(m_record.m_roleCodes,tmpstr.c_str(),STR_ROLECODE_ENUMLEN);
	m_record.m_roleCodes[STR_ROLECODE_ENUMLEN] = '\0';
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_eventTypeId = atoi(tmpstr.c_str());

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_eventPolicyId = atoi(tmpstr.c_str());	

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_priority = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_cdrCompFlag = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_execMode = tmpstr[0];	
	
	m_record.splitEnumValue((const char*)m_record.m_roleCodes, 
													m_record.m_roleCode,
	                        m_record.m_roleCodeVector, 
	                        m_record.m_roleCodeMap);
	
	m_record.m_next = 0;
	m_record.m_eventFeePolicy = 0;
	m_record.m_priceParamImpl = 0;
			
	return true;
}


PO_FeePolicyComp::PO_FeePolicyComp(const PO_FeePolicyComp& right)
{
	*this = right;
}


void PO_FeePolicyComp::operator = (const PO_FeePolicyComp& right)
{
	m_record = right.m_record;
}


int operator == (const PO_FeePolicyComp &left,const PO_FeePolicyComp& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_FeePolicyComp &left,const PO_FeePolicyComp& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_FeePolicyComp& po)
{	
	return os;
}


PO_Property PO_FeePolicyComp::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_FeePolicyComp","","","",
    "SELECT FEEPOLICY_COMP_ID,FEEPOLICY_ID,EFFECT_ROLE_CODE,EVENT_TYPE_ID,"
    "EVENT_FEEPOLICY_ID,EVENT_PRIORITY,CDR_COMP_FLAG,EXEC_MODE "
    "FROM TD_B_FEEPOLICY_COMP",8,"SELECT count(*) FROM TD_B_FEEPOLICY_COMP"};

