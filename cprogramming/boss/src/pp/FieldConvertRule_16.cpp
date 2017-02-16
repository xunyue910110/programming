// ##########################################
// Source file : FieldConvertRule_16.cpp
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080903
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#include "FieldConvertRule_16.h"
#include "FieldInfo.h"
#include "FieldInfos.h"
#include <unistd.h>
#include <sys/stat.h>

//##ModelId=485626B500FA
//##查找列表类型数据中的第N个，如果N＝-1,则查找最后一个
bool FieldConvertRule_16::initialize(string ruleText)
{
    int len,ndx;
	char cTmp[100];
	vector<string> tmpStrs,tmpStrs1;
	
	// rule_18:FIELD=ALL;seq=find(seqNumber,1)
	StringUtil::split(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() < 2)
	{
		// 小于1个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
	if (tmpStrs.size() == 2)
	{
        tmpStrs[0]=tmpStrs[0].substr(8);
        if (tmpStrs[0].compare(0,5,"FIELD") == 0)
        {
            m_callType = "ALL";
            m_conditionFieldName = "ALL";
        }
        else if(tmpStrs[0].compare(0,9,"CALL_TYPE") == 0)
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
        
        // 提取 seq=find(seqNumber,1)
        len = tmpStrs[1].size();
	    if ( ( ndx = tmpStrs[1].find("=") ) < 0 )
	    {
	        return false;
	    }
	   
	    m_desFieldName	= lrTrim( tmpStrs[1].substr(0,ndx) );
	    tmpStrs[1]  = lrTrim( tmpStrs[1].substr(ndx+1,len-ndx-1) );

        // 得到了find(seqNumber,1)
	    len = tmpStrs[1].size();
	    if ( ( ndx = tmpStrs[1].find("(") ) < 0 )
	    {
		    return false;
	    }
	    tmpStrs[1] = lrTrim( tmpStrs[1].substr(ndx+1,len-ndx-1) );
	    len = tmpStrs[1].size();
	    if ( ( ndx = tmpStrs[1].find(")") ) < 0 )
	    {
		    return false;
	    }
	    
        tmpStrs[1] = lrTrim( tmpStrs[1].substr(0,ndx) );

	    tmpStrs1.clear();StringUtil::split(tmpStrs[1],",",tmpStrs1);
	
	    if( tmpStrs1.size() == 2 )
	    {
	        m_srcFieldName = tmpStrs1[0];
		    m_iPosition = atoi(tmpStrs1[1].c_str());
	    }
	    else
	    {
		    return false;	
	    }
		this->getFieldObj();
        return true;
    }
    else if(tmpStrs.size() == 3)
    {
        tmpStrs[0]=tmpStrs[0].substr(8);
        if (tmpStrs[0].compare(0,9,"CALL_TYPE") == 0)
        {
            tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
            m_callType = tmpStrs1[1];
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
            }	
        }
        
        if (tmpStrs[1].compare(0,9,"CALL_TYPE") == 0)
        {
            tmpStrs1.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs1);
            m_callType = tmpStrs1[1];
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
        
        // 提取 seq=find(seqNumber,1)
        len = tmpStrs[2].size();
	    if ( ( ndx = tmpStrs[2].find("=") ) < 0 )
	    {
	        return false;
	    }
	    
	    m_desFieldName	= lrTrim( tmpStrs[2].substr(0,ndx) );
	    tmpStrs[2]  = lrTrim( tmpStrs[2].substr(ndx+1,len-ndx-1) );

        // 得到了find(seqNumber,1)
	    len = tmpStrs[2].size();
	    if ( ( ndx = tmpStrs[2].find("(") ) < 0 )
	    {
		    return false;
	    }
	    tmpStrs[2] = lrTrim( tmpStrs[2].substr(ndx+1,len-ndx-1) );
	    len = tmpStrs[2].size();
	    if ( ( ndx = tmpStrs[2].find(")") ) < 0 )
	    {
		    return false;
	    }
	    
        tmpStrs[2] = lrTrim( tmpStrs[2].substr(0,ndx) );

	   tmpStrs1.clear(); StringUtil::split(tmpStrs[2],",",tmpStrs1);
	
	    if( tmpStrs1.size() == 2 )
	    {
	        m_srcFieldName = tmpStrs1[0];
		    m_iPosition = atoi(tmpStrs1[1].c_str());
	    }
	    else
	    {
		    return false;	
	    }
	    this->getFieldObj();
	    return true;
	}
	else
	{
	    return false;
	}
}
	
        
        
        
  
//##ModelId=485626BC030D
bool FieldConvertRule_16::convertField()
{
	if( FieldConvertRule::generalCondJudge() == false )
	{
		return false;	
	}

	// 找到最近的m_srcField的key值 by chenm 2007-3-5
	if( m_pFieldSrc->getNumCount() > 0
		&& m_pFieldSrc->getNumCount() >= m_iPosition)
	{
		if(m_iPosition==-1)
		{
			m_pFieldDes->setDesValue(m_pFieldSrc->m_fieldInfoVect[m_pFieldSrc->getNumCount()-1]->m_FieldDesValue);	
		}
		else
		{
			m_pFieldDes->setDesValue(m_pFieldSrc->m_fieldInfoVect[m_iPosition-1]->m_FieldDesValue);				
		}
	}
		
    return true;
}

//##ModelId=485626EA0000
FieldConvertRule_16::~FieldConvertRule_16()
{
}

void FieldConvertRule_16::getFieldObj()
{
    m_pFieldCondition = m_pFieldInfos->getFieldInfoByName(m_conditionFieldName.c_str());  
    m_pFieldSrc       = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName(m_srcFieldName.c_str()) );  
    m_pFieldDes       = m_pFieldInfos->getFieldInfoByName(m_desFieldName.c_str());  

	return FieldConvertRule::getFieldObj();		
}
