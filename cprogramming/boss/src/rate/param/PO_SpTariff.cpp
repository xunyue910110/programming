#include "PO_SpTariff.h"


bool PO_SpTariff::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}
    

bool PO_SpTariff::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;

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
	m_record.m_bizTypeCode = tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_recvObjectType = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_itemCode = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_billingCycle = tmpstr[0];

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_relativeCycle = atoi(tmpstr.c_str());
		
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	strncpy(m_record.m_price,tmpstr.c_str(),MIN_STR_LENGTH);
	m_record.m_price[MIN_STR_LENGTH] = '\0';
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_newUserRecvType = tmpstr[0];

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_judgeHalfTime= atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	strncpy(m_record.m_halfPrice,tmpstr.c_str(),MIN_STR_LENGTH);
	m_record.m_halfPrice[MIN_STR_LENGTH] = '\0';
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_displayType = tmpstr[0];
	
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
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_judgeSilence = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_judgeAverage = tmpstr[0];
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
  if(tmpstr.length()<1)
	   	m_record.m_feeType=INIT_FEETYPE;
	else
	   m_record.m_feeType = tmpstr[0];
			
	return true;
}


PO_SpTariff::PO_SpTariff(const PO_SpTariff& right)
{
	*this = right;
}


void PO_SpTariff::operator=(const PO_SpTariff& right)
{
	m_record = right.m_record;
}


int operator==(const PO_SpTariff &left,const PO_SpTariff& right)
{
    return ((left.m_record.m_dataTypeCode == right.m_record.m_dataTypeCode)&&
            (left.m_record.m_spCode == right.m_record.m_spCode)&&
            (left.m_record.m_spBizCode == right.m_record.m_spBizCode));
}


int operator<(const PO_SpTariff &left,const PO_SpTariff& right)
{    
    if (left.m_record.m_dataTypeCode == right.m_record.m_dataTypeCode)
    {
    	if(left.m_record.m_spCode == right.m_record.m_spCode)
    	{
    		return (left.m_record.m_spBizCode < right.m_record.m_spBizCode);
    	}
    	else
    	{
        	return (left.m_record.m_spCode < right.m_record.m_spCode);
        }
    }
    else
    {    
        return (left.m_record.m_dataTypeCode < right.m_record.m_dataTypeCode);
    }
}


ostream& operator<<(ostream& os,const PO_SpTariff& po)
{
	os<<po.m_record.m_dataTypeCode     <<" , "<<
	    po.m_record.m_spCode           <<" , "<<
		  po.m_record.m_spBizCode          <<" , "<<
		  po.m_record.m_bizTypeCode      <<" , "<<
		  po.m_record.m_recvObjectType   <<" , "<<
		  po.m_record.m_itemCode         <<" , "<<
		  po.m_record.m_billingCycle     <<" , "<<
		  po.m_record.m_relativeCycle    <<" , "<<
		  po.m_record.m_price            <<" , "<<
		  po.m_record.m_newUserRecvType  <<" , "<<
		  po.m_record.m_judgeHalfTime    <<" , "<<
		  po.m_record.m_halfPrice        <<" , "<<
		  po.m_record.m_displayType      <<" , "<<
		  po.m_record.m_validStartCycle  <<" , "<<
		  po.m_record.m_validEndCycle    <<" , "<<		 
		  po.m_record.m_startDate    	   <<" , "<<
		  po.m_record.m_endDate          <<" , "<<
		  po.m_record.m_judgeSilence     <<" , "<<
		  po.m_record.m_judgeAverage     <<" , "<<
      po.m_record.m_feeType; 
	return os;
}


PO_Property PO_SpTariff::m_property={FROM_DATABASE,BY_FLAG,
    "","PO_SpTariff","","","",
    "SELECT DATA_TYPE_CODE,SP_CODE,    \
    SP_BIZ_CODE,BIZ_TYPE_CODE,RECV_OBJECT_TYPE,ITEM_CODE,	\
    BILLING_CYCLE,RELATIVE_CYCLE,PRICE,	\
    NEWUSER_RECV_TYPE,JUDGE_HALF_TIME,	\
    HALF_PRICE,DISPLAY_TYPE,	\
    VALID_START_CYCLE,VALID_END_CYCLE,\
    to_char(START_DATE,'yyyymmddhh24miss'),                \
    to_char(END_DATE,'yyyymmddhh24miss'),  \
    SILENCE_DEAL,AVERAGE_DEAL,FEETYPE FROM TD_A_SP_TARIFF",
    20,"SELECT count(*) FROM TD_A_SP_TARIFF"};

