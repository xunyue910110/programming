#ifndef PO_SYSPARAM_H_INCLUDE_20081014_
#define PO_SYSPARAM_H_INCLUDE_20081014_

#include "dbparam/PO_Common.h"
#include "BillingCommon.h"
#include "FieldsXX.h" 


class PO_SysParam
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_SysParam(){}

    PO_SysParam(const PO_SysParam &right);

    void operator=(const PO_SysParam &right);

    friend int operator==(const PO_SysParam &left,const PO_SysParam &right);

    friend int operator<(const PO_SysParam &left,const PO_SysParam &right);

	  friend ostream& operator<<(ostream& os,const PO_SysParam& po);

    ProcSysParam  m_record;

  private:
	  static PO_Property m_property;
}; 
#endif //PO_SYSPARAM_H_INCLUDE_20081014_
