
#ifndef __PROCEDURE_H__
#define __PROCEDURE_H__

#include "config-all.h"

#include "FieldsXX.h"
#include "RateCdr.h"

#include <map>
#include <string>

class Processor;
class RateGlobalInfo;
class PMManager;
class RateConfig;
class BillManager;
/**
 * 计费过程类
 */
class Procedure 
{
    friend class Processor;
  public:
    Procedure();
    virtual ~Procedure();
    void setRateInfoObj(RateGlobalInfo *r_pRateInfo);
    
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
    RateGlobalInfo  *m_pRateInfo;
    PCDR             m_pcdr;
    PMManager       *m_pmManager;
    RateConfig      *m_pRateConfig;
    BillManager     *m_billMgr;
  public:  
    string m_className;
    string m_arguments;
};






//------------------------------------------------------------------------------
class ProcedureHelper 
{
  public:
    static map<string,Procedure*> *GetProcedures();
    static Procedure * GetProcedure(const string & name );
    static int SetProcedure(const string &name,Procedure * procedure);
    static int ExecuteProcedure(const string &name);
    //不带参数方式执行
    static int ExecuteProcedure(Procedure* pProc);
};

/*
#define INSTALL_PROCEDURE_INSTANCE(classname) \
int classname##InstallTemp = ProcedureHelper::SetProcedure(#classname,new classname());
*/

#define INSTALL_PROCEDURE_INSTANCE(classname) \
int classname##InstallTemp = ProcedureHelper::SetProcedure(#classname,new classname());

#endif




