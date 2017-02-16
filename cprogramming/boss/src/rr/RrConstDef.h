#ifndef _RRCONSTDEF_H_20080808_
#define _RRCONSTDEF_H_20080808_

//告警代码定义
const int  BOSS_RR_ERROR_BASE = 2000;
///////////////////////////////////////////////////
const int  E_RR_UNKNOWN_ERROR = BOSS_RR_ERROR_BASE + 0;//未知错误, 参阅错误信息
///////////////////////////////////////////////////
const int  E_RR_FILE_OPEN     = BOSS_RR_ERROR_BASE + 1;//文件打不开
const int  E_RR_FILE_READ     = BOSS_RR_ERROR_BASE + 2;//文件读错误
const int  E_RR_FILE_WRITE    = BOSS_RR_ERROR_BASE + 3;//文件写错误
const int  E_RR_FILE_CLOSE    = BOSS_RR_ERROR_BASE + 4;//文件关闭失败
const int  E_RR_FILE_REMOVE   = BOSS_RR_ERROR_BASE + 5;//文件删除失败
const int  E_RR_FILE_MOVE     = BOSS_RR_ERROR_BASE + 6;//文件移动错误
const int  E_RR_FILE_LINK     = BOSS_RR_ERROR_BASE + 7;//LINK文件错误
const int  E_RR_FILE_UNLINK   = BOSS_RR_ERROR_BASE + 8;//UNLINK文件错误
const int  E_RR_GETFILE_ERROR = BOSS_RR_ERROR_BASE + 9;//从目录中读取文件失败

///////////////////////////////////////////////////
const int  E_RR_PARAM_NULL_VALUE     = BOSS_RR_ERROR_BASE + 21;//空的配置参数
const int  E_RR_PARAM_INVALID_VALUE  = BOSS_RR_ERROR_BASE + 22;//无效的配置参数
const int  E_RR_PARAM_INVALID_DIR    = BOSS_RR_ERROR_BASE + 23;//目录配置参数无效
const int  E_RR_PARAM_INVALID_SELRULE= BOSS_RR_ERROR_BASE + 24;//字段选取配置参数错误
const int  E_RR_PARAM_NORRRULE       = BOSS_RR_ERROR_BASE + 25;//根据文件名照不到对应排重规则
const int  E_RR_CONSTRUCT_TABLE      = BOSS_RR_ERROR_BASE + 26;//构造HASHLIST错误
const int  E_RR_MEM_INSERTFID        = BOSS_RR_ERROR_BASE + 27;//内存插入过滤信息错误
///////////////////////////////////////////////////
const int  E_RR_ENV_NULL_VALUE     = BOSS_RR_ERROR_BASE + 43;//空的环境变量
const int  E_RR_ENV_INVALID_VALUE  = BOSS_RR_ERROR_BASE + 44;//无效的环境变量



//其它一些常量定义
const int  E_RR_SLEEPTIME   = 5;     //定义一次休眠时间：秒
const int  E_RR_WARNTIME    = 300;   //相同告警时间间隔：秒
const int  E_RR_LINEMAXLEN  = 1024*8;//一条话单最大长度

//定义排重类型
const int  E_RR_RRTYPE_NONE    = -1;  //不需排重
const int  E_RR_RRTYPE_EQUAL   = 0;  //完全排重
const int  E_RR_RRTYPE_CONTAIN = 1;  //包容排重
const int  E_RR_RRTYPE_FILE    = 2;  //文件级排重added by hanyu 08-12-02
//定义重单类型
const int  E_RR_REPFLAG_NONE   = -1; //不需排重
const int  E_RR_REPFLAG_UNIQ   = 0;  //非重单
const int  E_RR_REPFLAG_INFILE = 1;  //文件内重单
const int  E_RR_REPFLAG_INMEM  = 2;  //内存中重单
const int  E_RR_REPFLAG_INHIS  = 3;  //历史重单
const int  E_RR_REPFLAG_ERROR  = 4;  //错单
//是否已经排重标记
const int  E_RR_RRFLAG_NONE    = 0;  //未经过排重
//定义一个TableHash大小
const size_t  RR_HASH_SIZE       = 10 * 10000;       //内存HASH桶大小
//定义一个文件包含多少秒时间的过滤信息
//要求：该秒数是1800的倍数/但小于1天
const size_t  RR_ONEFILE_TIME    = 1800*2*12;  //多少时间一个文件(12个小时)
const size_t  RR_MEMFID_MINTIME  = 1800;       //内存存放的最短时间为半个小时     
const size_t  RR_MEMFID_STDTIME  = 2*3600;     //内存存放的默认时间为2个小时     
const size_t  RR_MEMFID_STDMAXNUM= 20* 10000;  //50万
//定义一个过滤文件对应的一些常量
const size_t  RR_FIDFILE_SIZE        = 30*1024*1024;  //50 * 1024 *1024; 一个文件大小50M
const size_t  RR_FIDFILE_NODENUM     = 30 * 10000;    //一个过滤文件最多存放30万条过滤信息
const size_t  RR_FIDFILE_RESERVESIZE = 100;           //定义文件最后预留最大空间
const int     RR_FID_SIZE            = 80;            //FID估算的大概大小
#endif //_RRCONSTDEF_H_20080808_
