// ############################################
// Source file : ExpStrnEq.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031128
// Update      : 20031128
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef EXPSTRNEQ_H_INCLUDED_C0392558
#define EXPSTRNEQ_H_INCLUDED_C0392558

#include "Exp.h"
class ParameterCdr;

//##ModelId=3FC6D82E02C0
//##Documentation
//## 按短长度进行字符串相等比较的表达式
class ExpStrnEq : public Exp
{
  public:
    //##ModelId=3FC6D8470063
    ExpStrnEq();

    //##ModelId=3FC6D84A00B7
    virtual ~ExpStrnEq();

    //##ModelId=3FC6D8D60285
    virtual bool evaluate(const vector<string> *right,const ParameterCdr *parameterCdr);

    //##ModelId=3FC6D8EC0114
    virtual void setCdr(const ParameterCdr *parameterCdr);

};



#endif /* EXPSTRNEQ_H_INCLUDED_C0392558 */
