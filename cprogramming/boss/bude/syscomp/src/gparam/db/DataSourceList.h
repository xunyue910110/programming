// ############################################
// Source file : DataSourceList.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20030925
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef DATASOURCELIST_H_INCLUDED_C08DA573
#define DATASOURCELIST_H_INCLUDED_C08DA573

#include <string>
#include <vector>

#include "DataSource.h" 
#include "DbDataSource.h"
#include "FileDataSource.h"
class ParameterLog;

//##ModelId=3F7272B1039E
//##Documentation
//## 数据源管理类
class DataSourceList
{   
  public:         
    //##ModelId=3F72756B00C7
    DataSourceList();
  
    //##ModelId=3F72757302DB
    ~DataSourceList();   
    
    //##ModelId=3F72750D0112
    //##Documentation
    //## 初始化方法，从配置文件中读取数据源的配置，创建数据源对象。
    bool initialize(const int &type,const vector <string> & param);
    
    //##ModelId=3F72764501E3
    //##Documentation
    //## 根据sourceID求得该数据源对象的指针
    //DataSource *getInstance(string name);
    DataSource *getInstance(string name,int &type);
    //##ModelId=3F9F52A70212
    void close();

  protected:

  private:
    //##ModelId=3F7273CD01E4
    //##Documentation
    //## 数据源对象列表
    vector<DataSource * > m_dataSource;
    vector<int>           m_sourceTypeList; //类型列表 add by gaojf
    //##ModelId=3F75306B0292
    ParameterLog *m_parameterLog;

};

#endif /* DATASOURCELIST_H_INCLUDED_C08DA573 */
