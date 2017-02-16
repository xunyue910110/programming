#ifndef __REC_CONST_DEF__
#define __REC_CONST_DEF__

#include "base/Types.h"

const int MAXFIELDLEN_REC     = 600;   //定义字段最大长度
const int MAXFILEDNUM_REC     = 250;  //定义最大字段数
const int MAXSQLLENGTH        = 5000; //定义SQL的最大字符数
const int MAXCDRNUM           = 1000; //缺省一个文件包含最大的记录数
const int MAXFILENUM          = 10000;//一个通道一次产生最大的文件数

const int SPEC_FEE_FIELD      = 20;   //特殊账单的费用域

//const string ROAM_TYPE_NATIONAL_OUT       ("4")   ;
//const string ROAM_TYPE_INTERNATIONAL_OUT  ("6")   ;

const int    MAX_BIZ_TYPE     = 11;  //2009.10.23 8 -> 11
/*const string BIZ_TYPE_GSM     ("1");	//CDMA GSM
const string BIZ_TYPE_SM      ("2");	//SMS
const string BIZ_TYPE_GPRS    ("3");	//GPRS
const string BIZ_TYPE_MISC    ("4");	//增值业务
const string BIZ_TYPE_CDMA1X  ("5");	//CDMA1X
const string BIZ_TYPE_IP      ("6");	//17911/193业务*/

const string GPRS_MMS_APNNI   ("GBOSS.CBS.BILLING.GPRS.MMS");

const string PREFIX("#");
const string POSTFIX_REDO("REDO");
const string POSTFIX_RCYL("RCYL");

//省内
/*const string CHANNEL_PROV                 ("P")   ;
//无主
const string CHANNEL_NOINFO               ("U")   ;
//国内来访
const string CHANNEL_INTERPROV            ("N")   ;
//国际来访
const string CHANNEL_INTERNATIOANAL       ("I")   ;
//智能网
const string CHANNEL_SCP                  ("S")   ;*/

const int MAX_BILL_ITEM     	= 3000;
                            	
// 特殊字段位置             	
const int CDR_SPACE_INDEX		= 200;	// 空字段
const int CDR_END_DATE_INDEX	= 201;	// 结束日期
const int CDR_END_TIME_INDEX	= 202;	// 结束时间
const int CDR_COUNT_INDEX		= 205;	// 话单个数

                              
const int ERROR_BASE_REC      	= 11000;              //定义错单回收错误代码
const int ERROR_CONNECTDB     	= ERROR_BASE_REC+1;   //连接数据库错
const int ERROR_PROCEDURE     	= ERROR_BASE_REC+2;   //调用存储过程失败
const int ERROR_EXECUTESQL    	= ERROR_BASE_REC+3;   //执行SQL语句失败
const int ERROR_OUTPUTCDR     	= ERROR_BASE_REC+4;   //执行SQL语句失败
const int ERROR_FLAGERROR     	= ERROR_BASE_REC+5;   //标志表错
const int ERROR_BILLLOGERROR    = ERROR_BASE_REC+6;   //账单日志错
const int ERROR_CYCLEORCHANNELNO= ERROR_BASE_REC+7;   //账期或者通道错
const int ERROR_FILECOMMITERROR = ERROR_BASE_REC+8;   //文件提交失败

const int E_FILE_BILL_COMMIT 		= ERROR_BASE_REC +  8;   // 文件帐单提交错误
const int E_CDR_BILL_COMMIT  		= ERROR_BASE_REC +  9;   // 清单帐单提交错误
const int E_RECOVERY_OK		  		= ERROR_BASE_REC + 10;   // 帐单恢复成功
const int E_NO_BILL_LOG_FILE 		= ERROR_BASE_REC + 11;   // 没有帐单日志文件
const int E_BILL_FILE_INITIALIZE    = ERROR_BASE_REC + 12;   // 帐单文件初始化错
const int E_BILL_FILE		  		= ERROR_BASE_REC + 13;   // 帐单文件错
const int E_BILL_INITIALIZE  		= ERROR_BASE_REC + 14;   // 帐单初始化错
const int E_BILL_INIT				= ERROR_BASE_REC + 20; // 帐单初始化错误
const int E_BILL_END				= ERROR_BASE_REC + 24; // 帐单初始化错误
const int ERROR_SHOWMESSAGE		= ERROR_BASE_REC + 25; // 日志提醒显示
const int E_PARAM_INI				= ERROR_BASE_REC + 17;   // 参数s初始化错误


#endif //


