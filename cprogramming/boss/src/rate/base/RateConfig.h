#ifndef _RATECONFIG_H_INCLUDE_20080619_
#define _RATECONFIG_H_INCLUDE_20080619_
#include "cdr/CdrFmtConfig.h"
#include "InoutputRule.h"
#include "ProcessRule.h"
#include "cdr/CDRJScript.h"
#include "MdbInfo.h"
#include "RateConstDef.h"


class SysParam;
class RateLog;
class CDRJScript;
class RateConfig
{
  public:
    RateConfig();
    virtual ~RateConfig();
  public:
    CdrFmtConfig       m_cdrFmtConf;   //话单格式配置信息
    
    string             m_appName;       //进程名
    int                m_channelNo;     //通道号
    string             m_channelType;   //通道类型
    int                m_cycleDbNo;     //对应的帐期表库号
    //Added By Sunam 2009/9/19 11:19:05 对应的账期表 EparchyCode
    PString            m_eparchyCode;
    PString            m_workPath;      //工作目录
    bool               m_drFlag;        //是否支持容灾系统
    bool               m_ocsFlag;       //是否支持OCS系统
    CDRJScript        *m_ocsOutPutCdrRule;//OCS话单输出规则

    InputRuleMgr       m_inputRuleMgr;  //输入规则
    OutputRuleMgr      m_outputRuleMgr; //输出规则
    ProcessRuleMgr     m_processRuleMgr;//处理规则
    // 小的脚本片断(主要用于评估某个表达式)
	  map<string,CDRJScript *> m_scriptApplets;
    
    int                m_doubleCycMode; //1 双帐期模式、2帐期延迟模式
    OPERATOR_TYPE      m_operatorType;  //运行商类型
    //磁盘数据库信息
    string             m_dbUser;          //DB用户
    string             m_dbPassword;      //DB口令
    string             m_dbServName;      //DB实例名称
    //内存数据库信息
    MdbInfo            m_mdbInfo;          //缺省Mdb信息
    MdbInfo            m_mdbInfo_userInfo; //用户资料MDB
    MdbInfo            m_mdbInfo_billInfo; //账单信息MDB
    
    int                m_interval;        //更新间隔
    string             m_provinceCode;    //省代码
  public:
    bool getSysParam(SysParam *r_sysParam,const char * r_appName,const int &r_channelNo);
    CDRJScript* setScriptApplet(const string &script);
    CDRJScript* getScriptApplet(const string &script);
    bool valid();//有效性检查

    void dump() const;
};
#endif //_RATECONFIG_H_INCLUDE_20080619_
