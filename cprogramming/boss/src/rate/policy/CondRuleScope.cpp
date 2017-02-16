#include "CondRuleScope.h"

//操作符号   ()
bool CondRuleScope::judgeCond(T_OBJECT_ATTR &value)
{
	T_OBJECT_ATTR minValue,maxValue;
	
	if(getMinValue(minValue))
	{		
		if(value <=minValue)
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

bool CondRuleScope::judgeCond(long & value)
{
	long minValue,maxValue;
	
	if(getMinValue(minValue))
	{		
		if(value <=minValue)
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

void CondRuleScope::dump(char *prefix)
{
	char myPrefix[256] = "  ";
	
	strcat(myPrefix,prefix);
	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"----CondRuleScope : " <<m_id<<" ----" <<endl;	
	CondRule::dump(myPrefix);
	cout <<myPrefix<<"}" <<endl;
}
