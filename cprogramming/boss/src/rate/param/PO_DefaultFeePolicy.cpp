#include "PO_DefaultFeePolicy.h"


bool PO_DefaultFeePolicy::getProperty(PO_Property &property)
{
	property = m_property;
	return true;
}

bool PO_DefaultFeePolicy::convert(const StringVector &argname)
{
	int i = 0;
	string tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_eventTypeId = tmpstr;
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_eventPolicyId = atoi(tmpstr.c_str());
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_brandCode = tmpstr;

	//add by cuiyw 2009-12-05 10:54:00 for SHANXI
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_eparchyCode = tmpstr;
	    
    tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_cityCode = tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_userNetTypeCode = tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_org = tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_areType = tmpstr;

	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_netLowValue = tmpstr;

    tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_netHighValue = tmpstr;
    
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_hireType = tmpstr;

    tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_ruleId = tmpstr;
	m_record.m_condRule=NULL;
 //add by cuiyw end for shanxi
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_startDate = atoi(tmpstr.c_str());	
	
	tmpstr = *(argname.begin()+i++);
	tmpstr.erase(0, tmpstr.find_first_not_of(" \t\n\r"));
	tmpstr.erase(tmpstr.find_last_not_of(" \t\n\r")+1, tmpstr.npos);
	m_record.m_endDate = atoi(tmpstr.c_str());

			
	return true;
}


PO_DefaultFeePolicy::PO_DefaultFeePolicy(const PO_DefaultFeePolicy& right)
{
	*this = right;
}


void PO_DefaultFeePolicy::operator = (const PO_DefaultFeePolicy& right)
{
	m_record = right.m_record;
}


int operator == (const PO_DefaultFeePolicy &left,const PO_DefaultFeePolicy& right)
{
    return (/*left.m_record.m_eventTypeId == right.m_record.m_eventTypeId &&*/
            (left.m_record.m_brandCode == "*"||right.m_record.m_brandCode == "*"||left.m_record.m_brandCode == right.m_record.m_brandCode) &&
            (left.m_record.m_eparchyCode == "*"||right.m_record.m_eparchyCode == "*"||left.m_record.m_eparchyCode == right.m_record.m_eparchyCode)&&
            (left.m_record.m_cityCode == "*"||right.m_record.m_cityCode== "*"||left.m_record.m_cityCode == right.m_record.m_cityCode)&&
            (left.m_record.m_userNetTypeCode == right.m_record.m_userNetTypeCode||left.m_record.m_userNetTypeCode == "*"||right.m_record.m_userNetTypeCode== "*")&&
            (left.m_record.m_org == "*"||right.m_record.m_org== "*"||left.m_record.m_org == right.m_record.m_org)&&    
            (left.m_record.m_areType == "*"||right.m_record.m_areType== "*"||left.m_record.m_areType == right.m_record.m_areType)&&
            (left.m_record.m_netLowValue == "*"||right.m_record.m_netLowValue== "*"||left.m_record.m_netLowValue.value() < right.m_record.m_netLowValue.value())&&
            (left.m_record.m_netHighValue == "*"||right.m_record.m_netHighValue== "*"||left.m_record.m_netHighValue.value() >= right.m_record.m_netHighValue.value())&&
            (left.m_record.m_hireType == "*"||right.m_record.m_hireType== "*"||left.m_record.m_hireType == right.m_record.m_hireType)
           ); //add by cuiyw for SHANXI  At 2009-12-05 10:59:06
}


int operator < (const PO_DefaultFeePolicy &left,const PO_DefaultFeePolicy& right)
{
      if (left.m_record.m_brandCode != "*"&& right.m_record.m_brandCode != "*")
      {
         if(left.m_record.m_brandCode < right.m_record.m_brandCode)
           return true;       
         if(left.m_record.m_brandCode > right.m_record.m_brandCode)
            return false;           
      }
      
        if (left.m_record.m_eparchyCode != "*"&& right.m_record.m_eparchyCode != "*")
      {
         if(left.m_record.m_eparchyCode < right.m_record.m_eparchyCode)
           return true;       
         if(left.m_record.m_eparchyCode > right.m_record.m_eparchyCode)
            return false;           
      }
      
       if (left.m_record.m_cityCode != "*"&& right.m_record.m_cityCode != "*")
      {
         if(left.m_record.m_cityCode < right.m_record.m_cityCode)
           return true;       
         if(left.m_record.m_cityCode > right.m_record.m_cityCode)
            return false;           
      }

       if (left.m_record.m_userNetTypeCode != "*"&& right.m_record.m_userNetTypeCode != "*")
      {
         if(left.m_record.m_userNetTypeCode < right.m_record.m_userNetTypeCode)
           return true;       
         if(left.m_record.m_userNetTypeCode > right.m_record.m_userNetTypeCode)
            return false;           
      }

       if (left.m_record.m_org != "*"&& right.m_record.m_org != "*")
      {
         if(left.m_record.m_org < right.m_record.m_org)
           return true;       
         if(left.m_record.m_org > right.m_record.m_org)
            return false;           
      }
      
      if (left.m_record.m_areType != "*"&& right.m_record.m_areType != "*")
      {
         if(left.m_record.m_areType < right.m_record.m_areType)
           return true;       
         if(left.m_record.m_areType > right.m_record.m_areType)
            return false;           
      }
      
       if (left.m_record.m_netLowValue != "*"&& right.m_record.m_netLowValue != "*")
      {
         if(left.m_record.m_netLowValue.value() > right.m_record.m_netLowValue.value())
           return true;       
         if(left.m_record.m_netLowValue.value() < right.m_record.m_netLowValue.value())
            return false;           
      }
      
       if (left.m_record.m_netHighValue != "*"&& right.m_record.m_netHighValue != "*")
      {
         if(left.m_record.m_netHighValue.value() > right.m_record.m_netHighValue.value())
           return true;       
         if(left.m_record.m_netHighValue.value() < right.m_record.m_netHighValue.value())
            return false;           
      }
      
       if (left.m_record.m_hireType != "*"&& right.m_record.m_hireType != "*")
      {
         if(left.m_record.m_hireType < right.m_record.m_hireType)
           return true;       
         if(left.m_record.m_hireType > right.m_record.m_hireType)
            return false;           
      }
      
      return false;           
}



ostream& operator << (ostream& os,const PO_DefaultFeePolicy& po)
{	
	os<<po.m_record.m_eventTypeId<<" , "<<
	    po.m_record.m_eventPolicyId<<" , "<<
	    po.m_record.m_brandCode<<" , "<<
	    po.m_record.m_eparchyCode<<" , "<<	    
	    po.m_record.m_cityCode<<" , "<<
        po.m_record.m_userNetTypeCode<<" , "<<
        po.m_record.m_org<<","<<
        po.m_record.m_areType<<" , "<<
        po.m_record.m_netLowValue<<" , "<<
        po.m_record.m_netHighValue<<" , "<<
        po.m_record.m_hireType<<" , "<<
        po.m_record.m_ruleId<<" , "<<	
	    po.m_record.m_startDate<<" , "<<
	    po.m_record.m_endDate;//add by cuiyw for shanxi At 2009-12-05 10:58:20
	    
	return os;
}


PO_Property PO_DefaultFeePolicy::m_property = {FROM_DATABASE,BY_FLAG,
    "","PO_DefaultFeePolicy","","","",
    "SELECT EVENT_TYPE_ID,DEFAULT_FEEPOLICY_ID,BRAND_CODE,EPARCHY_CODE, \
     CITY_CODE,USER_TYPE_CODE,ORG,AREATYPE_CODE,NET_LOW_VALUE,NET_HIGH_VALUE,\
     HIRE_TYPE,RULE_ID,to_char(START_DATE,'YYYYMMDD'),to_char(END_DATE,'YYYYMMDD') \
    FROM TD_B_DEFAULT_FEEPOLICY",14,
    "SELECT count(*) FROM TD_B_DEFAULT_FEEPOLICY"};

