#ifndef PO_DetailItem_H
#define PO_DetailItem_H


#include "dbparam/PO_Common.h"

class DetailItemInfo
{
	public:		  	
	 DetailItemInfo & operator =(const DetailItemInfo &r_value) 
	 {
      this->m_itemId         =   	 r_value.m_itemId;         
			this->m_itemUseType    =     r_value.m_itemUseType;     
			this->m_addUpElemType  =     r_value.m_addUpElemType;   
			this->m_itemClass      =     r_value.m_itemClass;       
			this->m_oweTag         =     r_value.m_oweTag;          
			this->m_lateFeeCalcTag =     r_value.m_lateFeeCalcTag;  
	    this->m_carryMode 		 =     r_value.m_carryMode; 
	    return *this;
	  }	 
	    
	public:
   int  m_itemId;            //明细帐目标识	      
   char m_itemUseType;       //明细帐目用途  
   char m_addUpElemType;     //累计元素类型  
   char m_itemClass;         //帐目分类      
   char m_oweTag;            //是否算欠费    
   char m_lateFeeCalcTag;	   //是否计算滞纳金
   char m_carryMode;				 //'0':四舍五入，'1'：去尾数法，'2 ：直接进位

};


class PO_DetailItem
{
      public:

          bool getProperty(PO_Property &property);
          
          bool convert(const StringVector &argname);
          
          PO_DetailItem(){}
          
          PO_DetailItem(const PO_DetailItem& right);
          
          void operator = (const PO_DetailItem& right);
          
          friend int operator == (const PO_DetailItem &left,
                                  const PO_DetailItem& right);
          
          friend int operator < (const PO_DetailItem &left,
                                 const PO_DetailItem& right);
          
          friend ostream& operator << (ostream& os,const PO_DetailItem& po);
          
          DetailItemInfo m_record;
      
      private:

          static PO_Property m_property;
};


#endif

