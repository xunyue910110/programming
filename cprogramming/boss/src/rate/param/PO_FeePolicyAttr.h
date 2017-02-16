#ifndef PO_FEEPOLICYATTR_H_20090507
#define PO_FEEPOLICYATTR_H_20090507

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class CondBase;
class PO_FeePolicyAttr
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_FeePolicyAttr(){}

    PO_FeePolicyAttr(const PO_FeePolicyAttr& right);

    void operator=(const PO_FeePolicyAttr& right);

    friend int operator==(const PO_FeePolicyAttr &left,const PO_FeePolicyAttr& right);

    friend int operator<(const PO_FeePolicyAttr &left,const PO_FeePolicyAttr& right);

	 friend ostream& operator<<(ostream& os,const PO_FeePolicyAttr& po);

    struct
    {
		T_TP_ID           m_tpId;     //资费标识
		T_RULE_ID         m_ruleId;   //条件ID
		CondBase         *m_condRule; //条件指针
		int               m_attrid;       //资费标识
		T_ATTRI_VAL       m_value;
    }m_record;

  private:
	  static PO_Property m_property;

};

#endif //PO_FEEPOLICYATTR_H_20090507
