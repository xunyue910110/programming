#ifndef _FEEPOLICYCOMP_H_
#define _FEEPOLICYCOMP_H_

#include "FieldsXX.h"
#include "RateConstDef.h"
#include <vector>
#include <map>

class EventFeePolicy;
class PriceParamImpl;
class PolicyInfo;

class FeePolicyComp
{
public:
    FeePolicyComp() { m_eventFeePolicy=0; m_priceParamImpl=0; m_next=0;}
    ~FeePolicyComp() {}
	bool execute();
	void dump();
	void splitEnumValue(const char* pEnumValue,
	                    T_MEMBER_TYPE &value, 
	                    vector<T_MEMBER_TYPE> &vValue,
	                    map<T_MEMBER_TYPE, T_MEMBER_TYPE> &mapValue);
	
public:
	bool operator < (const FeePolicyComp &right) const
	{
		if( m_id == right.m_id)				
		{
			if( m_eventTypeId == right.m_eventTypeId)
				return (m_priority < right.m_priority);
			else
				return (m_eventTypeId < right.m_eventTypeId);
		}
		else
			return (m_id < right.m_id);			
	}

	bool operator == (const FeePolicyComp &right) const
	{
		return (m_id == right.m_id);
	}      
	
public:	
	char                m_cdrCompFlag;     //整条费用比较标记 0:按费用域比较  1:按整条话单费用比较
	char                m_execMode;        //执行方式 0：独占性作用 1：协作性作用
	short               m_priority;        //事件政策优先级	
	T_TP_ID             m_id;              //资费标识
	int                 m_compId;          //资费构成标识
	T_MEMBER_TYPE       m_roleCode;        //作用角色编码
	char                m_roleCodes[STR_ROLECODE_ENUMLEN+1]; //作用角色编码基准值
	vector<T_MEMBER_TYPE> 	m_roleCodeVector;  //作用角色编码容器 离散型枚举
	map<T_MEMBER_TYPE, T_MEMBER_TYPE> m_roleCodeMap;//作用角色编码容器 连续型枚举
	int                 m_eventTypeId;     //事件类型标识
	int                 m_eventPolicyId;   //事件资费标识
	EventFeePolicy     *m_eventFeePolicy;  //事件资费指针
	PriceParamImpl     *m_priceParamImpl;  //批价参数实现指针
	FeePolicyComp      *m_next;	           //下一个资费构成指针 
  	
private:
	static PolicyInfo	*m_policyInfo;
};

#endif