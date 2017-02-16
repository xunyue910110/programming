// ##########################################
// Source file : CdrErrorCode.h
// System      : Mobile Billing System
// Version     : 0.0.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011012
// Update      : 20020318
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef CDRERRORCODE_H_INCLUDED
#define CDRERRORCODE_H_INCLUDED

// 本文件定义了预处理原始话单字段错误代码


//严重错误 001 -049
const int SERIOUS_ERROR_BASE          =  1;


//文件错误 050 - 099
const int FILE_ERROR_BASE             =  50;

const int ERROR_FILE_LENGTH           =  FILE_ERROR_BASE +  1; // 文件长度错误
const int ERROR_FILE_NAME             =  FILE_ERROR_BASE +  5; // 文件名错误

//记录错误 100 - 149
const int CDR_ERROR_BASE              =  100;

const int CDR_FILTER      			  =  CDR_ERROR_BASE + 1;  // 话单被过滤
const int ERROR_CDR_LENGTH            =  CDR_ERROR_BASE + 2;  // 话单记录长度错误
const int INVALID_DATE_TIME           =  CDR_ERROR_BASE + 5;  // 时间错误
const int INVALID_DURATION            =  CDR_ERROR_BASE + 10; // 时长超长超短错误
const int INVALID_SHORT_CDR           =  CDR_ERROR_BASE + 15; // 连续短话单
const int INVALID_MDN                 =  CDR_ERROR_BASE + 20; // 无效的MDN编码
const int INVALID_ESN                 =  CDR_ERROR_BASE + 25; // 无效的ESN编码
const int INVALID_MIN                 =  CDR_ERROR_BASE + 30; // 无效的MIN编码
const int INVALID_IMSI                =  CDR_ERROR_BASE + 35; // 无效的IMSI编码
const int INVALID_BASIC_SERVICE_EMPTY =  CDR_ERROR_BASE + 40; // 无效的基本服务代码
const int INVALID_TIME_DELAY          =  CDR_ERROR_BASE + 45;  // 通话时间距处理时间超时错






//一般字段错误 150 - 199
const int GENERAL_FIELD_BASE          =  150;

const int EMPTY_FIELD                 =  GENERAL_FIELD_BASE + 1; // 字段值为空
const int INVALID_FIELD               =  GENERAL_FIELD_BASE + 2; // 字段值无效
const int FORBIDDEN_CHARACTER         =  GENERAL_FIELD_BASE + 3; // 字段值含非法字符
const int LENGTH_FIELD                =  GENERAL_FIELD_BASE + 4; // 字段值长度错误
const int TYPE_ERROR                  =  GENERAL_FIELD_BASE + 5; // 值类型错误
const int GPRS_DATA_NULL              =  GENERAL_FIELD_BASE + 20;// gprs上下行流量为空




//具体字段错误 200 - 299
const int FIELD_ERROR_BASE            =  200;

const int RECORD_TYPE_EMPTY               =  FIELD_ERROR_BASE + 1; // 记录类型为空
const int RECORD_TYPE_INVALID             =  FIELD_ERROR_BASE + 2; // 记录类型无效
const int RECORD_FORBIDDEN_CHARACTER      =  FIELD_ERROR_BASE + 3; // 记录类型含非法字符

const int CALL_TYPE_EMPTY                 =  FIELD_ERROR_BASE + 11; // 呼叫类型为空
const int CALL_TYPE_INVALID               =  FIELD_ERROR_BASE + 12; // 呼叫类型无效
const int CALL_TYPE_FORBIDDEN_CHARACTER   =  FIELD_ERROR_BASE + 13; // 呼叫类型含非法字符

const int IMSI_EMPTY                      =  FIELD_ERROR_BASE + 21; // IMSI为空
const int IMSI_INVALID                    =  FIELD_ERROR_BASE + 22; // IMSI无效
const int IMSI_FORBIDDEN_CHARACTER        =  FIELD_ERROR_BASE + 23; // IMSI含非法字符
const int IMSI_LENGTH                     =  FIELD_ERROR_BASE + 24; // IMSI字段长度错误

const int OTHER_PARTY_EMPTY               =  FIELD_ERROR_BASE + 31; // 对方号码为空
const int OTHER_PARTY_INVALID             =  FIELD_ERROR_BASE + 32; // 对方号码无效
const int OTHER_PARTY_FORBIDDEN_CHARACTER =  FIELD_ERROR_BASE + 33; // 对方号码含非法字符

const int CELL_ID_EMPTY                   =  FIELD_ERROR_BASE + 20; // CELL_ID为空
const int CELL_ID_INVALID                 =  FIELD_ERROR_BASE + 21; // CELL_ID含非法字符

const int LAC_EMPTY                       =  FIELD_ERROR_BASE + 25; // LAC为空
const int LAC_FORBIDDEN_CHARACTER         =  FIELD_ERROR_BASE + 26; // LAC含非法字符

const int SUPPLEMENT_EMPTY                =  FIELD_ERROR_BASE + 30; // 补充业务代码为空
const int SUPPLEMENT_FORBIDDEN_CHARACTER  =  FIELD_ERROR_BASE + 31; // 补充业务代码含非法字符


//其他错误 300 - 399
const int OTHER_ERROR_BASE                =  300;

const int INVALID_ROAM_TYPE               =  OTHER_ERROR_BASE +  1; // 无效漫游类型
const int VPMN_CDR                        =  OTHER_ERROR_BASE +  2; // vpmn话单
const int VOICE_CUE_CDR                   =  OTHER_ERROR_BASE +  3; // 语音提示话单
const int FORE_REPEAT_CDR                 =  OTHER_ERROR_BASE +  4; // 呼转重单



#endif /* CDRERRORCODE_H_INCLUDED */
