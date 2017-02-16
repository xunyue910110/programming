#ifndef PO_SysParamInfo_H
#define PO_SysParamInfo_H


#include "dbparam/PO_Common.h"
#include "include/MonthEndCommon.h"


class PO_SysParamInfo
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_SysParamInfo(){}
  		
  		PO_SysParamInfo(const PO_SysParamInfo& right);
  		
  		void operator = (const PO_SysParamInfo& right);
  		
  		friend int operator == (const PO_SysParamInfo &left,
  		                        const PO_SysParamInfo& right);
  		
  		friend int operator < (const PO_SysParamInfo &left,
  		                       const PO_SysParamInfo& right);
  		
  		friend ostream& operator << (ostream& os,const PO_SysParamInfo& po);
  		
  		struct SysParamInfo m_record;
  	
  	private:

  	  	static PO_Property m_property;

};


#endif

