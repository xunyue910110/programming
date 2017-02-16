#ifndef __FIELDS_H__
#define __FIELDS_H__

#include "base/Types.h"

USING_NAMESPACE(std)

//typedef AttributeValue  T_ATTRIBUTE_VALUE;
typedef String<100>  T_ATTRIBUTE_VALUE;
typedef String<100>  T_ATTRI_VAL;

typedef String<1>    T_FLAG;
typedef String<8>    T_BIZ_TYPE;
typedef String<16>   T_USER_ID;
typedef String<16>   T_SERVER_ID;
typedef String<16>   T_CUST_ID;

typedef String<4>    T_GROUP_ID;
typedef String<16>   T_VPN_ID;
typedef String<24>   T_MSISDN;
typedef String<15>   T_IMSI;
typedef String<15>   T_IMSI_SEG;
typedef String<24>   T_OTHER_PARTY;
typedef String<24>   T_ORIGIN_NO;
typedef String<11>   T_MSRN;
typedef String<11>   T_PHONE_NUMBER;
typedef String<14>   T_DEAL_TIME;

//m_homeAreaCode,m_visitAreaCode
typedef String<10>   T_AREA_CODE;
typedef String<10>   T_NATIONAL_AREA_CODE;
typedef String<3>    T_PROVINCE_CODE;
typedef String<15>   T_MSC;
typedef String<5>    T_LAC;
typedef String<5>    T_CELL_ID;
typedef String<10>   T_LAC_CELL;
typedef String<7>    T_TRUNK_GROUP;
typedef String<3>    T_SERVICE_TYPE;
typedef String<4>    T_SERVICE_CODE;
typedef String<4>    T_LONG_GROUP;
typedef String<16>   T_IMEI;

typedef String<1>    T_ASP;
typedef String<6>    T_INTER_ASPCODE;
typedef String<11>   T_INTER_ASPGROUP;
typedef String<2>    T_SERV_TYPE;
typedef String<1>    T_HOME_TYPE;
typedef String<1>    T_STATIS_TYPE;
typedef String<1>    T_ROAM_TYPE;
typedef String<2>    T_CALL_TYPE;
typedef String<2>    T_CALLED_TYPE;
typedef String<1>    T_CARRIER_TYPE;
typedef String<1>    T_LONG_TYPE;
typedef String<25>   T_FILE_NO;
typedef String<1>    T_TERM_CAUSE;
typedef String<4>    T_CITY_CODE;
typedef String<15>   T_M123;
typedef String<4>    T_USER_TYPE;
typedef String<4>    T_OFFICE_CODE;
typedef String<2>    T_FEE_TYPE;
typedef String<1>    T_FORWARD_CAUSE;
typedef String<1>    T_EDGE_FLAG;
typedef String<24>   T_FREE_CODE;
typedef String<3>    T_ERROR_CODE;
typedef String<3>    T_TP_ID_ABBR;
typedef String<8>    T_PRODUCT_ID;

typedef String<14>   T_DATE_TIME;

typedef int          T_TIMES;
typedef Float        T_FEE;
typedef int          T_FREEMIN_ID;
typedef int          T_FREEMIN_VAL;


typedef Integer      T_CYCLE;
typedef Integer      T_TP_ID;
typedef Integer      T_RULE_ID;
typedef Integer      T_FEE_ID;
typedef Integer      T_FEE_ACCOUNT_ID;

typedef Integer      T_DATE;
typedef Integer      T_TIME;

typedef Integer      T_DATETIME;
typedef Integer      T_DURATION;
typedef Integer      T_FEE_FIELD;
typedef Integer      T_FEE_UNIT;
typedef Integer      T_PRI;
typedef Integer      T_ATTRIBUTE_ID;
typedef Integer      T_GROUP;
typedef Integer      T_CHANNEL;
typedef Integer      T_FLOW;
typedef Integer      T_FLOW_ID;

//新增的类型
typedef T_ATTRIBUTE_VALUE  S_T_FEE_PRI;
typedef T_ATTRIBUTE_VALUE  S_T_FEE_TYPE;
typedef T_ATTRIBUTE_VALUE  S_T_GROUP_ID;
typedef T_ATTRIBUTE_VALUE  S_T_CALL_TYPE;
typedef T_ATTRIBUTE_VALUE  S_T_PHONE_NUMBER;
typedef T_ATTRIBUTE_VALUE  S_T_USER_TYPE;
typedef T_ATTRIBUTE_VALUE  S_T_PRE_METHOD;
typedef T_ATTRIBUTE_VALUE  S_T_PRICISION;
typedef T_ATTRIBUTE_VALUE  S_T_CELL_ID;

//新增加
typedef long                T_QUOTA;//added by zhanglong for ocs 2006-6-19 15:07

//add by gaojf for 内容计费
typedef String<200> T_GPRS_SERVCODE;
typedef String<3>   T_GPRS_SERVTYPE;
typedef String<37>	T_GPRS_APNNI;
typedef String<10>	T_GPRS_APNGROUP;
typedef String<4>   T_GPRS_APNTYPE;
typedef String<64>  T_GPRS_APN;
typedef String<15>  T_GPRS_SGSN;

//分拣增加
typedef String<1>   T_TD_TYPE;        //TD类型
typedef String<1>   T_MATCH_TYPE;     //匹配类型
typedef String<10>  T_SERV_CODE_TYPE; //短信业务类型
typedef String<10>  T_SM_TYPESTAT;    //短信统计类型
typedef String<20>  T_SP_CODE;        //SP代码
typedef String<10>  T_OPER_CODE;      //业务代码
typedef String<1>   T_SP_TYPE;        //SP类型
typedef String<2>   T_CHARGE_TYPE;    //费用类型
typedef String<4>   T_EPARCHY_CODE;   //地市编码
typedef String<2>   T_SOURCE_TYPE;    //数据源
typedef String<4>   T_BRAND_CODE;     //品牌编码 如全球通、动感地带等
typedef String<1>   T_PREFIX_TYPE;    //前缀匹配类型


/////////////批价新增////////////////////////
typedef String<4>    T_MEMBER_TYPE;   //成员类型
typedef String<1>    T_RELATION_TYPE; //关联类型
typedef Integer      T_TPOBJ_ID;      //资费实例
typedef String<32>   T_TPPARAM_VALUE; //参数值
typedef Integer      T_LEVEL_TYPE;    //关联参数层次类型
typedef String<1>    T_TP_TYPE;       //套餐类型
typedef Number       T_SPECIALINFO_ID;//特殊信息标识定义
typedef String<32>   T_MEMBER_ID     ;//成员ID
typedef Integer      T_PARTITION_ID  ;//小区ID
typedef Integer      T_BILL_ID       ;//账单项类型
typedef String<50>   T_SYSPARAMCODE  ;//系统参数标识定义
typedef String<50>   T_SYSVALUE_STR  ;//字符参数类型
typedef String<10>   T_USERSTATE     ;//用户状态
                                                           
//typedef T_ATTRIBUTE_VALUE   T_OBJECT_ATTR   ;//对象属性 zhaogc add
typedef String<20> T_OBJECT_ATTR  ; 
typedef String<20>   T_STR_MIN    ;//字符串 zhaogc add
typedef String<50>   T_STR_MAX    ;//字符串 zhaogc add
typedef String<20>   T_STR_NUMBER    ;//字符串 zhaogc add

//定义用户、帐户、客户ID类型、用户群类型
enum ID_TYPE 
{
  USER_ID_TYPE  = '0', 
  ACCT_ID_TYPE  = '1', 
  CUST_ID_TYPE  = '2',
  GROUP_ID_TYPE = '3'
};

#endif



 
