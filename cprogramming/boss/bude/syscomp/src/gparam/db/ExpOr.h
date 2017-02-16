// ############################################
// Source file : ExpOr.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20031124
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef EXPOR_H_INCLUDED_C03E3607
#define EXPOR_H_INCLUDED_C03E3607 

#include "Exp.h"
class ParameterCdr;

//##ModelId=3FAF50E00380
//##Documentation
//## 或者表达式
class ExpOr : public Exp
{
  public:
    //##ModelId=3FAF50F20386
    ExpOr();

    //##ModelId=3FAF50F400FE
    virtual ~ExpOr();

    //##ModelId=3FC1A2B2000F
    virtual bool evaluate(const vector<string> *right,const ParameterCdr *parameterCdr);

    //##ModelId=3FC1A2D80064
    virtual void setCdr(const ParameterCdr *parameterCdr);

};



#endif /* EXPOR_H_INCLUDED_C03E3607 */
