#ifndef PO_VPNRELFEEPOLICY_H
#define PO_VPNRELFEEPOLICY_H

#include "dbparam/PO_Common.h"
#include "CondBase.h"

class PO_VpnRelFeePolicy
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_VpnRelFeePolicy(){}
  		
  		PO_VpnRelFeePolicy(const PO_VpnRelFeePolicy& right);
  		
  		void operator = (const PO_VpnRelFeePolicy& right);
  		
  		friend int operator == (const PO_VpnRelFeePolicy &left,
  		                        const PO_VpnRelFeePolicy& right);
  		
  		friend int operator < (const PO_VpnRelFeePolicy &left,
  		                       const PO_VpnRelFeePolicy& right);
  		
  		friend ostream& operator << (ostream& os,const PO_VpnRelFeePolicy& po);
  		
  		struct
        {
            T_SERVER_ID m_vpnIdA;
            T_SERVER_ID m_vpnIdB;
            T_TP_ID     m_feePolicyId;
            T_DATE_TIME m_beginTime;
            T_DATE_TIME m_endTime;
        }m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

