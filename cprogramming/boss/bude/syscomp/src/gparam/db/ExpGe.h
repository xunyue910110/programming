// ############################################
// Source file : ExpGe.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20031124
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef EXPGE_H_INCLUDED_C03E6D05
#define EXPGE_H_INCLUDED_C03E6D05 

#include "Exp.h"

//##ModelId=3FAF4B41018C
//##Documentation
//## 大于等于表达式
class ExpGe : public Exp
{
  public:
    //##ModelId=3FAF4B4F0377
    ExpGe();

    //##ModelId=3FAF4B510059
    virtual ~ExpGe();

    //##ModelId=3FC1A29D02CC
    virtual bool evaluate(const vector<string> *right,const ParameterCdr *parameterCdr);

    //##ModelId=3FC1A2CE0019
    virtual void setCdr(const ParameterCdr *parameterCdr);

};



#endif /* EXPGE_H_INCLUDED_C03E6D05 */
