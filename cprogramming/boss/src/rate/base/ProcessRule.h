#ifndef _PROCESSRULE_H_INCLUDE_20080618_
#define _PROCESSRULE_H_INCLUDE_20080618_
#include "config-all.h"
#include "cdr/CdrFormat.h"
#include <string>
#include <vector>

using namespace std;

class SysParam;
class CDRJScript;
class RateConfig;
class RateCdr;
class RateInfo;
class Procedure;
class ProcessRule 
{
  public:
    CDRJScript     *m_rule;
    vector<string> m_procedures;
    string         m_baseTpFactors;
    string         m_rateFeeIdFactors;
    string         m_acctFeeIdFactors;
    
    vector<Procedure*> m_pProcedures;
    vector<int>    m_baseTpFactorIds;
    vector<int>    m_rateFeeIdFactorIds;
    vector<int>    m_acctFeeIdFactorIds;
  public:
    void clear();
    ProcessRule();
    virtual ~ProcessRule();
    ProcessRule(const ProcessRule &r_obj);
    void parseIdStr();
    bool setProcedures();
    void dump() const;
  protected:
    void parseIdStr(const string &r_IdStr,vector<int> &r_IdList);
};
//处理
class ProcessRuleMgr 
{
  public:
    vector<ProcessRule> m_rules;
    ProcessRule  m_defaultRule;
  public:
    bool initialize(SysParam *r_sysparam,const char* r_appName,
                    const int &r_channelNo,
                    const CdrAttribMacros &r_attribMacros,
                    RateConfig *r_config);
    //matche process rule by cdr and rateinfo
    ProcessRule *getProcessRule(RateCdr *r_cdr);
    
    void dump() const;
};
#endif //_PROCESSRULE_H_INCLUDE_20080618_



