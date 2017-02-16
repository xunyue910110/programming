#include "PO_EffectFeeCount.h"


bool PO_EffectFeeCount::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_EffectFeeCount::convert(const StringVector &argname)
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
	m_record.m_object = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	if(strlen(tmpstr.c_str())>0)
		m_record.m_computeMethod = atoi(tmpstr.c_str());
	else
	  m_record.m_computeMethod = 0; //默认为普通费率
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_baseUnit = atol(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	if(strlen(tmpstr.c_str())>0)
	  m_record.m_timesIntTag = tmpstr[0];	
	else
	  m_record.m_timesIntTag = '0'; //默认为进一
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	//strncpy(m_record.m_unitRatio,tmpstr.c_str(),MIN_STR_LENGTH);
	//m_record.m_unitRatio[MIN_STR_LENGTH] = '\0';
	strncpy(m_record.m_unitRatio,tmpstr.c_str(),100);
  m_record.m_unitRatio[100] = '\0';
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_addupRule = (EffectAddup *)atoi(tmpstr.c_str());
	
	//add for GPRS 0.X厘费率 2009,0904
    tmpstr = *(argname.begin()+i++);
    tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
    tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
    m_record.m_pricision = atoi(tmpstr.c_str());
		
	return true;
}


PO_EffectFeeCount::PO_EffectFeeCount(const PO_EffectFeeCount& right)
{
	*this = right;
}


void PO_EffectFeeCount::operator = (const PO_EffectFeeCount& right)
{
	m_record = right.m_record;
}


int operator == (const PO_EffectFeeCount &left,const PO_EffectFeeCount& right)
{
    return (left.m_record == right.m_record);
}


int operator < (const PO_EffectFeeCount &left,const PO_EffectFeeCount& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_EffectFeeCount& po)
{
	//os<< po.m_record<<endl; 

	return os;
}

///modify for GPRS 0.X厘费率 2009,0904
/*
PO_Property PO_EffectFeeCount::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_EffectFeeCount","","","",
    "SELECT FEECOUNT_ID,OBJECT_ID,COMPUTE_METHOD,"
    "BASE_UNIT,TIMES_INTTAG,UNIT_RATIO,ADDUP_ID FROM TD_B_FEECOUNT ",7,
    "SELECT count(*) FROM TD_B_FEECOUNT "};
*/

PO_Property PO_EffectFeeCount::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_EffectFeeCount","","","",
    "SELECT FEECOUNT_ID,OBJECT_ID,COMPUTE_METHOD,"
    "BASE_UNIT,TIMES_INTTAG,UNIT_RATIO,ADDUP_ID,PRECISION FROM TD_B_FEECOUNT ",8,
    "SELECT count(*) FROM TD_B_FEECOUNT "};
