//定义批价用到的一些常量

#ifndef _RATE_CONST_DEF_H_20080801_
#define _RATE_CONST_DEF_H_20080801_

#include "FieldsXX.h"
#include "cdr/AttrIdConstDef.h"
#include "BillingConstDef.h"

USING_NAMESPACE(std)

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
const string GRP_FILE_BILL      ("URG"); //用户群出帐文件
const string ACCT_FILE_BILL     ("URA"); //账户级出帐文件

//实扣月租计算方式
const char RENT_COMPMETHOD_NORMAL   = '0'; //全月
const char RENT_COMPMETHOD_HALF     = '1'; //半月
const char RENT_COMPMETHOD_OPENDATE = '2'; //入网日期
const char RENT_COMPMETHOD_TPDATE   = '3'; //套餐日期

//Added By Sunam 2009/10/6 19:47:09 非月累计量折算
const char RENT_COMPMETHOD_SUM2     = '4'; //套餐日期
//Added By Sunma 2009/11/10 10:44:28 按当月最早开机时间折算
const char RENT_COMPMETHOD_EARLIESTOPENDATE     = '5'; //套餐日期
const char RENT_COMPMETHOD_SPEACIALDATE         = '6'; //特殊日期

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
const char EXEC_METHOD_SUPEREXCLUDE     ('3')   ;//超级强制(一批免费的照强制)
//Added By Sunam 2009/9/2 9:56:25 
//第一个费用不同作用，任意一个资费，如果其结果与标准资费不同则以当前结果为准，后续资费不再处理
const char EXEC_METHOD_FIRSTEXCLUDE1    ('4')   ;
//Added By Sunam 2009/12/2 10:33:45
//第一个资费作用，任意一个资费，只要其条件满足则以当前结果为准，后续资费不再处理
const char EXEC_METHOD_FIRSTEXCLUDE2    ('5')   ;
//add by xuf 2010-1-30 10:38:12
const char EXEC_METHOD_ACCTPARTINHERIT  ('8')   ;//如资费挂在账户上，且作用角色为-1 ,表示此条事件资费为是为账户下USERID最小的用户继承（非往月停保）
const char EXEC_METHOD_GRPDISPATCH      ('9')   ;//如资费挂在虚拟用户上，且作用角色>0 ,表示此条事件资费为是费用分摊到指定角色的用户上

// 批价政策 比较方式
const char COMP_METHOD_FIELD            ('0')   ;//费用项比较
const char COMP_METHOD_CDR              ('1')   ;//整条话单费用比较
//批价政策进位方式
// 批价政策进位标记
const char CARRY_TAG_NODEAL             ('0')   ;//0:不处理（保持现状）
const char CARRY_TAG_DEL                ('1')   ;//1:舍去
const char CARRY_TAG_ADD                ('2')   ;//2:进位
const char CARRY_TAG_ROUND              ('3')   ;//3:四舍五入
//Added By Sunam 2009/9/8 20:00:53
const char CARRY_TAG_SPECIAL            ('4')   ;//4:特殊方式，固网前3分钟两次的计费方式
const char CARRY_TAG_DOUBLE             ('5')   ;//5:一分钟占用两分钟的累计时长，天津存在这样的需求

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

//////V网集团关系定义/////////////////////////////
const int  VPN_FLAG_OUT  = 0; //集团外 
const int  VPN_FLAG_SMALL= 1; //小集团内
const int  VPN_FLAG_BIG  = 2; //集团间

//////V网角色编码定义/////////////////////////////
const T_MEMBER_TYPE VPN_MEMTYPE_NULL      = "0000"; //非VPN成员
const T_MEMBER_TYPE VPN_MEMTYPE_QQMAIN    = "1001"; //亲情主卡
const T_MEMBER_TYPE VPN_MEMTYPE_QQSUB     = "1002"; //亲情付卡
const T_MEMBER_TYPE VPN_MEMTYPE_VPNINNET  = "2001"; //VPN普通成员
const T_MEMBER_TYPE VPN_MEMTYPE_VPNOUTNET = "2002"; //VPN重要成员
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
const T_MEMBER_TYPE ROLECODE_VIRTUAL  = "-2"; //"-2:虚拟用户本身使用，其他都不用"
const T_MEMBER_TYPE ROLECODE_WIDECARD = "-1"; //"-1:关联资费(虚拟用户下所有用户)"
const T_MEMBER_TYPE ROLECODE_SELF     = "0" ; //"0:普通资费(个人用户)，关联资费(虚拟用户)"
                                              //  ">0:具体角色编码"
const T_MEMBER_TYPE ROLECODE_ENUM    = "-3"; //"-3:角色为枚举类型 " 
const T_MEMBER_TYPE ROLECODE_ACCT    = "-4"; //"-4:个人用户参考账户下所有用户费用" add by xuf 2010-2-1 0:26:49 
const T_MEMBER_TYPE ROLECODE_GROUPALL    = "-5"; //个人用户参考群下总费用(关联用户id=0) add by dingr 2010-08-30
const T_MEMBER_TYPE ROLECODE_CONDID      = "-6";  //当COND_ID非0 或空时，此处角色填-6.	
const T_MEMBER_TYPE ROLECODE_ENUMCONDID      = "-7";  //当COND_ID非0 (或空)且角色为枚举类型 时	

//////资费参数关联类型定义//////////////////////
const T_RELATION_TYPE T_RELATYPE_NONE        = "0";  //普通参数，无关联
const T_RELATION_TYPE T_RELATYPE_SPECIALVPN  = "1";  //群关联，适用计费方在与群内号码通话优惠
const T_RELATION_TYPE T_RELATYPE_SPECIALPHONE= "2";  //特殊号码关联，适用计费方在与群特殊号码通话
const T_RELATION_TYPE T_RELATYPE_SPECIALCELL = "3";  //特殊区域关联，适用计费方在特殊区域内通话

///////////资费参数来源//////////////////////////
const char  PARAM_SOURCE_FIX  = '0';  ///固定值
const char  PARAM_SOURCE_CRM1 = '1';  ///CRM参数1，参数不充许缺失
//Added By Sunam 2009/9/11 17:15:12
const char  PARAM_SOURCE_PRM  = '2';  ///PRM参数
const char  PARAM_SOURCE_CRM2 = '3';  ///CRM参数2，参数充许缺失
//add by xuf 2009-9-27 13:39:18
const char  PARAM_SOURCE_USERITEM     = '4';//USER_ITEM参数，用户自身
const char  PARAM_SOURCE_USERITEM_VPN = '5';//USER_ITEM参数，关联虚拟用户
const char  PARAM_SOURCE_SERVPARAM    = '6';//SERVPARAM参数

//定义明细账单参数账单类型
const int   DBILL_TYPE_RATE      = 1;  //计费累计量
const int   DBILL_TYPE_ACCT      = 2;  //帐务累计量

//定义内存ROLLBACKBILL帐单类型
const int   BILL_TYPE_USER       = 0;  //用户普通帐单:系统级
const int   BILL_TYPE_SUM1       = 1;  //用户累计量1
const int   BILL_TYPE_SUM2       = 2;  //用户累计量2
const int   BILL_TYPE_ACCT       = 3;  //帐户帐单
const int   BILL_TYPE_RATEDISCNT = 4;  //计费优惠累计量
const int   BILL_TYPE_SUM3       = 5;  //用户累积量3
const int   BILL_TYPE_MONFEEBILL = 6;  //用户包月费账单

//const int   BILL_FIRSTCALL       = -1; //第一次开打时间账单项
//账单操作
const int   BILL_OPERATE_UPDATE  = 0;  //更新
const int   BILL_OPERATE_INSERT  = 1;  //新增
const int   BILL_OPERATE_DELETE  = 2;  //删除(采用更新为0)

//费用计算方法
const int FEE_COMP_METHOD_COMMON       = 0;  //按段计费，比如((时长+基准单元-1)/基准单元)*单元费率                        
const int FEE_COMP_METHOD_SENDMIN      = 1;  //赠送时长, 比如（免500分钟、100分钟内：0.2/分钟 100分钟外 0.1元/分钟）
const int FEE_COMP_METHOD_SENDFEE      = 2;  //赠送费用, 比如 (免100元):按作用前累计 
const int FEE_COMP_METHOD_FIXED        = 3;  //直接作用，比如固定费用                                               
const int FEE_COMP_METHOD_DISCOUNT     = 4;  //费用打折                                                             
const int FEE_COMP_METHOD_ADDRATE      = 5;  //增加费率                                                             
const int FEE_COMP_METHOD_DECRATE      = 6;  //减费率       
const int FEE_COMP_METHOD_SENDDATA     = 7;  //赠送流量
const int FEE_COMP_METHOD_FEELIMIT     = 8;  //费用封顶
const int FEE_COMP_METHOD_SENDFEE_SUF  = 9;  //赠送费用, 比如 (50元内0.2/分钟 ):按作用后累计(按0.2累计到50)
//Add by yangb For gd 1/1M
const int FEE_COMP_METHOD_TOTALDATA    = 10;  //按当月总流量进行计费

//Added By Sunam 2009/9/12 13:04:31
const int FEE_COMP_METHOD_TOTAL        = 11; //整条话单计费，如果每小时2元，则33分钟的费用=33/60*2.0元
//modify by rent new version begin
const int FEE_COMP_METHOD_DIRECT       = 1000; //固定费直接作用
const int FEE_COMP_METHOD_CMCCFORMULA  = 1001; //固定费移动公式x*12/365;
const int FEE_COMP_METHOD_CNUCFORMULA  = 1002; //固定费联通公式x/当月天数;
const int FEE_COMP_METHOD_LOOP         = 17; //循环累计计费
//modify by rent new version end

const T_SERVER_ID INVALID_SERVER_ID    = "-1";

const T_SERVER_ID ZERO_SERVER_ID       = "0";  //0用户

//add by xuf 2009-11-26 20:06:35
const T_SERVER_ID  GRPDISCNT_LOCK_ID  = "-2"; //帐务群优惠系统锁ID

//基本费、长途费免费标志
const string FREE_FEE                     ("1");
const string NOT_FREE_FEE                 ("0");

const int FEEFIELDS_MAX  = 6;

//免费政策
const int FREE_FEE_TPID[FEEFIELDS_MAX] = {0        , 88001000 , 88002000 ,
                                          88003000 , 88004000 , 88005000 } ;

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

//add by xuf 2009-8-20 21:41:56
const int CONDID_STATE_DISCNT_INVALID = 21000000; //使优惠失效的用户状态条件，如果TD_B_SYSPARAM中有配置，以配置为准，否则默认取此值。
const int CONDID_ISNEED_CALC_RENT     = 21000001; //无需计算固定费的用户条件，如果TD_B_SYSPARAM中有配置，以配置为准，否则默认取此值。
const int CONDID_ISNEED_CALC_BAL      = 21000002; //无需计算结余条件，如果TD_B_SYSPARAM中有配置，以配置为准，否则默认取此值。
const int CONDID_ISNEED_ACCT_DISCNT   = 21000003; //不参与账户级优惠的用户系统条件，如果TD_B_SYSPARAM中有配置，以配置为准，否则默认取此值。
const int CONDID_NOTNEED_ACCTDEAL     = 21000018; //不出帐用户条件ID
//add end 2009-8-20 21:42:01


//定义触发系统资费的事件类型
const T_FLAG   BASETP_BIZ_TYPE_U      = "1";  // 仅U文件触发的事件类型

//定义群优惠角色的枚举串长度
const int STR_ROLECODE_ENUMLEN = 50;

//定义资费级特使处理标记specDealTag
const int DEALTAG_DEFAULT_ZERO = 0;
const int DEALTAG_LOADDETAILBILL_NOBILL = 1; //无账单用户是参与账户优惠

const int USERIDSUNIONTAG_ALL = 1; //部分用户群优惠涉及全账单

#endif  //_RATE_CONST_DEF_H_20080801_
