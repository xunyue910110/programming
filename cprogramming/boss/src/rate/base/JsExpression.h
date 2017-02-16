#ifndef _EXPRESSION_H_INCLUDE_20090813_
#define _EXPRESSION_H_INCLUDE_20090813_

#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <time.h>

using namespace std;

class Variable
{
  public :
    string m_varName;
    int m_varAttrid;
    int m_varType;
    //added by hany 2011/3/21 19:51:27
    int m_offsetValue;
    friend bool operator < (const Variable &left,const Variable &right);
    friend bool operator == (const Variable &left,const Variable &right);
};

class JsExpression
{
  public :
    string m_jsStatement;
    vector<Variable> m_variables;
  public :
    void dump();
    //分析简单表达式,E($100+3)*$101
    bool parseExpr(const string &r_expr);
    //added by hany at 2011/3/31 16:44:27
    //分析特殊表达式,H($100~1)*$101~2
    bool ParseSpecialExpr(const string &r_expr);
    //分析JavaScript表达式如 J(var result = (inBuf.getValue($100)+3)*inBuf.getValue($101); outBuf.pushValue('result',result);
    bool parseJScript(const string &r_expr);
};

//int main(int argc, char *argv[])
//{
//  Expression expr;
//  cout << " Expr = ?5-100*?1*?2+?3+?4/33-55+$2000 " << endl;
//  expr.parseExpr("?5-100*?1*?2+?3+?4/33-55+$2000");
//  expr.dump();
//  return 0;
//}

#endif
