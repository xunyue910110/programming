#ifndef _ADUUPITEM_H_
#define _ADUUPITEM_H_
#include "FieldsXX.h"
#include <vector>
const char ADDUP_ELEM_TYPE_EVENT   = '1';//事件元素
const char ADDUP_ELEM_TYPE_ITEM    = '2';//帐目
const char ADDUP_ELEM_TYPE_ACYCNUM = '3';//使用帐期数
const char ADDUP_ELEM_TYPE_CURFEE  = '4';//当月的费用值

const char ADDUP_ELEM_VALUESRC_ORGIN  = '0';//原始值
const char ADDUP_ELEM_VALUESRC_DISCNT = '1';//优惠值
const char ADDUP_ELEM_VALUESRC_FACT   = '2';//实际值

class AddupCycleRule;
class RateGlobalInfo;
class RateTpInfo;
class RateBillId;
class AddupItem
{
  public:
  	AddupItem(){m_cycleRule = 0;}
  	virtual ~AddupItem(){}
  	bool operator < (const AddupItem &right) const
  	{
  		return (m_id < right.m_id);			
  	}
  
  	bool operator == (const AddupItem &right) const
  	{
  		return (m_id == right.m_id);
  	}
  public:
    //根据资费信息和费用作用的累计信息取对应的累计ID信息
    //Modified By Sunam 2009/12/29 10:26:02 根据指定时间r_cdrStartTime 求累计量周期
    //bool getRateBillIdInfo(RateGlobalInfo   *r_pGlobalInfo,
    //                       const RateTpInfo *r_rateTpInfo,
    //                             RateBillId *r_rateBillId) const;
	bool getRateBillIdInfo(RateGlobalInfo   *r_pGlobalInfo,
                           const RateTpInfo *r_rateTpInfo,
                           const T_DATE_TIME &r_cdrStartTime,
                                 RateBillId *r_rateBillId) const;
    //取周期起始时间和终止时间
    //Modified By Sunam 2009/12/29 10:26:02 根据指定时间r_cdrStartTime 求累计量周期
	//bool getCyleTime(RateGlobalInfo   *r_pGlobalInfo,
    //               const RateTpInfo *r_rateTpInfo,
    //                  long          &r_sumBeginTime,
    //                  long          &r_sumEndTime) const;
	bool getCyleTime(RateGlobalInfo   *r_pGlobalInfo,
                   const RateTpInfo *r_rateTpInfo,
                   const T_DATE_TIME &r_cdrStartTime,
                      long          &r_sumBeginTime,
                      long          &r_sumEndTime) const;
                      
	  /*@function 获取累计累计值
	   *@param    r_pGlobalInfo  接口信息
	   *@param    r_rateTpInfo   资费信息
	   *@param    r_roleCode     累计对象角色
	   *@param    r_sumBeginTime 周期起始时间
	   *@param    r_sumEndTime   周期终止时间
	   *@param    r_addupValue   值
	   *@param    r_startNum     开始取值周期偏移，取多个帐期累计值时有效
	   *@param    r_endNum       结束取值周期偏移，取多个帐期累计值时有效
	   *@return  true OK ,false 失败
	   */
    bool getAddupValue(RateGlobalInfo      *(&r_pGlobalInfo),
                       RateTpInfo          *(&r_rateTpInfo),
                       const long          &r_sumBeginTime,
                       const long          &r_sumEndTime,
                       long                &r_addupValue,
                       short               r_startNum = (short)0,
                       short               r_endNum = (short)0) const;
    
	  /*@function 获取系统级累计账单信息
	   *@param    r_pGlobalInfo  接口信息
	   *@param    r_rateTpInfo   资费信息
	   *@param    r_userIdList   ID列表
	   *@param    r_cycleIdList  帐期列表
	   *@param    r_cycleId      周期起始时间
	   *@param    r_addupValue   值
	   *@return  
	   */
    bool getBillValue(RateGlobalInfo      *r_pGlobalInfo,
                      const RateTpInfo    *r_rateTpInfo,
                const vector<T_SERVER_ID> &r_userIdList,
                const vector<int>         &r_cycleIdList,
					                  long          &r_addupValue) const;

  	//更新累计值
  	bool updateAddupValue(RateGlobalInfo    *(&r_pGlobalInfo),
  	                      RateTpInfo        *(&r_rateTpInfo),
                            const long          &r_sumBeginTime,
                            const long          &r_sumEndTime,
                            const long          &addupValue,
                            char                addupMode);
      
      //获取帐务的当前费用                            
  	bool getAcctCurValue(RateGlobalInfo *(&r_pGlobalInfo), long &curValue);
  
  public:	
  	void dump(char *prefix);
  	
  
  public:
  	char m_elemType;         //累计的元素类型
  	char m_elemValueSrc;     //累计的元素值来源
  	int  m_elemId;           //累计的元素标识
  	int  m_id;               //累计帐目标识
  	int  m_cycleRuleId;      //累计周期规则标识
    AddupCycleRule * m_cycleRule; //周期规则
};	

#endif
