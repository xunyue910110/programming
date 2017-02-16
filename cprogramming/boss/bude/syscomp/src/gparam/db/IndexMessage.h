// ############################################
// Source file : IndexMessage.cpp
// Version     : 0.1.0
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031028
// Update      : 20031028
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################     

#ifndef QUERYRESULT_H_INCLUDED_C0621D24
#define QUERYRESULT_H_INCLUDED_C0621D24

#include "config-all.h"

#include <vector>

class ParameterCdr;   
class QueryUseExpression;

USING_NAMESPACE(std);

//##ModelId=3F9CC68500B7
//## 索引信息类
//## 此类为index类保存了字段顺序号(代表降序还是升序)、字段Id、输入字段值等信息。
//## 此类设置的目的：
//## 1. 减少index类的属性，降低使用内存
//## 2. 提供查找表达式的实现。
class IndexMessage   
{
  public:
    //##ModelId=3F9CC6C3017F
    IndexMessage();
    
    //##ModelId=3F9CC6C40393
    ~IndexMessage();
    
    IndexMessage(const IndexMessage &indexMessage);   
    
  public:
    //##ModelId=3F9CC6DA0377
    const vector<bool> *m_order; //一张表的一种索引的全部字段做为索引的排序方法(0 升序;其余 降序)

    //##ModelId=3F9DC42103B8
    const vector<int> *m_fieldId;//一张表的一种索引的全部字段在 表配置项 fields_name 中的坐标 
    
    const vector <bool> *m_isCompress;
    
    //##ModelId=3FB85BAB0020
    QueryUseExpression *m_queryUseExpression;

};

#endif /* QUERYRESULT_H_INCLUDED_C0621D24 */
