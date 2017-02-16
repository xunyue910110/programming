#include "PO_AddupCycleRule.h"


bool PO_AddupCycleRule::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_AddupCycleRule::convert(const StringVector &argname)
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
	if(strlen(tmpstr.c_str())>0)
	{
	  m_record.m_cycleType = tmpstr[0];
	}else
	{
	  m_record.m_cycleType = '8';
	}
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_cycleNum = atoi(tmpstr.c_str());

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	if(strlen(tmpstr.c_str())>0)
	{
	  m_record.m_defineTag = tmpstr[0];
  }else
	{
	  m_record.m_defineTag = '0';
	}	
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_offsetValue = tmpstr;
		
	return true;
}


PO_AddupCycleRule::PO_AddupCycleRule(const PO_AddupCycleRule& right)
{
	*this = right;
}


void PO_AddupCycleRule::operator = (const PO_AddupCycleRule& right)
{
	m_record = right.m_record;
}


int operator == (const PO_AddupCycleRule &left,const PO_AddupCycleRule& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_AddupCycleRule &left,const PO_AddupCycleRule& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_AddupCycleRule& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_AddupCycleRule::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_AddupCycleRule","","","",
    "SELECT CYCLE_RULE_ID,CYCLE_TYPE_CODE,CYCLE_NUM,DEFINE_TAG,"
    "OFFSET_VALUE FROM TD_B_ADDUP_CYCLERULE",5,
    "SELECT count(*) FROM TD_B_ADDUP_CYCLERULE"};

