#ifndef _PROCESSORRULE_H_INCLUDE_20080721_
#define _PROCESSORRULE_H_INCLUDE_20080721_
#include "config-all.h"
//#include "cdr/CdrFormat.h"
#include <string>
#include <vector>

USING_NAMESPACE(std);

class InputRuleMgr;
class ProcessBase;
class ProcessorRule
{
  public:
//    CDRJScript     *m_rule;
    string         m_subChanName;        //子进程名称
    vector<string> m_process_name;       //处理过程名
    vector<ProcessBase*> m_pProcessbase; //处理类

  public:
    ProcessorRule();
    virtual ~ProcessorRule();
    ProcessorRule(const ProcessorRule &r_obj);
    bool setProcedures();
    void dump() const;
};
//处理
class ProcessorRuleMgr
{
  public:
    vector<ProcessorRule> m_processorRule;
    ProcessorRule  m_defaultRule;
  public:
    bool initialize(InputRuleMgr *r_inputRuleMgr);
    //ProcessorRule *getProcessorRule(CDR *r_cdr);
    ProcessorRule *getProcessorRule(const string & subname);

    void dump() const;
};
#endif //_PROCESSORRULE_H_INCLUDE_20080721_


