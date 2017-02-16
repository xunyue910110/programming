#include "CondRuleScopeL.h"

//操作符号   [)
bool CondRuleScopeL::judgeCond(T_OBJECT_ATTR &value)
{
	T_OBJECT_ATTR minValue,maxValue;
	
	if(getMinValue(minValue))
	{		
		if(value<minValue)
			 return false;
	}
	else
	{
		//error deal
		return false;
	}

	if(getMaxValue(maxValue))
	{		
		if(value >= maxValue)
			 return false;
		else
			return true;
	}
	else
	{
		//error deal
		return false;
	}	
	
	return false;
}

bool CondRuleScopeL::judgeCond(long &value)
{
	long minValue,maxValue;
	
	if(getMinValue(minValue))
	{		
		if(value<minValue)
			 return false;
	}
	else
	{
		//error deal
		return false;
	}

	if(getMaxValue(maxValue))
	{		
		if(value >= maxValue)
			 return false;
		else
			return true;
	}
	else
	{
		//error deal
		return false;
	}	
	
	return false;
}

void CondRuleScopeL::dump(char *prefix)
{
	char myPrefix[256] = "  ";
	
	strcat(myPrefix,prefix);	
	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"----CondRuleScopeL : " <<m_id<<" ----" <<endl;	
	CondRule::dump(myPrefix);
	cout <<myPrefix<<"}" <<endl;
}
