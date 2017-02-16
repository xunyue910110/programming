// ##########################################
// Source file : FieldConvertRule_8.cpp
// System      : Mobile Billing System
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030109
// Update      : 20030109
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ##########################################

#include "FieldConvertRule_8.h"
#include "RecordFormat.h"

//##ModelId=3C7F4D3A0076
// filed2的字段值是"200#0#02#02"，此规则用来得到其中的子字段值
bool FieldConvertRule_8::initialize(string ruleText)
{
   	vector<string> tmpStrs,tmpStrs1;
   	char cTmp[50];

   	// rule_8;01;ALL;field2;\#;4;Call_Type
   	// rule_8;ALL;MSRN_FLAG=91;field2;\#;4;Call_Type
   	// filed2的字段值是"200#0#02#02"，此规则用来得到其中的子字段值。
	splitStrByToken(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 7)
	{
		// 小于7个分号的,不符合规则的书写格式
		return false;	
	}

	// 提取规则中的信息
	m_callType           = tmpStrs[1];                 //跳过 'rule_8',get 呼叫类型 "01"
	m_compositeFieldName = tmpStrs[3];                 // get "field2"，为复合字段名
	m_postion            = atoi(tmpStrs[5].c_str());   // get "4"，为子字段顺序号
	m_dstFieldNames      = tmpStrs[6];                 // get "Call_Type"，为子字段名

    // 规则8中除呼叫类型外的条件初始化:'MSRN_FLAG=91'或'ALL'
    if(strcmp(tmpStrs[2].c_str(),"ALL")==0) 
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

	// get "\#"，为分割符
	strcpy(m_myToken,tmpStrs[4].c_str());
	StringUtil::eraseChar(m_myToken,'\\');

    return true;
}

//##ModelId=3C7F578300D6
bool FieldConvertRule_8::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    int i;
    char *tok;
    char tmp[100],tmp1[100];
    static string srcString;
    FieldInfo *pFieldInfo;
    FieldInfo *tmpFieldInfo,*desFieldInfo,*srcFieldInfo;
    static string callType("CALL_TYPE"),all("ALL");

    // 条件1：通话类型
    if(m_callType.compare(all)!=0) 
    {
    	if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,callType))==0 )
    	{
	 		return false;
	 	}

        if(tmpFieldInfo->m_FieldDesValue.compare(m_callType)!=0) 
        {
            return false;
        }
    }

    // 条件2：其他字段值
	if(m_conditionFieldName.compare(all)!=0) 
	{            // not is "ALL"
	    if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_conditionFieldName))==0 )
		    return false;

		if(m_conditionFieldValue.compare(all)!=0) 
		{             // 字段值不是"ALL"
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
        {                                            // 字段值是"ALL"
            if(m_conditionExp.compare("!=")==0) 
            {                    // 如果字段没有值(!=ALL),即不为空
                if(tmpFieldInfo->m_FieldDesValue.size()>0)
                    return false;                           // 有值，不满足条件
            } 
            else if(m_conditionExp.compare("=")==0) 
            {              // 如果字段有值(=ALL),即为空
                if(tmpFieldInfo->m_FieldDesValue.size()<=0)
                    return false;                           // 没有值，不满足条件
            } 
            else 
            {
                return false;
            }
        }
    }

    if( (srcFieldInfo=getFieldMap(theFieldInfoMaps,m_compositeFieldName))==0 )
	 	return false;

    if( (desFieldInfo=getFieldMap(theFieldInfoMaps,m_dstFieldNames))==0 ) 
    {
	   	pFieldInfo = new FieldInfo(0);
//	   	pFieldInfo->m_IsFormat 	    = true;
	   	pFieldInfo->m_FieldName_ 	= m_dstFieldNames;
	   	theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_dstFieldNames.c_str(),pFieldInfo) );

	   	desFieldInfo = getFieldMap(theFieldInfoMaps,m_dstFieldNames);
    }

	i=0;
	strcpy(tmp,srcFieldInfo->m_FieldDesValue.c_str());
	tok=strtok(tmp,m_myToken);
   	while(tok!=NULL) 
   	{
   		if(i==m_postion) 
   		{
   			desFieldInfo->m_FieldDesValue=tok;
   			break;
   		}
   		tok=strtok(NULL,m_myToken);
		i++;
   	}

    return true;
}
