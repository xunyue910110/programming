#ifndef PO_HOMEAREACODEBYBUREAU_H
#define PO_HOMEAREACODEBYBUREAU_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_HomeAreaCodeByBureau
{
public:
    bool getProperty(PO_Property& property);
    
    bool convert(const StringVector& argname);
    
    PO_HomeAreaCodeByBureau(){}
    
    PO_HomeAreaCodeByBureau(const PO_HomeAreaCodeByBureau& right);
    
    void operator=(const PO_HomeAreaCodeByBureau& right);
    
    friend int operator==(const PO_HomeAreaCodeByBureau& left, const PO_HomeAreaCodeByBureau& right);
    
    friend int operator<(const PO_HomeAreaCodeByBureau& left, const PO_HomeAreaCodeByBureau& right);
    
    friend ostream& operator<<(ostream& os, const PO_HomeAreaCodeByBureau& po);
    
    struct
    {
        //T_BUREAU m_bureau;
	  T_AREA_CODE m_bureau;
	  T_AREA_CODE m_feeCode;
    	  T_DATE_TIME m_beginTime;
    	  T_DATE_TIME m_endTime;
    }recordStruct;

private:
    static PO_Property m_property;
};
#endif /* PO_HOMEAREACODEBYBUREAU_H */
