#ifndef _REPLACEPARAMMGR_H_
#define _REPLACEPARAMMGR_H_

#include "base/Types.h"

class GlobalInfo;

class ReplaceParamMgr 
{
public:
	ReplaceParamMgr(){}
	~ReplaceParamMgr(){}
public:
	bool initialize(GlobalInfo*);
	bool replaceParam(char* pParam,int& value); 
	bool replaceParam(char* pParam,AttributeValue& attributeValue); 
public:	
	GlobalInfo* m_globalInfo;
};

#endif