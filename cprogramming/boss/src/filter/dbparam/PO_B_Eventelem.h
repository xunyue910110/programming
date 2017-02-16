#ifndef _PO_B_Eventelem_H_
#define _PO_B_Eventelem_H_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_B_Eventelem
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_B_Eventelem(){}

    PO_B_Eventelem(const PO_B_Eventelem& right);

    void operator=(const PO_B_Eventelem& right);

    friend int operator==(const PO_B_Eventelem& left,
    		const PO_B_Eventelem& right);

    friend int operator<(const PO_B_Eventelem& left,
    		const PO_B_Eventelem& right);

	friend ostream& operator<<(ostream& os,const PO_B_Eventelem& po);

    struct
    {
    	int				m_elementId; //事件元素标识
    	int   		m_elementKey;//事件元素关键字(话单下标)
    }recordStruct;

  private:
	static PO_Property m_property;

};

#endif /* _PO_B_Eventelem_H_ */
