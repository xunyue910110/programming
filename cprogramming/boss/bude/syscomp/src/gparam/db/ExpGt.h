// ############################################
// Source file : ExpGt.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20031124
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef EXPGT_H_INCLUDED_C03E080C
#define EXPGT_H_INCLUDED_C03E080C

#include "Exp.h"

//##ModelId=3FAF498602E4
//##Documentation
//## 大于表达式
class ExpGt : public Exp
{
  public:
    //##ModelId=3FAF4A350097
    ExpGt();

    //##ModelId=3FAF4A360160
    virtual ~ExpGt();

    //##ModelId=3FC1A2A40023
    virtual bool evaluate(const vector<string> *right,const ParameterCdr *parameterCdr);

    //##ModelId=3FC1A2DA039C
    virtual void setCdr(const ParameterCdr *parameterCdr);

};



#endif /* EXPGT_H_INCLUDED_C03E080C */
