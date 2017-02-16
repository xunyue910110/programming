#ifndef PO_NRCHANNELBYPROVCODE_H
#define PO_NRCHANNELBYPROVCODE_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_NrChannelByProvCode
{
  public:
    bool getProperty(PO_Property &property);
    
    bool convert(const StringVector &argname);
    
    PO_NrChannelByProvCode(){}
    
    PO_NrChannelByProvCode(const PO_NrChannelByProvCode& right);
    
    void operator=(const PO_NrChannelByProvCode& right);
    
    friend int operator==(const PO_NrChannelByProvCode &left,
    		const PO_NrChannelByProvCode& right);
    		
    friend int operator<(const PO_NrChannelByProvCode &left,
    		const PO_NrChannelByProvCode& right);
    		
	friend ostream& operator<<(ostream& os,const PO_NrChannelByProvCode& po);

    struct
    {
    	T_PROVINCE_CODE m_provCode;
	    int             m_channelNo;
    }recordStruct;

  private:
		static PO_Property m_property;
};

#endif /* PO_NRCHANNELBYPROVCODE_H */
