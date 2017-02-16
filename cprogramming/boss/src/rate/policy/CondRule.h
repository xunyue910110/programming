#ifndef _CONDRULE_H_
#define _CONDRULE_H_

#include "CondBase.h"
#include "base/Types.h"
#include <map>

const char TAG_YESORNO_YES = '0';//取反标志: 正常
const char TAG_YESORNO_NO  = '1';//取反标志: 取反

const char TAG_DIVIED_NO       = '0';//分段标志:话单不分段
const char TAG_DIVIED_ADDUP    = '1';//分段标志:累计量分段
const char TAG_DIVIED_DURATION = '2';//分段标志:非累计量分段(按时长)
const char TAG_DIVIED_BUSY     = '3';//分段标志:非累计量分段(按相对时间段,如：忙闲时)
//Modified By Sunam 2009/8/13 12:28:38
//const char TAG_DIVIED_HOLIDAY  = '4';//分段标志:非累计量分段(按绝对时间段,如：节假日)
const char TAG_DIVIED_DAY      = '4';//分段标志:非累计量分段(按绝对时间段，如按天)
const char TAG_DIVIED_WEEK     = '5';//分段标志:非累计量分段(按周段) 
const char TAG_DIVIED_SUBDTIME = '6';//分段标志:非累计量分段(按绝对时间) 
const char TAG_DIVIED_DATA     = '7';//分段标志:非累计量分段(按流量)
const char TAG_DIVIED_BILL     = '8';//分段标志:账单总量分段
//Addedy Sunam 2009/8/13 12:28:24
const char TAG_DIVIED_HOLIDAY  = '9';//分段标志:按节假日分段(TD_B_HOLIDAY)
//Added By Sunam 2009/12/24 13:27:56
const char TAG_JUDGE_MODE_SUMSPEACIALDATE = 'A';// 条件判断模式：按指定日期折算上下限比较 [)

const char TAG_JUDGE_MODE_SCOPE    = '0';// 条件判断模式：()    
const char TAG_JUDGE_MODE_SCOPE_R  = '1';// 条件判断模式：(]
const char TAG_JUDGE_MODE_SCOPE_L  = '2';// 条件判断模式：[)		
const char TAG_JUDGE_MODE_SCOPE_LR = '3';// 条件判断模式：[]    	
const char TAG_JUDGE_MODE_IN       = '4';// 条件判断模式：in
const char TAG_JUDGE_MODE_EXISTS   = '5';// 条件判断模式：exists
//add by gaojf 2009-2-18 11:20 为了支持 
//入网或第一次选套餐的时候累计上下限按照日期来折算
const char TAG_JUDGE_MODE_SUMOPENDATE   = '6';// 条件判断模式：根据入网日期折算上下限比较 [)
const char TAG_JUDGE_MODE_SUMTPDATE     = '7';// 条件判断模式：根据套餐日期折算上下限比较 [)

//Added By Sunam 2009/10/6 19:37:50 非月累计量折算
const char TAG_JUDGE_MODE_SUM2          = '8';// 条件判断模式：非月累计量折算上下限比较 [)
//Added By Sunam 2009/11/10 10:23:35 
const char TAG_JUDGE_MODE_SUMEARLIESTOPENDATE = '9';// 条件判断模式：按当月最早开机时间折算上下限比较 [)
const char TAG_JUDGE_MODE_HALFMONTH = 'A';//条件判断模式：按上下半月折算上下限比较 [)
const char TAG_VALUE_METHOD_COMMON = '0';//取值函数:普通    
const char TAG_VALUE_METHOD_COMPUTE= '1';//取值函数:取模
const char TAG_VALUE_METHOD_SUBSTR = '2';//取值函数:SUBSTR  
const char TAG_VALUE_METHOD_STRCAT = '3';//取值函数:STRCAT

const char TAG_VALUE_TYPE_NUMBER = '0';//取值类型:数字
const char TAG_VALUE_TYPE_CHAR   = '1';//取值类型:字符    


class ObjectBase;
//class ReplaceParamMgr;

class CondRule :public CondBase
{
public:
	CondRule();
	virtual ~CondRule();
public:
	virtual bool execute();
	virtual void dump(char *);
	virtual bool judge(vector<CdrSeg> *r_pCdrSegList,
                     vector<CdrSeg> *r_pSepCdrSegList,
                     bool           &r_splitFlag,
                     SumTpRule      &r_sumRuleInfo);
                     
  void splitEnumValue(const char* pEnumValue, vector<T_STR_MIN>&,
	                    map<T_STR_MIN, T_STR_MIN>&);
protected:
	virtual bool judgeCond(T_OBJECT_ATTR &value)  = 0;
	virtual bool judgeCond(long &value)  = 0;
protected:
	bool getMinValue(T_OBJECT_ATTR &value);	
	bool getMaxValue(T_OBJECT_ATTR &value);

	bool getMinValue(long &value);
	bool getMaxValue(long &value);		
public:
	char m_yesOrNo;    //取反标志 0:正常 1:取反
	char m_diviedTag;  //分段标志 0:话单不分段 1:累计量分段 2:非累计量话单分段 
	char m_valueMethed;//取值函数 0:普通 1:数值运算(取模) 2:SUBSTR 3.STRCAT
	char m_valueType;  //值类型   0:数值 1:字符
	char m_valueParam[MIN_STR_LENGTH+1];//取值函数参数
	char m_minValue[MIN_STR_LENGTH+1];  //最小基准值
	char m_maxValue[MIN_STR_LENGTH+1];  //最大基准值
	//added by hany at 2011/3/31 16:48:49
	char m_judgeValue[MIN_STR_LENGTH+1];//判断对象用于表达式		
	char m_judgeMode;//判断方式 0:() 1:(] 2:[) 3:[] 4:IN 5:EXISTS 	
	ObjectBase *m_judgeObj;//判断对象标识
	//Added By Sunam 2009/8/14 9:25:24
	char m_judgeObjType;     //属主类型 0:帐目 1:累计量 2:事件元素 3:资料元素 4:系统级累计账单 5:常量 6:参数 7: 用户属性参数
	ObjectBase *m_minObj;  //最小对象标识
	ObjectBase *m_maxObj;  //最大对象标识	
};

#endif