#ifndef PO_ATTRFACTORS_H_INCLUDE_20081117_
#define PO_ATTRFACTORS_H_INCLUDE_20081117_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"
class PO_AttrFactors
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_AttrFactors(){}

    PO_AttrFactors(const PO_AttrFactors& right);

    void operator=(const PO_AttrFactors& right);

    friend int operator==(const PO_AttrFactors &left,const PO_AttrFactors& right);

    friend int operator<(const PO_AttrFactors &left,const PO_AttrFactors& right);

	friend ostream& operator<<(ostream& os,const PO_AttrFactors& po);

    struct{
    	T_BIZ_TYPE   			m_biztype;// 业务类型
		  int								m_fType;  // 1 基本政策因素，2 明细帐单因素
		  int         			m_factn;  // 对应域td_feebase_all_tp中FACTORn中的n
		  vector<string>    m_fvalues;// 枚举 中间用英文分号分割
    }m_record;

  private:
	  static PO_Property m_property;
};
#endif //PO_ATTRFACTORS_H_INCLUDE_20081117_

