// ##########################################
// Source file : FieldConvertRule_17.h
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// mender      : li zhao
// E_mail      : li zhao@lianchuang.com
// Create      : 20010921
// Update      : 20080926
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef CONVERTRULE_17_H_INCLUDED_C4555C9F
#define CONVERTRULE_17_H_INCLUDED_C4555C9F

#include "FieldConvertRule.h"
#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

class FieldInfos;
//##ModelId=3BAAC0440237
//##Documentation
//## 对满足条件的话单,在指定的目录下,生成以 “原始文件名+msisdn”的文件,送网络部,以控制用户的通话
class FieldConvertRule_17 : public FieldConvertRule
{
  public:
  	FieldConvertRule_17(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
	//##ModelId=3BAAEABE023D
	virtual bool initialize(string ruleText);

	//##ModelId=3BAAEAFF02AF
	virtual bool convertField();
	
	virtual ~FieldConvertRule_17();

  protected:
  	virtual void getFieldObj();

  private:
	//输出文件目录
	string m_desPath;
	
	//输出临时目录
	string m_tmpPath;

    static FILE *m_fDes;    
        
    //上一个处理的文件名    
    static string m_lastFileName;    
        
    //正在处理的文件名    
    static string m_thisFileName;    
        
    //上一个临时文件(带路径)    
    static string m_lastTmpFileName;    
        
    //正在处理的临时文件名(带路径)    
    static string m_thisTmpFileName;
    
    static string m_PREFIX;    
	
	FieldInfo *m_pFieldMsisdn;
	FieldInfo *m_pFieldDu;
};



#endif /* CONVERTRULE_2_H_INCLUDED_C4555C9F */
