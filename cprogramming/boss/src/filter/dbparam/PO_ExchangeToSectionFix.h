#ifndef PO_EXCHANGETOSECTIONFIX_H_20090814
#define PO_EXCHANGETOSECTIONFIX_H_20090814

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_ExchangeToSectionFix
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_ExchangeToSectionFix(){}

    PO_ExchangeToSectionFix(const PO_ExchangeToSectionFix& right);

    void operator=(const PO_ExchangeToSectionFix& right);

    friend int operator==(const PO_ExchangeToSectionFix&left,
    		const PO_ExchangeToSectionFix& right);

    friend int operator<(const PO_ExchangeToSectionFix &left,
    		const PO_ExchangeToSectionFix& right);

	friend ostream& operator<<(ostream& os,const PO_ExchangeToSectionFix& po);

    struct
    {
    	T_ATTRI_VAL   m_exchangeCode; //局向
    	T_ATTRI_VAL   m_sectionCode;  //营业区
    }recordStruct;

  private:
	static PO_Property m_property;

};



#endif /* PO_EXCHANGETOSECTIONFIX_H_20090814 */
