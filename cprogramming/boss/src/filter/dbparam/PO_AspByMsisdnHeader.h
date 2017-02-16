#ifndef PO_ASPBYMSISDNHEADER_H
#define PO_ASPBYMSISDNHEADER_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_AspByMsisdnHeader
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_AspByMsisdnHeader(){}

    PO_AspByMsisdnHeader(const PO_AspByMsisdnHeader& right);

    void operator=(const PO_AspByMsisdnHeader& right);

    friend int operator==(const PO_AspByMsisdnHeader&left, const PO_AspByMsisdnHeader& right);

    friend int operator<(const PO_AspByMsisdnHeader &left, const PO_AspByMsisdnHeader& right);

	friend ostream& operator<<(ostream& os,const PO_AspByMsisdnHeader& po);

    struct
    {
    	T_MSISDN     m_msisdnSeg;
    	T_ASP        m_asp;
    	T_DATE_TIME  m_beginTime;
    	T_DATE_TIME  m_endTime;
    	T_TD_TYPE    m_tdType;
    }recordStruct;

  private:
	static PO_Property m_property;

};



#endif /* PO_GPRSAPN_H */
