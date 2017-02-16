/**
*   话单结构中的属性下标常量定义.
*   @author 高将飞 <gaojf@lianchuang.com>
*   @version 1.0
*   @date 2008-7-31
*/
#ifndef __ATTRIDCONST_DEF_H_20080731_
#define __ATTRIDCONST_DEF_H_20080731_


//属性列表分配：
//费用结果：[ATTRI_FEE_MIN,ATTRI_FEEID_MAX)
//超长字段编号:[ATTRI_SPLONG_MIN,ATTRI_SPLONG_MAX]
//普通属性：(ATTRI_COMMON,ATTRI_COMMON_E]
//原始字段信息：[ATTRI_ORG_MIN,ATTRI_ORG_MAX],
//        实际存放在输入缓冲区中
//帐单总量：(ATTRI_ORG_MAX,...)
const int ATTRI_MAX       = 1000;
const int MAX_ITEM_COUNT  = ATTRI_MAX + 1;
const int ATTRI_FEE_MIN   = 0;
const int ATTRI_FEEID_MAX = 50;
const int ATTRI_SPLONG_MIN= 91;
const int ATTRI_SPLONG_MAX= 100;
const int ATTRI_COMMON    = 100;
const int ATTRI_COMMON_E  = 800;
const int ATTRI_ORG_MIN   = 801;
const int ATTRI_ORG_MAX   = 1000;


////////////////////////////////////////////////////////////////////////
//超长属性
const int ATTRI_SPLONG_FREETRACK = ATTRI_SPLONG_MIN + 1; //优惠轨迹
const int ATTRI_SPLONG_BILLLIST  = ATTRI_SPLONG_MIN + 2; //明细账单列表
const int ATTRI_GPRS_RCDEXTS     = ATTRI_SPLONG_MIN + 3; //GPRS内容计费字段
const int ATTRI_SCP_BILLINFO     = ATTRI_SPLONG_MIN + 4; //智能网帐单字段
//------------------------------------------------------------------------------
//公共的属性
//所有类型话单都要有的属性

const int ATTRI_FILE_NAME                = ATTRI_COMMON + 1 ;
const int ATTRI_DEAL_TIME                = ATTRI_COMMON + 2 ;
const int ATTRI_BIZ_TYPE                 = ATTRI_COMMON + 3 ;
const int ATTRI_SOURCE_TYPE              = ATTRI_COMMON + 4 ;//话单来源
const int ATTRI_FILE_TYPE                = ATTRI_COMMON + 5 ;
const int ATTRI_CYCLE_DATE               = ATTRI_COMMON + 6 ;
const int ATTRI_BEGIN_DATE               = ATTRI_COMMON + 7 ;
const int ATTRI_BEGIN_TIME               = ATTRI_COMMON + 8 ;
const int ATTRI_DURATION                 = ATTRI_COMMON + 9 ;
const int ATTRI_END_DATE                 = ATTRI_COMMON + 10;
const int ATTRI_END_TIME                 = ATTRI_COMMON + 11;
const int ATTRI_MSISDN_A                 = ATTRI_COMMON + 12;//a端号码
const int ATTRI_CDR_CYCLE_DATE           = ATTRI_COMMON + 13;
const int ATTRI_USER_ASP_EXTB            = ATTRI_COMMON + 14;
const int ATTRI_FILE_NO                  = ATTRI_COMMON + 17;
const int ATTRI_ERROR_CODE               = ATTRI_COMMON + 18;//话单错误码，0为非错单
const int ATTRI_IMSI_NUMBER              = ATTRI_COMMON + 19;//imsi号
const int ATTRI_CUST_ID                  = ATTRI_COMMON + 20;
const int ATTRI_USER_ID                  = ATTRI_COMMON + 21;
const int ATTRI_USER_SERVICE_TYPE_A_YZ   = ATTRI_COMMON + 22;
const int ATTRI_PRODUCT_ID_A             = ATTRI_COMMON + 23;
const int ATTRI_PRODUCT_ID_A_ORG         = ATTRI_COMMON + 24;
const int ATTRI_CHANNEL_NO               = ATTRI_COMMON + 25;
const int ATTRI_OFFICE_CODE              = ATTRI_COMMON + 26;
const int ATTRI_PRODUCT_ID_B             = ATTRI_COMMON + 27;
const int ATTRI_USER_BRAND_TYPE_B        = ATTRI_COMMON + 28;
const int ATTRI_OFFICE_CODE_YZ           = ATTRI_COMMON + 29;
const int ATTRI_CALL_TYPE                = ATTRI_COMMON + 30;//FF为trash话单，预处理置
const int ATTRI_ROAM_TYPE                = ATTRI_COMMON + 31;
const int ATTRI_DURATION_ORG             = ATTRI_COMMON + 32;
const int ATTRI_ENDDATE_ORG              = ATTRI_COMMON + 33;
const int ATTRI_ENDTIME_ORG              = ATTRI_COMMON + 34;
const int ATTRI_MSISDN_A_ORG             = ATTRI_COMMON + 35;//a端号码
const int ATTRI_MSISDN_B                 = ATTRI_COMMON + 36 ;//对端号码
const int ATTRI_MSISDN_B_ORG             = ATTRI_COMMON + 37 ;//对端号码，3个号码最先有org号码
const int ATTRI_MSISDN_B_FOR_ROAM        = ATTRI_COMMON + 38 ;//对端号码
const int ATTRI_HOME_CODE_A              = ATTRI_COMMON + 39 ;
const int ATTRI_VISIT_CODE_A             = ATTRI_COMMON + 40 ;
const int ATTRI_VISIT_CODE_A_ORG         = ATTRI_COMMON + 41 ;
const int ATTRI_HOME_CODE_B              = ATTRI_COMMON + 42 ;
const int ATTRI_VISIT_CODE_B             = ATTRI_COMMON + 43 ;
const int ATTRI_USER_TYPE_A              = ATTRI_COMMON + 44 ;
const int ATTRI_USER_ASP_A               = ATTRI_COMMON + 45 ;//a端号码所属运营商
const int ATTRI_USER_SERVICE_TYPE_A      = ATTRI_COMMON + 46 ;
const int ATTRI_USER_HOME_TYPE_A         = ATTRI_COMMON + 47 ;//a号码归属地区的类型
const int ATTRI_USER_TYPE_B              = ATTRI_COMMON + 48 ;
const int ATTRI_USER_ASP_B               = ATTRI_COMMON + 49 ;
const int ATTRI_USER_SERVICE_TYPE_B      = ATTRI_COMMON + 50 ;
const int ATTRI_USER_HOME_TYPE_B         = ATTRI_COMMON + 51 ;
const int ATTRI_IMEI                     = ATTRI_COMMON + 52 ;
const int ATTRI_RESULT                   = ATTRI_COMMON + 53 ;
const int ATTRI_LAC_A                    = ATTRI_COMMON + 54 ;
const int ATTRI_CELLID_A                 = ATTRI_COMMON + 55 ;
const int ATTRI_SP_CODE                  = ATTRI_COMMON + 56 ;
const int ATTRI_SERVICE_CODE             = ATTRI_COMMON + 57 ;
const int ATTRI_OPER_CODE                = ATTRI_COMMON + 58 ;
const int ATTRI_CHARGE_TYPE              = ATTRI_COMMON + 59 ;

const int ATTRI_NEED_BILL                = ATTRI_COMMON + 60 ;  
const int ATTRI_RATE_TIMES               = ATTRI_COMMON + 61 ;//批价次数，话单每重批一次加1，原始值1
const int ATTRI_HALF_INNET_FLAG          = ATTRI_COMMON + 62 ;//套餐后半月生效标志
const int ATTRI_CURRENT_INNET_FLAG       = ATTRI_COMMON + 63 ;//套餐首月申请标识
const int ATTRI_TPVPN_FLAGA              = ATTRI_COMMON + 64 ;//政策级计费方VPN角色类型
const int ATTRI_TPVPN_FLAGB              = ATTRI_COMMON + 65 ;//政策级对方VPN角色类型
const int ATTRI_TPCELL_FLAG              = ATTRI_COMMON + 66 ;//政策级CELL_FLAG
const int ATTRI_TPRELA_LEVELTYPE         = ATTRI_COMMON + 67 ;//政策级资费关联层次类型

const int ATTRI_NEED_BASETP              = ATTRI_COMMON + 68 ;//总费用免费标记
const int ATTRI_NEED_USERTP              = ATTRI_COMMON + 69 ;//总费用免费标记
const int ATTRI_BASETP_FACTORS           = ATTRI_COMMON + 70 ;//基本政策决定因素
const int RATE_ATTRI_FEEID_FACTORS       = ATTRI_COMMON + 71 ;//批价明细账单项决定因素
const int ACCT_ATTRI_FEEID_FACTORS       = ATTRI_COMMON + 72 ;//帐务明细账单项决定因素
const int ATTRI_CDR_CYCLE_ORG            = ATTRI_COMMON + 73 ;//根据话单时间计算出来的帐期

const int ATTRI_OPEN_DATE                = ATTRI_COMMON + 75 ;//用户入网时间从清单中获得
const int ATTRI_HALF_OPEN_FLAG           = ATTRI_COMMON + 76 ;//用户是否半月入网
const int ATTRI_CURRENT_OPEN_FLAG        = ATTRI_COMMON + 77 ;//用户是否首月入网

const int ATTRI_RR_FLAG                  = ATTRI_COMMON + 78 ;//是否过排重
const int ATTRI_RR_FID                   = ATTRI_COMMON + 79 ;//排重过滤信息
const int ATTRI_SEND_FILENAME            = ATTRI_COMMON + 80 ;//上发文件名
const int ATTRI_MNS_TYPE                 = ATTRI_COMMON + 81 ;//无线侧网络标识 0：2G无线侧网络 1：TD无线侧网络
const int ATTRI_BEGIN_DATETIME           = ATTRI_COMMON + 82 ;//通话开始日期时间YYYYMMDDHHMISS
const int ATTRI_BEGIN_WEEK               = ATTRI_COMMON + 83 ;//通话开始日期所在的星期几
const int ATTRI_VPN_FLAG_A               = ATTRI_COMMON + 84 ;//计费方VPN角色
const int ATTRI_TPVPN_FLAG               = ATTRI_COMMON + 85 ;//AB集团关系(0 集团外 ,1小集团 ,2 集团间)
//const int ATTRI_TPVPN_LAST_PARENT_ID_A   = ATTRI_COMMON + 86 ;//计费号码的直接母集团ID
//const int ATTRI_TPVPN_LAST_PARENT_ID_B   = ATTRI_COMMON + 87 ;//对方号码的直接母集团ID
//const int ATTRI_RATE_FLAG                = ATTRI_COMMON + 90 ;//Added by yangrl 2009-10-26 17:51:30  判断话单是否需要计费
//const int ATTRI_EARLIEST_OPEN_DATE       = ATTRI_COMMON + 91 ;//Added by Suanm 2009/11/9 19:59:11 用户当月最早开机时间
//Modify by yangb For sx At 2009/12/25 21:06:47
const int ATTRI_RATE_FLAG                = ATTRI_COMMON + 86 ;//Added by yangrl 2009-10-26 17:51:30  判断话单是否需要计费
const int ATTRI_EARLIEST_OPEN_DATE       = ATTRI_COMMON + 87 ;//Added by Suanm 2009/11/9 19:59:11 用户当月最早开机时间
const int ATTRI_TPVPN_LAST_PARENT_ID_A   = ATTRI_COMMON + 90 ;//计费号码的直接母集团ID
const int ATTRI_TPVPN_LAST_PARENT_ID_B   = ATTRI_COMMON + 91 ;//对方号码的直接母集团ID
const int ATTRI_RECORD_TYPE              = ATTRI_COMMON + 88 ;//Added By Sunam 2009/9/24 13:22:03ATTRI_RECORD_TYPE
const int ATTRI_FIXVOICE_COMPATTR        = ATTRI_COMMON + 89 ;//Added By Sunam 2009/9/25 17:08:28固网语单组合属性 in_cell|isnoneed_tp|roam_type|speicial_phone_group
const int ATTRI_FIXSM_SHIPATTR           = ATTRI_COMMON + 92 ;//add by  xiangbin 短信业务用户判断用户是否有亲情资费  at 2009-11-7 16:43:50
const int ATTRI_SERVICE_KEY              = ATTRI_COMMON + 93 ;//add by xiangbin 业务键用于代收判断 at 2009-11-7 16:54:32
const int ATTRI_NET_CODE                 = ATTRI_COMMON + 94 ;//add by xiangbin 网关判断用户短信SP业务 at 2009-11-7 16:57:41
const int ATTRI_PRODUCT_SERV_TYPE        = ATTRI_COMMON + 95 ;//Added by yangyi 2009/11/16  用户产品ID类型
//Add by yangb For 山西长途话单类型 At 2009/12/31 15:08:27
const int ATTRI_LRECORD_TYPE             = ATTRI_COMMON + 96 ;
const int ATTRI_BRECORD_TYPE             = ATTRI_COMMON + 97 ;//内部使用的市话话单类型
const int ATTRI_THIRD_HOME_CODE          = ATTRI_COMMON + 98 ;//山西求取第三方号码的归属地，报表用
const int ATTRI_THIRD_PARTY_GSM          = ATTRI_COMMON + 99 ;//山西求取第三方号码，报表用
const int ATTRI_THREE_FLAG               = ATTRI_COMMON + 100 ;//add by dph 2009-12-28 三方通话标识

const int ATTRI_COMMON2             = 700;
const int ATTRI_SEND_STATUS         = ATTRI_COMMON2 + 1  ;
const int ATTRI_LFEE_DESC           = ATTRI_COMMON2 + 2 ;//长途费描述 C网
const int ATTRI_SPEC_SERVICE        = ATTRI_COMMON2 + 3 ;//特殊服务使用说明 C网               
const int ATTRI_SP_PROV_CODE        = ATTRI_COMMON2 + 4 ;
const int ATTRI_SP_SERV_AREA        = ATTRI_COMMON2 + 5 ;
const int ATTRI_HOME_COUNTRY        = ATTRI_COMMON2 + 6 ;//A归属市县编码
const int ATTRI_VISIT_COUNTRY       = ATTRI_COMMON2 + 7 ;//A到访市县编码
const int ATTRI_HOME_COUNTRY_B      = ATTRI_COMMON2 + 8 ;//B归属市县编码
const int ATTRI_VISIT_COUNTRY_B     = ATTRI_COMMON2 + 9 ;//B到访市县编码
const int ATTRI_TD_TYPE             = ATTRI_COMMON2 + 10 ;//TD号段标识 0：普通网络 1：TD网络
const int ATTRI_PROV_CHANNEL        = ATTRI_COMMON2 + 11 ;//国内区号分组 用于漫游来访分组
const int ATTRI_ERROR_FLAG          = ATTRI_COMMON2 + 12 ;//错单标识 0：正常 1：error_code不为0
const int ATTRI_TRASH_FLAG          = ATTRI_COMMON2 + 13 ;//错单标识 0：正常 1：trash
const int ATTRI_NATIONAL_CODE_B     = ATTRI_COMMON2 + 14 ;//B国家代码
const int ATTRI_CDR_CHANNEL         = ATTRI_COMMON2 + 15 ; //不同话单相同内容通道名
const int ATTRI_MUX_CHANNEL         = ATTRI_COMMON2 + 16 ; //不同话单相同内容通道名
const int ATTRI_BACK_CHANNEL        = ATTRI_COMMON2 + 17 ; //备份通道
const int ATTRI_SCP_USERTYPE        = ATTRI_COMMON2 + 18 ; //语音中SCP话单属性 0-普通用户1-PPC 2-WVPN 3-IVPN 4-PPS
const int ATTRI_CITY_CHANNEL        = ATTRI_COMMON2 + 19 ;//省内区号分组 将省内用户按地市分组
const int ATTRI_SUM_SERVTYPE        = ATTRI_COMMON2 + 20 ; //用于合帐的业务类型
const int ATTRI_MONTH_ENDTAG        = ATTRI_COMMON2 + 21 ; //月末优惠有效标记
const int ATTRI_AVISIT_CODE         = ATTRI_COMMON2 + 22 ; //预处理置 国内来访为城市代码，国际来访为IMSI号头
const int ATTRI_ABSAMEHOME_FLAG     = ATTRI_COMMON2 + 23 ; //主被叫同归属Y,不同归属N
const int ATTRI_NEED_OUTPUTCDR      = ATTRI_COMMON2 + 24 ; //是否需要输出对应的清单
const int ATTRI_ACCT_DATE           = ATTRI_COMMON2 + 25 ; //帐务账单日期
const int ATTRI_INTERPROV_VPNFLAG   = ATTRI_COMMON2 + 26 ; //跨省集团标记
const int ATTRI_SUM_SIGNFLAG        = ATTRI_COMMON2 + 27 ; //合帐正负标记
const int ATTRI_NET_TYPE_CODE       = ATTRI_COMMON2 + 28;  //网别编码
const int ATTRI_DEALMSISDN_FLAG     = ATTRI_COMMON2 + 29;  //对方号码是否需要处理标记
const int ATTRI_EPARCHY_CODE        = ATTRI_COMMON2 + 30;  //地市编码 --BF
const int ATTRI_PREPAY_FLAG         = ATTRI_COMMON2 + 31;  //是否预付费标志 0:预付费 1:后付费   --BF
const int ATTRI_USER_ID_B           = ATTRI_COMMON2 + 32;  //对端号码用户标识                   --BF
const int ATTRI_USER_GROUP_A        = ATTRI_COMMON2 + 33;  //add by yangyi 2009,1125 求取客户群
const int ATTRI_USER_GROUP_B        = ATTRI_COMMON2 + 34;  //add by yangyi 2009,1125 求取客户群
const int ATTRI_AB_USER_GROUP_FLAG  = ATTRI_COMMON2 + 35;  //add by yangyi 2009,1125 求取客户群 
//------------------------------------------------------------------------------
// 费用相关的属性。

const int ATTRI_FEE_FIELD               = 0;

//费用项 DISC为实收费用项 
const int ATTRI_DISCFEE_FIELD1           = ATTRI_FEE_FIELD + 1   ;
const int ATTRI_DISCFEE_FIELD2           = ATTRI_FEE_FIELD + 2   ;
const int ATTRI_DISCFEE_FIELD3           = ATTRI_FEE_FIELD + 3   ;
const int ATTRI_DISCFEE_FIELD4           = ATTRI_FEE_FIELD + 4   ;
const int ATTRI_DISCFEE_FIELD5           = ATTRI_FEE_FIELD + 5   ;
//一次批价费用
const int ATTRI_FEE_FIELD1               = ATTRI_FEE_FIELD + 6   ;
const int ATTRI_FEE_FIELD2               = ATTRI_FEE_FIELD + 7   ;
const int ATTRI_FEE_FIELD3               = ATTRI_FEE_FIELD + 8   ;
const int ATTRI_FEE_FIELD4               = ATTRI_FEE_FIELD + 9   ;
const int ATTRI_FEE_FIELD5               = ATTRI_FEE_FIELD + 10  ;
//ORG为原始费用项
const int ATTRI_ORGFEE_FIELD1            = ATTRI_FEE_FIELD + 11  ;
const int ATTRI_ORGFEE_FIELD2            = ATTRI_FEE_FIELD + 12  ;
const int ATTRI_ORGFEE_FIELD3            = ATTRI_FEE_FIELD + 13  ;
const int ATTRI_ORGFEE_FIELD4            = ATTRI_FEE_FIELD + 14  ;
const int ATTRI_ORGFEE_FIELD5            = ATTRI_FEE_FIELD + 15  ;
const int ATTRI_TARIFF_FEE               = ATTRI_FEE_FIELD + 16  ;
//语音 (5项费用)
const int ATTRI_FEE_ALL                  = ATTRI_FEE_FIELD + 17  ;
const int ATTRI_DISC_ALL                 = ATTRI_FEE_FIELD + 18  ;

const int ATTRI_CFEE_TIMES               = ATTRI_FEE_FIELD + 19  ;
const int ATTRI_LFEE_TIMES               = ATTRI_FEE_FIELD + 20  ;

const int ATTRI_FEEUNIT_1                = ATTRI_FEE_FIELD + 21  ;
const int ATTRI_FEEUNIT_2                = ATTRI_FEE_FIELD + 22  ;
const int ATTRI_FEEUNIT_3                = ATTRI_FEE_FIELD + 23  ;
const int ATTRI_FEEUNIT_4                = ATTRI_FEE_FIELD + 24  ;
const int ATTRI_FEEUNIT_5                = ATTRI_FEE_FIELD + 25  ;

//是否免费标记
const int ATTRI_FREE_FEE1                = ATTRI_FEE_FIELD + 30  ;
const int ATTRI_FREE_FEE2                = ATTRI_FEE_FIELD + 31  ;
const int ATTRI_FREE_FEE3                = ATTRI_FEE_FIELD + 32  ;
const int ATTRI_FREE_FEE4                = ATTRI_FEE_FIELD + 33  ;
const int ATTRI_FREE_FEE5                = ATTRI_FEE_FIELD + 34  ;
const int ATTRI_FREE_FEE                 = ATTRI_FEE_FIELD + 35  ;
//是否有原始费用标识
const int ATTRI_ORIGINFLAG_FEE1          = ATTRI_FEE_FIELD + 36  ;
const int ATTRI_ORIGINFLAG_FEE2          = ATTRI_FEE_FIELD + 37  ;
const int ATTRI_ORIGINFLAG_FEE3          = ATTRI_FEE_FIELD + 38  ;
const int ATTRI_ORIGINFLAG_FEE4          = ATTRI_FEE_FIELD + 39  ;
const int ATTRI_ORIGINFLAG_FEE5          = ATTRI_FEE_FIELD + 40  ;
const int ATTRI_RATE_DISC_FEE            = ATTRI_FEE_FIELD + 41  ;//总共优惠了的费用
const int ATTRI_FREE_TIME_ALL            = ATTRI_FEE_FIELD + 42  ;//总共的免费分钟数

//------------------------------------------------------------------------------
// 1.GSM语音话单类属性定义:201-400
const int ATTRI_GSM_BASE                = 200   ;

// 1.GSM业务类属性定义
const int ATTRI_LONG_TYPE1          = ATTRI_GSM_BASE + 1  ;
const int ATTRI_LONG_TYPE2          = ATTRI_GSM_BASE + 2  ;
const int ATTRI_LONG_GROUP1         = ATTRI_GSM_BASE + 3  ;
const int ATTRI_LONG_GROUP2         = ATTRI_GSM_BASE + 4  ;
const int ATTRI_USER_TYPE_C         = ATTRI_GSM_BASE + 5  ;
const int ATTRI_USER_ASP_C          = ATTRI_GSM_BASE + 6  ;
const int ATTRI_USER_HOME_TYPE_C    = ATTRI_GSM_BASE + 7  ;
const int ATTRI_USER_SERVICE_TYPE_C = ATTRI_GSM_BASE + 8  ;
const int ATTRI_VPN_FLAG            = ATTRI_GSM_BASE + 9  ;  //对方VPN类型(话单级)
const int ATTRI_CELL_FLAG           = ATTRI_GSM_BASE + 10 ;  //CELL_FLAG(话单级)
const int ATTRI_EDGE_FLAG           = ATTRI_GSM_BASE + 11 ;
const int ATTRI_CARRIER_TYPE        = ATTRI_GSM_BASE + 12 ;
const int ATTRI_UNICOM_TRANS        = ATTRI_GSM_BASE + 13 ;
const int ATTRI_TRAN_TYPE_B         = ATTRI_GSM_BASE + 14 ;
const int ATTRI_SPECIAL_PHONE_GROUP = ATTRI_GSM_BASE + 15 ;
const int ATTRI_MSC                 = ATTRI_GSM_BASE + 16 ;
const int ATTRI_TRUNKGROUPOUT       = ATTRI_GSM_BASE + 17 ;
const int ATTRI_TRUNKGROUPIN        = ATTRI_GSM_BASE + 18 ;
const int ATTRI_GSM_SERVICE_CODE    = ATTRI_GSM_BASE + 19 ;
const int ATTRI_SERVICE_TYPE        = ATTRI_GSM_BASE + 20 ;
const int ATTRI_MSRN                = ATTRI_GSM_BASE + 21 ;
const int ATTRI_LAC_B               = ATTRI_GSM_BASE + 22 ;
const int ATTRI_CELLID_B            = ATTRI_GSM_BASE + 23 ;
const int ATTRI_FORWARD_CAUSE       = ATTRI_GSM_BASE + 24 ;
const int ATTRI_TERM_CAUSE          = ATTRI_GSM_BASE + 25 ;
const int ATTRI_ORG_PHONE           = ATTRI_GSM_BASE + 26 ;
const int ATTRI_RATE_INDICATE       = ATTRI_GSM_BASE + 27 ;
const int ATTRI_USER_STATE          = ATTRI_GSM_BASE + 28 ;
const int ATTRI_USER_STATE_GROUP    = ATTRI_GSM_BASE + 29 ;

const int ATTRI_PROVINCE_CODE       = ATTRI_GSM_BASE + 30 ; //计费方归属省代码
const int ATTRI_PROVINCE_CODE_B     = ATTRI_GSM_BASE + 31 ; //对方归属省代码
const int ATTRI_CALLED_TYPE_B       = ATTRI_GSM_BASE + 32 ;
const int ATTRI_VPN_ID              = ATTRI_GSM_BASE + 34 ;  //VPNID
const int ATTRI_DIAL_TYPE           = ATTRI_GSM_BASE + 35 ;  //拔号类型
const int ATTRI_ISNONEED_TP         = ATTRI_GSM_BASE + 36 ;  //特殊号码表里isNoNeedTp，判断对于某个特殊的号码是否需要做用户政策
const int ATTRI_OTHERGROUP          = ATTRI_GSM_BASE + 37 ;
const int ATTRI_SHARE_TARIFF        = ATTRI_GSM_BASE + 38 ;  //是否参与套餐优惠
//Added by Sunam 2005-12-29 A用户的other_group
const int ATTRI_VPNOTHERGROUP_A     = ATTRI_GSM_BASE + 39 ;
//Added by Sunam 2006-2-20 13:53 增优惠基站的分组
const int ATTRI_SUB_CELLGROUP       = ATTRI_GSM_BASE + 40 ;
//Added by Sunam 2006-2-28 17:09 增加AspCode
const int ATTRI_INTER_ASPCODE       = ATTRI_GSM_BASE + 41 ;
//Added by Sunam 2006-5-26 16:17 增加AspGroup 在表td_imsii中
const int ATTRI_INTER_ASPGROUP      = ATTRI_GSM_BASE + 42 ;
//Added by liuyh 2006-9-10 14:17 增加CfeeAdd
const int ATTRI_CFEE_ADD_FLAG		= ATTRI_GSM_BASE + 43 ;

//Added by Sunam 2006-10-11 9:36
//闭合用户群标志
const int ATTRI_CLOSEDUSERGROUP_FLAG  = ATTRI_GSM_BASE + 44 ;
//网外号码组标志
const int ATTRI_OUTNETPHONEGROUP_FLAG = ATTRI_GSM_BASE + 45 ;

//Added by Sunam 2007-7-4 14:11
const int ATTRI_LONG_GROUP_EX1	= ATTRI_GSM_BASE + 46 ;
const int ATTRI_LONG_GROUP_EX2	= ATTRI_GSM_BASE + 47 ;

const int ATTRI_DOUBLEMODE      = ATTRI_GSM_BASE + 48 ; //双模标记
const int ATTRI_OFFICE_CODE_B   = ATTRI_GSM_BASE + 49; //对方市县代码

const int ATTRI_FCI             = ATTRI_GSM_BASE + 50; //FreeFormatIndicate SCP中下发的免费标识
const int ATTRI_MULTIMEDIA      = ATTRI_GSM_BASE + 51; //视频呼叫标志 0为非视频呼叫 1为视频呼叫
const int ATTRI_PA_TYPE         = ATTRI_GSM_BASE + 52; //区内、区间属性
const int ATTRI_SPE_PHONE_FLAG  = ATTRI_GSM_BASE + 53; //是否特殊号码 0:普通号码 1:特殊号码

const int ATTRI_TRUNK_DIRECT    = ATTRI_GSM_BASE + 57;  //中继方向 0:入中继 1:出中继 2:出入中继 --BF
const int ATTRI_BILL_FLAG       = ATTRI_GSM_BASE + 58;  //计费标志 0:仅需合帐 1:重新计费 --BF
const int ATTRI_TRUNK_FLAG      = ATTRI_GSM_BASE + 59;  //中继计费标志 0:普通计费 1:中继计费 --BF
const int ATTRI_SUB_HOMECODE_A  = ATTRI_GSM_BASE + 60; //A固网子区号 --BF
const int ATTRI_SUB_HOMECODE_B  = ATTRI_GSM_BASE + 61; //B固网子区号 --BF
const int ATTRI_EXCHANGECODE_A  = ATTRI_GSM_BASE + 62; //A固网局向 --BF
const int ATTRI_EXCHANGECODE_B  = ATTRI_GSM_BASE + 63; //B固网局向 --BF
const int ATTRI_ADM_REGION_A    = ATTRI_GSM_BASE + 64; //A行政区信息 --BF
const int ATTRI_ADM_REGION_B    = ATTRI_GSM_BASE + 65; //B行政区信息 --BF
const int ATTRI_PHYSICAL_PHONE_A  = ATTRI_GSM_BASE + 66; //A物理号码 --BF
const int ATTRI_PHYSICAL_PHONE_B  = ATTRI_GSM_BASE + 67; //B物理号码 --BF
const int ATTRI_LOGIC_PHONE_A     = ATTRI_GSM_BASE + 68; //A逻辑号码 资料 --BF
const int ATTRI_LOGIC_PHONE_B     = ATTRI_GSM_BASE + 69; //B逻辑号码 资料 --BF
const int ATTRI_PHYCRM_PHONE_A    = ATTRI_GSM_BASE + 70; //A物理号码 资料 --BF
const int ATTRI_PHYCRM_PHONE_B    = ATTRI_GSM_BASE + 71; //B物理号码 资料 --BF
const int ATTRI_BRANCHCODE_A      = ATTRI_GSM_BASE + 72; //A支局 山西存放第4层ORG --BF
const int ATTRI_BRANCHCODE_B      = ATTRI_GSM_BASE + 73; //B支局 山西存放第4层ORG --BF
const int ATTRI_CRMUSER_TYPE_CODE = ATTRI_GSM_BASE + 74; //用户主表的用户类型 资料 --BF

//add 20100730
const int ATTRI_CAMEL_NUMBER     = ATTRI_GSM_BASE + 75; //接续号码（Camel Destination number）
const int ATTRI_CONNECTED_NUMBER = ATTRI_GSM_BASE + 76; //实际接续号码（Connected number）
const int ATTRI_CAMEL_SERVERKEY  = ATTRI_GSM_BASE + 77; //智能网标识（CamelServerKey）

const int ATTRI_NBR_NUMBER        = ATTRI_GSM_BASE + 78; //千群百号代表号码 //add by dph,2009-12-28 15:08:00
// 2.SMS业务类属性定义 401 - 450
const int ATTRI_SM_BASE                = 400   ;
// 2.SMS业务类属性定义
//------------------------------------------------------------------------------
const int ATTRI_THIRD_PARTY         = ATTRI_SM_BASE + 1  ;
const int ATTRI_ORIG_TON            = ATTRI_SM_BASE + 2  ;
const int ATTRI_DEST_TON            = ATTRI_SM_BASE + 3  ;
const int ATTRI_SM_USER_TYPE        = ATTRI_SM_BASE + 4 ;
const int ATTRI_SM_USER_TYPE_B      = ATTRI_SM_BASE + 5 ;
const int ATTRI_SM_SERV_CODE_TYPE_A = ATTRI_SM_BASE + 6 ;
const int ATTRI_SM_SERV_CODE_TYPE_B = ATTRI_SM_BASE + 7 ;
const int ATTRI_SM_TYPESTAT         = ATTRI_SM_BASE + 8 ;
const int ATTRI_PRIORITY            = ATTRI_SM_BASE + 9  ;
const int ATTRI_INFO_LEN            = ATTRI_SM_BASE + 10 ;
const int ATTRI_ISMG_CODE           = ATTRI_SM_BASE + 11 ;
const int ATTRI_FORW_ISMG           = ATTRI_SM_BASE + 12 ;
const int ATTRI_SMSC_CODE           = ATTRI_SM_BASE + 13 ;
const int ATTRI_CFEE_TYPE           = ATTRI_SM_BASE + 14 ; //通讯费计费类别
const int ATTRI_SMS_SEQ             = ATTRI_SM_BASE + 15 ; //短信序列号
const int ATTRI_SP_NAME             = ATTRI_SM_BASE + 16 ; //SP名称 从表中获取
const int ATTRI_MONTH_ID            = ATTRI_SM_BASE + 17 ; //帐期月 SP包月类话单 YYYYMM
/////////////////////////GPRS的字段已经扩充到260了 而MISC从250开始 考虑重新分配
// 3.GPRS业务类属性定义 451 - 550
const int ATTRI_GPRS_BASE                = 450   ;
// 3.GPRS业务类属性定义
const int ATTRI_GPRS_APNNI          = ATTRI_GPRS_BASE + 1;
const int ATTRI_GPRS_TARIFF1        = ATTRI_GPRS_BASE + 2;
const int ATTRI_GPRS_DATA_UP1       = ATTRI_GPRS_BASE + 3;
const int ATTRI_GPRS_DATA_DOWN1     = ATTRI_GPRS_BASE + 4;
const int ATTRI_GPRS_DURATION1      = ATTRI_GPRS_BASE + 5;
const int ATTRI_GPRS_TARIFF2        = ATTRI_GPRS_BASE + 6;
const int ATTRI_GPRS_DATA_UP2       = ATTRI_GPRS_BASE + 7;
const int ATTRI_GPRS_DATA_DOWN2     = ATTRI_GPRS_BASE + 8;
const int ATTRI_GPRS_DURATION2      = ATTRI_GPRS_BASE + 9;
const int ATTRI_GPRS_ACT_DATA       = ATTRI_GPRS_BASE + 10;
const int ATTRI_GPRS_FLOWID         = ATTRI_GPRS_BASE + 11;
//Added by Sunam 2005-12-12
const int ATTRI_GPRS_TARIFF         = ATTRI_GPRS_BASE + 12;
const int ATTRI_GPRS_UPDOWN         = ATTRI_GPRS_BASE + 13;
//扩充add by gaojf 9/18/2006 2:54PM
const int ATTRI_GPRS_APNOI          = ATTRI_GPRS_BASE + 14  ;//GPRSAPNOI

const int ATTRI_GPRS_SERVICETYPE    = ATTRI_GPRS_BASE + 16  ;//GPRS 业务类型 复用
const int ATTRI_GPRS_SERVICECODE    = ATTRI_GPRS_BASE + 17  ;//GPRS 业务代码 复用
const int ATTRI_GPRS_UPDWONFLAG     = ATTRI_GPRS_BASE + 18  ;//GPRS上下性标记：0 不分.1 上行.2下行
const int ATTRI_GPRS_RATEOBJECT     = ATTRI_GPRS_BASE + 19  ;//GPRS计费对象:0 时长,1总流量,2上下行流量,3按条
const int ATTRI_GPRS_RECORDTYPE     = ATTRI_GPRS_BASE + 20  ;//话单类型SCDR/GCDR
const int ATTRI_GPRS_APNNIGROUP     = ATTRI_GPRS_BASE + 22  ; //APNI分组
const int ATTRI_GPRS_APNNITYPE      = ATTRI_GPRS_BASE + 23  ; //APNI大类
const int ATTRI_GPRS_MMSFLAG        = ATTRI_GPRS_BASE + 24  ; //GPRS 是否包含彩信业务标识
const int ATTRI_GPRS_SGSN           = ATTRI_GPRS_BASE + 25  ; //GPRS SGSN地址

//为支持CMWAP主页面赠送业务而增加2007-4-26 8:36 gaojf begin
const int ATTRI_GPRS_DATA_UP5       = ATTRI_GPRS_BASE + 26;
const int ATTRI_GPRS_DATA_DOWN5     = ATTRI_GPRS_BASE + 27;
const int ATTRI_GPRS_DATA_UP6       = ATTRI_GPRS_BASE + 28;
const int ATTRI_GPRS_DATA_DOWN6     = ATTRI_GPRS_BASE + 29;
const int ATTRI_GPRS_RCDSEQNUM      = ATTRI_GPRS_BASE + 30;
//联通GPRS总部接口需要
const int ATTRI_GPRS_CHARGED_ITEM   = ATTRI_GPRS_BASE + 31;
const int ATTRI_GPRS_CHARGED_OPER   = ATTRI_GPRS_BASE + 32;
const int ATTRI_GPRS_CHARGED_UNITS  = ATTRI_GPRS_BASE + 33;
const int ATTRI_GPRSGROUPID         = ATTRI_GPRS_BASE + 34;  //国际GPGS漫游分组
const int ATTRI_GPRS_VISIT_HOMETYPE = ATTRI_GPRS_BASE + 35;  //到访地类型
const int ATTRI_GPRS_CHARGING_ID    = ATTRI_GPRS_BASE + 36;  //CHARGE_ID
const int ATTRI_GPRS_GGSN           = ATTRI_GPRS_BASE + 37;  //GPRS GGSN地址
const int ATTRI_GPRS_SGSN_HEX       = ATTRI_GPRS_BASE + 38;  //GPRS SGSN16进制地址
const int ATTRI_GPRS_BUSITYPEFLAG   = ATTRI_GPRS_BASE + 39;  //GPRS忙闲时标记 --BF
const int ATTRI_GPRS_SERVTYPE1      = ATTRI_GPRS_BASE + 40;  //用来专门标识SERVICE_TYPE字段 FEMTO
const int ATTRI_GPRS_SERV_GROUP     = ATTRI_GPRS_BASE + 41;  //add by zhangjz 20091105 3g无线上网卡

// 4.MISCS业务类属性定义 550 - 650
const int ATTRI_MISC_BASE                = 550   ;
// 4.MISCS业务类属性定义
const int ATTRI_MISC_STR5           = ATTRI_MISC_BASE + 1;//wlan业务的热点标识
const int ATTRI_MISC_STR8           = ATTRI_MISC_BASE + 2;//MMS发送状态
const int ATTRI_MISC_STR2           = ATTRI_MISC_BASE + 3;//IMMS的ReceiveAddress
const int ATTRI_MISC_STR3           = ATTRI_MISC_BASE + 4;//ATTRI_MISC_STR3:是多方通话费service_type
const int ATTRI_MISC_STR7           = ATTRI_MISC_BASE + 5;//str7:彩铃的servkey 
const int ATTRI_MISC_STR1           = ATTRI_MISC_BASE + 6;//STR1 PIM的PIMCODE，需要批价
const int ATTRI_MISC_STR6           = ATTRI_MISC_BASE + 7;//彩信的sp_code
const int ATTRI_MISC_STR18          = ATTRI_MISC_BASE + 8;
const int ATTRI_MISC_STR19          = ATTRI_MISC_BASE + 9;
const int ATTRI_MISC_STR10          = ATTRI_MISC_BASE + 10; //--BF
const int ATTRI_MISC_NUM1           = ATTRI_MISC_BASE + 11;
const int ATTRI_MISC_NUM2           = ATTRI_MISC_BASE + 12;

// 4.联通SP增值业务
const int ATTRI_SP_BASE             = 550;
const int ATTRI_SP_SRVTYPE          = ATTRI_SP_BASE + 1; //大业务类型wap/mms/LBS等
const int ATTRI_SP_SRVKIND          = ATTRI_SP_BASE + 2; //业务类别PUSH类00;浏览类01;下载类02;互动类03
const int ATTRI_SP_SRVID            = ATTRI_SP_BASE + 3; //业务代码 
const int ATTRI_CONTENT_CODE        = ATTRI_SP_BASE + 4; //内容/应用代码
const int ATTRI_NAI_NAME            = ATTRI_SP_BASE + 5; //NAI用户名
const int ATTRI_NAI_TYPE            = ATTRI_SP_BASE + 6; //NAI类型
const int ATTRI_SP_QTY              = ATTRI_SP_BASE + 7; //增值业务次数
const int ATTRI_SP_TOTALDATA        = ATTRI_SP_BASE + 8; //总流量
const int ATTRI_SP_RATEUNIT         = ATTRI_SP_BASE + 9; //计费单位
const int ATTRI_SP_FEETYPE          = ATTRI_SP_BASE + 10; //计费类型
const int ATTRI_SYS_INTRAKEY        = ATTRI_SP_BASE + 11; //系统内部主键
//Add by zhaoly For js 3G test At 2009-4-15 13:48:57
const int ATTRI_M_TIMES             = ATTRI_SP_BASE + 12; //3G业务M资源数
const int ATTRI_T_TIMES             = ATTRI_SP_BASE + 13; //3G业务T资源数
//add by zhaoly for 3g-partyprice 2009-5-6 18:29:53
const int ATTRI_BILLING_MODECODE    = ATTRI_SP_BASE + 14; //3G业务VAC的计费模式
const int ATTRI_3GFEE_TYPE          = ATTRI_SP_BASE + 15; //3G业务VAC的feeType:2:按次;6:按时长;5:按流量


//5.IP业务类属性定义 601 - 620
const int ATTRI_IP_BASE             = 600   ;
const int ATTRI_CARD_NO             = ATTRI_IP_BASE + 1  ; //话单卡号 原始字段
const int ATTRI_CARD_TYPE           = ATTRI_IP_BASE + 2  ; //话单类型 原始字段
const int ATTRI_IPUSER_TYPE         = ATTRI_IP_BASE + 3  ; //话单IP用户类型 原始字段
const int ATTRI_IPACCESS_TYPE       = ATTRI_IP_BASE + 4  ; //话单类型 根据对端号码前缀判
const int ATTRI_BUSI_TYPE           = ATTRI_IP_BASE + 5  ; //业务类型 中继表中获取 1按中继计费 0按号码计费
const int ATTRI_IPNUMBER_TYPE       = ATTRI_IP_BASE + 6  ; //用户类型 中继表用户号段表中获取
const int ATTRI_IP_SERV_TYPE_A      = ATTRI_IP_BASE + 7  ; //用户服务类型 31
const int ATTRI_IP_ORIGIN_TYPE      = ATTRI_IP_BASE + 8  ; //原始计费类别

//6.统计类业务属性定义 601 - 650
const int ATTRI_STATIS_BASE         = 600   ;
const int ATTRI_STATIS_STR1         = ATTRI_STATIS_BASE + 1  ;
const int ATTRI_STATIS_STR2         = ATTRI_STATIS_BASE + 2  ;
const int ATTRI_STATIS_STR3         = ATTRI_STATIS_BASE + 3  ;
const int ATTRI_STATIS_STR4         = ATTRI_STATIS_BASE + 4  ;
const int ATTRI_STATIS_STR5         = ATTRI_STATIS_BASE + 5  ;
const int ATTRI_STATIS_STR6         = ATTRI_STATIS_BASE + 6  ;
const int ATTRI_STATIS_STR7         = ATTRI_STATIS_BASE + 7  ;
const int ATTRI_STATIS_STR8         = ATTRI_STATIS_BASE + 8  ;
const int ATTRI_STATIS_STR9         = ATTRI_STATIS_BASE + 9  ;
const int ATTRI_STATIS_STR10        = ATTRI_STATIS_BASE + 10 ;
const int ATTRI_STATIS_STR11        = ATTRI_STATIS_BASE + 11 ;
const int ATTRI_STATIS_STR12        = ATTRI_STATIS_BASE + 12 ;
const int ATTRI_STATIS_STR13        = ATTRI_STATIS_BASE + 13 ;
const int ATTRI_STATIS_STR14        = ATTRI_STATIS_BASE + 14 ;
const int ATTRI_STATIS_STR15        = ATTRI_STATIS_BASE + 15 ;
const int ATTRI_STATIS_STR16        = ATTRI_STATIS_BASE + 16 ;
const int ATTRI_STATIS_STR17        = ATTRI_STATIS_BASE + 17 ;
const int ATTRI_STATIS_STR18        = ATTRI_STATIS_BASE + 18 ;
const int ATTRI_STATIS_STR19        = ATTRI_STATIS_BASE + 19 ;
const int ATTRI_STATIS_STR20        = ATTRI_STATIS_BASE + 20 ;
const int ATTRI_ITEM_ID             = ATTRI_STATIS_BASE + 21 ; //账单项

const int ATTRI_RENT_BASE = 200   ;
const int ATTRI_RENT_SUMTOINTTAG  = ATTRI_RENT_BASE + 1;
const int ATTRI_RENT_STARTSUBID   = ATTRI_RENT_BASE + 2;
const int ATTRI_RENT_ENDSUBID     = ATTRI_RENT_BASE + 3;
const int ATTRI_RENT_ITEMCODE     = ATTRI_RENT_BASE + 4;
const int ATTRI_RENT_DISPLAYFEE   = ATTRI_RENT_BASE + 5;
const int ATTRI_RENT_COMPUTERDATE = ATTRI_RENT_BASE + 6;

const int ATTRI_RENT_FEE_BASE  = 210;
const int ATTRI_RENT_SUMFEE    = ATTRI_RENT_FEE_BASE + 0;
const int ATTRI_RENT_SUBFEE_01 = ATTRI_RENT_FEE_BASE + 1;
const int ATTRI_RENT_SUBFEE_02 = ATTRI_RENT_FEE_BASE + 2;
const int ATTRI_RENT_SUBFEE_03 = ATTRI_RENT_FEE_BASE + 3;
const int ATTRI_RENT_SUBFEE_04 = ATTRI_RENT_FEE_BASE + 4;
const int ATTRI_RENT_SUBFEE_05 = ATTRI_RENT_FEE_BASE + 5;
const int ATTRI_RENT_SUBFEE_06 = ATTRI_RENT_FEE_BASE + 6;
const int ATTRI_RENT_SUBFEE_07 = ATTRI_RENT_FEE_BASE + 7;
const int ATTRI_RENT_SUBFEE_08 = ATTRI_RENT_FEE_BASE + 8;
const int ATTRI_RENT_SUBFEE_09 = ATTRI_RENT_FEE_BASE + 9;
const int ATTRI_RENT_SUBFEE_10 = ATTRI_RENT_FEE_BASE + 10;
const int ATTRI_RENT_SUBFEE_11 = ATTRI_RENT_FEE_BASE + 11;
const int ATTRI_RENT_SUBFEE_12 = ATTRI_RENT_FEE_BASE + 12;
const int ATTRI_RENT_SUBFEE_13 = ATTRI_RENT_FEE_BASE + 13;
const int ATTRI_RENT_SUBFEE_14 = ATTRI_RENT_FEE_BASE + 14;
const intTTRI_STATIS_STR16        = ATTRI_STATIS_BASE + 16 ;
const int ATTRI_STATIS_STR17        = ATTRI_STATIS_BASE + 17 ;
const int ATTRI_STATIS_STR18        = ATTRI_STATIS_BASE + 18 ;
const int ATTRI_STATIS_STR19        = ATTRI_STATIS_BASE + 19 ;
const int ATTRI_STATIS_STR20        = ATTRI_STATIS_BASE + 20 ;
const int ATTRI_ITEM_ID             = ATTRI_STATIS_BASE + 21 ; //璐﹀崟椤