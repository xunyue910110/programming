#ifndef PO_USSDSERVCODE_H
#define PO_USSDSERVCODE_H

#include "dbparam/PO_Common.h"
#include "CdrProperty.h"
class PO_UssdServCode
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_UssdServCode(){}

    PO_UssdServCode(const PO_UssdServCode& right);

    void operator=(const PO_UssdServCode& right);

    friend int operator==(const PO_UssdServCode &left,
    		const PO_UssdServCode& right);

    friend int operator<(const PO_UssdServCode &left,
    		const PO_UssdServCode& right);

	friend ostream& operator<<(ostream& os,const PO_UssdServCode& po);

    SpProperty  recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_USSDSERVCODE_H */
