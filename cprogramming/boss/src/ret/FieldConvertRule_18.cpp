// ##########################################
// Source file : FieldConvertRule_18.cpp
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

#include "FieldConvertRule_18.h"
#include "RecordFormat.h"

//##查找列表类型数据中的第N个，如果N＝-1,则查找最后一个
bool FieldConvertRule_18::initialize(string ruleText)
{
	int len,ndx;
	char cTmp[100];
	vector<string> tmpStrs,tmpStrs1;
	
	// rule_18;ALL;ALL;seq=find(seqNumber,1)
	splitStrByToken(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 4)
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
	m_callType = tmpStrs[1]; //跳过 'rule_18',get "01"
	
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
	// 提取 seq=find(seqNumber,1)
    len = tmpStrs[3].size();
	if ( ( ndx = tmpStrs[3].find("=") ) < 0 )
		return false;

	m_desField	= lrTrim( tmpStrs[3].substr(0,ndx) );
	tmpStrs[3]  = lrTrim( tmpStrs[3].substr(ndx+1,len-ndx-1) );

    // 得到了find(seqNumber,1)
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
	
	if( tmpStrs1.size() == 2 )
	{
		m_srcField = tmpStrs1[0];
		m_position = tmpStrs1[1];
	}
	else
	{
		return false;	
	}
	
    return true;
}

//##ModelId=3C7F578300D6
bool FieldConvertRule_18::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
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
	// 得到目标字段
    if( (desFieldInfo=getFieldMap(theFieldInfoMaps,m_desField))==0 ) 
    {
		pFieldInfo = new FieldInfo(0);
	 	pFieldInfo->m_FieldName_ 	= m_desField;
	 	theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_desField.c_str(),pFieldInfo) );

	 	desFieldInfo = getFieldMap(theFieldInfoMaps,m_desField);
    }

	int iPosition,i,j=1,iSize,iLastPos=-1;
	iPosition = atoi(m_position.c_str());	
	iSize     = theFieldInfoMaps.size();
	// 找到最近的m_srcField的key值 by chenm 2007-3-5
	multimap<MyString,FieldInfo *>::iterator itr;
	itr = theFieldInfoMaps.lower_bound(m_srcField.c_str());
	while( itr!=theFieldInfoMaps.end()
	       && 
	       ( (itr->second)->m_FieldName_.compare(0,m_srcField.length(),m_srcField) == 0 )
	   )
	{
		if(j == iPosition)
		{
			desFieldInfo->m_FieldDesValue = (itr->second)->m_FieldDesValue;
			break;	
		}
		else
		{
			++j;	
		}
		iLastPos = 1;
		++itr;
	}
	// 如果配置中配的是取最后一个字段,并且在theFieldInfoMaps已经查找到了
	if( iPosition == -1 && iLastPos != -1 )
	{
		desFieldInfo->m_FieldDesValue = ((--itr)->second)->m_FieldDesValue;	
	}
		
    return true;
}
