// ##########################################
// Source file : FieldConvertRule_16.h
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20020516
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef CONVERTRULE_16_H_INCLUDED_C4555C9F
#define CONVERTRULE_16_H_INCLUDED_C4555C9F

#include "FieldConvertRule.h"
#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

//##ModelId=3BAAC0440237
//##Documentation
//## 对满足条件的话单,在指定的目录下,生成以 “原始文件名+msisdn”的文件,送网络部,以控制用户的通话
class FieldConvertRule_16 : public FieldConvertRule
{
  public:
	//##ModelId=3BAAEABE023D
	virtual bool initialize(string ruleText);

	//##ModelId=3BAAEAFF02AF
	virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);
	
	virtual ~FieldConvertRule_16();

  private:
	//##ModelId=3BAFF1ED006F
  	string m_conditionFieldName;

	//##ModelId=3C19E10F02FE
	string m_conditionFieldValue;
	
	//##ModelId=3C39462000C8
	string m_conditionExp;

	//输出文件目录
	string m_desPath;
	
	//输出临时目录
	string m_tmpPath;

    static FILE *m_fDes;    
        
    //上一个处理的文件名    
    static string m_lastFileName;    
        
    //正在处理的文件名    
    static string m_thisFileName;    
        
    //上一个临时文件(带路经)    
    static string m_lastTmpFileName;    
        
    //正在处理的临时文件名(带路径)    
    static string m_thisTmpFileName;
    
    static string m_PREFIX;    

};



#endif /* CONVERTRULE_2_H_INCLUDED_C4555C9F */
