// ##########################################
// Source file : FieldValidateRule_2.cpp
// System      : Mobile Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080921
// Update      : 20080922
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#include "FieldValidateRule_2.h"
#include "FieldInfos.h"

//##ModelId=4856458502CE
bool FieldValidateRule_2::initialize(string ruleText)
{
    int len,ndx;
	string tmp,tmp1;
	vector<string> tmpStrs,tmpStrs1;

    // example:	rule_2:VALID_TYPE=system;FIELD=FILE_NO;substr(10,0)            # 文件连续性校验
    // example:	rule_2:VALID_TYPE=system;FIELD=FILENAME;substr(1,2)=FT         # 文件子串校验
	//splitStrByToken(ruleText,";",tmpStrs);
		StringUtil::split(ruleText,";",tmpStrs);

	if( tmpStrs.size() != 3 )
	{
		// 小于2个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
	
	//splitStrByToken(tmpStrs[1],"=",tmpStrs1);
		StringUtil::split(tmpStrs[1],"=",tmpStrs1);

	m_fieldName = tmpStrs1[1];          // 跳过 "rule_2:VALID_TYPE=system"
                 


	len = tmpStrs[2].size();
	if ( ( ndx  = tmpStrs[2].find(")") ) < 0 )
	{
	    return false;
	}
	
	tmp1 = lrTrim( tmpStrs[2].substr(0,ndx+1) );
	
	if( setSubstr( tmp1 ) != true )                       // substr(1,8)
	{
	    return false;
	}
	
	if ( ( ndx  = tmpStrs[2].find("=") ) > 0 ) 
	{
	   m_validValue = lrTrim( tmpStrs[2].substr(ndx+1,len-ndx) );  // example : FT
	   
	}
	
	return true;
}

//##ModelId=4856458E00DA
//int FieldValidateRule_2::validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
int FieldValidateRule_2::validField()

{
    int fileNo,errorCode=0;
	string thefileName,tmpStr,tmp;
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
	     return errorCode;
    }	
	substr(thefileName,tmp);
	if(tmp.size()== 0)            // 参数配置错误
	{
	   theErrorMessages->insert(PP_ERROR_CONFIG_FILE, "valid rule_2 error! ");
	   return -1;
	}
	if(m_fieldName.compare("FILE_NO")==0)       // 进行文件连续性校验
	{
	   fileNo=atoi(tmp.c_str());
	   
	   if( (m_PpLog_->m_fileNo != -1) && (m_PpLog_->m_fileNo + 1 != fileNo ) ) 
	   {
	       // 文件连续性错误，文件照常处理。
	      theErrorMessages->insert(PP_ERROR_FILE_NO, "文件 "+thefileName+" 连续性错误!");
	   }
	
	   m_PpLog_->m_fileNo = fileNo;
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

