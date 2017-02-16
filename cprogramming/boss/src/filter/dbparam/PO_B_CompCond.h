#ifndef _PO_B_COMPCOND_H_
#define _PO_B_COMPCOND_H_

#include "dbparam/PO_Common.h"
#include "FieldsXX.h"

class PO_B_CompCond
{
  public:
    bool getProperty(PO_Property &property);

    bool convert(const StringVector &argname);

    PO_B_CompCond(){}

    PO_B_CompCond(const PO_B_CompCond& right);

    void operator=(const PO_B_CompCond& right);

    friend int operator==(const PO_B_CompCond& left,
    		const PO_B_CompCond& right);

    friend int operator<(const PO_B_CompCond& left,
    		const PO_B_CompCond& right);

	friend ostream& operator<<(ostream& os,const PO_B_CompCond& po);

    struct
    {
    	int				m_condId;    //条件标识ID
    	int   		m_orderNo;   //顺序号
    	T_FLAG   	m_andOrTag;  //与或标志 0:并且 1:或者 暂不支持
    	int   		m_subCondId; //子条件标识ID
    	PO_B_CompCond  *m_next;     //外部条件
    	PO_B_CompCond  *m_condRule; //下一个条件
    }recordStruct;

  private:
	static PO_Property m_property;

};

#endif /* _PO_B_COMPCOND_H_ */
