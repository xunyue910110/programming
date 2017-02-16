#include "PO_SpDiscount.h"


bool PO_SpDiscount::getProperty(PO_Property &property)
{
	property = m_property;	
	return true;
}
    

bool PO_SpDiscount::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_discntId = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_orderNo = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_priority = atoi(tmpstr.c_str());

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_dataTypeCode = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_spCode = tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_spBizCode = tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_validType = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_itemCode = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_discountType = tmpstr[0];

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_discountValueType = tmpstr[0];

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_discountMode = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_minValue = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_maxValue = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_referValue = atoi(tmpstr.c_str());
		
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	strncpy(m_record.m_discountValue,tmpstr.c_str(),MIN_STR_LENGTH);
	m_record.m_discountValue[MIN_STR_LENGTH] = '\0';
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_addupMethod = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_addupType = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_addupValue = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_validStartCycle = atoi(tmpstr.c_str());

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_validEndCycle = atoi(tmpstr.c_str());

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_startDate = tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_endDate = tmpstr;
			
	return true;
}


PO_SpDiscount::PO_SpDiscount(const PO_SpDiscount& right)
{
	*this = right;
}


void PO_SpDiscount::operator=(const PO_SpDiscount& right)
{
	m_record = right.m_record;
}


int operator==(const PO_SpDiscount &left,const PO_SpDiscount& right)
{
    return (left.m_record.m_discntId == right.m_record.m_discntId);
}


int operator<(const PO_SpDiscount &left,const PO_SpDiscount& right)
{    
    
    return (left.m_record.m_discntId < right.m_record.m_discntId);    
}


ostream& operator<<(ostream& os,const PO_SpDiscount& po)
{
	os<<po.m_record.m_discntId   	 <<" , "<<
	    po.m_record.m_orderNo        <<" , "<<		
	    po.m_record.m_priority       <<" , "<<		
	    po.m_record.m_dataTypeCode   <<" , "<<
	    po.m_record.m_spCode         <<" , "<<
	    po.m_record.m_spBizCode      <<" , "<<
		po.m_record.m_validType      <<" , "<<		
		po.m_record.m_itemCode       <<" , "<<
		po.m_record.m_discountType   <<" , "<<
		po.m_record.m_discountValueType  <<" , "<<	
		po.m_record.m_discountMode   <<" , "<<	
		po.m_record.m_minValue       <<" , "<<
		po.m_record.m_maxValue       <<" , "<<
		po.m_record.m_referValue     <<" , "<<		
		po.m_record.m_discountValue  <<" , "<<		
		po.m_record.m_validStartCycle<<" , "<<
		po.m_record.m_validEndCycle  <<" , "<<		 
		po.m_record.m_startDate    	 <<" , "<<
		po.m_record.m_endDate; 

	return os;
}


PO_Property PO_SpDiscount::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_SpDiscount","","","",
    "SELECT FEEPOLICY_ID,ORDER_NO,PRIORITY,DATA_TYPE_CODE,\
    SP_CODE,SP_BIZ_CODE,VALID_TYPE,ITEM_CODE,\
    DISCOUNT_TYPE,DISCOUNT_VALUE_TYPE,DISCOUNT_MODE,MIN_VALUE,MAX_VALUE,\
    REFER_VALUE,DISCOUNT_VALUE,ADDUP_METHOD,ADDUP_TYPE,ADDUP_VALUE,\
    VALID_START_MONTH,VALID_END_MONTH,\
    to_char(START_DATE,'yyyymmddhh24miss'),\
    to_char(END_DATE,'yyyymmddhh24miss') FROM TD_A_SP_DISCNT",
    22,"SELECT count(*) FROM TD_A_SP_DISCNT"};
