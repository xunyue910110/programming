// ##########################################
// Source file : FieldValidateRule_9.cpp
// System      : Mobile Billing System
// Version     : 0.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : chengm@lianchuang.com
// Create      : 20021114
// Update      : 20021114
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################
#include "FieldValidateRule_9.h"

bool FieldValidateRule_9::initialize(string ruleText)
{
	vector<string> tmpStrs;
	// example:	rule_9;fields;ALL;CDR_NO   
	// 此校验方法专用于对给定字段进行连续性校验
    splitStrByToken(ruleText,";",tmpStrs);
    
	if( tmpStrs.size() != 4 )
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}
	
	m_callType  = tmpStrs[2];  // skip "rule_5","fields" get "ALL"
	m_fieldName = tmpStrs[3];  // is "CDR_NO"
 
	return true;
}

// 此校验方法专用于对给定字段进行连续性校验
int FieldValidateRule_9::validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    int  cdrNo=0;
    char tmp[10];
    string thefileName;
    FieldInfo *tmpFieldInf = 0;
    
	thefileName = m_RetLog_->getSrcFileName();
    
    tmpFieldInf = getFieldMap(theFieldInfoMaps,m_fieldName);
    if( tmpFieldInf != 0 )
    {
    	cdrNo = atoi (tmpFieldInf->m_FieldDesValue.c_str() );	
    }
    
    if(cdrNo>0)     // 如果 字段"CDR_NO"存在，对话单进行话单连续性校验 
    {                    
      if(m_RetLog_->getBeginCdrNo()==0) 
      {
            m_RetLog_->setBeginCdrNo(cdrNo);
      }
      m_RetLog_->setEndCdrNo(cdrNo);
      if( (m_cdrFlag->m_cdrNo != -1) && (m_cdrFlag->m_cdrNo + 1 != cdrNo)
            && (cdrNo != 1) ) //zww added 20060309 话单序列号循环从0开始则不告警
      {
         sprintf(tmp,"%d",cdrNo);
         theErrorMessages->insert(PP_ERROR_CDR_NO,"话单连续性错误(文件："+
                                 thefileName+",No."+tmp+")!");
      }
      m_cdrFlag->m_cdrNo = cdrNo;
    }
    return 0;
}
