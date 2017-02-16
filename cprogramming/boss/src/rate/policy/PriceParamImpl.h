#ifndef _PriceParamImpl_H_
#define _PriceParamImpl_H_

class Price;

class PriceParamImpl
{
public:
    PriceParamImpl() { m_next = 0;}
    ~PriceParamImpl() {}
	void dump(int &);
public:
	bool operator < (const PriceParamImpl &right) const
	{
		if( m_id == right.m_id)				
		{
			return (m_priceParamId < right.m_priceParamId);
		}
		else
			return (m_id < right.m_id);
			
	}

	bool operator == (const PriceParamImpl &right) const
	{
		return (m_id == right.m_id);
	}      
	
public:		
	char m_implMethod;     //实现方式 0:固定值   1:来源资费_参数定义
	char m_paramValueType; //属性值类型 0:数值  1:字符串
	char m_fixedValue[20+1];          //固定值       
	int m_id;              //资费构成标识
	int m_priceParamId;    //批价政策参数标识
	int m_feePolicyParamId;//资费参数标识
	PriceParamImpl *m_next;	        //下一个批价参数
};

#endif