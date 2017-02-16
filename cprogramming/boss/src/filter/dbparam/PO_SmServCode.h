#ifndef PO_SMSERVCODE_H
#define PO_SMSERVCODE_H

#include "dbparam/PO_Common.h"
#include "CdrProperty.h"
class PO_SmServCode
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_SmServCode(){}

    PO_SmServCode(const PO_SmServCode& right);

    void operator=(const PO_SmServCode& right);

    friend int operator==(const PO_SmServCode &left,
    		const PO_SmServCode& right);

    friend int operator<(const PO_SmServCode &left,
    		const PO_SmServCode& right);

	friend ostream& operator<<(ostream& os,const PO_SmServCode& po);

    SmProperty  recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_SMSERVCODE_H */
