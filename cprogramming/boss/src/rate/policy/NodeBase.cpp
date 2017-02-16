#include "NodeBase.h"
#include "RateGlobalInfo.h"
#include "PolicyInfo.h"

extern RateGlobalInfo g_globalInfo;
PolicyInfo NodeBase::m_policyInfo;
RateGlobalInfo * NodeBase::m_globalInfo = &g_globalInfo;

bool  NodeBase::judge(vector<CdrSeg> *r_pCdrSegList,
                      vector<CdrSeg> *r_pSepCdrSegList,
                                bool &r_splitFlag,
                           SumTpRule &r_sumRuleInfo)
{
  return true;
}
                   
bool NodeBase::rateExecute(const vector<CdrSeg> &r_pCdrSegList)
{
  return true;
}

