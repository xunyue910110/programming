// ##########################################
// Source file : FieldConvertRule_14.cpp
// System      : Mobile Billing System
// Version     : 0.1.16
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20020526
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "FieldConvertRule_14.h"
#include <stdlib.h>

//##内容计费表示形式:(均采用十进制)业务代码|上行流量|下行流量.多个业务直接用;隔开.现支持4个业务一条话单
bool FieldConvertRule_14::initialize(string ruleText)
{
	char cTmp[50];
	vector<string> tmpStrs,tmpStrs1,tmpStrs2,tmpStrs3;

    //serviceid|dataVolumeGPRSUp|dataVolumeGPRSDn(业务代码|上行流量|下行流量)的顺序不能改变
    //rule_14;ALL;ALL;cbc=serviceid|dataVolumeGPRSUp|dataVolumeGPRSDn
	splitStrByToken(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 4)
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}
	
	// 提取规则中的信息
	m_callType = tmpStrs[1];       //跳过 'rule_14',get "ALL" 
	
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
	
	// 得到规则14中的 具体操作部分:cbc=dataVolumeGPRSUp|dataVolumeGPRSDn|serviceid
	splitStrByToken(tmpStrs[3],"=",tmpStrs2);
	if( tmpStrs2.size() != 2 )
    {
        return false;
    }
	m_cbc = lrTrim(tmpStrs2[0]);
	splitStrByToken(tmpStrs2[1],"|",tmpStrs3);//得到serviceid|dataVolumeGPRSUp|dataVolumeGPRSDn
	if( tmpStrs3.size() != 4 )
    {
        return false;
    }
    m_serviceIdName = lrTrim(tmpStrs3[0]);
    m_DataUpName    = lrTrim(tmpStrs3[1]);
    m_DataDnName    = lrTrim(tmpStrs3[2]);
    m_DurationName  = lrTrim(tmpStrs3[3]);
		
	return true;
}

//##ModelId=3BAAEAFF02AF
bool FieldConvertRule_14::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
	int len,i;
	FieldInfo *tmpFieldInfo;
	int l_field,l_value;
	static string dstFieldString,dstFieldName;
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
	if(m_conditionFieldName.compare(all) != 0)                // not is "ALL" 
	{            
	    //若条件2的右键是定义的字段名,则取该字段的值进行比较,否则直接用右键的值进行比较
	    if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_conditionFieldValue)) == 0 ) 
	    {
	        dstFieldString = m_conditionFieldValue;
	    } 
	    else 
	    {
            dstFieldString = tmpFieldInfo->m_FieldDesValue;
        }
        
        //若条件2的左键不是已定义的字段名,返回false
        if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_conditionFieldName))==0 )
		    return false;

		//  字段值不是"ALL"
		if(m_conditionFieldValue.compare(all)!=0)               
		{
		    if(m_conditionExp.compare("!=")==0) 
		    {
		        if(tmpFieldInfo->m_FieldDesValue.compare(dstFieldString)==0) 
		        {
                    return false;
                }
		    } 
		    else if(m_conditionExp.compare("=")==0) 
		    {
		        if(tmpFieldInfo->m_FieldDesValue.compare(dstFieldString)!=0) 
		        {
                    return false;
                }
		    } 
		    else if(m_conditionExp.compare(">=")==0) 
		    {
		        l_field=atoi( tmpFieldInfo->m_FieldDesValue.c_str() );
		        l_value=atoi( dstFieldString.c_str() );
		        if( l_field < l_value ) 
		        {
		            return false;
		        }
		    } 
		    else if(m_conditionExp.compare("<=")==0) 
		    {
                l_field=atoi( tmpFieldInfo->m_FieldDesValue.c_str() );
		        l_value=atoi( dstFieldString.c_str() );
		        if( l_field > l_value ) 
		        {
		            return false;
		        }
		    } 
		    else 
		    {
		        return false;
		    }

        }                              
        else                                               // 字段值是"ALL"
        {                                          
            if(m_conditionExp.compare("!=")==0)                     // 如果字段没有值(!=ALL),即不为空
            {
                if(tmpFieldInfo->m_FieldDesValue.size()>0)  // 有值，不满足条件
                    return false;                          
            } 
            else if(m_conditionExp.compare("=")==0)                 // 如果字段有值(=ALL),即为空
            {
                if(tmpFieldInfo->m_FieldDesValue.size()<=0) // 没有值，不满足条件
                    return false;                          
            } 
            else 
            {
                return false;
            }
        }
    }
    
	// 复位
	m_serviceId.clear();
	m_dataFlowUps.clear();
	m_dataFlowDns.clear();
	m_durations.clear();

	// 从字段容器中提取 业务代码,上行流量,下行流量 的值
	getMultiFieldValues(theFieldInfoMaps,m_serviceIdName,m_serviceId);
	getMultiFieldValues(theFieldInfoMaps,m_DataUpName,m_dataFlowUps);
	getMultiFieldValues(theFieldInfoMaps,m_DataDnName,m_dataFlowDns);
	getMultiFieldValues(theFieldInfoMaps,m_DurationName,m_durations);
	
	// 把字段组合成为：serviceId|dataVolumeGPRSUp|dataVolumeGPRSDn
	len=m_serviceId.size();
	m_cbcInfo = new FieldInfo(0);
//	m_cbcInfo->m_IsFormat   = true;
	m_cbcInfo->m_FieldName_ = m_cbc;
	m_cbcInfo->m_FieldDesValue = "";
	for( i=0;i<len;i++)
	{
		if(i>0)
			m_cbcInfo->m_FieldDesValue +=";";
		m_cbcInfo->m_FieldDesValue += m_serviceId[i] + "|" + m_dataFlowUps[i] + "|" + m_dataFlowDns[i] + "|" + m_durations[i] + "|0";
    }
	
	theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_cbc.c_str(),m_cbcInfo) );
	
	return true;
}
