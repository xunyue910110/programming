// ############################################
// Source file : DataSource.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20030925
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef DATASOURCE_H_INCLUDED_C08DDF48
#define DATASOURCE_H_INCLUDED_C08DDF48   

#include "Config.h"

#include <string>
#include <vector> 
#include <stdio.h>

#include "ParameterLog.h"

USING_NAMESPACE(std);

//##ModelId=3F72703B0009
//##Documentation
//## 数据源基类
class DataSource
{
  public:
    //##ModelId=3F72717902AF
    //##Documentation
    //## 初始化对象属性
    virtual bool initialize(const vector<string> & name)=0;   
    
    //##ModelId=3F7275260096
    DataSource();
    
    //add by gaojf 2006-6-7 14:36
    DataSource(const DataSource &right);
    void operator=(const DataSource& right);
    
    //##ModelId=3F72752D0367
    virtual ~DataSource();
    
    //##ModelId=3F7278BA03BE
    //##Documentation
    //## 从数据源读取一条记录
    virtual int getRecord(vector <string> &cdr);   
    
    //##ModelId=3F74E8FC03A5
    string getSourceID();   
    
    //##ModelId=3F75463802D3
    virtual long reset(const string &attribute) = 0;   
    
    //##ModelId=3F9F4F24010B
    virtual void close() = 0;  
    
    //##ModelId=3FD7D08E0131
    virtual bool needUpdata(const string &srcFile) = 0;

    
  protected:
    //##ModelId=3F727687010B
    //##Documentation
    //## 数据源属性，如果是数据库，则为select语句；如果是文件，则为文件名称。
    string m_attribute;
    
    //##ModelId=3F7279D901E0
    //##Documentation
    //## 数据源ID
    string m_sourceID; 
    
    //##ModelId=3F75305A014D
    ParameterLog *m_parameterLog; 
    
    //##ModelId=3F9F4EDF0288
    int m_dp;



  private:

};
#endif /* DATASOURCE_H_INCLUDED_C08DDF48 */
