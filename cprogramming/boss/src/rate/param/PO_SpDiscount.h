#ifndef PO_SpDiscount_H
#define PO_SpDiscount_H


#include "dbparam/PO_Common.h"
#include "AccountCommon.h"
#include "NodeBase.h"

class PO_SpDiscount
{
  	public:
  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_SpDiscount(){}
  		
  		PO_SpDiscount(const PO_SpDiscount& right);
  		
  		void operator=(const PO_SpDiscount& right);
  		
  		friend int operator==(const PO_SpDiscount &left,
  		                      const PO_SpDiscount& right);
  		
  		friend int operator<(const PO_SpDiscount &left,
  		                     const PO_SpDiscount& right);
  		
  		friend ostream& operator<<(ostream& os,const PO_SpDiscount& po);
  		
  		struct
  		{  		    
  		    char		m_dataTypeCode;
  		    char		m_validType;
  		    char		m_discountType;
  		    char		m_discountValueType;  		    
					char		m_discountMode;	//优惠金额作用方式(当m_discountType='5' 关联优惠 时有效)  0 按金额几何平均 1 按账目优先级
  		    int			m_itemCode;
  		    int			m_discntId;
  		    int			m_orderNo;
  		    int			m_priority;  		    
  		    int			m_minValue;   //额定用户数最小值
  		    int			m_maxValue;   //额定用户数最大值
  		    int			m_referValue; //超出额定数量部分的每个用户的费用。  		    
  		    char    m_discountValue[MIN_STR_LENGTH+1];  		    
  		    int			m_validStartCycle;
  		    int			m_validEndCycle;
  		    
  		    char    m_addupMethod;//累计方式（根据业务规则添加，0：普通方式）
  		    char    m_addupType;//累计类型（0：天、1：月、2：年）
          int     m_addupValue;//累计参考值
           
					string	m_spCode;
					string	m_spBizCode;
  		    String<14>	m_startDate;
  		    String<14>	m_endDate;
  		    
  	    } m_record;
  	
  	private:
  	  	static PO_Property m_property;
};


#endif
