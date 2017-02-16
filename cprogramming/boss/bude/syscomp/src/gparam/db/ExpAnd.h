// ############################################
// Source file : ExpAnd.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20031124
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef EXPAND_H_INCLUDED_C03E4AF8
#define EXPAND_H_INCLUDED_C03E4AF8 

#include "Exp.h"
class ParameterCdr;

//##ModelId=3FAF50BE00F6
//##Documentation
//## 并且表达式
class ExpAnd : public Exp
{
  public:
    //##ModelId=3FAF50D00142
    ExpAnd();

    //##ModelId=3FAF50D1028E
    virtual ~ExpAnd();

    //##ModelId=3FC1A2AD017A
    virtual bool evaluate(const vector<string> *right,const ParameterCdr *parameterCdr);

    //##ModelId=3FC1A2D4018A
    virtual void setCdr(const ParameterCdr *parameterCdr);

};



#endif /* EXPAND_H_INCLUDED_C03E4AF8 */
