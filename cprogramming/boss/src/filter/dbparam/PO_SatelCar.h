#ifndef PO_SATELCAR_H
#define PO_SATELCAR_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_SatelCar
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_SatelCar(){}

    PO_SatelCar(const PO_SatelCar& right);

    void operator=(const PO_SatelCar& right);

    friend int operator==(const PO_SatelCar &left,
    		const PO_SatelCar& right);

    friend int operator<(const PO_SatelCar &left,
    		const PO_SatelCar& right);

	friend ostream& operator<<(ostream& os,const PO_SatelCar& po);

    struct
    {
    	T_PROVINCE_CODE  province_code;
    	T_MSC  msc;
    	T_LAC  lac;
    	T_CELL_ID cellId;
    	T_DATE_TIME beginTime;
    	T_DATE_TIME endTime;
    }recordStruct;

  private:
	static PO_Property m_property;

};

#endif /* PO_SATELCAR_H */
