#ifndef PO_FEEPOLICYREMIDINFO_H_INCLUDE_20090808_
#define PO_FEEPOLICYREMIDINFO_H_INCLUDE_20090808_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h" 
#include "BillingCommon.h" 


class PO_FeePolicyRemidInfo
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_FeePolicyRemidInfo(){}

    PO_FeePolicyRemidInfo(const PO_FeePolicyRemidInfo &right);

    void operator=(const PO_FeePolicyRemidInfo &right);

    friend int operator==(const PO_FeePolicyRemidInfo &left,const PO_FeePolicyRemidInfo &right);

    friend int operator<(const PO_FeePolicyRemidInfo &left,const PO_FeePolicyRemidInfo &right);

	  friend ostream& operator<<(ostream& os,const PO_FeePolicyRemidInfo& po);

    FeePolicyRemidInfo  m_record;

  private:
	  static PO_Property m_property;
}; 
#endif //PO_FEEPOLICYREMIDINFO_H_INCLUDE_20090808_
