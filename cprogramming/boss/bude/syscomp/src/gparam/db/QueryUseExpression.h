// ############################################
// Source file : QueryUseExpression.h
// Version     : 0.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031113
// Update      : 20031113
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef QUERYUSEEXPRESSION_H_INCLUDED_C04CF340
#define QUERYUSEEXPRESSION_H_INCLUDED_C04CF340 

#include "Config.h"

#include <vector> 
#include <string>     

class Index; 
class Parameter;
class ParameterLog;
class ParameterCdr;
class Exp; 

USING_NAMESPACE(std);

//##ModelId=3F9488000038 代表了一个 query 的配置 如 getServInfoByImsi
class QueryUseExpression
{
  public:
    //##ModelId=3FAEED5303D5
    QueryUseExpression();

    //##ModelId=3FAEED55020B
    ~QueryUseExpression();

    //##ModelId=3FAEEF0A0191
    //##Documentation
    //## 等于方法
    bool eq(const Index &left, const Index &right);

    //##ModelId=3FAEEF880247
    //##Documentation
    //## 小于方法
    bool lt(const Index &left, const Index &right);

    //##ModelId=3FB0A1510131
    int find(const vector<string> &fields, char *result);

    //##ModelId=3FB0A16103A1
    void getExpression();  
    
    //##ModelId=3FB462C70161
    vector <Parameter *> *getList(); 
    
    //##ModelId=3FB479000107
    bool setInFields(const string &fields);  
    
    //##ModelId=3FB8810C014C
    //##Documentation
    //## 此方法供parameterIndexBcd调用，用来对输入参数进行BCD编码
    const vector<string> *getFields();  
    
    //##ModelId=3FB9C90203A4
    //##Documentation
    //## 表达式初始化方法
    bool setExpression(const string &eq, int order);  
    
    //##ModelId=3FB9DA86029E
    void setFields(const vector<string> *fields);   
    
    //##ModelId=3FBC45A003BE
    int getSeachType(); 
    
    //##ModelId=3FCD40E90386
    bool setOutFields(const string &fields);  
    
    bool isCompress(int fieldId);
    
    void setBufferSize(const int &size);
    
    string getQuery();
    
  private:


    //##ModelId=3FB0969B008E 一个 query 的配置 项会用到哪些查询表 如 getServInfoByImsi 对应 <parameter_table>:ServInfoPnIncre ,ServInfoPn
    vector<Parameter *> m_parameterList; 
    
    //##ModelId=3FB478CF02C9
    vector<int> m_inFieldList; //是这个query 中配置的in_fields字段 在上面表中(第一个表)配置字段的 位置

    //##ModelId=3FC371890242
    vector<string> m_inFieldName;  
    
    //##ModelId=3FB47E7A009F
    ParameterLog *m_parameterLog;  
    
    //##ModelId=3FB864A50287 //客户端传进来的值 460003013700536,20031001000000
    const vector<string > *m_fields;  
    
    //##ModelId=3FBC45A002E2
    //##Documentation
    //## 查询类型(0:顺序查找;1:二分法查找)
    int m_seachType; 
    
    //##ModelId=3FC2105700F6
    Exp *m_exp; 
        
    //##ModelId=3FCD41E80274
    vector<int> m_outFieldList;
    
    vector <bool> *m_isCompress;
    
    int m_bufferSize;
    
};



#endif /* QUERYUSEEXPRESSION_H_INCLUDED_C04CF340 */
