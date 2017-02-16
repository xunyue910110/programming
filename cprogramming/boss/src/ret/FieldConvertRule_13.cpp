// ##########################################
// Source file : FieldConvertRule_13.cpp
// System      : Mobile Billing System
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030429
// Update      : 20030429
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ##########################################

#include "FieldConvertRule_13.h"

//##ModelId=3CA5F3D303E2
//此规则实现字段之间的 +,-,*,/ 操作
bool FieldConvertRule_13::initialize(string ruleText)
{
	char cTmp[100];
	vector<string> tmpStrs,tmpStrs1;

	// rule_9;01;ALL;cfee=cfee1+cfee2
	splitStrByToken(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 4)
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}
	
	// 提取规则中的信息
	m_callType = tmpStrs[1]; //跳过 'rule_9',get "01"
	
    // 规则9中除呼叫类型外的条件初始化:MSRN_FLAG=91
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
	//拆分 cfee=cfee1+cfee2  
	splitStrByToken(tmpStrs[3],"=",tmpStrs1);
	{
		if( tmpStrs1.size() != 2 )
		{
			return false;	
		}
		else
		{
			m_desFieldName = tmpStrs1[0];   // get "cfee"
			// 得到赋值语句中的运算符
        	strcpy(cTmp,tmpStrs1[1].c_str());
	        if(strstr(cTmp,"+")!=NULL) 
	        {
	            m_desOperat = "+";
	        } 
	        else if(strstr(cTmp,"-")!=NULL) 
	        {
	            m_desOperat = "-";
	        } 
	        else if(strstr(cTmp,"*")!=NULL) 
	        {
	            m_desOperat = "*";
	        } 
	        else if(strstr(cTmp,"/")!=NULL) 
	        {
	            m_desOperat = "/";
	        } 
	        else 
	        {
	            return false;
	        }
	        // 根据前面求出的运算符,分割条件字段,取得运算符左右键值
	        splitStrByToken(cTmp,m_desOperat.c_str(),tmpStrs1);
	        if(tmpStrs1.size()!=2)
	        {
	        	return false;	
	        }
	        else
	        {
	        	m_operatLeftFieldName  = tmpStrs1[0];   // get "cfee1"
	        	m_operatRightFieldName = tmpStrs1[1];	// get "cfee2"
	        }		        	
		}
	}	
	          
    return true;
}

//##ModelId=3CA5F3D4007C
bool FieldConvertRule_13::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    double src1,src2,des;
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
        {                                        // 字段值是"ALL"
            if(m_conditionExp.compare("!=")==0) 
            {                 // 如果字段没有值(!=ALL),即不为空
                if(tmpFieldInfo->m_FieldDesValue.size()>0)
                    return false;                          // 有值，不满足条件
            } 
            else if(m_conditionExp.compare("=")==0) 
            {           // 如果字段有值(=ALL),即为空
                if(tmpFieldInfo->m_FieldDesValue.size()<=0)
                    return false;                          // 没有值，不满足条件
            } 
            else 
            {
                return false;
            }
        }
    }
    
	// 如果找不到目的字段,则创建出来
    if( (desFieldInfo=getFieldMap(theFieldInfoMaps,m_desFieldName))==0 )
    {
		pFieldInfo = new FieldInfo(0);
		pFieldInfo->m_FieldName_	= m_desFieldName;	
		pFieldInfo->m_FieldDesValue	= "";
//		pFieldInfo->m_IsFormat		= true;
//		pFieldInfo->m_IsCheck 		= true;
		theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_desFieldName.c_str(),tmpFieldInfo) );  
		desFieldInfo=getFieldMap(theFieldInfoMaps,m_desFieldName);    	
    }
    if( (srcFieldInfo=getFieldMap(theFieldInfoMaps,m_operatLeftFieldName))==0 ) 
    {
        src1    =   atof( m_operatLeftFieldName.c_str() );     
    } 
    else 
    {
        src1    =   atof( srcFieldInfo->m_FieldDesValue.c_str() );  
    }
    
	if( (srcFieldInfo=getFieldMap(theFieldInfoMaps,m_operatRightFieldName))==0 ) 
	{
        src2    =   atof( m_operatRightFieldName.c_str() );     
    } 
    else 
    {
        src2    =   atof( srcFieldInfo->m_FieldDesValue.c_str() );   
    }   
    
    if(m_desOperat.compare("+")==0) 
    {
        des=src1+src2;
    } 
    else if (m_desOperat.compare("-")==0) 
    {
        des=src1-src2;
    } 
    else if (m_desOperat.compare("*")==0) 
    {
        des=src1*src2;
    } 
    else if (m_desOperat.compare("/")==0) 
    {  
        if(src2>0)
            des=src1/src2;
    } 

    sprintf(tmp,"%0.f",des);  
    desFieldInfo->m_FieldDesValue=tmp;
        

    return true;
}


