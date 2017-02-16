#ifndef PO_PRODUCTREL_H_INCLUDE
#define PO_PRODUCTREL_H_INCLUDE

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_ProductRel
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_ProductRel(){}

    PO_ProductRel(const PO_ProductRel& right);

    void operator=(const PO_ProductRel& right);

    friend int operator==(const PO_ProductRel &left,const PO_ProductRel& right);

    friend int operator<(const PO_ProductRel &left,const PO_ProductRel& right);

	friend ostream& operator<<(ostream& os,const PO_ProductRel& po);

    struct
    {
    	T_PRODUCT_ID     m_productId;
    	String<10>       m_status;
    	T_PRODUCT_ID     m_parentProductId;
		T_DATE_TIME      m_beginTime;
  		T_DATE_TIME      m_endTime;
    }m_record;

  private:
	static PO_Property m_property;

};


#endif /* PO_ProductRel */
