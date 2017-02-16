#ifndef __CONST_DEF_H__
#define __CONST_DEF_H__

#include "FieldsXX.h"
#include "cdr/AttrIdConstDef.h"  //话单字段定义，与批价共用
#include "BillingConstDef.h"

//内部用通道
const string CHANNEL_FILEINPUT  ("fileInputXXX");  //输入文件通道

//默认的通道名
const string CHANNEL_TRASH      ("trash");
const string CHANNEL_ERROR      ("error");
const string CHANNEL_PROV       ("prov");       //一次分检
const string CHANNEL_BINDPROV   ("bindprov");   //二次分检
const string CHANNEL_NR         ("nr");
const string CHANNEL_IR         ("ir");
const string CHANNEL_BR         ("br");         //国内边漫来访
const string CHANNEL_NOINFO     ("noinfo");
const string CHANNEL_POCNOINFO  ("pocNoinfo");
const string CHANNEL_SCP        ("scpTrash");
const string CHANNEL_SMTRASH    ("smTrash");
const string CHANNEL_STAT       ("stat");
const string CHANNEL_C          ("CRecord");    //梦网稽核部分
const string CHANNEL_SCPSM      ("scpsm");
const string CHANNEL_TR         ("tr");
const string CHANNEL_TDPROV     ("tdprov");
const string CHANNEL_SPTEST     ("sptest");
const string CHANNEL_MUX        ("mult");       //multiplex channel 都能一分多，通道前缀必须要为muli
const string CHANNEL_BACKUP     ("open");       //openacct   channel 都能一分多，通道前缀必须要为open

const string ATTRI_TRASH_2    ("2");  //特殊 话单
const string ATTRI_TRASH_Y    ("1");  //TRASH 话单
const string ATTRI_TRASH_N    ("0");  //非TRASH 话单
const string ATTRI_ERROR_Y    ("1");  //ERROR 话单
const string ATTRI_ERROR_N    ("0");  //非ERROR 话单
const string ATTRI_SCP_Y      ("1");  //SCP 话单
const string ATTRI_SCP_N      ("0");  //非SCP 话单
const string ATTRI_ROAM_IR    ("2");  //国际来访 话单
const string ATTRI_ROAM_NR    ("1");  //国内来访 话单
const string ATTRI_ROAM_PROV  ("0");  //省内用户 话单
const string ATTRI_SPECIALPHONE_Y ("1"); //特殊号码
const string ATTRI_SPECIALPHONE_N ("0"); //非特殊号码



//USER_TYPE DEFINE第一位:asp,第二、第三位serv_type，第4位home_type
const string USER_TYPE_B_DEFAULT          ("0031");
const string USER_TYPE_B_DEFAULT_CMCC     ("1011");
const string USER_TYPE_B_DEFAULT_UNICOM   ("2011");
const string USER_TYPE_B_DEFAULT_FIX      ("3031");
const string USER_TYPE_B_FIX_NATIONAL     ("0033");
const string USER_TYPE_A_DEFAULT          ("1011");
const string USER_TYPE_A_UNICOM_TRANS     ("10T1");
const string USER_TYPE_B_MVOICE           ("1011");
const string USER_TYPE_B_UVOICE           ("2011");
const string USER_TYPE_A_UNICOM_CNC       ("2031");
const string USER_TYPE_B_DEFAULT_CNCFIX   ("4031");

//短信费用类型 CHARGE_TYPE
const string CHARGE_TYPE_00               ("00"); //核减不均衡通信费
const string CHARGE_TYPE_01               ("01"); //免费
const string CHARGE_TYPE_02               ("02"); //按条
const string CHARGE_TYPE_03               ("03"); //包月
const string CHARGE_TYPE_04               ("04"); //SP自收费

//USER_TYPE
const string  USER_SM_TYPE_QQT            ("0");  //全球通 本运营商
const string  USER_SM_TYPE_SZX            ("1");  //神州行
const string  USER_SM_TYPE_VPN            ("2");  //集团用户

//USER_STATE
const string  USER_NORMAL                 ("0");  //用户正常
const string  USER_STOP                   ("1");  //用户停机
const string  USER_DESTROY                ("2");  //用户销户
const string  USER_UNDEFINED              ("3");  //用户状态未知

//roam_type 以计费用户和SP归属网关为依据
//const char 	ROAM_TYPE_NONE		     = '0';
//const char	ROAM_TYPE_INVISITOUT	 = '1';
//const char	ROAM_TYPE_OUTVISITIN	 = '2';
//const char	ROAM_TYPE_OUTVISITOUT	 = '3';

//ADDRESS TON
const char UNKNOWN               = '0';
const char MY_MOBILE             = '1';
const char OTHER_MOBILE          = '2';
const char APPLICATION           = '3';

const int USER_TYPE_LENGTH                = 4;

const int  DEFAULT_SPECIAL_PHONE_GROUP    = 0;

//CMWAP的分组
const string GPRS_APNGROUP_CMWAP          ("001");

//语音信箱
const string UVOICEBOX_NUMBER   ("13010");
const string MVOICEBOX_NUMBER   ("13800");

const string DEFAULT_GROUP_ID   ("0");
const string DEFAULT_GROUP_ID_B ("20");
const string GROUP_ID_B_N       ("30");

//计费标志
const string BILL_FLAG_NOMAL   ("0");  //普通计费
const string BILL_FLAG_1       ("1");  //千群百号计费
const string BILL_FLAG_2       ("2");  //号码转换计费
const string BILL_FLAG_3       ("4");  //号段拨打号段计费
const string BILL_FLAG_4       ("8");  //
const string BILL_FLAG_5       ("16"); //

const string TRUNK_FLAG_NO     ("0");  //普通计费
const string TRUNK_FLAG_YES    ("1");  //中继计费
const string PREPAY_FLAG       ("0");  //预付费
const string POSTPAY_FLAG      ("1");  //后付费

//中继方向
const string TRUNK_IN          ("0");  //入中继
const string TRUNK_OUT         ("1");  //出中继
const string TRUNK_ALL         ("2");  //出入中继

//区内区间
const string PA_IN             ("0");  //区内
const string PA_BETWEEN        ("1");  //区间
const string PA_OUT            ("2");  //非区内区间

//属性求解增加
//TOLL_MSC IP业务交换机
enum MSC_TYPE  {
     ALL_MSC = 0,
     NON_VITRUAL_MSC,
     VITRUAL_MSC,
     TOLL_MSC,
     FIX_MSC};

enum GET_A_HOME_METHOD  {
     A_HOME_BY_IMSI = 1,
     A_HOME_BY_MSISDN};

enum GET_A_VISIT_METHOD {
     A_VISIT_BY_MSC_LAC = 1,
     A_VISIT_BY_MSC_LAC_CELLID,
     A_VISIT_BY_CELLID,
     A_VISIT_BY_MSC_CELLID,
     A_VISIT_BY_LAC,
     A_VISIT_BY_MSC,
     A_VISIT_BY_LAC_CELLID};

enum GET_GPRS_A_VISIT_METHOD {
     A_GPRS_VISIT_BY_PROV_CODE = 1,
     A_GPRS_VISIT_BY_MSC_LAC,
     A_GPRS_VISIT_BY_MSC_LAC_CELLID,
     A_GPRS_VISIT_BY_CELLID,
     A_GPRS_VISIT_BY_MSC_CELLID,
     A_GPRS_VISIT_BY_LAC,
     A_GPRS_VISIT_BY_MSC,
     A_GPRS_VISIT_BY_LAC_CELLID};

enum GET_B_HOME_METHOD  {B_HOME_BY_MSISND = 1,B_HOME_BY_MSISDN_IMSI};

enum GET_B_VISIT_METHOD {B_VISIT_BY_LAC_MSRN = 1,B_VISIT_BY_MSRN,B_VISIT_BY_HOME,B_VISIT_BY_MSRN1,B_VISIT_BY_MSRN2};

enum GET_RO_A_HOME_METHOD   {A_RO_HOME_BY_MSISDN = 1,
	                           A_RO_HOME_BY_IMSI2MSISDN,
	                           A_RO_HOME_BY_MSISDN_IMSI2MSISDN};
//Modified by Sunam 2005-12-12 增加下发文件求A到访地的配置
enum GET_RO_A_VISIT_METHOD  {A_RO_VISIT_BY_MSC   = 1,A_RO_VISIT_BY_ORG,A_RO_VISIT_BY_CITY};
enum GET_RO_B_HOME_METHOD   {B_RO_HOME_BY_MSISND = 1};
enum GET_RO_B_VISIT_METHOD  {B_RO_VISIT_BY_MSRN  = 1};
//modify by gux 2006-3-11 19:46
//modified by Sunam 2006-4-14 15:18
enum GET_EDGE_ROAM_METHOD   {EDGE_ROAM_BY_CELLID=1,
         EDGE_ROAM_BY_LAC_CELLID,
         EDGE_ROAM_BY_MSC_LAC_CELLID,
         EDGE_ROAM_ROAMOUT_BY_CELLID,
         EDGE_ROAM_ROAMOUT_BY_LAC_CELLID,
         EDGE_ROAM_ROAMOUT_BY_MSC_LAC_CELLID,
         EDGE_ROAM_BY_MSC_CELLID,
         EDGE_ROAM_BY_VISITAREA_CELLID,
         EDGE_ROAM_BY_AV_CELLID};


//原先保留
//获取用户资料方式
enum BIND_METHOD {
	   BIND_BY_MSISDN = 1,
	   BIND_BY_IMSI,
	   BIND_BY_MSISDN_IMSI,
	   BIND_BY_TRUNK,
	   BIND_BY_USERID,
	   BIND_BY_MSISDN_NETTYPE};

//短信处理方式
enum PROCESS_METHOD {
	   SM_PROCESS_METHOD = 1,
	   SP_PROCESS_METHOD,
	   USSD_PROCESS_METHOD,
	   HY_PROCESS_METHOD,
	   UNITE_PPS_PROCESS_METHOD,
	   FIX_SM_PROCESS_METHOD,
	   FIX_SP_PROCESS_METHOD};

//本地彩信B状态获取方式
enum GET_LOCAL_MMS_B_METHOD {
	   B_BY_SP_SERVICE = 1,
	   B_BY_SP_SERVICE_OPER};

//用户状态稽核方式
enum STATUS_AUDIT_METHOD {
	   FINISH_TIME = 1,
	   CURRENT_TIME,DELAY_TIME,
	   DELAY_CURRENT_TIME,
	   START_TIME};

//稽核顺序 1 先稽核用户状态、订购关系后稽核用户信息服务状态 2先稽核用户信息服务状态再稽核其他状态
enum AUDIT_PROCESS_SEQUENCE {
	   AUDIT_TO_FREE = 1,
	   FREE_TO_AUDIT};



#endif  /*__CONST_DEF_H__*/
