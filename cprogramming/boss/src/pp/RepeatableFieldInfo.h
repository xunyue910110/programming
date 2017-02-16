// ##########################################
// Source file : RepeatableFieldInfo.h
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#ifndef REPEATABLEFIELDINFO_H_HEADER_INCLUDED_B7A62C5C
#define REPEATABLEFIELDINFO_H_HEADER_INCLUDED_B7A62C5C

#include "config-all.h"
#include "FieldInfo.h"
#include <string>
#include <vector>

USING_NAMESPACE(std);

class MyString;

//##ModelId=4850CE02031C
//##Documentation
//## 处理字段保存类
class RepeatableFieldInfo:public FieldInfo
{
  public:
    //##ModelId=4850CE560157
    RepeatableFieldInfo();

    //##ModelId=4850CE670251
    virtual ~RepeatableFieldInfo();
    
    virtual void reset();
    virtual int getNumCount();
    
    virtual void setDesValue(string &value);
    
    virtual void setDesValue(const char *value);
    
    virtual void setSrcValue(const unsigned char *value,const int iLength);

  public:
	int m_iNum;                            // 处理一条cdr时,重复的字段数,每处理完一条,清零
	vector<FieldInfo *> m_fieldInfoVect;        // 每处理完一条cdr 不清空 进程退出时 才clear
};



#endif /* REPEATABLEFIELDINFO_H_HEADER_INCLUDED_B7A62C5C */
