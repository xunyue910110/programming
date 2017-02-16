// ############################################
// Source file : ExpEq.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20031124
// Copyright(C): 2003 by Yang Zhang, Linkage.
// #############################################

#ifndef EXPEQ_H_INCLUDED_C03E47CA
#define EXPEQ_H_INCLUDED_C03E47CA 

#include "Exp.h"

//##ModelId=3FAF4A6D00A1
//##Documentation
//## 等于表达式
class ExpEq : public Exp
{
  public:
    //##ModelId=3FAF4A820065
    ExpEq();

    //##ModelId=3FAF4A83008F
    virtual ~ExpEq();

    //##ModelId=3FC1A2990103
    virtual bool evaluate(const vector<string> *right,const ParameterCdr *parameterCdr);

    //##ModelId=3FC1A2DE0301
    virtual void setCdr(const ParameterCdr *parameterCdr);

};



#endif /* EXPEQ_H_INCLUDED_C03E47CA */
