// ############################################
// Source file : ParameterIndex.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20030925
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef PARAMETERINDEX_H_INCLUDED_C08D1A7A
#define PARAMETERINDEX_H_INCLUDED_C08D1A7A   

#include <string>   
#include <vector>
#include "Config.h"

#include "Index.h"
#include "ParameterLog.h"
#include "ParameterCdr.h"  
#include "ParameterPool.h" 
#include "IndexMessage.h"  
#include "QueryUseExpression.h"


//##ModelId=3F72918C0194 一张表的一种索引的全部
class ParameterIndex
{   
  public:
    
    //##ModelId=3F72A46803D1
    ParameterIndex(); 
    
    //##ModelId=3F7505C800C1
    bool initialize(const string name, const int id, const string order);

    //##ModelId=3F72A46F0164
    ~ParameterIndex();
    
    //##ModelId=3F72AF760069
    //##Documentation
    //## 初始化对象属性
    void setKey(ParameterCdr *cdr, vector <bool> *isCompress);  
    
    //##ModelId=3F7997F9015D
    int getFieldID();  
    
    //##ModelId=3F79A1E70271
    void resize(long size);    
    
    //##ModelId=3F7A38B60076
    void sort();  
    
    //##ModelId=3F8CD58600A5
    int seach(vector <Index> &result,QueryUseExpression *queryUseExpression ) ;  
    
    //##ModelId=3FB85CF800E7
    void setQueryUseExpression(QueryUseExpression *queryUseExpression); 
    
    //##ModelId=3FD3EB45039F
    bool clear();
    
    int dump(string &result,const int &no);
    int dump(string &result);
    
    int getCount();
    
    void setIsCompress(const vector <bool> *isCompress);

     
  protected:
    //##ModelId=3F729E9D013A
    //##Documentation
    //## 关键字字段名称
    vector<string> m_keyName; 
     
    //##ModelId=3F75146B0371
    vector<int> m_fieldId; //一张表的一种索引的全部字段在 表配置项 fields_name 中的坐标 
    
    //##ModelId=3FD3EA180215
    ParameterPool<Index> * m_indexList;   
        
    //##ModelId=3F7530A30134
    ParameterLog *m_parameterLog;  
    
    //##ModelId=3F9619AF0365
    vector<bool> m_order; //一张表的一种索引的全部字段做为索引的排序方法(0 升序;其余 降序) 
    
    //##ModelId=3F9CBF850024
    Index        m_index;//当前的index(含一条记录及索引)
    
    //##ModelId=3F9DDF7D000C
    IndexMessage m_indexMessage;
    
    int m_no;    

    const vector <bool> *m_isCompress;
    
};

#endif /* PARAMETERINDEX_H_INCLUDED_C08D1A7A */
