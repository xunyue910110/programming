// ############################################
// Source file : ExpNe.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20031124
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef EXPNE_H_INCLUDED_C03E1A13
#define EXPNE_H_INCLUDED_C03E1A13  

#include "Exp.h"

//##ModelId=3FAF4BCE0257
//##Documentation
//## 不等于表达式
class ExpNe : public Exp
{
  public:
    //##ModelId=3FAF4BDA036D
    ExpNe();

    //##ModelId=3FAF4BDB03B5
    virtual ~ExpNe();

    //##ModelId=3FC1A29502B6
    virtual bool evaluate(const vector<string> *right,const ParameterCdr *parameterCdr);

    //##ModelId=3FC1A2E10279
    virtual void setCdr(const ParameterCdr *parameterCdr);

};



#endif /* EXPNE_H_INCLUDED_C03E1A13 */
