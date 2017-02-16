// ##########################################
// Source file : FieldConvertRule_4.cpp
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

#include "FieldConvertRule_4.h"
#include "FieldInfos.h"
#include <stdlib.h>

//##ModelId=485616A9003E
bool FieldConvertRule_4::initialize(string ruleText)
{
	char cTmp[100];
	vector<string> tmpStrs,tmpStrs1,tmpStrs2;

	//rule_4:CALL_TYPE=08;ntemp!=B76;FIELD=other_party;1,A=0         # 无条件替换类型1:字段内全部"A"替换为"0"
	StringUtil::split(ruleText,";",tmpStrs);

	if(tmpStrs.size() < 2)
	{
		// 小于2个分号的,不符合规则的书写格式
		return false;	
	}
	
	// 提取规则中的信息
	/*RULE_4:FIELD=other_party;1,A=0*/
	if (tmpStrs.size() == 2)
	{
	    tmpStrs[0] = tmpStrs[0].substr(7);
	    if (tmpStrs[0].compare(0,strlen(DEAL_OBJ_NAME),DEAL_OBJ_NAME) != 0)
	    {
	        return false;
	    }
	    else
	    {
	        tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
	        m_callType = "ALL";                  //得到呼叫类型条件
	        m_conditionFieldName = "ALL";       //得到字段值条件
	        m_fieldName = tmpStrs1[1];          //得到待转换字段名  
	    }
	    
	    // 提取 "1,A=0 "
	    tmpStrs1.clear();StringUtil::split(tmpStrs[1],",",tmpStrs1);
	    if ( tmpStrs1.size() == 2 )
	    {
		    m_replaceType  = atoi(tmpStrs1[0].c_str());
		    
		    // get "A=0"
		    tmpStrs2.clear();StringUtil::split(tmpStrs1[1],"=",tmpStrs2);
		    if(tmpStrs2.size() == 2)
		    {
		        m_srcValue = tmpStrs2[0];
			    if ( tmpStrs2[1].compare(0,strlen(EMPTY),EMPTY) == 0 )
			    {
				    m_desValue = "";
			    }
			    else
			    {
				    m_desValue = tmpStrs2[1]; 
	
				    
				    // 去掉类似 '\#' 的配置前面的 '\'
				    m_desValue.erase(0,m_desValue.find_first_not_of('\\'));	
			    }			
		    }
		    else
		    {
			    return false;	
		    }		
	    }
	    else
	    {
		    return false;		
	    }
	}
	/*RULE_4:ntemp!=B76;FIELD=other_party;1,A=0*/
	else if (tmpStrs.size()==3)
	{
	    tmpStrs[0] = tmpStrs[0].substr(7);
	    
	    
	    if (tmpStrs[0].compare(0,strlen(CALL_TYPE),CALL_TYPE) == 0)
	    {
	        tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
	        m_callType = tmpStrs1[1];
	        m_conditionFieldName = "ALL";
	    }
	    else 
	    {
	        strcpy(cTmp,tmpStrs[0].c_str());
	        // 得到条件中的运算符
	        if(strstr(cTmp,"!=")!=NULL) 
            {
                m_conditionExp = "!=";
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
            m_callType = "ALL";
        }
        
	    
	    if(tmpStrs[1].compare(0,strlen(DEAL_OBJ_NAME),DEAL_OBJ_NAME) == 0)
	    {
	    
	        // 根据前面求出的运算符,分割条件字段,取得运算符左右键值
	        tmpStrs1.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs1);
            if(tmpStrs1.size()!=2)
            {
        	    return false;	
            }
            else
            {
                m_fieldName = tmpStrs1[1];
            }
        }
        else
        {
            return false;   
        }
	    // 提取 "1,A=0 "
	    tmpStrs1.clear();StringUtil::split(tmpStrs[2],",",tmpStrs1);
	    if ( tmpStrs1.size() == 2 )
	    {
		    m_replaceType  = atoi(tmpStrs1[0].c_str());
		    
		    // get "A=0"
		    tmpStrs2.clear();StringUtil::split(tmpStrs1[1],"=",tmpStrs2);
		    if(tmpStrs2.size() == 2)
		    {
		        m_srcValue = tmpStrs2[0];
			    if ( tmpStrs2[1].compare(0,5,"EMPTY") == 0 )
			    {
				    m_desValue = "";
			    }
			    else
			    {
				    m_desValue = tmpStrs2[1];
				  
				    
				    // 去掉类似 '\#' 的配置前面的 '\'
				    m_desValue.erase(0,m_desValue.find_first_not_of('\\'));	
			    }			
		    }
		    else
		    {
			    return false;	
		    }		
	    }
	    else
	    {
		    return false;		
	    }
    }
    /*RULE_4:CALL_TYPE=00;ntemp!=B76;FIELD=other_party;1,A=0*/
    else if (tmpStrs.size() == 4)
    {
        tmpStrs[0] = tmpStrs[0].substr(7);
	    
	    if (tmpStrs[0].compare(0,strlen(CALL_TYPE),CALL_TYPE) == 0)
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
	    
	    
        if(tmpStrs[2].compare(0,5,"FIELD") == 0)
	    {
	        	    
	        // 根据前面求出的运算符,分割条件字段,取得运算符左右键值
	        tmpStrs1.clear();StringUtil::split(tmpStrs[2],"=",tmpStrs1);
            if(tmpStrs1.size()!=2)
            {
        	    return false;	
            }
            else
            {
                m_fieldName = tmpStrs1[1];
            }
        }
        else
        {
            return false;   
        }
        // 提取 "1,A=0 "
	    tmpStrs1.clear();StringUtil::split(tmpStrs[3],",",tmpStrs1);
	    if ( tmpStrs1.size() == 2 )
	    {
		    m_replaceType  = atoi(tmpStrs1[0].c_str());
		    
		    // get "A=0"
		    tmpStrs2.clear();StringUtil::split(tmpStrs1[1],"=",tmpStrs2);
		    if(tmpStrs2.size() == 2)
		    {
		        m_srcValue = tmpStrs2[0];
			    if ( tmpStrs2[1].compare(0,5,"EMPTY") == 0 )
			    {
				    m_desValue = "";
			    }
			    else
			    {
				    m_desValue = tmpStrs2[1]; 
				    
				    // 去掉类似 '\#' 的配置前面的 '\'
				    m_desValue.erase(0,m_desValue.find_first_not_of('\\'));	
			    }			
		    }
		    else
		    {
			    return false;	
		    }		
	    }
	    else
	    {
		    return false;		
	    }		 
        
    }
    else
    {
        return false;
    }
	this->getFieldObj();

    return true;
 }

//##ModelId=485616B0003E
bool FieldConvertRule_4::convertField()
{
    char *tok;
	int i,len,len1,len2,oldLen,count;
	char tmp[100],tmp1[100],tmp2[100],tokTmp[100];
	string srcTmp,srctmp1;
	
	if( m_pFieldCallType->m_iDesValueLen == 0 )
		return false;
	
	if( FieldConvertRule::generalCondJudge() == false )
	{
		return false;	
	}

	// 开始转换
	if( m_pFieldTarget->m_iDesValueLen == 0 )
		return false;
	
	if( strstr(m_pFieldTarget->m_FieldDesValue,m_srcValue.c_str())==NULL)
	{
	    //zww 2005-12-7增加对首位字符替换操作:CHARACTER=EMPTY
	    if(m_srcValue.compare("CHARACTER") != 0)            
	    {
            return true;
        }
    }
	
	if (m_replaceType==0)             // 用于对首位开始的串进行替换
	{
	    //zww 2005-12-7增加对首位字符替换操作:CHARACTER=EMPTY
        if(  m_srcValue.compare("CHARACTER") == 0 ) 
	    {
            strcpy(tmp,m_pFieldTarget->m_FieldDesValue);
            len=m_pFieldTarget->m_iDesValueLen;
            for(i=0;i<len;i++)
            {
                if( tmp[i]>=0x30 && tmp[i]<=0x39) 
                {
                    break;
                }
            }
            if(i>=len) 
            {
                return true;
            }
            strncpy(tmp1,tmp,i);
            tmp1[i]= 0;
            srcTmp = tmp1;
        } 
        else 
        {
            srcTmp = m_srcValue;
        }
        len  = srcTmp.size();
	    len1 = m_pFieldTarget->m_iDesValueLen;
	    if( strncmp(m_pFieldTarget->m_FieldDesValue,srcTmp.c_str(),len)==0 ) 
	    {
	       strncpy(tmp,m_pFieldTarget->m_FieldDesValue+len,(len1 - len));
	       tmp[len1 - len]=0;
	       m_pFieldTarget->setDesValue( (m_desValue + tmp).c_str() );
	    }
	} 
	else if (m_replaceType==1)      // 用于对整个字段内的串进行替换
	{
	    tmp1[0]=0;
	    int tip =0;
	    strcpy(tmp2,m_pFieldTarget->m_FieldDesValue);
	    len2=m_pFieldTarget->m_iDesValueLen;
	    
	    //zww 2005.12.8 对任何位置出现的字符将其去掉(1,CHARACTER=EMPTY),或替换(1,CHARACTER=0)
	    if(  m_srcValue.compare("CHARACTER") == 0 ) 
	    {
	        for(i=0;i<len2;i++)
            {
                //zww 遇到非数字的符号则统一置为'A'
                if( tmp2[i]<0x30 || tmp2[i]>0x39) 
                {
                    tmp2[i] = 0x41;
                    tip=1;//开关
                }
            }
            if(i>=len2 && tip==0) 
            {
                return true;
            }
            srctmp1="A";
        }
	    else
	    {
	        srctmp1=m_srcValue;
	    } 
	    strnrps(tmp2,srctmp1.c_str(),m_desValue.c_str(),tmp1,100-1);
	    if(strlen(tmp1)>0) 
	    {
	       m_pFieldTarget->setDesValue(tmp1);
	    }
	}
	
	if( (m_CdrFlag->m_errorCode==FORBIDDEN_CHARACTER) 
	    &&(m_fieldName==m_CdrFlag->m_fieldName) )
	{
	    m_CdrFlag->m_errorCode=0;
	    m_CdrFlag->m_isError=false;
	}
	
	
	return true;
}

void FieldConvertRule_4::getFieldObj()
{
    m_pFieldCondition = m_pFieldInfos->getFieldInfoByName(m_conditionFieldName.c_str());  
    m_pFieldTarget    = m_pFieldInfos->getFieldInfoByName(m_fieldName.c_str());        	
	
	return FieldConvertRule::getFieldObj();		
}
