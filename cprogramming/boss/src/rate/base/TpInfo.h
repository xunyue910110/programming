
#ifndef TPINFO_H_INCLUDED_20080829_
#define TPINFO_H_INCLUDED_20080829_

#include <string>
#include <vector>
#include <map>
#include "FieldsXX.h"

class UserTpParam
{
  public:
    T_TPOBJ_ID        m_tpObjId;        // 资费实例标识
    T_TP_ID           m_tpParamId;      // 资费参数标识
    char              m_valueType;      // 参数值类型
    T_TPPARAM_VALUE   m_tpValue;        // 参数值
  	T_DATE_TIME       m_beginTime;      // 起始时间
  	T_DATE_TIME       m_endTime;        // 终止时间
  	T_RELATION_TYPE   m_relatinType;    // 关联类型0:普通参数，无关联
                                        //         1:特殊群关联，适用计费方在与群内号码通话优惠
                                        //         2:特殊号码关联，适用计费方在与群特殊号码通话
                                        //         3:特殊区域关联，适用计费方在特殊区域内通话
    //以下为特殊关联时有效
    T_LEVEL_TYPE      m_levelType;      //对于小区：小区级别(县、乡、村)
                                        //对于特殊号码和群: 群类型(部门、公司等)
    T_MEMBER_TYPE     m_memberType;     //     成员角色(业务类型、主付号等)
  public:
    friend ostream& operator<<(ostream& r_os,const UserTpParam &r_right);
};

class UserTp
{
  public:
    T_TPOBJ_ID          m_tpObjId;        // 资费实例标识
  	T_TP_ID             m_tpId;           // 资费政策标识
  	T_DATE_TIME         m_beginTime;      // 起始时间
  	T_DATE_TIME         m_endTime;        // 终止时间
    char                m_idType;         // 三户类型
    T_SERVER_ID         m_userId;         // 三户标识
    long                m_productId;      // 产品标识
    long                m_servBund_Id;    // 服务包标识
    long                m_servId;         // 服务标识
    long                m_feeBund_Id;     // 资费包标识   
    T_SERVER_ID         m_relationVpnId;  // 关联群标识
    
  	T_FLAG              m_halfInNetFlag;  // 半月入网标志
  	T_FLAG              m_firstMonthFlag; // 首月标志
  	T_TP_TYPE           m_subTpType;      // 普通个人资费、基站资费、VPN资费、VPN基站套餐 
  	bool                m_validFlag;      // 是否有效（特殊号码等对方在指定群内）
  	T_FLAG              m_cellFlag;       // 小区内、小区外标记
  	int                 m_vpnFlag;        // V网集团关系定义
  	T_MEMBER_TYPE       m_memberType_A;   // VPN套餐时，A角色类型// 小区套餐时：小区内、小区外
  	T_MEMBER_TYPE       m_memberType_B;   // VPN套餐时，B角色类型
    vector<UserTpParam> m_paramList;  // 参数: CRM

	// add by chenm 2009-4-15 8:58:10
	T_SERVER_ID  m_lastVpnParentIDA;    //计费号码的直接母集团ID
	T_SERVER_ID  m_lastVpnParentIDB;    //对方号码的直接母集团ID
	// over 2009-4-15 8:58:10
  public:
    UserTp();
    UserTp(const T_TP_ID &r_tpId,const T_SERVER_ID &r_userId);
    bool isValid(const T_DATE_TIME &r_beginTime,const T_DATE_TIME &r_endTime) const;
    //added by liugj 20090523
    bool isValid4Event24(const T_DATE_TIME &r_beginTime,const T_DATE_TIME &r_endTime) const;
    //只判断日期是否有效
    bool isValid(const int &r_beginDate) const;
    friend ostream& operator<<(ostream& r_os,const UserTp &r_right);
};

class RateTpParam
{
  public:
    T_TP_ID          m_rateTpNo;     //批价政策序列号
    T_TP_ID          m_rateParamId;  //批价政策参数ID
    char             m_source;       //参数来源：0固定值、1用户资料
    char             m_valueType;    //参数值类型
    T_TP_ID          m_userParamId;  //用户参数ID
    T_TPPARAM_VALUE  m_value;        //参数值
    UserTpParam     *m_pUserParam;   //用户参数值信息
  public:
    friend int operator==(const RateTpParam &r_left,
    		                  const RateTpParam &r_right);
    friend int operator<(const RateTpParam &r_left,
    		                 const RateTpParam &r_right);
    friend ostream& operator<<(ostream& r_os,const RateTpParam &r_right);
};

class EventFeePolicy;
class RateTpInfo
{
  public:
    T_TP_ID              m_rateTpId ;    //资费标识
    UserTp              *m_pUserTp  ;    //用户政策
    T_BIZ_TYPE           m_eventType;    //事件类型
    T_TP_ID              m_rateTpNo;     //批价政策序列号
    int                  m_priority;     //批价政策优先级
    vector<RateTpParam>  m_rateTpParam;  //批价政策参数 
    char                 m_compMethod;   //费用项比较、整条话单费用比较
    char                 m_execMode;     //执行方式：独占、协同、部分强制
    T_FLAG               m_realRentFlag; //实扣标记
    int                  m_sumBillId;    //实扣累计
    int                  m_rentBillId;   //实扣帐目
    long                 m_monthRent;    //全月实扣月租
    //long                 m_halfRent;     //半月实扣月租
    //计算方法 add by gaojf 2009-2-18 12:28
    char                 m_rentCompMethod; 
    
    T_MEMBER_TYPE        m_rateRoleCode; //作用角色参数
    EventFeePolicy      *m_pEventTp;     //事件资费政策实现入口
    bool                     m_isDealed;       // add by chenm 2009-4-20 标识改政策是否成功作用
   //角色为枚举类型 add by xuf 2009-10-28 19:51:20 
   vector<T_MEMBER_TYPE> 	m_roleCodeVector;  //作用角色编码容器 离散型枚举
	 map<T_MEMBER_TYPE,T_MEMBER_TYPE> m_roleCodeMap;//作用角色编码容器 连续型枚举
	 
  public:
    RateTpInfo(){m_pEventTp=NULL;m_isDealed=false;}
    virtual ~RateTpInfo(){m_pEventTp=NULL;}
  public:
    //根据事件判断是否有效
    bool isValid(const T_DATE_TIME &r_beginTime,const T_DATE_TIME &r_endTime) const;
    bool isValid(const int &r_beginDate) const;
    
    //added by liugj 20090523
    bool isValid4Event24(const T_DATE_TIME &r_beginTime,const T_DATE_TIME &r_endTime) const;
    
    //根据参数ID取对应的值，找不到返回NULL
    const char* getParamValue(const T_TP_ID &r_paramId) const;
    
    //add by gaojf 2009-2-18 11:20 为了支持 
    //根据日期计算实扣月租
    //r_compMethod: 计算方法 
    //r_openDate  : 开户日期时间 
    //r_monthDay  ：该月天数
    //Modified By Sunam 2009/11/10 10:50:03
    //long getRentValue(const char &r_openFlag,const T_DATE_TIME &r_openDate,
    //                  const int  &r_monthDay) const;
    long getRentValue(const char &r_openFlag,const T_DATE_TIME &r_openDate,
                      const int  &r_monthDay,const T_DATE_TIME &r_earliestOpenDate) const;

    //入网或第一次选套餐的时候累计上下限按照日期来折算
    //根据日期和判断方法求取值
    //r_value     : 值
    //r_compMethod: 计算方法 
    //r_openFlag  : 是否首月开户标记
    //r_openDate  : 开户日期时间 
    //r_monthDay  ：该月天数
    //Modified By Sunam 2009/11/10 10:48:30
    //long getValue(const long        &r_value,
    //              const char        &r_compMethod,
    //              const char        &r_openFlag,
    //              const T_DATE_TIME &r_openDate,
    //              const int         &r_monthDay) const;
	long getValue(const long        &r_value,
                  const char        &r_compMethod,
                  const char        &r_openFlag,
                  const T_DATE_TIME &r_openDate,
                  const int         &r_monthDay,
                  const T_DATE_TIME &r_earliestOpenDate,
                  const int         &r_billCycle = 0,
                  const char        *r_param = NULL) const;
    //Added By Sunam 2009/10/6 19:49:43
    long getValue(const long        &r_value,
                          const char        &r_compMethod,
                          const char        &r_openFlag,
                          const T_DATE_TIME &r_openDate,
                          const T_DATE_TIME &r_startDateTime,
                          const int         &r_monthDay,
                          const int         &r_billCycle,
                          const char        *r_param) const;
    
    friend int operator<(const RateTpInfo &r_left,
    		                 const RateTpInfo &r_right);
    friend ostream& operator<<(ostream& r_os,const RateTpInfo &r_right);
};

#endif //TPINFO_H_INCLUDED_20080829_