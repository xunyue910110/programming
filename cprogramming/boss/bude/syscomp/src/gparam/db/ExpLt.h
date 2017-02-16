// ############################################
// Source file : ExpLt.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20031124
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef EXPLT_H_INCLUDED_C03E4D5A
#define EXPLT_H_INCLUDED_C03E4D5A   

#include "Exp.h"

//##ModelId=3FAF49D5012F
//##Documentation
//## 小于表达式
class ExpLt : public Exp
{
  public:
    //##ModelId=3FAF49EE0121
    ExpLt();

    //##ModelId=3FAF49EF03C1
    virtual ~ExpLt();

    //##ModelId=3FC1A28F020D
    virtual bool evaluate(const vector<string> *right,const ParameterCdr *parameterCdr);

    //##ModelId=3FC1A2CA0190
    virtual void setCdr(const ParameterCdr *parameterCdr);

};



#endif /* EXPLT_H_INCLUDED_C03E4D5A */
