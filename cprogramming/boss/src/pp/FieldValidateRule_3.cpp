// ##########################################
// Source file : FieldValidateRule_3.cpp
// System      : Mobile Billing System
// Version     : 0.1.3
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080922
// Update      : 20080922
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################


#include "FieldValidateRule_3.h"
#include "FieldInfos.h"


//##ModelId=485645B302EE
bool FieldValidateRule_3::initialize(string ruleText)
{
    vector<string> tmpStrs,tmpStrs1;
	
	// example:    rule_3;VALID=fields;FIELD=ALL;IMSI,OTHER_PARTY,CALL_TYPE
	//splitStrByToken(ruleText,";",tmpStrs);
			StringUtil::split(ruleText,";",tmpStrs);
	if( tmpStrs.size() < 3)
	{
		// 小于2个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
	
	if (tmpStrs.size() == 3)
	{
	    if (tmpStrs[1].compare(0,5,"FIELD") == 0)
	    {
	        //splitStrByToken(tmpStrs[1],"=",tmpStrs1);
	        tmpStrs1.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs1);

	        if (tmpStrs1.size() != 2)
	        {
	            return false;
	        }
	        else
	        {
	            m_conditionFieldName = tmpStrs1[1];
	            m_callType = "ALL";
	        }
	    }
	    else if(tmpStrs[1].compare(0,9,"CALL_TYPE") == 0)
	    {
	       // splitStrByToken(tmpStrs[1],"=",tmpStrs1);
	        tmpStrs1.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs1);

	        if (tmpStrs1.size() != 2)
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
	        //splitStrByToken(tmpStrs[1],"=",tmpStrs1);
	        tmpStrs1.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs1);

	        if (tmpStrs1.size() != 2)
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
	    
	    // 得到 "IMSI,OTHER_PARTY,CALL_TYPE" 的信息
	    //splitStrByToken(tmpStrs[2],",",tmpStrs1);
	    tmpStrs1.clear();StringUtil::split(tmpStrs[2],"=",tmpStrs1);

	    m_fields = tmpStrs1;

	    return true;
	}
	else if(tmpStrs.size() == 4)
	{

	    if (tmpStrs[1].compare(0,9,"CALL_TYPE") == 0)
	    {
	       // splitStrByToken(tmpStrs[1],"=",tmpStrs1);
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

	   // splitStrByToken(tmpStrs[2],"=",tmpStrs1);
      tmpStrs1.clear();StringUtil::split(tmpStrs[2],"=",tmpStrs1);
    
	    if (tmpStrs1.size() != 2)
	    {
	        return false;
	    }
	    else
	    {
	        m_conditionFieldName = tmpStrs1[0];
	        m_conditionFieldValue = tmpStrs1[1];
	    }
	    // 得到 "IMSI,OTHER_PARTY,CALL_TYPE" 的信息
	   // splitStrByToken(tmpStrs[2],",",tmpStrs1);
	    tmpStrs1.clear();StringUtil::split(tmpStrs[2],"=",tmpStrs1);

	    m_fields = tmpStrs1;

	    return true;
	    
	}
	
	return false;
}


//##ModelId=485645BA0203
int FieldValidateRule_3::validField()
{
    int i,len1;
    FieldInfo *aFieldInfo=0;
    int len,fieldValue,errorCode=0;
    static string callType("CALL_TYPE"),all("ALL");

    if( (m_cdrFlag->m_errorCode > 0) && (m_cdrFlag->m_errorCode!=FORBIDDEN_CHARACTER) )  // edit by zhangy in 2002.4.22
    {
        return m_cdrFlag->m_errorCode;
    }
    //if( (aFieldInfo=getFieldMap(theFieldInfoMaps,callType))==0 )
	if((aFieldInfo=m_pFieldInfos->getFieldInfoByName(CALL_TYPE))==0)

	{
	    return 0;
	}

    // 条件1：通话类型
    if(m_callType.compare(all)!=0) 
    {
       // if(aFieldInfo->m_FieldDesValue.compare(m_callType)!=0)
        if(	m_pFieldInfos->getFieldInfoByName(CALL_TYPE)!=0)
        {
            return 0;
        }
    }

    // 条件2：其他字段值
	if(m_conditionFieldName.compare(all)!=0) 
	{      // not is "ALL"
	   // if( (aFieldInfo=getFieldMap(theFieldInfoMaps,m_conditionFieldName))==0 )
	if((aFieldInfo=m_pFieldInfos->getFieldInfoByName(m_conditionFieldName.c_str()))==0)

	    {
		    return false;
		}
	    if(m_conditionFieldValue.compare(all)!=0) 
	    {
            //if(aFieldInfo->m_FieldDesValue.compare(m_conditionFieldValue)!=0)
 	if((aFieldInfo=m_pFieldInfos->getFieldInfoByName(m_conditionFieldName.c_str()))!=0)

            {
               return 0;
            }
        }
        else
        {
        	aFieldInfo=m_pFieldInfos->getFieldInfoByName(m_conditionFieldName.c_str());
           //if(aFieldInfo->m_FieldDesValue.size()==0) 
         		if( aFieldInfo->m_iDesValueLen == 0)
            {      // 如果字段为空
                return 0;
            }
        }
    }      

    len1=m_fields.size();

    for(i=0;i<len1;i++)
    {
        FieldInfo *pFieldInfo;
		//pFieldInfo = getFieldMap( theFieldInfoMaps,m_fields[i] );
	  pFieldInfo =	m_pFieldInfos->getFieldInfoByName( m_fields[i].c_str());
		//if( (pFieldInfo->m_FieldDesValue).size()==0 ) 
		//	if(pFieldInfo->m_FieldDesValue.size()==0)
		if( pFieldInfo->m_iDesValueLen == 0)
		{
		    m_cdrFlag->m_isError    = true;
		    m_cdrFlag->m_fieldName  = aFieldInfo->m_FieldName_;
		
		    return  EMPTY_FIELD;
		}
    }

	return 0;
}

