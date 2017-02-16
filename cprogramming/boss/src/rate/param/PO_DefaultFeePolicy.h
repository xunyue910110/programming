#ifndef PO_DefaultFeePolicy_H
#define PO_DefaultFeePolicy_H


#include "dbparam/PO_Common.h"
#include "FieldsXX.h" 

class CondBase;
class PO_DefaultFeePolicy
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_DefaultFeePolicy(){}
  		
  		PO_DefaultFeePolicy(const PO_DefaultFeePolicy& right);
  		
  		void operator = (const PO_DefaultFeePolicy& right);
  		
  		friend int operator == (const PO_DefaultFeePolicy &left,
  		                        const PO_DefaultFeePolicy& right);
  		
  		friend int operator < (const PO_DefaultFeePolicy &left,
  		                       const PO_DefaultFeePolicy& right);
  		
  		friend ostream& operator << (ostream& os,const PO_DefaultFeePolicy& po);
  		
  		struct
  		{
  			T_BIZ_TYPE          m_eventTypeId;
  			T_TP_ID             m_eventPolicyId;
  			T_BRAND_CODE      m_brandCode;
  			T_EPARCHY_CODE    m_eparchyCode;
  			T_EPARCHY_CODE    m_cityCode;
  			T_MATCH_TYPE       m_userNetTypeCode;
  			T_TPPARAM_VALUE    m_org;
  			T_TPPARAM_VALUE    m_areType;
  			T_TPPARAM_VALUE    m_netLowValue;
  			T_TPPARAM_VALUE    m_netHighValue;
  			T_TPPARAM_VALUE    m_hireType;
  			T_RULE_ID          m_ruleId;
  			CondBase      *m_condRule; //条件指针
  			int               m_startDate;
  			int               m_endDate;
  		}m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

