#ifndef PO_PARTYPRICE_H
#define PO_PARTYPRICE_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"
#include "policy/PartyPrice.h"

class PO_PartyPrice
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_PartyPrice(){}

    PO_PartyPrice(const PO_PartyPrice& right);

    void operator=(const PO_PartyPrice& right);

    friend int operator==(const PO_PartyPrice &left,
    		const PO_PartyPrice& right);

    friend int operator<(const PO_PartyPrice &left,
    		const PO_PartyPrice& right);

	friend ostream& operator<<(ostream& os,const PO_PartyPrice& po);

    PartyPrice recordStruct;

  private:
	static PO_Property m_property;
};

#endif /* PO_PARTYPRICE_H */
