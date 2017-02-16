#include "PolicyInfo.h"
#include "RateGlobalInfo.h"


extern RateGlobalInfo g_globalInfo;

PolicyInfo::PolicyInfo()
{
  m_globalInfo = &g_globalInfo;
}



