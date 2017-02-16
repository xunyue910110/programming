#ifndef _PO_SPOPERFEE_H_050708_
#define _PO_SPOPERFEE_H_050708_


#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_SpOperFee
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_SpOperFee(){}

    PO_SpOperFee(const PO_SpOperFee& right);

    void operator=(const PO_SpOperFee& right);
    
    friend int operator==(const PO_SpOperFee &left,const PO_SpOperFee &right);
    
    friend int operator<(const PO_SpOperFee &left,const PO_SpOperFee& right);
    
    friend ostream& operator<<(ostream& os,const PO_SpOperFee& po);
      
  struct
  {
  	T_ATTRI_VAL    spCode;
  	T_ATTRI_VAL    operCode;
  	T_ATTRI_VAL    serviceCode;
  	T_ATTRI_VAL    feeType;
  	int            infoFee;
  	char           infoCheckType;
  	int            giftFee;
  	char           giftCheckType;
    T_DATE_TIME    m_beginTime;
    T_DATE_TIME    m_endTime;
  }recordStruct;

  private:
    static PO_Property m_property;

};

#endif //_PO_SPOPERFEE_H_050708_ 

