#ifndef PO_FeepolicyMutexRela_H
#define PO_FeepolicyMutexRela_H


#include "dbparam/PO_Common.h"
#include "FieldsXX.h" 
#include "AccountCommon.h"


class PO_FeepolicyMutexRela
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_FeepolicyMutexRela(){}
  		
  		PO_FeepolicyMutexRela(const PO_FeepolicyMutexRela& right);
  		
  		void operator = (const PO_FeepolicyMutexRela& right);
  		
  		friend int operator == (const PO_FeepolicyMutexRela &left,
  		                        const PO_FeepolicyMutexRela& right);
  		
  		friend int operator < (const PO_FeepolicyMutexRela &left,
  		                       const PO_FeepolicyMutexRela& right);
  		
  		friend ostream& operator << (ostream& os,const PO_FeepolicyMutexRela& po);
  		
  		MutexTp m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

