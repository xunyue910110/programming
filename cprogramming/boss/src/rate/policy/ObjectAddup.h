#ifndef _OBJECTADDUP_H_
#define _OBJECTADDUP_H_

#include "ObjectBase.h"
#include <vector>
class AddupItem;
class RateTpInfo;
class ObjectAddup :public ObjectBase
{
  public:
  	ObjectAddup(){m_addupItem = NULL;}
  	virtual ~ObjectAddup(){}
  	
  	bool operator < (const ObjectAddup &right) const
  	{
  		return (m_id < right.m_id);			
  	}
  
  	bool operator == (const ObjectAddup &right) const
  	{
  		return (m_id == right.m_id);
  	} 
  public:	
  	virtual bool getObjectValue(T_OBJECT_ATTR &value);
  	virtual bool getObjectValue(long &value);
  	virtual void dump(char *prefix);
  protected:
    //根据系统帐期取前后几个月的帐期信息
    void getCycleIdList(RateGlobalInfo *r_pGlobalInfo,
                                 const int      &r_startOffNum,
                                 const int      &r_endOffNum,
                                 vector<int>    &r_cycleIdList) const;
    void getUserIdList(RateGlobalInfo      *r_pGlobalInfo,
                                const RateTpInfo    *r_rateTpInfo,
                                const T_MEMBER_TYPE &r_roleCode,
                                vector<T_SERVER_ID> &r_userIdList) const;
    
  public:
  	char          m_attrSource;  //值来源
    short         m_startNum;    //开始取值周期偏移
    short         m_endNum;      //结束取值周期偏移
    AddupItem    *m_addupItem;   //累计帐目指针
};	

#endif
