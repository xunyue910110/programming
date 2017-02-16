#ifndef PO_TRANSNUMBER_H
#define PO_TRANSNUMBER_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_TransNumber
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_TransNumber(){}

    PO_TransNumber(const PO_TransNumber& right);

    void operator=(const PO_TransNumber& right);

    friend int operator==(const PO_TransNumber &left,
    		const PO_TransNumber& right);

    friend int operator<(const PO_TransNumber &left,
    		const PO_TransNumber& right);

	friend ostream& operator<<(ostream& os,const PO_TransNumber& po);

	struct
	{
	  S_T_PHONE_NUMBER m_transNumber;
	  T_ATTRI_VAL m_carrierType;
	  T_ATTRI_VAL m_transType;
	  int m_numberLength;
	  char m_asp;
	  char m_dialType;
    char m_isNoNeedTp; //add by shenglj 2009-08-12 20:37:01
	}recordStruct;

  private:
	static PO_Property m_property;
};

class PO_TransNumber_CNC:public PO_TransNumber
{
  public:
    bool getProperty(PO_Property &property);
  private:
	static PO_Property m_property;
};

#endif /* PO_TRANSNUMBER_H */
