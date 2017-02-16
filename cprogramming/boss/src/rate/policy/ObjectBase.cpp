#include "ObjectBase.h"
#include "NodeBase.h"
#include "RateGlobalInfo.h"

extern RateGlobalInfo g_globalInfo;
RateGlobalInfo * ObjectBase::m_globalInfo = &g_globalInfo;
PolicyInfo *ObjectBase::m_policyInfo = &NodeBase::m_policyInfo;

void ObjectBase::dump(char *prefix)
{
	cout <<prefix<<"m_id         = "<< m_id <<endl;
	cout <<prefix<<"m_attrId     = "<< m_attrId<<endl;
	cout <<prefix<<"m_attrType   = "<< m_attrType<<endl;	
}
