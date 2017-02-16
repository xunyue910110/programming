//定义批价用到的一些常量

#ifndef _RATE_CONST_DEF_H_20080801_
#define _RATE_CONST_DEF_H_20080801_

#include "FieldsXX.h"
#include "cdr/AttrIdConstDef.h"
#include "BillingConstDef.h"


//帐期标记
const T_FLAG CYC_ADDTAG_NORMAL     = "0"; //正常时期
const T_FLAG CYC_ADDTAG_MONTHEND   = "1"; //月末优惠生效
const T_FLAG CYC_ADDTAG_CHECKSTART = "2"; //开始核帐
const T_FLAG CYC_ADDTAG_CHECKEND   = "3"; //核帐结束

//SP增值业务
const string SP_RATE_UNIT_RECORD   ("0"); //按条
const string SP_RATE_UNIT_TIME     ("1"); //按时长
const string SP_RATE_UNIT_QRT      ("2"); //按次数
const string SP_RATE_UNIT_DATA     ("3"); //按流量
const string SP_RATE_UNIT_CLRECORD ("4"); //按栏目条
const string SP_RATE_UNIT_CLTIME   ("5"); //按栏目时长
const string SP_RATE_UNIT_CLQRT    ("6"); //按栏目次数
const string SP_RATE_UNIT_CLDATA   ("7"); //按栏目流量 


//固定费文件前缀定义
const string RENTFEE_FILE_INCRE ("RI"); //固定费增量文件
const string RENTFEE_FILE_ALL   ("RA"); //固定费全量文件
const string USER_FILE_BILL     ("UR"); //用户出帐文件


//批价帐期运行模式
const int  CYCLEMODE_ONE    = 0; //单帐期 
const int  CYCLEMODE_DOUBLE = 1; //双帐期

const int  BILL_ID_RATEDISCNT = 18000; ///计费优惠ID值
const int  TRACK_INFO_MAXLEN  = 1024*10;
//定义普通月为周期的累计规则标识
const int  CYCLERULEID_COMMONMONTH = 0; //普通月周期标识

//定义一条话单时间允许跨的最大天数
const int  DAYSEGS_MAX = 10;
// 批价政策 执行方式
const char EXEC_METHOD_EXCLUDE          ('0')   ;//强制作用
const char EXEC_METHOD_ALL              ('1')   ;//共同作用
const char EXEC_METHOD_PARTEXCLUDE      ('2')   ;//部分强制(已经免费的不覆盖)
// 批价政策 比较方式
const char COMP_METHOD_FIELD            ('0')   ;//费用项比较
const char COMP_METHOD_CDR              ('1')   ;//整条话单费用比较
//批价政策进位方式
// 批价政策进位标记
const char CARRY_TAG_NODEAL             ('0')   ;//0:不处理（保持现状）
const char CARRY_TAG_DEL                ('1')   ;//1:舍去
const char CARRY_TAG_ADD                ('2')   ;//2:进位
const char CARRY_TAG_ROUND              ('3')   ;//3:四舍五入

// 批价政策 批次类型：一次批价 二次批价
const char RATE_TIMES_1                 ('1')   ; //一次批价
const char RATE_TIMES_2                 ('2')   ; //二次批价

/////定义套餐类型//////////////////////////////
const T_TP_TYPE  TP_TYPE_NORMAL = "0";  //普通套餐
const T_TP_TYPE  TP_TYPE_CELL   = "1";  //小区套餐
const T_TP_TYPE  TP_TYPE_VPN    = "2";  //VPN套餐

/////定义特殊信息缺省标识//////////////////////
const T_SPECIALINFO_ID  NULL_SPECAILINFO_ID(0,0);
const T_SERVER_ID       NULL_VPN_ID         = "0";

//////特殊信息组织层次:特殊信息类型///////////
const T_MEMBER_TYPE SPECIALINFO_TYPE = "0"; //特殊号码
const T_MEMBER_TYPE SPECIALINFO_CELL = "1"; //小区
/////关联参数层次类型，对应表特殊信息组织层次////////////
const T_LEVEL_TYPE  LEVELTYPE_NULL  = 0; //小区：不在指定的小区内
const T_LEVEL_TYPE  LEVELTYPE_CELL_NORMAL  = 1000; //小区：不区分
const T_LEVEL_TYPE  LEVELTYPE_CELL_COUNTY  = 1001; //小区：县
const T_LEVEL_TYPE  LEVELTYPE_CELL_TOWN    = 1002; //小区：乡镇
const T_LEVEL_TYPE  LEVELTYPE_CELL_VILLAGE = 1003; //小区：村


//////特殊号码组构成表:成员类型///////////////
const T_MEMBER_TYPE SPECIALGRP_MEMTYPE_VPN   = "0";  //V网
const T_MEMBER_TYPE SPECIALGRP_MEMTYPE_NUMBER= "1";  //特殊号码

//////V网角色编码定义/////////////////////////////
const T_MEMBER_TYPE VPN_MEMTYPE_NULL      = "0000"; //非VPN成员
const T_MEMBER_TYPE VPN_MEMTYPE_QQMAIN    = "1001"; //亲情主卡
const T_MEMBER_TYPE VPN_MEMTYPE_QQSUB     = "1002"; //亲情付卡
const T_MEMBER_TYPE VPN_MEMTYPE_VPNINNET  = "2001"; //VPN网内
const T_MEMBER_TYPE VPN_MEMTYPE_VPNOUTNET = "2002"; //VPN网外
const T_MEMBER_TYPE VPN_MEMTYPE_EFIX      = "3001"; //E家固话
const T_MEMBER_TYPE VPN_MEMTYPE_ENET      = "3002"; //E家宽带
const T_MEMBER_TYPE VPN_MEMTYPE_EHBS      = "3003"; //E家小灵通
const T_MEMBER_TYPE VPN_MEMTYPE_ECDMA     = "3004"; //E家CDMA

//////V网ID类型定义//////////////////////////////
const T_MEMBER_TYPE VPN_MEMIDTYPE_USER   = "0"; //用户
const T_MEMBER_TYPE VPN_MEMIDTYPE_CUST   = "1"; //客户
const T_MEMBER_TYPE VPN_MEMIDTYPE_ACCT   = "2"; //帐户
const T_MEMBER_TYPE VPN_MEMIDTYPE_OUTNET = "3"; //网外

//作用角色定义:
const T_MEMBER_TYPE ROLECODE_WIDECARD = "-1"; //"-1:关联资费(虚拟用户下所有用户)"
const T_MEMBER_TYPE ROLECODE_SELF     = "0" ; //"0:普通资费(个人用户)，关联资费(虚拟用户)"
                                              //  ">0:具体角色编码"

//////资费参数关联类型定义//////////////////////
const T_RELATION_TYPE T_RELATYPE_NONE        = "0";  //普通参数，无关联
const T_RELATION_TYPE T_RELATYPE_SPECIALVPN  = "1";  //群关联，适用计费方在与群内号码通话优惠
const T_RELATION_TYPE T_RELATYPE_SPECIALPHONE= "2";  //特殊号码关联，适用计费方在与群特殊号码通话
const T_RELATION_TYPE T_RELATYPE_SPECIALCELL = "3";  //特殊区域关联，适用计费方在特殊区域内通话

///////////资费参数来源//////////////////////////
const char  PARAM_SOURCE_FIX = '0';  ///固定值
const char  PARAM_SOURCE_CRM = '1';  ///CRM参数

//定义明细账单参数账单类型
const int   DBILL_TYPE_RATE      = 1;  //计费累计量
const int   DBILL_TYPE_ACCT      = 2;  //帐务累计量

//定义内存ROLLBACKBILL帐单类型
const int   BILL_TYPE_USER       = 0;  //用户普通帐单:系统级
const int   BILL_TYPE_SUM1       = 1;  //用户累计量1
const int   BILL_TYPE_SUM2       = 2;  //用户累计量2
const int   BILL_TYPE_ACCT       = 3;  //帐户帐单
const int   BILL_TYPE_RATEDISCNT = 4;  //计费优惠累计量

//const int   BILL_FIRSTCALL       = -1; //第一次开打时间账单项
//账单操作
const int   BILL_OPERATE_UPDATE  = 0;  //更新
const int   BILL_OPERATE_INSERT  = 1;  //新增
const int   BILL_OPERATE_DELETE  = 2;  //删除(采用更新为0)

//定义用户、帐户、客户ID类型、用户群类型
enum ID_TYPE 
{
  USER_ID_TYPE  = '0', 
  ACCT_ID_TYPE  = '1', 
  CUST_ID_TYPE  = '2',
  GROUP_ID_TYPE = '3'
};

//费用计算方法
const int FEE_COMP_METHOD_COMMON       = 0;//按段计费，比如((时长+基准单元-1)/基准单元)*单元费率                        
const int FEE_COMP_METHOD_SENDMIN      = 1;//赠送时长, 比如（免500分钟、100分钟内：0.2/分钟 100分钟外 0.1元/分钟）
const int FEE_COMP_METHOD_SENDFEE      = 2;//赠送费用, 比如 (免100元)                                             
const int FEE_COMP_METHOD_FIXED        = 3;//直接作用，比如固定费用                                               
const int FEE_COMP_METHOD_DISCOUNT     = 4;//费用打折                                                             
const int FEE_COMP_METHOD_ADDRATE      = 5;//增加费率                                                             
const int FEE_COMP_METHOD_DECRATE      = 6;//减费率       
const int FEE_COMP_METHOD_SENDDATA     = 7;//赠送流量                                                        
const int FEE_COMP_METHOD_DIRECT       = 11; //固定费直接作用
const int FEE_COMP_METHOD_CMCCFORMULA  = 12; //固定费移动公式x*12/365;
const int FEE_COMP_METHOD_CNUCFORMULA  = 13; //固定费联通公式x/当月天数;


const T_SERVER_ID INVALID_SERVER_ID    = "0";


//基本费、长途费免费标志
const string FREE_FEE                     ("1");
const string NOT_FREE_FEE                 ("0");

const int FEEFIELDS_MAX  = 6;

//免费政策
const int FREE_FEE_TPID[FEEFIELDS_MAX] = {0        , 10000000 , 22000000 ,
                                          20000000 , 23000000 , 21000000 } ;

//定义缺省费率单元
const int  DEFAULT_CFEE_UNIT              = 60;
const int  DEFAULT_LFEE_UNIT              = 6;
const int  DEFAULT_IPFEE_UNIT             = 60;
const int  DEFAULT_DATA_UNIT              = 1024;


//定义通道类型
const string CHANNEL_PROV                 ("P")   ;//省内
const string CHANNEL_NOINFO               ("U")   ;//无主
const string CHANNEL_INTERPROV            ("N")   ;//国内来访
const string CHANNEL_INTERNATIOANAL       ("I")   ;//国际来访
const string CHANNEL_SCP                  ("S")   ;//智能网

#endif  //_RATE_CONST_DEF_H_20080801_
