#ifndef PO_DiscntStatecode_H
#define PO_DiscntStatecode_H


#include "dbparam/PO_Common.h"
#include "FieldsXX.h"
#include <vector>
const int STR_ENUMLEN = 20;

class PO_DiscntStatecode
{
  	public:

  		bool getProperty(PO_Property &property);
  		
  		bool convert(const StringVector &argname);
  		
  		PO_DiscntStatecode(){}
  		
  		PO_DiscntStatecode(const PO_DiscntStatecode& right);
  		
  		void operator = (const PO_DiscntStatecode& right);
  		
  		friend int operator == (const PO_DiscntStatecode &left,
  		                        const PO_DiscntStatecode &right);
  		
  		friend int operator < (const PO_DiscntStatecode &left,
  		                       const PO_DiscntStatecode &right);
  		
  		friend ostream& operator << (ostream& os,const PO_DiscntStatecode& po);
  		
	  	struct
	    {
	       int     m_feediscntid; 
	      // char    m_statecode;
	       char    m_stateCodes[STR_ENUMLEN+1];
	       vector <char> m_stateCodeVector;
	       
	       void splitEnumValue(const char* pEnumValue,
	                           vector<char> &vValue)
 				{
					char paramValue[STR_ENUMLEN+1];
					char * pValue;
					char t_value;
					strncpy(paramValue, pEnumValue, STR_ENUMLEN);
					paramValue[STR_ENUMLEN]='\0';
					vValue.clear();
					pValue = strtok(paramValue, ",");	
		 
					while (pValue != NULL)
					{	
						t_value = pValue[0];
						vValue.push_back(t_value);
						pValue = strtok(NULL, ",");				
					}
		 
				}		       
	    }  m_record;
  	
  	private:

  	  	static PO_Property m_property;
};


#endif

