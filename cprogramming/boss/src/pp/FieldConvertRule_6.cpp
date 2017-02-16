// ##########################################
// Source file : FieldConvertRule_6.cpp
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

#include "FieldConvertRule_6.h"
#include "FieldInfos.h"


//##ModelId=48561791005D
bool FieldConvertRule_6::initialize(string ruleText)
{
    int len,ndx;
	char cTmp[50];
	vector<string> tmpStrs,tmpStrs1;
	
	// rule_6:FIELD=ALL;calling_size=length(msisdn)
    StringUtil::split(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() < 2)
	{
		// 小于1个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
	if (tmpStrs.size() == 2)
	{
	    tmpStrs[0] = tmpStrs[0].substr(7);
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
            else if(strstr(cTmp,"<=")!= NULL)
            {
                m_conditionExp = "<=";
            }
            else if(strstr(cTmp,"=") != NULL)
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
	    // 提取 calling_size=length(msisdn)
	    
	    tmpStrs1.clear();StringUtil::split(tmpStrs[1],",",tmpStrs1);
	    for (int i=0; i<tmpStrs1.size(); i++)
	    {
	        len = tmpStrs1[i].size();
	        if ( ( ndx = tmpStrs1[i].find("=") ) < 0 )
	        {
		        return false;
		    }

	        m_desField.push_back( lrTrim( tmpStrs1[i].substr(0,ndx) ) );
	        tmpStrs1[i] = lrTrim( tmpStrs1[i].substr(ndx+1,len-ndx-1) );

	        // length(msisdn),其实只有括号里的字段才有用,"length"不起作用,实际配置时可以是任意值
            len = tmpStrs1[i].size();
	        if ( ( ndx = tmpStrs1[i].find("(") ) < 0 )
	        {
		        return false;
	        }
	        tmpStrs1[i] = lrTrim( tmpStrs1[i].substr(ndx+1,len-ndx-1) );
	        len = tmpStrs1[i].size();
	        if ( ( ndx = tmpStrs1[i].find(")"))  < 0 )
	        {
		        return false;
	        }

	        m_srcField.push_back( lrTrim( tmpStrs1[i].substr(0,ndx) ) );  // get "msisdn"
	    
        }	
        this->getFieldObj();
        return true;
	}
	else if (tmpStrs.size() == 3)
	{
	    tmpStrs[0] = tmpStrs[0].substr(7);
	    
	    if (tmpStrs[0].compare(0,9,"CALL_TYPE") == 0)
	    {
	        tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
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
                else if(strstr(cTmp,">=")!= NULL)
                {
                    m_conditionExp = ">=";
                }
                else if(strstr(cTmp,"<=") != NULL)
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
	       
        
        // 提取 calling_size=length(msisdn)
        tmpStrs1.clear();StringUtil::split(tmpStrs[2],",",tmpStrs1);
        for (int i=0; i<tmpStrs1.size(); i++)
        {
	    len = tmpStrs1[i].size();
	    if ( ( ndx = tmpStrs1[i].find("=") ) < 0 )
	    {
	        return false;
	    }

	    m_desField.push_back( lrTrim( tmpStrs1[i].substr(0,ndx) ) );
	    tmpStrs1[i]=lrTrim( tmpStrs1[i].substr(ndx+1,len-ndx-1) );

	    // length(msisdn),其实只有括号里的字段才有用,"length"不起作用,实际配置时可以是任意值
        len = tmpStrs1[i].size();
	    if ( ( ndx = tmpStrs1[i].find("(") ) < 0 )
	    {
		    return false;
	    }
	    tmpStrs1[i] = lrTrim( tmpStrs1[i].substr(ndx+1,len-ndx-1) );
	    len = tmpStrs1[i].size();
	    if ( ( ndx = tmpStrs1[i].find(")") ) < 0 )
	    {
		    return false;
	    }

	    m_srcField.push_back(lrTrim( tmpStrs1[i].substr(0,ndx) ));  // get "msisdn"
        }    
	
		this->getFieldObj();
    	return true;
    }
    else
    {
    	this->getFieldObj();
        return false;
    }
}
    

//##ModelId=485617980109
bool FieldConvertRule_6::convertField()
{
    int len,l_field,l_value;
	char tmp[100];
	
	if( FieldConvertRule::generalCondJudge() == false )
	{
		return false;	
	}

    for (int i=0; i<m_srcField.size(); i++)
    {
        sprintf(tmp,"%d",m_pSrcFields[i]->m_iDesValueLen);
        m_pDesFields[i]->setDesValue(tmp);
    }

    return true;
  
}

void FieldConvertRule_6::getFieldObj()
{
    m_pFieldCondition = m_pFieldInfos->getFieldInfoByName(m_conditionFieldName.c_str());  
    
    int i;
    m_pSrcFields.clear();
    for(i=0;i<m_srcField.size();++i)
    {
    	m_pSrcFields.push_back(m_pFieldInfos->getFieldInfoByName(m_srcField[i].c_str()));
    }

	m_pDesFields.clear();
    for(i=0;i<m_desField.size();++i)
    {
    	m_pDesFields.push_back(m_pFieldInfos->getFieldInfoByName(m_desField[i].c_str()));
    }	
   
	return FieldConvertRule::getFieldObj();		
}
