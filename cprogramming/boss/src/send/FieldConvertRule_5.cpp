// ##########################################
// Source file : FieldConvertRule_5.cpp
// System      : Mobile Billing System
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20020301
// Update      : 20020302
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "FieldConvertRule_5.h"
#include "RecordFormat.h"

//##ModelId=3C7F4D3A0076
bool FieldConvertRule_5::initialize(string ruleText)
{
	int len,ndx;
	char cTmp[100];
	vector<string> tmpStrs,tmpStrs1;
	
	// rule_5;01;ALL;calling=substr(calling_bak,1,calling_size)
	// rule_5;ALL;ALL;called=substr(called_bak,1,called_size)
	splitStrByToken(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 4)
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
	m_callType = tmpStrs[1]; //跳过 'rule_6',get "01"
	
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
	// 提取 calling=substr(calling_bak,1,calling_size)
    len = tmpStrs[3].size();
	if ( ( ndx = tmpStrs[3].find("=") ) < 0 )
		return false;

	m_desField	= lrTrim( tmpStrs[3].substr(0,ndx) );
	tmpStrs[3]  	      = lrTrim( tmpStrs[3].substr(ndx+1,len-ndx-1) );

    // 得到了substr(calling_bak,1,calling_size)
    len = tmpStrs[3].size();
	if ( ( ndx = tmpStrs[3].find("(") ) < 0 )
	{
		return false;
	}
	tmpStrs[3] = lrTrim( tmpStrs[3].substr(ndx+1,len-ndx-1) );
	len = tmpStrs[3].size();
	if ( ( ndx = tmpStrs[3].find(")") ) < 0 )
	{
		return false;
	}
	// 得到了括号中的内容: calling_bak,1,calling_size
	tmpStrs[3] = lrTrim( tmpStrs[3].substr(0,ndx) );

	splitStrByToken(tmpStrs[3],",",tmpStrs1);
	
	if( tmpStrs1.size() == 3 )
	{
		m_srcField = tmpStrs1[0];
		m_substart = tmpStrs1[1];
		m_subLength = tmpStrs1[2]; //如果m_subLength为关键字"CHARACTER",则长度为从0开始到第一个字符。		
	}
	else
	{
		return false;	
	}
	
    return true;
}

//##ModelId=3C7F578300D6
bool FieldConvertRule_5::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    int i,len,substart;
    char tmp[100],tmp1[100];
    static string srcString;
    FieldInfo *pFieldInfo;
    FieldInfo *tmpFieldInfo,*desFieldInfo,*srcFieldInfo;
    static string callType("CALL_TYPE"),all("ALL");

    if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,callType))==0 )
	 	return false;

    // 条件1：通话类型
    if(m_callType.compare(all)!=0) 
    {
        if(tmpFieldInfo->m_FieldDesValue.compare(m_callType)!=0) 
        {
            return false;
        }
    }

    // 条件2：其他字段值
	if(m_conditionFieldName.compare(all)!=0)                           // not is "ALL"   
	{           
	    if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_conditionFieldName))==0 )
		    return false;

		if(m_conditionFieldValue.compare(all)!=0) 
		{                                                 // 字段值不是"ALL"
		    if(m_conditionExp.compare("!=")==0) 
		    {
		        if(tmpFieldInfo->m_FieldDesValue.compare(m_conditionFieldValue)==0) 
		        {
                    return false;
                }
		    } 
		    else if(m_conditionExp.compare("=")==0) 
		    {
		        if(tmpFieldInfo->m_FieldDesValue.compare(m_conditionFieldValue)!=0) 
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
        {                                                    // 字段值是"ALL"
            if(m_conditionExp.compare("!=")==0) 
            {                                                // 如果字段没有值(!=ALL),即不为空
                if(tmpFieldInfo->m_FieldDesValue.size()>0)
                    return false;                            // 有值，不满足条件
            } 
            else if(m_conditionExp.compare("=")==0) {                 // 如果字段有值(=ALL),即为空
                if(tmpFieldInfo->m_FieldDesValue.size()<=0)
                    return false;                            // 没有值，不满足条件
            } 
            else 
            {
                return false;
            }
        }
    }

    if( (srcFieldInfo=getFieldMap(theFieldInfoMaps,m_srcField))==0 )
	 	return false;

    if( (desFieldInfo=getFieldMap(theFieldInfoMaps,m_desField))==0 ) 
    {
	 	pFieldInfo = new FieldInfo(0);
//	 	pFieldInfo->m_IsFormat 	    = true;
	 	pFieldInfo->m_FieldName_ 	= m_desField;
	 	theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_desField.c_str(),pFieldInfo) );

	 	desFieldInfo = getFieldMap(theFieldInfoMaps,m_desField);
    }

    if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_substart))!=0 ) 
    {
        substart = atoi(tmpFieldInfo->m_FieldDesValue.c_str());
    } 
    else 
    {
        substart = atoi(m_substart.c_str());
    }

    strcpy(tmp,srcFieldInfo->m_FieldDesValue.c_str());
    // add by zhangy in 2002.5.25 start -----------------------------
    // 例：号码为：0252299394A323 求得的len为10。
    if(m_subLength.compare("CHARACTER")==0 ) 
    {
        len=strlen(tmp);
        for(i=0;i<len;i++)
        {
            if( tmp[i]>0x39 || tmp[i]<0x30 ) 
            {
                break;
            }
        }
        if( i > substart )
            len = i - substart ;
        else
            len = i;
        if( (m_CdrFlag->m_errorCode==FORBIDDEN_CHARACTER)
             &&(m_srcField == m_CdrFlag->m_fieldName) )
        {
            m_CdrFlag->m_errorCode=0;
            m_CdrFlag->m_isError=false;
        }
        // add by zhangy in 2002.5.25 end -----------------------------
    }
    else if( m_subLength.compare("CHAR2ZERO")==0)
    {
    	// add by chenm 2004-4-12 针对中兴综合接入网的话单,把非法字符转换成0
    	len=strlen(tmp);
        for(i=0;i<len;i++)
        {
            if( tmp[i]== 'A' ) 
            {
                tmp[i]= '0';
            }
        }
        if( (m_CdrFlag->m_errorCode==FORBIDDEN_CHARACTER) 
            &&(m_srcField == m_CdrFlag->m_fieldName))
        {
            m_CdrFlag->m_errorCode=0;
            m_CdrFlag->m_isError=false;
        }	
    } 
    else if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_subLength))==0 ) 
    {
        len = atoi(m_subLength.c_str());
    } 
    else 
    {
        len = atoi(tmpFieldInfo->m_FieldDesValue.c_str());
    }
	
	// 如果超长
	if( strlen(tmp)<(substart+len))
	{
		len = strlen(tmp) - substart;	
	}
	if( len<0 )
	{
	    return false;
	}
    memcpy(tmp1,tmp+substart,len);
    tmp1[len]=0;
    desFieldInfo->m_FieldDesValue=tmp1;


    return true;
}
