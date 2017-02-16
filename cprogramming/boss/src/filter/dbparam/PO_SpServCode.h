#ifndef PO_SPSERVCODE_H
#define PO_SPSERVCODE_H

#include "dbparam/PO_Common.h"
#include "CdrProperty.h"
class PO_SpServCode
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_SpServCode(){}

    PO_SpServCode(const PO_SpServCode& right);

    void operator=(const PO_SpServCode& right);

    friend int operator==(const PO_SpServCode &left,
    		const PO_SpServCode& right);

    friend int operator<(const PO_SpServCode &left,
    		const PO_SpServCode& right);

	friend ostream& operator<<(ostream& os,const PO_SpServCode& po);

    SpProperty  recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_SPSERVCODE_H */
