// ##########################################
// Source file : FieldValidateRule_4.cpp
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080921
// Update      : 20080922
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#include "FieldValidateRule_4.h"
#include "FieldInfos.h"

//##ModelId=485645EB01B5
// 此校验方法专用于根据交换机号区分相同目录下的原始文件，进行文件连续性校验。
bool FieldValidateRule_4::initialize(string ruleText)
{
    vector<string> tmpStrs,tmpStrs1;

	    
    // example:	rule_4:VALID_TYPE=system;MSC_TYPE=SwithNo(1,4);FileNo(5,4)
    // 根据交换机号区分相同目录下的原始文件，进行文件连续性校验
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
        return false;
    }
    //得到swichno的起始位置和长度
    if( setSubstr( tmpStrs1[1] ) != true )                       // example :   SwithNo(0,3)
    {
        return false;
    }
    m_iSubSwitchStart = m_substart;
    m_iSubSwitchLenth = m_subLength;
    //over

    //得到fileno的起始位置和长度
    if( setSubstr( tmpStrs[2] ) != true )                       // example :   substr(1,8)
    {
        return false;
    }
    m_iSubFileStart = m_substart;
    m_iSubFileLenth=m_subLength;
    //over
    return true;
}

//##ModelId=485645F30167
int FieldValidateRule_4::validField()
{
    string thefileName,sSwithNo,tmp,tmpStr;
    int FileNo = 0;
    map<string, int>::iterator iteor;
	//map<string, FieldInfo>::iterator itr;
	FieldInfo *tmpFieldInfo = 0;
	
	tmpStr = string("FILENAME");
	//tmpFieldInfo = getFieldMap( theFieldInfoMaps,tmpStr );
	tmpFieldInfo =m_pFieldInfos->getFieldInfoByName( m_fieldName.c_str());
	if( tmpFieldInfo != 0 )
	{
		thefileName = tmpFieldInfo->m_FieldDesValue;	
	}

    if(thefileName.size()==0)     // 找不到文件名，不校验。
    {
        return 0;
    }    
    sSwithNo=thefileName.substr(m_iSubSwitchStart,m_iSubSwitchLenth);
    tmp     =thefileName.substr(m_iSubFileStart,m_iSubFileLenth);
    
    if(tmp.size()==0 || sSwithNo.size()==0 )  // 参数配置错误
    {
       theErrorMessages->insert(PP_ERROR_CONFIG_FILE, "valid rule_4 error! ");
       return -1;
    }
    
   iteor = m_FileNoMap.find( sSwithNo );  //按交换机号查找文件序号
   
    if( iteor != m_FileNoMap.end() )    //若找到，则进行连续性校验
  
   {
      FileNo=atoi(tmp.c_str());
      if( (iteor->second + 1) != FileNo )
      {
          // 文件连续性错误，文件照常处理。
         theErrorMessages->insert(PP_ERROR_FILE_NO, "文件 "+thefileName+" 连续性错误!");
      }
      iteor->second = FileNo;
   }
   else  //没有找到则压入map
   {
       m_FileNoMap.insert( map<string,int>::value_type(sSwithNo,FileNo) );
   }
    return 0;
}

