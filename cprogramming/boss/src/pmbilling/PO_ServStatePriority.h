#ifndef PO_ServStatePriority_H
#define PO_ServStatePriority_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h" 

class PO_ServStatePriority
{
  	public:
  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_ServStatePriority(){}
  		
  		PO_ServStatePriority(const PO_ServStatePriority& right);
  		
  		void operator=(const PO_ServStatePriority& right);
  		
  		friend int operator==(const PO_ServStatePriority &left,
  		                      const PO_ServStatePriority& right);
  		
  		friend int operator<(const PO_ServStatePriority &left,
  		                     const PO_ServStatePriority& right);
  		
  		friend ostream& operator<<(ostream& os,const PO_ServStatePriority& po);
  		
  		struct 
  		{
  			int          m_serviceId;      //服务标识
  			char         m_servStateCode;  //服务状态编码
  			int          m_priority;       //服务状态优先级
  			T_DATE_TIME  m_startDate;      //生效时间             
    		T_DATE_TIME  m_endDate;        //失效时间
  		} m_record;
  	
  	private:
  	  	static PO_Property m_property;
};

#endif
