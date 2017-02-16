#ifndef _POLICYINFO_H_
#define _POLICYINFO_H_

#include "PriceParamImpl.h"
#include "PriceExtend.h"
#include "FieldsXX.h"
#include "TpInfo.h"


class RateGlobalInfo;
class FeePolicyComp;

class PolicyInfo 
{
public:	
	PolicyInfo();
	virtual ~PolicyInfo(){}
	//参数信息
	
	//FeePolicyComp
	const FeePolicyComp * m_feePolicyComp;
	
	//Price
	const Price * m_price;
	
	//批价政策实现信息
	RateTpInfo * m_rateTpInfo;	
	
	RateGlobalInfo *m_globalInfo;
};

#endif
	