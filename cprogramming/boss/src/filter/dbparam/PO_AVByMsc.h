#ifndef PO_AVBYMSC_H
#define PO_AVBYMSC_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_AVByMsc
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_AVByMsc(){}

    PO_AVByMsc(const PO_AVByMsc& right);

    void operator=(const PO_AVByMsc& right);

    friend int operator==(const PO_AVByMsc &left,
    		const PO_AVByMsc& right);

    friend int operator<(const PO_AVByMsc &left,
    		const PO_AVByMsc& right);

	friend ostream& operator<<(ostream& os,const PO_AVByMsc& po);

	struct
	{
		T_MSC       m_msc;
		int         m_mscType;
		T_DATE_TIME m_beginTime;
		T_DATE_TIME m_endTime;
		T_AREA_CODE m_visitAreaCode;
	}recordStruct;

  private:
	static PO_Property m_property;
};



#endif /* PO_AVBYMSC_H */
