#include "PO_EffectDiscnt.h"


bool PO_EffectDiscnt::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}


bool PO_EffectDiscnt::convert(const StringVector &argname)
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
	m_record.m_orderNo = atoi(tmpstr.c_str());

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_computeObj = (ObjectBase *) atoi(tmpstr.c_str());	
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_computeMethod = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	strncpy(m_record.m_childValue,tmpstr.c_str(),MIN_STR_LENGTH);
	m_record.m_childValue[MIN_STR_LENGTH] = '\0';
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	strncpy(m_record.m_parentValue,tmpstr.c_str(),MIN_STR_LENGTH);
	m_record.m_parentValue[MIN_STR_LENGTH] = '\0';
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	strncpy(m_record.m_discntFee,tmpstr.c_str(),MIN_STR_LENGTH);
	m_record.m_discntFee[MIN_STR_LENGTH] = '\0';
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	strncpy(m_record.m_baseFee,tmpstr.c_str(),MIN_STR_LENGTH);
	m_record.m_baseFee[MIN_STR_LENGTH] = '\0';
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_baseObj = (ObjectBase *) atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_baseAdjustMethod = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_adjustMethod = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_effectObj = (ObjectBill *) atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_dispatchMethod = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_remainDealMethod = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_addupRule = (EffectAddup *) atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_displayMethod = tmpstr[0];
	
	m_record.m_next = 0;
			
	return true;
}


PO_EffectDiscnt::PO_EffectDiscnt(const PO_EffectDiscnt& right)
{
	*this = right;
}


void PO_EffectDiscnt::operator = (const PO_EffectDiscnt& right)
{
	m_record = right.m_record;
}


bool operator == (const PO_EffectDiscnt &left,const PO_EffectDiscnt& right)
{
    return (left.m_record == right.m_record);
}


bool operator < (const PO_EffectDiscnt &left,const PO_EffectDiscnt& right)
{
    return (left.m_record < right.m_record);
}


ostream& operator << (ostream& os,const PO_EffectDiscnt& po)
{
	//os<< po.m_record<<endl; 

	return os;
}


PO_Property PO_EffectDiscnt::m_property = {FROM_DATABASE,BY_FLAG,
"","PO_EffectDiscnt","","","",
"SELECT FEEDISCNT_ID,ORDER_NO,COMPUTE_OBJECT_ID,COMPUTE_METHOD,\
DIVIED_CHILD_VALUE,DIVIED_PARENT_VALUE,DISCNT_FEE,BASE_FEE,\
BASE_OBJECT_ID,BASE_ADJUST_METHOD,FEE_ADJUST_METHOD,EFFECT_OBJECT_ID,\
DISPATCH_METHOD,REMAIN_DEAL_METHOD,ADDUP_ID,DISPLAY_METHOD \
FROM TD_B_FEEDISCNT",16,"SELECT count(*) FROM TD_B_FEEDISCNT"};

