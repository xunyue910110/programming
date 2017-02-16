// ##########################################
// Source file : FieldConvertRule_4.cpp
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011212
// Update      : 20011222
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include <stdlib.h>
#include "FieldConvertRule_4.h"

//##ModelId=3C16C83D0227
bool FieldConvertRule_4::initialize(string ruleText)
{
	char cTmp[100];
	vector<string> tmpStrs,tmpStrs1;

	//rule_4;08;ALL;other_party;1,A=0         # 无条件替换类型1:字段内全部"A"替换为"0"
	splitStrByToken(ruleText,";",tmpStrs);

	if(tmpStrs.size() != 5)
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
	m_callType = tmpStrs[1]; //跳过 'rule_4',get "08"	
	
	// 提取规则中除了呼叫类型外的条件: get "MSRN_FLAG=91" or "ALL"
    if(strcmp(tmpStrs[2].c_str(),"ALL")==0)      // "MSRN_FLAG=91" or "ALL"
    {
    	m_conditionFieldName = "ALL";
    } 
    else 
    {
       // 得到条件中的运算符
        strcpy(cTmp,tmpStrs[2].c_str());
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
        splitStrByToken(tmpStrs[2],m_conditionExp.c_str(),tmpStrs1);
        if(tmpStrs1.size()!=2)
        {
        	return false;	
        }
        else
        {
        	m_conditionFieldName  = tmpStrs1[0];
        	m_conditionFieldValue = tmpStrs1[1];	
        }	
	}
	
	// 提取待转换字段--other_party
	m_fieldName = tmpStrs[3];
	
	// 提取 "1,A=0 "	
	splitStrByToken(tmpStrs[4],",",tmpStrs1);
	if( tmpStrs1.size() == 2 )
	{
		m_replaceType  = atoi(tmpStrs1[0].c_str());
		// get "A=0"
		string tmpStr;
		tmpStr = tmpStrs1[1];
		splitStrByToken(tmpStr,"=",tmpStrs1);
		if(tmpStrs1.size() == 2)
		{
			m_srcValue = tmpStrs1[0];
			if( tmpStrs1[1].compare("EMPTY") == 0 )
			{
				m_desValue = "";
			}
			else
			{
				m_desValue = tmpStrs1[1];
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

	return true;
}

//##ModelId=3C16C8470036
bool FieldConvertRule_4::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
	char *tok;
	bool flag=false;
	FieldInfo *otherFieldInfo;
	int i,len,len1,len2,oldLen,count;
	char tmp[100],tmp1[100],tmp2[100],tokTmp[100];
	static string callType("CALL_TYPE"),all("ALL");
	string srcTmp,srctmp1;
	
	if( (otherFieldInfo=getFieldMap(theFieldInfoMaps,callType))==0 )
		return false;
	
	// 条件1：通话类型
	if(m_callType.compare(all)!=0) 
	{
	   if(otherFieldInfo->m_FieldDesValue.compare(m_callType)!=0)
	      return false;
	}

   // 条件2：其他字段值
	if(m_conditionFieldName.compare(all)!=0)       // not is "ALL"
	{
		if( (otherFieldInfo=getFieldMap(theFieldInfoMaps,m_conditionFieldName))==0 )
		   return false;
		
		if( m_conditionFieldValue.compare(all) != 0 )  
		{
			if( m_conditionExp.compare("!=") == 0 ) 
			{
				if(otherFieldInfo->m_FieldDesValue.compare(m_conditionFieldValue)==0) 
				{
    			  return false;
    			}
			} 
			else if( m_conditionExp.compare("=") == 0 ) 
			{
				if(otherFieldInfo->m_FieldDesValue.compare(m_conditionFieldValue)!=0) 
				{
    			  return false;
    			}
			} 
			else 
			{
				return false;
			}
      	} 
    	else if(otherFieldInfo->m_FieldDesValue.size()==0)    // 如果字段为空  
    	{      
    		return false;
    	}
    }

	// 开始转换
	
	if ( (otherFieldInfo=getFieldMap(theFieldInfoMaps,m_fieldName)) == 0 )
		return false;
	
	if( strstr(otherFieldInfo->m_FieldDesValue.c_str(),m_srcValue.c_str())==NULL)
	    //zww 2005-12-7增加对首位字符替换操作:CHARACTER=EMPTY
	    if(m_srcValue.compare("CHARACTER") != 0)            
	        {
            return true;
            }
	
	if (m_replaceType==0)             // 用于对首位开始的串进行替换
	{
	    //zww 2005-12-7增加对首位字符替换操作:CHARACTER=EMPTY
        if(  m_srcValue.compare("CHARACTER") == 0 ) 
	    {
            strcpy(tmp,otherFieldInfo->m_FieldDesValue.c_str());
            len=otherFieldInfo->m_FieldDesValue.size();
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
	    len1 = otherFieldInfo->m_FieldDesValue.size();
	    if(otherFieldInfo->m_FieldDesValue.compare(0,len,srcTmp)==0) 
	    {
	       strncpy(tmp,otherFieldInfo->m_FieldDesValue.c_str()+len,(len1 - len));
	       tmp[len1 - len]=0;
	       otherFieldInfo->m_FieldDesValue = m_desValue + tmp;
	    }
	} 
	else if (m_replaceType==1)      // 用于对整个字段内的串进行替换
	{
	    tmp1[0]=0;
	    int tip =0;
	    strcpy(tmp2,otherFieldInfo->m_FieldDesValue.c_str());
	    len2=otherFieldInfo->m_FieldDesValue.size();
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
	       otherFieldInfo->m_FieldDesValue = tmp1;
	    }
	}
	
	if( (m_CdrFlag->m_errorCode==FORBIDDEN_CHARACTER) 
	    &&(m_fieldName == m_CdrFlag->m_fieldName))
	{
	    m_CdrFlag->m_errorCode=0;
	    m_CdrFlag->m_isError=false;
	}
	
	return true;
}

