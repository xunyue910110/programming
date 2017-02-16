#ifndef SYSTEPARA_H
#define SYSTEPARA_H

//话单类型
const int JINT_METER                = 1  ;              //净跳次话单
const int LEIJI_METER               = 2  ;              //累计跳次话单
const int FANZHUAN_METER            = 3  ;              //翻转跳次话单
const int BATCH_INSERT_COUNT        = 1;

const int MAX_SIZE                  = 500;
const int CV_ERROR_CDR_SIZE_ERROR   = 301;
const int CV_ERROR_CREATE_OBJECT    = 302;
const int CV_ERROR_PHONE_FILE_BLOCK = 303;
const int CV_ERROR_MEMORY_ALLOCATE  = 304;
const int CV_ERROR_GET_CONFIG       = 305;
const int CV_ERROR_OPEN_PHONE_FILE  = 307;
const int CV_CLOSE_FILE_ERROR       = 308;
const int CV_ERROR_CDR_NO           = 309;
const int CV_ERROR_TOO_MANY_ERROR   = 310;
const int CV_ERROR_UNLINK_FILE      = 311;
const int CV_ERROR_CONFIG_FILE      = 312;
const int CV_ERROR_FILE_NO          = 313;
const int CV_ERROR_FILE_NAME        = 314;
const int CV_ERROR_VOLID_PHONE_FILE = 315;
const int CV_ERROR_PARAMETER        = 316;
const int CV_ERROR_LINK_FILE        = 317;
const int CV_ERROR_WRITE_FILE       = 318;
const int CV_ERROR_TIME_OUT         = 319;
const int CV_ERROR_CDR              = 320;
const int CV_ERROR_LEJMTER_LESS_LASTVALUE = 321;

//数据库访问错误
const int CV_ERROR_DB                   = 500;
const int CV_ERROR_DB_CON               =CV_ERROR_DB + 1;
const int CV_ERROR_DB_BIND              =CV_ERROR_DB + 2;
const int CV_ERROR_DB_EXEC              =CV_ERROR_DB + 3;
const int CV_GET_FULLVAL                =CV_ERROR_DB + 21;
//致命错误基数
const int CV_ERROR_BASE       = 700;
const int CV_ERROR_INITIALIZE           = CV_ERROR_BASE + 1;   // 初始化错误
const int E_GET_INPUT_PARAM             = CV_ERROR_BASE + 2;   //根据子通道名获取输入子配置错误
const int E_CDR_FIELD_NUMBER            = CV_ERROR_BASE + 3;   //话单格式字段个数错误
const int E_MOVE_FILE_ERR               = CV_ERROR_BASE + 4;   //移走话单失败
const int E_GET_DIVID_PARAM             = CV_ERROR_BASE + 5;   //根据话单内容获取排重配置错误
const int E_GET_CHANNEL_PARAM           = CV_ERROR_BASE + 6;   //根据话单内容获取分通道配置错误
const int E_OUT_CHANNEL_DEFINED         = CV_ERROR_BASE + 7;   //配置中没有定义输出通道路径

#endif