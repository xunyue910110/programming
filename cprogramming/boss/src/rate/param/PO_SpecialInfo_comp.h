#ifndef PO_SPECIALINFO_COMP_H_INCLUDE_20080901_
#define PO_SPECIALINFO_COMP_H_INCLUDE_20080901_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_SpecialInfo_comp
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_SpecialInfo_comp(){}

    PO_SpecialInfo_comp(const PO_SpecialInfo_comp& right);

    void operator=(const PO_SpecialInfo_comp& right);

    friend int operator==(const PO_SpecialInfo_comp &left,const PO_SpecialInfo_comp& right);

    friend int operator<(const PO_SpecialInfo_comp &left,const PO_SpecialInfo_comp& right);

	  friend ostream& operator<<(ostream& os,const PO_SpecialInfo_comp& po);

    struct
    {
    	T_SPECIALINFO_ID m_spInfoId;   //特殊分组标识
    	T_MEMBER_ID      m_memberId;   //成员号码或标识
  		T_DATE_TIME      m_beginTime;  //起始时间
  		T_DATE_TIME      m_endTime;    //终止时间
  		T_FLAG           m_matchType;  //匹配方式 0 前缀匹配、2 完全匹配
  		T_FLAG           m_preFixType; //前缀方式 0: 无前缀 （不考虑前缀情况，一般为手机号码）
                                     //         1: 有前缀  （肯定有前缀，去掉前缀与参数中的配置匹配）
                                     //         2: 可能有前缀 （如果有区号则去掉区号进行匹配）
                                     //         3: 强制加前缀  （如果有区号直接匹配，如果没有区号则加上对方号码的归属地区号进行匹配）
  		T_SERV_TYPE      m_servType;   //计费业务类型
  		T_MEMBER_TYPE    m_memberType; //成员类型：0:V网  1:号码或者号码前缀
    }m_record;

  private:
	  static PO_Property m_property;
};



#endif //PO_SPECIALINFO_COMP_H_INCLUDE_20080901_

