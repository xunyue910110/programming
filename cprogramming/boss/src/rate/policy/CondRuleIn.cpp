#include "CondRuleIn.h"
#include "PolicyInfo.h"
#include "base/exceptions.h"
#include "RateGlobalInfo.h"

//操作符号   in  1,2,10-20,30,40  in ?1,2-4,?5
bool CondRuleIn::judgeCond(T_OBJECT_ATTR &value)
{	
	//const char *pEnumValue = getEnumValue();
	vector<const T_STR_MIN *>::iterator iter;
	map<T_STR_MIN,T_STR_MIN>::iterator iterMap;
	vector<const T_STR_MIN *> enumValue;
    
    //Modified By Sunam 2009/9/12 10:27:32
	//if(pEnumValue!=NULL)
	//{
	//	splitEnumValue(pEnumValue, m_enumVector, m_enumMap);
	//}
	       
	//for(iter=m_enumVector.begin(); iter!=m_enumVector.end(); ++iter)
	//{
	//	if((*iter) == value)
	//		return true;
	//}
	getEnumValue(enumValue);
	for(iter=enumValue.begin(); iter!=enumValue.end(); ++iter)
	{
		if((*(*iter)) == value)
			return true;
	}	
	
	for(iterMap=m_enumMap.begin(); iterMap!=m_enumMap.end(); ++iterMap)
	{
		if(value>=(*iterMap).first && value<=(*iterMap).second)
			return true;
	}
	
	return false;
}

const char * CondRuleIn::getEnumValue()
{
	if(m_enumValue[0] == '?') //需要替换参数
	{	  
		T_TP_ID r_paramId;
		const char *paramValue;
    	
		r_paramId = &m_enumValue[1];
		paramValue = m_policyInfo.m_rateTpInfo->getParamValue(r_paramId);
		if (paramValue == NULL)
		{
			THROW_C_P2(BusinessException, -1, "条件配置错误,未找到替换参数："
			                                  "cond_id=%ld,enum_value=%ld",
			                                   (long)m_id, (long)r_paramId);
		}
		return paramValue;
	}
	else if (strlen(m_enumValue)>3&&strncmp(m_enumValue,"EXP",3)==0) //表达式
	{
		char *expr;
		expr = &m_enumValue[3];
		long value=0;
		if (!m_globalInfo->explanExpr(value,expr,m_policyInfo.m_rateTpInfo))
		{
			THROW_C_P2(BusinessException, -1, "条件配置错误,表达式解析错误："
			                                  "cond_id=%ld,enum_value=%s",(long)m_id, m_enumValue);
		}
		
		char t_str[MIN_STR_LENGTH];
		sprintf(t_str,"%ld",value);
		const char *paramValue = t_str;
		
		return paramValue;
	}
	else if (strlen(m_enumValue)>1&&m_enumValue[0] == '#') //用户属性USER_ITEM
	{
		T_TP_ID r_paramId;
		const char *paramValue;
		r_paramId = &m_enumValue[1];		
		m_globalInfo->m_idInfo.init(m_globalInfo->m_cycParam.m_beginTime,
		                       m_globalInfo->m_cycParam.m_endTime,
		                       m_policyInfo.m_rateTpInfo->m_eventType);
		paramValue = m_globalInfo->m_idInfo.getParamValue(r_paramId);
		if (paramValue == NULL)
		{
			THROW_C_P2(BusinessException, -1, "条件配置错误,未找到替换参数："
			                                  "cond_id=%ld,enum_value=%ld",
			                                   (long)m_id,(long)r_paramId);
		}
		return paramValue;	
	}
	else
		return NULL;
}

//Added By Sunam 2009/9/12 10:23:02
int CondRuleIn::getEnumValue(vector<const T_STR_MIN *> &vValue)
{
    vector<T_STR_MIN>::iterator iter;
    T_TP_ID r_paramId;
    const char *paramValue;
    ObjectBase *t_obj;
	
	for (iter=m_enumVector.begin(); iter!=m_enumVector.end(); ++iter)
	{
	    //需要替换参数
	    if ((*iter)[0] == '?')
	    {
	      r_paramId = &(*iter)[1];
		    paramValue = m_policyInfo.m_rateTpInfo->getParamValue(r_paramId);
		    if (paramValue == NULL)
		    {
		    	THROW_C_P2(BusinessException, -1, "条件配置错误,未找到替换参数："
		    	                                  "cond_id=%ld,enum_value=%ld",
		    	                                   (long)m_id, (long)r_paramId);
		    }
		    vValue.push_back((const T_STR_MIN *)paramValue);
	    }
	    else if ((*iter)[0] == '*')
		{
			T_OBJECT_ATTR t_value;
			r_paramId = &(*iter)[1];
			t_obj = m_globalInfo->m_paramMgr->getObjectByObjId(r_paramId);
            //找到
            if (t_obj != NULL)
            {
            	t_obj->getObjectValue(t_value);
            	vValue.push_back((const T_STR_MIN *)t_value.c_str());
            }
            else
            {
            	THROW_C_P2(BusinessException, -1, "条件配置错误,未找至对象：cond_id=%ld,m_enumValue=%ld",(long)m_id, (long)r_paramId);
            }
		}
	    else if ((*iter).length()>3&&strncmp((*iter).c_str(),"EXP",3)==0) //表达式
	    { 
		    char *expr;
			  expr = &(*iter)[3];
				long value=0;
				if (!m_globalInfo->explanExpr(value,expr,m_policyInfo.m_rateTpInfo))
				{
					THROW_C_P2(BusinessException, -1, "条件配置错误,表达式解析错误："
			                                  "cond_id=%ld,enum_value=%ld",(long)m_id, (long)r_paramId);
				}
		
				char t_str[MIN_STR_LENGTH];
				sprintf(t_str,"%ld",value);
				const char *paramValue = t_str;
		
		   vValue.push_back((const T_STR_MIN *)paramValue);
	    }
	    else if ((*iter).length()>1&&(*iter)[0] == '#') //用户属性USER_ITEM
			{
				T_TP_ID r_paramId;
				const char *paramValue;
				long value=0;
				r_paramId = &(*iter)[1];		
				m_globalInfo->m_idInfo.init(m_globalInfo->m_cycParam.m_beginTime,
				                       m_globalInfo->m_cycParam.m_endTime,
				                       m_policyInfo.m_rateTpInfo->m_eventType);
				paramValue = m_globalInfo->m_idInfo.getParamValue(r_paramId);
				if (paramValue == NULL)
				{
				THROW_C_P2(BusinessException, -1, "条件配置错误,未找到替换参数："
			                                  "cond_id=%ld,enum_value=%ld",
			                                   (long)m_id, (long)r_paramId);
				}
							
		    vValue.push_back((const T_STR_MIN *)paramValue);
				
			}
	    else
	    {
	        vValue.push_back(&(*iter));
	    }
	}
	return vValue.size();
}

bool CondRuleIn::judgeCond(long & value)
{
	//const char *pEnumValue = getEnumValue();
	vector<const T_STR_MIN *>::iterator iter;
	map<T_STR_MIN,T_STR_MIN>::iterator iterMap;
	vector<const T_STR_MIN *> enumValue;

    //Modified By Sunam 2009/9/12 10:27:32
	//if(pEnumValue!=NULL)
	//{
	//	splitEnumValue(pEnumValue, m_enumVector, m_enumMap);
	//}
	//
	//for(iter=m_enumVector.begin(); iter!=m_enumVector.end(); ++iter)
	//{
	//	if((*iter).value() == value)
	//		return true;
	//}	
	getEnumValue(enumValue);
	for(iter=enumValue.begin(); iter!=enumValue.end(); ++iter)
	{
		if((*(*iter)).value() == value)
			return true;
	}	
	
	for(iterMap=m_enumMap.begin(); iterMap!=m_enumMap.end(); ++iterMap)
	{
		if(value>=(*iterMap).first.value() && value<=(*iterMap).second.value())
			return true;
	}
	
	return false;
}

void CondRuleIn::dump(char *prefix)
{
	char myPrefix[256] = "  ";
	
	strcat(myPrefix,prefix);	
	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"----CondRuleIn : " <<m_id<<" ----" <<endl;	
	CondRule::dump(myPrefix);
	cout <<myPrefix<<"m_enumValue   = "<< m_enumValue  <<endl;
	cout <<myPrefix<<"}" <<endl;
}
