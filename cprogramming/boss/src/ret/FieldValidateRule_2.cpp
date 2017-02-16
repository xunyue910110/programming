// ##########################################
// Source file : FieldValidateRule_2.cpp
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20011123
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "FieldValidateRule_2.h"

//##ModelId=3BAAA7DF0060
bool FieldValidateRule_2::initialize(string ruleText)
{
	int len,ndx;
	string tmp,tmp1;
	vector<string> tmpStrs,tmpStrs1;

    // example:	rule_2;system;FILE_NO;substr(10,0)            # 文件连续性校验
    // example:	rule_2;system;FILENAME;substr(1,2)=FT         # 文件子串校验
	splitStrByToken(ruleText,";",tmpStrs);
	
	if( tmpStrs.size() != 4 )
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
	m_fieldName = tmpStrs[2]; // 跳过 "rule_2","system"

	len = tmpStrs[3].size();
	if ( ( ndx  = tmpStrs[3].find(")") ) < 0 )
	   return false;
	
	tmp1=lrTrim( tmpStrs[3].substr(0,ndx+1) );
	if( setSubstr( tmp1 ) != true )                       // substr(1,8)
	   return false;
	
	if ( ( ndx  = tmpStrs[3].find("=") ) > 0 ) 
	{
	   m_validValue=lrTrim( tmpStrs[3].substr(ndx+1,len-ndx) );  // example : FT
	}
	
	return true;
}

//##ModelId=3BAAAA29017C
// 此校验方法专用于对文件名的校验，包括关键字校验和文件连续性校验。
int FieldValidateRule_2::validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
	int fileNo,errorCode=0;
	string thefileName,tmp,tmpStr;
	FieldInfo *tmpFieldInfo = 0;
	
	tmpStr = string("FILENAME");
	tmpFieldInfo = getFieldMap( theFieldInfoMaps,tmpStr );
	if( tmpFieldInfo != 0 )
	{
		thefileName = tmpFieldInfo->m_FieldDesValue;	
	}
	
	if(thefileName.size()==0)     // 找不到文件名，不校验。
	   return errorCode;
	
	substr(thefileName,tmp);
	if(tmp.size()==0)            // 参数配置错误
	{
	   theErrorMessages->insert(PP_ERROR_CONFIG_FILE, "valid rule_2 error! ");
	   return -1;
	}
	if(m_fieldName.compare("FILE_NO")==0)       // 进行文件连续性校验
	{
	   fileNo=atoi(tmp.c_str());
	   
	   if( (m_RetLog_->m_fileNo != -1) && (m_RetLog_->m_fileNo + 1 != fileNo ) ) 
	   {
	       // 文件连续性错误，文件照常处理。
	      theErrorMessages->insert(PP_ERROR_FILE_NO, "文件 "+thefileName+" 连续性错误!");
	   }
	
	   m_RetLog_->m_fileNo = fileNo;
	} 
	else if (tmp.compare(m_validValue)!=0)    // 进行文件关键字校验
	{
	   // 文件关键字错误，此文件不处理，移动到错误文件目录中。
	   theErrorMessages->insert(PP_ERROR_FILE_NAME, "文件 "+thefileName+" 不包含关键字 "+
	                            m_validValue+"!");
	   errorCode=PP_ERROR_FILE_NAME;  //
	}
	
	return errorCode;
}

