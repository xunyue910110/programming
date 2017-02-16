// ##########################################
// Source file : FieldConvertRule_8.cpp
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

#include "FieldConvertRule_8.h"
#include "RecordFormat.h"
#include "FieldInfos.h"

//##ModelId=485618870109

// filed2的字段值是"200#0#02#02"，此规则用来得到其中的子字段值
bool FieldConvertRule_8::initialize(string ruleText)
{
    vector<string> tmpStrs,tmpStrs1;
   	char cTmp[50];

   	// rule_8:CALL_TYPE=01;field2;\#;4;Call_Type
   	// rule_8:MSRN_FLAG=91;field2;\#;4;Call_Type
   	// filed2的字段值是"200#0#02#02"，此规则用来得到其中的子字段值。
	StringUtil::split(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() < 5)
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}

	// 提取规则中的信息
	
	if (tmpStrs.size() == 5)
	{
	    tmpStrs[0] = tmpStrs[0].substr(7);
	    if (tmpStrs[0].compare(0,5,"FIELD")==0)
	    {
	        m_callType = "ALL";
	        m_conditionFieldName = "ALL";
	    }
	    else if (tmpStrs[0].compare(0,9,"CALL_TYPE")==0)
	    {
            tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
            if(tmpStrs1.size()!=2)
            {
        	    return false;	
            }
            else
            {
                m_callType = tmpStrs1[1];
                m_conditionFieldName = "ALL";
            }	
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
	    m_compositeFieldName = tmpStrs[1];
	    m_postion = atoi(tmpStrs[3].c_str());
	    m_dstFieldName = tmpStrs[4];
	    
	    strcpy(m_myToken,tmpStrs[2].c_str());
	    StringUtil::eraseChar(m_myToken,'\\');

        this->getFieldObj();
        return true;
	}
	else if (tmpStrs.size() == 6)
	{
	    tmpStrs[0] = tmpStrs[0].substr(7);
	    
	    if (tmpStrs[0].compare(0,9,"CALL_TYPE")==0)
	    {
	        tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
	        if (tmpStrs1.size() != 2)
	        {
	            return false;
	        }
	        m_callType = tmpStrs1[1];
	    }
	    else
	    {
	        return false;	  
	    }
	    
	    strcpy(cTmp,tmpStrs[1].c_str());
	    
	  
	        // 得到条件中的运算符
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
            
            m_compositeFieldName = tmpStrs[2];
            
            m_postion = atoi(tmpStrs[4].c_str());
            
            m_dstFieldName = tmpStrs[5];
            
            strcpy(m_myToken,tmpStrs[3].c_str());
	        StringUtil::eraseChar(m_myToken,'\\');

            this->getFieldObj();
            return true; 
       }
       else
       {
           return false;
       }
}

//##ModelId=4856188D0232
bool FieldConvertRule_8::convertField()
{
    int i,l_field,l_value;
    char *tok;
    char tmp[100];

	if( FieldConvertRule::generalCondJudge() == false )
	{
		return false;	
	}

    if( m_pCompField->m_iDesValueLen==0 )
    {
	    return false;
	}

	i=0;
	strcpy(tmp,m_pCompField->m_FieldDesValue);
	tok=strtok(tmp,m_myToken);
   	while(tok!=NULL) 
   	{
   		if(i==m_postion) 
   		{
   			m_pDesField->setDesValue(tok);
   			break;
   		}
   		tok=strtok(NULL,m_myToken);
		i++;
   	}
   	
    return true;
}

void FieldConvertRule_8::getFieldObj()
{
    m_pFieldCondition = m_pFieldInfos->getFieldInfoByName(m_conditionFieldName.c_str());  
	m_pCompField      = m_pFieldInfos->getFieldInfoByName(m_compositeFieldName.c_str());
	m_pDesField       = m_pFieldInfos->getFieldInfoByName(m_dstFieldName.c_str());
	
	return FieldConvertRule::getFieldObj();		
}

