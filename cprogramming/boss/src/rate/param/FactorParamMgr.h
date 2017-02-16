#ifndef _FACTORPARAMMGR_H_INCLUDE_20081117_
#define _FACTORPARAMMGR_H_INCLUDE_20081117_

#include "PO_AttrFactors.h"
#include "dbparam/PO_Common.h"
#include <algorithm>

template<class T>
class FactorParamMgr
{
  public:
    T_BIZ_TYPE                m_bizType;     //业务类型
    vector<T>                 m_elementList; //该业务展开后的参数列表
    vector<T*>                m_pElementList;//该业务展开后的参数指针列表
    vector<PO_AttrFactors*>   m_factors;     //展开参数
static vector<PO_AttrFactors*> *m_pFactors;   //静态指针
  private:
    vector<T>                 m_tmpTpList[2]; //临时变量
    int                       m_factorSize;   //参数因素个数
  public:
    FactorParamMgr(){clear();}
    FactorParamMgr(const T_BIZ_TYPE &r_bizType)
    {m_bizType=r_bizType; clear();}
    ~FactorParamMgr(){clear();}

    //判断是否符合快速查询条件
    bool isMathFactors(const T &r_element)  const; 

    //比较元素的大小，相当于< 用于排序 考虑优先级
static bool cmpElement_sort(const T &r_left,const T &r_right);

    //比较元素的大小，相当于< 用于查找 不考虑优先级
static bool cmpElement_search(const T &r_left,const T &r_right);

    //比较元素*的大小，相当于< 用于排序 考虑优先级
static bool cmpPElement_sort(const T* r_left,const T * r_right);

    //比较FeebaseTP*的大小，相当于< 用于查询 不考虑优先级
static bool cmpPElement_search(const T* r_left,const T * r_right);

    //根据2分法查找政策ID
    bool getElement(const T &r_element,const int &r_fSize,const PCDR &r_pcdr,T * (&r_pElement));
    bool getElement(const T &r_element,const int &r_fSize,const PCDR &r_pcdr,vector<T *> &r_pElementList);
    void clear(); 
    void push_back(const T &r_element);//加入元素，并展开后加入目标列表中

    //设置要素
    void setFactors(const vector<PO_AttrFactors*> &r_factorList);

    void sort();//对展开结果进行排序

    bool isMatchFactor(const T_ATTRIBUTE_VALUE &r_attribute,
                       const vector<string>    &r_fAttrList);
  public:
    void dump(ostream &r_os);
  private:
    	bool m_result;
    	typename vector<T*>::const_iterator m_itr,m_upperItr;
    	pair<typename vector<T*>::const_iterator, 
    	     typename vector<T*>::const_iterator> m_equalItr;

};
#include "FactorParamMgr.cpp"

#endif //_FACTORPARAMMGR_H_INCLUDE_20081117_
