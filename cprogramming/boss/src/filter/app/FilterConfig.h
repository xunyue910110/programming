#ifndef _FILTERCONFIG_H_INCLUDE_200800709_
#define _FILTERCONFIG_H_INCLUDE_200800709_

#include "base/SysParam.h"
#include "base/StringUtil.h"

#include "cdr/CdrFmtConfig.h"
#include "cdr/CDRJScript.h"
#include "FilterRule.h"
#include "ProcessorRule.h"
#include "MdbInfo.h"

class SysParam;
class FilterConfig
{
  public:
    FilterConfig();
    virtual ~FilterConfig();
  public:
    //m_cdrFmtConf.m_attribMacros ::CdrAttribMacros宏定义:字段属性ATTRI_开头
    //m_cdrFmtConf.m_cdrFormatList ::vector<CdrFormat>:话单格式定义
    CdrFmtConfig       m_cdrFmtConf;   //话单格式配置信息

    string             m_appName;       //进程名
    int                m_channelNo;     //通道号
    string             m_channelType;   //通道类型
    InputRuleMgr       m_inputRuleMgr;  //输入规则
    OutputRuleMgr      m_outputRuleMgr; //输出规则
    DividRuleMgr       m_dividRuleMgr;   //排重提取规则
    ChannelRuleMgr     m_channelRuleMgr; //分通道字段提取规则
    ProcessorRuleMgr   m_processorRuleMgr;  //处理规则

    //小的脚本片断(主要用于评估某个表达式)
	  map<string,CDRJScript *> m_scriptApplets;

	  int                m_getAHomeMethod;        //求A归属地方法
	  int                m_getAVisitMethod;       //求A到访地方法
	  int                m_getGprsAVisitMethod;   //GPRS求A到访地方法
	  int                m_getBHomeMethod;        //求B归属地方法
	  int                m_getBVisitMethod;       //求B到访地方法
	  int                m_getRoAHomeMethod;      //漫游求A归属地方法
	  int                m_getRoAVisitMethod;     //漫游求A到访地方法
	  int                m_getRoBHomeMethod;      //漫游求B归属地方法
	  int                m_getRoBVisitMethod;     //漫游求B到访地方法
	  int                m_getProvEdgeRoamMethod;   //求省内边界漫游方法
	  int                m_getRoamOutEdgeRoamMethod;//求出省边界漫游方法

    int                m_default_bindMethod; //绑定资料方式
    int                m_getLocalMmsMethod;  //本地彩信求解属性方法
    int                m_statusAuditMethod;  //不同时间稽核方式
    int                m_svcAllowance;       //服务许可稽核方式
    int                m_delayTime;          //m_statusAuditMethod求取中延时时间 单位秒
    string             m_errorPath;          //无效目录
    char               m_default_separate;   //默认字段间分隔符
	  int                m_file_sort_method;   //文件排序方式
	  char               m_isVerifyFee;        //SP话单是否稽核 Y-稽核 其余否

    string             m_dbServName;         //DB实例名称
    string             m_dbUser;             //DB用户
    string             m_dbPassword;         //DB口令
    int                m_interval;           //DB自动刷新周期,单位秒
    string             m_provinceCode;       //省代码
    string             m_areaCode;           //省长途区号
    int                m_operatorType;       //运营商类型 1:CMCC 2:UNICOM 3: CT
    MdbInfo            m_mdbInfo;            //内存数信息
    
    map<string,int>    m_smsProcessMethod;   //短信处理方法
    map<string,const CdrFormat*> m_specialOutField;  //通道输出指定格式

    //add by shenglj 2009-08-12 20:37:01
    vector<string>     m_resetTimeSourceVector;  //延迟时间数据源合集
    int                m_resetTimeMethod;        //不同开始时间的选择方式
    map<string,int>    m_resetDelayTimeMap;      //针对不同数据源设定不同的延时秒数
  public:
    bool getSysParam(SysParam *r_sysParam,const char * r_appName,const int &r_channelNo);

    CDRJScript* setScriptApplet(const string &script);
    CDRJScript* getScriptApplet(const string &script);

    bool valid();//有效性检查
    void dump();
};
#endif //_FILTERCONFIG_H_INCLUDE_200800709_
