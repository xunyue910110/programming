#ifndef _PO_B_OBJECT_H_
#define _PO_B_OBJECT_H_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_B_Object
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_B_Object(){}

    PO_B_Object(const PO_B_Object& right);

    void operator=(const PO_B_Object& right);

    friend int operator==(const PO_B_Object& left,
    		const PO_B_Object& right);

    friend int operator<(const PO_B_Object& left,
    		const PO_B_Object& right);

	friend ostream& operator<<(ostream& os,const PO_B_Object& po);

    struct
    {
    	int				m_objectId; //对象标识
    	T_FLAG   	m_attrType; //属性类型 0:帐目 1:累计量 2:事件元素 3:资料元素
    	int   		m_attrId;   //属性ID 根据该ID再去查找事件元素KEY(话单下标)
    }recordStruct;

  private:
	static PO_Property m_property;

};

#endif /* _PO_B_OBJECT_H_ */
