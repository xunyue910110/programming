#ifndef PO_CYCLEEPARCHY_H_INCLUDE_20090814_
#define PO_CYCLEEPARCHY_H_INCLUDE_20090814_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h" 
#include "BillingCommon.h" 


class PO_CycleEparchy
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_CycleEparchy(){}

    PO_CycleEparchy(const PO_CycleEparchy &right);

    void operator=(const PO_CycleEparchy &right);

    friend int operator==(const PO_CycleEparchy &left,const PO_CycleEparchy &right);

    friend int operator<(const PO_CycleEparchy &left,const PO_CycleEparchy &right);

	  friend ostream& operator<<(ostream& os,const PO_CycleEparchy& po);

    CycParam  m_record;

  private:
	  static PO_Property m_property;
}; 
#endif //PO_CYCLEEPARCHY_H_INCLUDE_20090814_
