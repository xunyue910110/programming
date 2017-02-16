#ifndef __PROCESSBASE_H_INCLUDE_20080721_
#define __PROCESSBASE_H_INCLUDE_20080721_

#include "config-all.h"
#include "base/supdebug.h"

#include "FieldsXX.h"
#include "cdr/CdrBase.h"
#include "ConstDef.h"
#include "FilterGlobalInfo.h"
#include <map>
#include <string>

class Processorall;
class FilterGlobalInfo;
class FilterConfig;
class OutChanInfo;
class PMGlobalManager;
/*
 * 计费过程类
 */
class ProcessBase
{
    friend class Processorall;
  public:
    ProcessBase();
    virtual ~ProcessBase();
    void setGlobalInfo(FilterGlobalInfo *r_pFilterInfo);

	/**
	 * 初始化该过程
	 * 为该过程对象准备执行的环境信息；
	 * 该方法只在程序启动时被调用执行。
	 *
	 * @return
	 */
    virtual bool initialize();

	/**
	 * 对过程每次执行前进行复位或资源释放操作
	 * 需要执行父类方法。
	 */
    virtual int reset();

	/**
	 * 执行过程
	 *
	 */
    virtual int execute();

  protected:
    FilterGlobalInfo    *m_FilterInfo;
    PCDR                m_pcdr;
    FilterCdr           m_cdr;
    FilterConfig        *m_Config;
    PMGlobalManager    *m_globalManager;

  public:
    string m_className;
    string m_arguments;
};


//------------------------------------------------------------------------------
class ProcessBaseHelper
{
  public:
    static map<string,ProcessBase*> *GetProcedures();
    static ProcessBase * GetProcedure(const string & name);
    static int SetProcedure(const string &name,ProcessBase * processbase);
    static int ExecuteProcedure(const string &name);
    //不带参数方式执行
    static int ExecuteProcedure(ProcessBase* pProc);
};


#define INSTALL_PROCESSBASE_INSTANCE(classname) \
int classname##InstallTemp = ProcessBaseHelper::SetProcedure(#classname,new classname());


#endif //__PROCESSBASE_H_INCLUDE_20080721_

