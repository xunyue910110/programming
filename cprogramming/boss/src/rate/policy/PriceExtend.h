#ifndef _PRICEEXTEND_H_
#define _PRICEEXTEND_H_

//批价政策扩展
//该表用来记录批价政策的一些特殊需求导致的扩展信息，
//比如X包Y分钟，里面的累计分钟Y可能需要在这里定义，
//方便批价处理!

#include "FieldsXX.h"

class PriceExtend 
{
public:
	PriceExtend() {m_next = NULL;}	
    ~PriceExtend() {}
public:	
	void dump(char *,int &);
public:
	bool operator < (const PriceExtend &right) const
	{
		if (m_id == right.m_id)
		{
			return (m_extendId < right.m_extendId);
		}
		else
		{
			return (m_id < right.m_id);
		}
	}

	bool operator == (const PriceExtend &right) const
	{
		return (m_id == right.m_id);
	}      
	
public:		
	char m_extendType;     //扩展属性类型
	int m_id;              //批价政策标识
	int m_extendId;        //批价政策扩展标识
	PriceExtend *m_next;   //下一个扩展标识
	T_STR_NUMBER m_extendValue1;      //扩展属性值1
	T_STR_NUMBER m_extendValue2;      //扩展属性值2
	T_STR_NUMBER m_extendValue3;      //扩展属性值3
};

#endif
