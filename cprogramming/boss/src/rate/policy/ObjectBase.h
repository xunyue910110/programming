#ifndef _OBJECTBASE_H_
#define _OBJECTBASE_H_

#include "FieldsXX.h"
#include "RateConstDef.h"
#include <vector>
#include <map>

class RateGlobalInfo;
class PolicyInfo;

class ObjectBase
{
  public:
  	ObjectBase(){}
  	~ObjectBase(){}	
  	
  	virtual bool getObjectValue(T_OBJECT_ATTR &) = 0;
  	virtual bool getObjectValue(long &) = 0;
  	virtual void dump(char *prefix);
  	
  public:
  	char  m_attrType; //属主类型 0:帐目 1:累计量 2:事件元素 
  	                  //         3:资料元素 4:系统级累计账单 5:常量 6:参数
    T_MEMBER_TYPE m_roleCode;//作用角色编码,对帐目和累计量有效  
  	int   m_attrId;   //属性编码
  	int   m_id;
	
  	char  m_roleCodes[STR_ROLECODE_ENUMLEN+1];  //作用角色编码基准值
  	vector<T_MEMBER_TYPE> m_roleCodeVector;  //作用角色编码容器 离散型枚举
  	map<T_MEMBER_TYPE, T_MEMBER_TYPE> m_roleCodeMap;//作用角色编码容器 连续型枚举
  	//条件ID。
  	T_RULE_ID     m_condId;

  protected:
  	static RateGlobalInfo *m_globalInfo;
  	static PolicyInfo *m_policyInfo;
};

class Object
{
  public:
  	Object(){}
  	~Object(){}	
  	
  	bool operator < (const Object& right) const
  	{
  		return (m_id < right.m_id);			
  	}
  
  	bool operator == (const Object& right) const
  	{
  		return (m_id == right.m_id);
  	}    
  public:
  	int m_id;
  	ObjectBase *m_objectBase;       
};

#endif
