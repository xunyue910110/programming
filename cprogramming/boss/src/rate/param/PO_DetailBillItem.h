#ifndef PO_DETAILBILLITEM_H_INCLUDE_20081010_
#define PO_DETAILBILLITEM_H_INCLUDE_20081010_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

const int DTLBILL_FACTORS_COUNT = 20;

class CondBase;
class DetailBillItem 
{   
  public:
	T_BILL_ID         m_billId;    //明细账单标识
	int               m_billType;  //1 计费   2 帐务
	T_PRI             m_priority;  //优先级
	T_FEE_FIELD       m_feeField;  //对应事件元素标识(1-5 费用项、6时长 7 流量)

	T_BIZ_TYPE        m_bizType;   //事件类型
	T_ATTRIBUTE_VALUE m_factors[DTLBILL_FACTORS_COUNT+1];   
	T_RULE_ID         m_ruleId;    //扩展因子
	CondBase         *m_condRule; //条件指针
	T_ATTRIBUTE_VALUE m_remark;
	T_DATE            m_beginCycle;
	T_DATE            m_endCycle;
	//Added By Sunam 2009/8/14 15:02:23 对于账务账单合账增加按feePolicyId以及忙闲时
	T_TP_ID           m_feePolicyId;
	T_TIME            m_beginTime;
	T_TIME            m_endTime;
	
	DetailBillItem & operator = (const DetailBillItem &r) 
	{
		for (int i=0;i < DTLBILL_FACTORS_COUNT; ++i) 
		{
			m_factors[i] = r.m_factors[i];
		}
		m_billId        =  r.m_billId;  
		m_billType      =  r.m_billType;
		m_priority      =  r.m_priority;                            
		m_feeField      =  r.m_feeField;                           
		m_remark        =  r.m_remark;
		m_bizType       =  r.m_bizType;
		m_ruleId        =  r.m_ruleId;
		m_beginCycle    =  r.m_beginCycle;
		m_endCycle      =  r.m_endCycle;
		m_condRule      =  r.m_condRule;
		//Added By Sunam 2009/8/14 15:02:23 对于账务账单合账增加按feePolicyId以及忙闲时
		m_feePolicyId   =  r.m_feePolicyId;
		m_beginTime     =  r.m_beginTime;
		m_endTime       =  r.m_endTime; 
		return *this;
	}
	friend bool operator<(const DetailBillItem &r_left,const DetailBillItem &r_right)
	{
	  if(r_left.m_billType<r_right.m_billType) return true;
	  if(r_left.m_billType>r_right.m_billType) return false;

	  if(r_left.m_bizType<r_right.m_bizType) return true;
	  if(r_left.m_bizType>r_right.m_bizType) return false;
	    
	  if(r_left.m_feeField<r_right.m_feeField) return true;
	  if(r_left.m_feeField>r_right.m_feeField) return false;

    return false;
	}
	friend bool operator==(const DetailBillItem &r_left,const DetailBillItem &r_right)
	{
	  return (r_left.m_billType  == r_right.m_billType &&
	          r_left.m_bizType   == r_right.m_bizType &&
	          r_left.m_feeField  == r_right.m_feeField );
  }
  friend ostream& operator<<(ostream& r_os,const DetailBillItem& r_po)
  {
  	r_os<<r_po.m_billId<<" , "<<
  	  r_po.m_billType<<" , "<<
  		r_po.m_priority<<" , "<<
  		r_po.m_feeField<<" , "<<
  		r_po.m_bizType<<" , "<<
  		r_po.m_beginCycle <<" , "<<
  		r_po.m_endCycle <<" , "<<
  		r_po.m_ruleId <<" , "<<
  		r_po.m_feePolicyId <<" , "<<
  		r_po.m_beginTime <<" , "<<
  		r_po.m_endTime;
      
    for (int i=0;i < DTLBILL_FACTORS_COUNT; ++i) 
    {
		  r_os<<" , "<<r_po.m_factors[i];
    }
    return r_os;
  }
};



class PO_DetailBillItem
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_DetailBillItem(){}

    PO_DetailBillItem(const PO_DetailBillItem& right);

    void operator=(const PO_DetailBillItem& right);

    friend int operator==(const PO_DetailBillItem &left,
    		                  const PO_DetailBillItem &right);

    friend int operator<(const PO_DetailBillItem &left,
    		                 const PO_DetailBillItem& right);

	  friend ostream& operator<<(ostream& os,const PO_DetailBillItem& po);

    DetailBillItem m_record;

  private:
	  static PO_Property m_property;
};
#endif //PO_DETAILBILLITEM_H_INCLUDE_20081010_

