// ##########################################
// Source file : FieldExtractRule.cpp
// System      : Mobile Billing System
// Version     : NG1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#include "FieldExtractRule.h"
#include "FieldInfos.h"
#include "PpLog.h"
#include "Pp_Exception.h"

//##ModelId=48520D220222
FieldExtractRule::FieldExtractRule(int iSubNumber)
{
    m_PpLog_              = PpLog::getInstance();
    m_padding             = 0;
    m_order               = 0;
    m_CompositeFieldSelf_ = 0;
    m_isRepeatable        = false;
    m_dataType            = "";

    m_iSubChannelNum = iSubNumber;
    m_pFieldInfos = FieldInfos::getInstance(iSubNumber);
}

//##ModelId=48520D250128
FieldExtractRule::~FieldExtractRule()
{
}

//##ModelId=48520D44005D
void FieldExtractRule::getFmtValue()
{
}

//##ModelId=48520D510109
void FieldExtractRule::checkField()
{
}

//##ModelId=48520DEF0280
string FieldExtractRule::getFieldName() const
{
    return m_FieldName_;
}

//##ModelId=4852127E01B5
string FieldExtractRule::getSectionName() const
{
    return m_SectionName_;
}

//##ModelId=48520EB9038A
int FieldExtractRule::getLaye() const
{
    return m_theLayer_;
}

//##ModelId=48520FED008C
void FieldExtractRule::setSectionName(const string &left)
{
    m_SectionName_ = left;
}

//##ModelId=4852108D037A
FieldExtractRule* FieldExtractRule::getMySelf() const
{
    return m_CompositeFieldSelf_;
}

void FieldExtractRule::getFieldInfoByName(const bool isRepeatable
											,FieldInfo *pCompsiteFieldInfo)
{
    if(pCompsiteFieldInfo==NULL)
    {
    	m_isRepeatable = isRepeatable;
    	if(m_isRepeatable==false)
    	{	
			m_pFieldInfo = m_pFieldInfos->getFieldInfoByName(m_FieldName_.c_str());
		}
		else
		{
			m_pFieldInfo = m_pFieldInfos->getRepeatableFieldInfoByName(m_FieldName_.c_str());	
		}
    }
    else
    {
    	m_pFieldInfo = pCompsiteFieldInfo;	
    }    	
    m_pFieldInfo->m_FieldName_   = m_FieldName_;
    m_pFieldInfo->m_padding      = m_padding;
    m_pFieldInfo->m_order        = m_order;     
  //  if( m_pFieldInfo->m_dataType!=""
  //  	&& m_pFieldInfo->m_dataType!=m_dataType)
  //  {
  //  	throw Pp_Exception (__FILE__, __LINE__,"同名字段 %s 数据类型不一致:%s,%s!",m_FieldName_.c_str()
  //  	             															  ,m_dataType.c_str()
  //  	             															  ,m_pFieldInfo->m_dataType.c_str());
  //  }
  //  else
  //  {
    	m_pFieldInfo->m_dataType     = m_dataType;
  //  }
     
	return;
}

void FieldExtractRule::parseConfigRule(const string &argStr,vector<string> &strResults)
{
	vector<string> tmpStrs;
	
	tmpStrs.clear();
	StringUtil::split(argStr,SPLIT_CODE_1,tmpStrs,true);
	
	if(tmpStrs.size()==1)
	{
		tmpStrs.clear();
		StringUtil::split(argStr,SPLIT_CODE_2,tmpStrs,true);			
	}
	
	for(int i=0;i<tmpStrs.size();++i)
	{
		if( tmpStrs[i].length()==0
			||tmpStrs[i].compare(SPLIT_CODE_2)==0)
		{
			tmpStrs.erase(tmpStrs.begin()+i);
			--i;	
		}	
	}
	
	if(tmpStrs.size()!=2)
	{
		throw Pp_Exception (__FILE__, __LINE__,"解析配置记录 %s 错!",argStr.c_str());	
	}
	
	m_FieldName_ = tmpStrs[0]; //字段名称
	//cout<<"######## name :"<<tmpStrs[0]<<"  length:"<<tmpStrs.size()<<endl;
	strResults.clear();
	StringUtil::split(tmpStrs[1],SPLIT_COMMA,strResults,true); //得到字段名后面的配置项

	return;
}

void FieldExtractRule::getDataTypeRule(const string &strDataTypeRule)
{
    if(m_dataType.compare(1, 3, (char *)"BCD") == 0)
    {
	    m_padding = strDataTypeRule[0];                     //得到BCD填充字符
    }
    else if (m_dataType.compare((char *)"Binary") == 0)
    {
        m_order = atoi(strDataTypeRule.c_str());                    //得到高低位交换标志
    }
    else if (m_dataType == "HEX")
    {
        m_order = atoi(strDataTypeRule.c_str());                    //得到高低位交换标志
    }
    
    return;
}
