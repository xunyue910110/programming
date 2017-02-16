// ############################################
// Source file : Exp.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20031124
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef EXP_H_INCLUDED_C03E44DC
#define EXP_H_INCLUDED_C03E44DC

#include "Config.h"

#include <string>
#include <vector>
#include <stdio.h>

#include "util/Util.h"

#include "Parameter.h"
#include "ParameterCdr.h"
#include "ParameterLog.h"

USING_NAMESPACE(std);


//##ModelId=3FAF48F4001D
//##Documentation
//## 表达式基类   
class Exp
{
  public:
    //##ModelId=3FAF4B260039
    Exp();

    //##ModelId=3FAF4B270117
    virtual ~Exp();

    //##ModelId=3FC19FE801AD
    virtual bool evaluate(const vector<string> *right,const ParameterCdr *parameterCdr){return true;};

    //##ModelId=3FC1A03F0342
    virtual void setCdr(const ParameterCdr *parameterCdr){};
    
    void setCompress(vector<bool> *isCompress);

    //##ModelId=3FC1A04B01E1
    bool initialize(const string &where, Parameter *parameter, vector<string> *inFields);

    //##ModelId=3FC1A0560042
    bool initialize(const string &leftWhere, const string &rightWhere, Parameter *parameter, vector<string> *inFields);

    //##ModelId=3FC1A060034A
    Exp *getExp(const char *where, Parameter *parameter, vector<string> *inFields);  
    
    char * format(char *DST, const char *SRC);
    
  protected:
    //##ModelId=3FC1A09C03DC
    int m_leftId;   
    
    //##ModelId=3FC1A0A10267
    int m_rightId; //是个坐标 
    
    //##ModelId=3FC1A0B70313
    Exp *m_leftExp;  //可以形成一个树结构
    
    //##ModelId=3FC1A0BD039E
    Exp *m_rightExp; //可以形成一个树结构 
    
    //##ModelId=3FC1A0D50212
    char m_token[10];  
    
    //##ModelId=3FC1A0DE0251
    const ParameterCdr *m_parameterCdr;
    
    vector<bool> *m_isCompress;


  private:
    //##ModelId=3FC1D55E02A7
    void getExpType(const char *where, string &expType, string &leftWhere, string &rightWhere); 
    
    //##ModelId=3FC1F1EC0316
    string getOperator(const string &where);
    
    //##ModelId=3FC59C6C0340
    ParameterLog *m_parameterLog;


};



#endif /* EXP_H_INCLUDED_C03E44DC */
