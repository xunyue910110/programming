#ifndef _CYCLERULE_H_
#define _CYCLERULE_H_

#include "ParamBase.h"

class CycleRule : public ParamBase
{
public:
	CycleRule(){}
	~CycleRule(){}
public:
	unsigned short m_cycleType;
	unsigned short m_cycleNumber;
	unsigned short m_cycleOffset;
};

#endif
