#ifndef PO_MSCBYALIAS_H
#define PO_MSCBYALIAS_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_MscByAlias
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_MscByAlias(){}

    PO_MscByAlias(const PO_MscByAlias& right);

    void operator=(const PO_MscByAlias& right);

    friend int operator==(const PO_MscByAlias &left,
    		const PO_MscByAlias& right);

    friend int operator<(const PO_MscByAlias &left,
    		const PO_MscByAlias& right);

	friend ostream& operator<<(ostream& os,const PO_MscByAlias& po);

	struct
	{
		T_MSC msc;
		T_DATE_TIME beginTime;
		T_DATE_TIME endTime;
		T_MSC msc_alias;
	}recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_AVBYMSC_H */
