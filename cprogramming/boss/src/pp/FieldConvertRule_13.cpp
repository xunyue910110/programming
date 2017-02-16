// ##########################################
// Source file : FieldConvertRule_13.cpp
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080926
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#include "FieldConvertRule_13.h"
#include "FieldInfo.h"
#include "FieldInfos.h"
#include <stdlib.h>

//##ModelId=48561F5C0261
//用于提供内容计费业务列表合并: serviceid|dataVolumeGPRSUp|dataVolumeGPRSDn
//          业务代码|上行流量|下行流量 的顺序不能改变
bool FieldConvertRule_13::initialize(string ruleText)
{
    char cTmp[50];
	vector<string> tmpStrs,tmpStrs1,tmpStrs2,tmpStrs3;

    //serviceid|dataVolumeGPRSUp|dataVolumeGPRSDn(业务代码|上行流量|下行流量)的顺序不能改变
    //RULE_13:FIELD=ALL;cbc=serviceid|dataVolumeGPRSUp|dataVolumeGPRSDn
	StringUtil::split(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() < 2)
	{
		// 小于1个分号的,不符合规则的书写格式
		return false;	
	}
	
	// 提取规则中的信息
	if (tmpStrs.size() == 2)
	{
	    tmpStrs[0] = tmpStrs[0].substr(8);
	    
	    if (tmpStrs[0].compare(0,5,"FIELD") == 0)
	    {
	        m_callType = "ALL";
	        m_conditionFieldName = "ALL";
	    }
	    else if (tmpStrs[0].compare(0,9,"CALL_TYPE") == 0)
	    {
	        tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
	        m_callType = tmpStrs1[1];
	        m_conditionFieldName = "ALL";
	    }
	    else
	    {
	        // 得到条件中的运算符
            strcpy(cTmp,tmpStrs[0].c_str());
            
            if(strstr(cTmp,"!=")!=NULL) 
            {
                m_conditionExp = "!=";
            } 
            else if(strstr(cTmp,">=")!=NULL) 
            {
                m_conditionExp = ">=";
            } 
            else if(strstr(cTmp,"<=")!=NULL) 
            {
                m_conditionExp = "<=";
            } 
            else if(strstr(cTmp,"=")!=NULL) 
            {
                m_conditionExp = "=";
            } 
            else 
            {
                return false;
            }
            // 根据前面求出的运算符,分割条件字段,取得运算符左右键值
            tmpStrs1.clear();StringUtil::split(tmpStrs[0],m_conditionExp.c_str(),tmpStrs1);
            if(tmpStrs1.size()!=2)
            {
        	    return false;	
            }
            else
            {
	            m_conditionFieldName = tmpStrs1[0];
	            m_conditionFieldValue = tmpStrs1[1];
	            m_callType = "ALL";
            }
	    }
	    // 得到规则13中的具体操作部分:cbc=cbcServiceID|dd|cbcUp|cbcDown
	    tmpStrs2.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs2);
	    if( tmpStrs2.size() != 2 )
        {
            return false;
        }
	    m_cbc = lrTrim(tmpStrs2[0]);
	    tmpStrs3.clear();StringUtil::split(tmpStrs2[1],"|",tmpStrs3);//得到serviceid|dataVolumeGPRSUp|dataVolumeGPRSDn
	    if( tmpStrs3.size() != 4 )
        {
            return false;
        }
        m_serviceIdName = lrTrim(tmpStrs3[0]);
        m_DataUpName    = lrTrim(tmpStrs3[1]);
        m_DataDnName    = lrTrim(tmpStrs3[2]);
        m_DurationName  = lrTrim(tmpStrs3[3]);
		
		this->getFieldObj();
	    return true;
	}
	else if (tmpStrs.size() == 3)
	{
	    tmpStrs[0] = tmpStrs[0].substr(8);
	    
	    if (tmpStrs[0].compare(0,9,"CALL_TYPE") ==0)
	    {
	        tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
	        if (tmpStrs1.size() != 2)
	        {
	            return false;
	        }
	        else
	        {
	            m_callType = tmpStrs1[1];
	        }
	    }
	    else
	    {
	        return false;
	    }
	    
	    
	    if (tmpStrs[1].compare(0,9,"CALL_TYPE") == 0)
	    {
	        tmpStrs1.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs1);
	        if (tmpStrs1.size() != 2)
	        {
	            return false;
	        }
	        else
	        {
	            m_callType = tmpStrs1[1];
	        }
	    }
	    else
	    {
	        // 得到条件中的运算符
            strcpy(cTmp,tmpStrs[1].c_str());
            
            if(strstr(cTmp,"!=")!=NULL) 
            {
                m_conditionExp = "!=";
            } 
            else if(strstr(cTmp,">=")!=NULL) 
            {
                m_conditionExp = ">=";
            } 
            else if(strstr(cTmp,"<=")!=NULL) 
            {
                m_conditionExp = "<=";
            } 
            else if(strstr(cTmp,"=")!=NULL) 
            {
                m_conditionExp = "=";
            } 
            else 
            {
                return false;
            }
            // 根据前面求出的运算符,分割条件字段,取得运算符左右键值
            tmpStrs1.clear();StringUtil::split(tmpStrs[1],m_conditionExp.c_str(),tmpStrs1);
            if(tmpStrs1.size()!=2)
            {
        	    return false;	
            }
            else
            {
	            m_conditionFieldName = tmpStrs1[0];
	            m_conditionFieldValue = tmpStrs1[1];
            }
	    }
	    
	    // 得到规则13中的具体操作部分:cbc=cbcServiceID|cbcUp|cbcDown|dd
	    tmpStrs2.clear();StringUtil::split(tmpStrs[2],"=",tmpStrs2);
	    if( tmpStrs2.size() != 2 )
        {
            return false;
        }
	    m_cbc = lrTrim(tmpStrs2[0]);
	    tmpStrs3.clear();StringUtil::split(tmpStrs2[1],"|",tmpStrs3);//得到cbcServiceID|cbcUp|cbcDown|dd
	    if( tmpStrs3.size() != 4 )
        {
            return false;
        }
        m_serviceIdName = lrTrim(tmpStrs3[0]);
        m_DataUpName    = lrTrim(tmpStrs3[1]);
        m_DataDnName    = lrTrim(tmpStrs3[2]);
        m_DurationName  = lrTrim(tmpStrs3[3]);
		
		this->getFieldObj();
	    return true;
	}
	else
	{
	    return false;
	}
}


//##ModelId=48561F640186
bool FieldConvertRule_13::convertField()
{
    int i;
    string strCbcInfo;

	if( FieldConvertRule::generalCondJudge() == false )
	{
		return false;	
	}
	
	// 把字段组合成为：serviceId|dataVolumeGPRSUp|dataVolumeGPRSDn
	for( i=0;i<m_pServiceID->getNumCount();++i)
	{
		if(i>0)
		{
		    strCbcInfo +=";";
		}		
		strCbcInfo += string(m_pServiceID->m_fieldInfoVect[i]->m_FieldDesValue) + "|" 
		            + m_pDataFlowUps->m_fieldInfoVect[i]->m_FieldDesValue + "|" 
		            + m_pDataFlowDns->m_fieldInfoVect[i]->m_FieldDesValue + "|" 
		            + m_pCbcDu->m_fieldInfoVect[i]->m_FieldDesValue + "|0";
    }
	m_pFieldCbcInfo->setDesValue(strCbcInfo);
	
	return true;
}

void FieldConvertRule_13::getFieldObj()
{
    m_pFieldCondition = m_pFieldInfos->getFieldInfoByName(m_conditionFieldName.c_str());  
    m_pFieldCbcInfo   = m_pFieldInfos->getFieldInfoByName(m_cbc.c_str()); 

    m_pServiceID   = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName(m_serviceIdName.c_str()) );  
	m_pDataFlowUps = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName(m_DataUpName.c_str()) );  
	m_pDataFlowDns = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName(m_DataDnName.c_str()) );  
	m_pCbcDu       = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName(m_DurationName.c_str()) );  
	
	return FieldConvertRule::getFieldObj();		
}

