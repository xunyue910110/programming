#ifndef PO_SpTariff_H
#define PO_SpTariff_H


#include "dbparam/PO_Common.h"
#include "AccountCommon.h"
#include "NodeBase.h"

class PO_SpTariff
{
  	public:
  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_SpTariff(){}
  		
  		PO_SpTariff(const PO_SpTariff& right);
  		
  		void operator=(const PO_SpTariff& right);
  		
  		friend int operator==(const PO_SpTariff &left,
  		                      const PO_SpTariff& right);
  		
  		friend int operator<(const PO_SpTariff &left,
  		                     const PO_SpTariff& right);
  		
  		friend ostream& operator<<(ostream& os,const PO_SpTariff& po);
  		
  		struct
  		{
  		    char        m_dataTypeCode;
  		    String<18>  m_spCode;
  		    String<18>  m_spBizCode;  
  		    String<4>   m_bizTypeCode;		    
  		    char        m_recvObjectType;
  		    int         m_itemCode;
  		    char        m_billingCycle;
  		    int		      m_relativeCycle;
  		    char        m_price[MIN_STR_LENGTH+1];
  		    char        m_newUserRecvType;
  		    int         m_judgeHalfTime;
  		    char        m_halfPrice[MIN_STR_LENGTH+1];
  		    char        m_displayType;
  		    int         m_validStartCycle;
  		    int         m_validEndCycle;  		    
  		    String<14>  m_startDate;
  		    String<14>  m_endDate;
  		    char        m_judgeSilence;
  		    char        m_judgeAverage;  
  		    char        m_feeType;
  	    } m_record;
  	
  	private:
  	  	static PO_Property m_property;
};
#endif
