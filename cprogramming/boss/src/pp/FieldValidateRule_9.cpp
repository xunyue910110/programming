// ##########################################
// Source file : FieldValidateRule_9.cpp
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080921
// Update      : 20080924
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#include "FieldValidateRule_9.h"
#include "FieldInfos.h"


//##ModelId=4856479C006D
bool FieldValidateRule_9::initialize(string ruleText)
{
    vector<string> tmpStrs,tmpStrs1;
	// example:	rule_9:VALID_TYPE=fields;CALL_TYPE=ALL;CDR_NO   
	// 此校验方法专用于对给定字段进行连续性校验
  //splitStrByToken(ruleText,";",tmpStrs);
	StringUtil::split(ruleText,";",tmpStrs);
    
	if( tmpStrs.size() != 3 )
	{
		// 小于2个分号的,不符合规则的书写格式
		return false;	
	}
	//splitStrByToken(tmpStrs[1],"=",tmpStrs1);
  tmpStrs1.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs1);

	if (tmpStrs1.size() != 2)
	{
	    m_callType  = tmpStrs1[1];  //get "ALL"
    }
	m_fieldName = tmpStrs[2];     // get "CDR_NO"
 
	return true;
}

//##ModelId=485647A20196
int FieldValidateRule_9::validField()
{
    int  cdrNo=0;
    char tmp[10];
    string thefileName,t1;
    FieldInfo *tmpFieldInf = 0;
    
	thefileName = m_PpLog_->getSrcFileName();
    
    //tmpFieldInf = getFieldMap(theFieldInfoMaps,m_fieldName);
    tmpFieldInf =m_pFieldInfos->getFieldInfoByName(m_fieldName.c_str());
    if( tmpFieldInf != 0 )
    {
    	t1 =tmpFieldInf->m_FieldDesValue;
    	cdrNo = atoi (t1.c_str() );	
    }
    
    if(cdrNo>0)     // 如果字段"CDR_NO"存在，对话单进行话单连续性校验 
    {                    
      if(m_PpLog_->getBeginCdrNo()==0) 
      {
            m_PpLog_->setBeginCdrNo(cdrNo);
      }
      m_PpLog_->setEndCdrNo(cdrNo);
      if( (m_cdrFlag->m_cdrNo != -1) && (m_cdrFlag->m_cdrNo + 1 != cdrNo)
            && (cdrNo != 1) )               // zww added 20060309 话单序列号循环从0开始则不告警
      {
         sprintf(tmp,"%d",cdrNo);
         theErrorMessages->insert(PP_ERROR_CDR_NO,"话单连续性错误(文件："+
                                 thefileName+",No."+tmp+")!");
      }
      m_cdrFlag->m_cdrNo = cdrNo;
    }
    return 0;
}

