#ifndef PO_UserTpParam_def_H
#define PO_UserTpParam_def_H

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_UserTpParam_def
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_UserTpParam_def(){}

    PO_UserTpParam_def(const PO_UserTpParam_def& right);

    void operator=(const PO_UserTpParam_def& right);

    friend int operator==(const PO_UserTpParam_def &left,const PO_UserTpParam_def& right);

    friend int operator<(const PO_UserTpParam_def &left,const PO_UserTpParam_def& right);

	 friend ostream& operator<<(ostream& os,const PO_UserTpParam_def& po);

    struct
    {
      T_TP_ID           m_tpParamId;  //参数属性标识
    	T_TP_ID           m_tpId;       //资费标识
    	T_RELATION_TYPE   m_relatinType;//关联类型
    	char              m_valueType;  //参数值类型
    }m_record;

  private:
	  static PO_Property m_property;

};



#endif /* PO_UserTpParam_def_H */
