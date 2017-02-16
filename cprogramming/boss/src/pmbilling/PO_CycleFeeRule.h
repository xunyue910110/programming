#ifndef PO_CycleFeeRule_H
#define PO_CycleFeeRule_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h" 

class PO_CycleFeeRule
{
  	public:
  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_CycleFeeRule(){}
  		
  		PO_CycleFeeRule(const PO_CycleFeeRule& right);
  		
  		void operator=(const PO_CycleFeeRule& right);
  		
  		friend int operator==(const PO_CycleFeeRule &left,
  		                      const PO_CycleFeeRule& right);
  		
  		friend int operator<(const PO_CycleFeeRule &left,
  		                     const PO_CycleFeeRule& right);
  		
  		friend ostream& operator<<(ostream& os,const PO_CycleFeeRule& po);
  		
  		struct 
  		{
			int        m_itemCode;           //周期费用帐目项
			T_BIZ_TYPE m_eventTypeId;        //事件类型标识
			int        m_orderNo;            //顺序号
			int        m_sumMode;          //取值覆盖方式（0累加、1简单覆盖、择优覆盖：2按最大值、3按最小值）
  		char       m_cycleTypeId;      //子账期类型标识 add by lipf
  		} m_record;
  	
  	private:
  	  	static PO_Property m_property;
};

#endif

