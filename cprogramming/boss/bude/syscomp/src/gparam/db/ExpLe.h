// ############################################
// Source file : ExpLe.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20031124
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef EXPLE_H_INCLUDED_C03E2233
#define EXPLE_H_INCLUDED_C03E2233 

#include "Exp.h"

//##ModelId=3FAF4AB8009F
//##Documentation
//## 小于等于表达式
class ExpLe : public Exp
{
  public:
    //##ModelId=3FAF4ADA0012
    ExpLe();

    //##ModelId=3FAF4ADB0172
    virtual ~ExpLe();

    //##ModelId=3FC1A2A9002A
    virtual bool evaluate(const vector<string> *right,const ParameterCdr *parameterCdr);

    //##ModelId=3FC1A2D100D2
    virtual void setCdr(const ParameterCdr *parameterCdr);

};



#endif /* EXPLE_H_INCLUDED_C03E2233 */
