// ############################################
// Source file : Parameter.h
// Version     : 0.2
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20031030
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef PARAMETER_H_INCLUDED_C08D0711
#define PARAMETER_H_INCLUDED_C08D0711 

#include "DataSource.h" 
#include "DbDataSource.h"
#include "FileDataSource.h"
#include "ParameterCdr.h"  
#include "ParameterIndex.h"
#include "ParameterLog.h"
#include "ParameterPool.h" 
#include "ParameterBuffer.h"
#include "Index.h"  
#include "ParameterInterface.h" //added by zhanglong 2006-4-2 21:30
class QueryUseExpression;

//##ModelId=3F728EA8010E
//##Documentation
//## 参数类
//## 保存一个参数表的全部内容(一个表或一个文件)
class Parameter
{
  public:
    //##ModelId=3F72904803D3
    //##Documentation
    //## 设置m_dataSource
    //void setDataSource(DataSource *datasource);  
    //changed by gaojf 2006-6-7 14:29
    void setDataSource(DataSource *datasource,int sourceType);  
    //##ModelId=3F729F7E0129
    //##Documentation
    //## 对参数索引m_parameterIndex中的每一个索引对象进行排序
    void sort();

    //##ModelId=3F729F8E0349
    //##Documentation
    //## 获得参数记录
    bool getParameter(vector <string> *record);
  
    //##ModelId=3F72A06100B7
    Parameter();
  
    //##ModelId=3F72A066024F
    ~Parameter(); 
    
    //##ModelId=3F72A4B0023A
    //##Documentation
    //## 添加参数记录，步骤如下：
    //## 1. 创建一个ParameterCdr对象，添加到m_parameterCdr列表中
    //## 2. 向索引对象添加索引记录
    bool insert(const vector<string> & field);
     
    //##ModelId=3F74E5930151
    bool initialize(const vector<string> param_1, const vector<string> param_2);    
    
    //##ModelId=3F75173F0051
    int getFieldID(const string &name);  
    
    int getCount();
    
    //##ModelId=3F7542DF01E1
    bool getData(); 
    
    void setTableName(const string & tableName);
    
    string getTableName();
    
    //##ModelId=3F78E81B00E9
    bool getDataIndex();  
    
    //##ModelId=3F8376380330
    long capacity(); 
        
    //##ModelId=3FB83CF402A4
    int find(const vector <int> &inFieldList, vector<const ParameterCdr *> &result);
    
    //##ModelId=3FD3E372009B
    bool upData();  
    
    //##ModelId=3FD7BCE901BC
    bool needUpdata(const string &timeStamp, const vector<string> &tables);
    
    //##ModelId=4028809302F9
    int getTable(string &result);
    
    bool isCompressField(int fieldId);
    
    vector <bool> * getCompressVector();
    
    int dump(string &result,const int &no);
    int dump(string &result);

  private:
    //##ModelId=3FA08A02005D
    char *format(char *strDes, const char *strSrc, const string &format,const bool isCompress);
  
    void toSwitch();
  
  private:
    //##ModelId=3F7290310394
    //##Documentation
    //## 数据源对象指针，用来从数据源取参数
    DataSource *m_dataSource;
    
    //##ModelId=3F74F0F10029
    //##Documentation
    //## 数据源属性，如果是数据库，则为select语句；如果是文件，则为文件名称。
    string m_name;  
    
    string m_tableName1;
    
    //##ModelId=3F74F122014C
    vector <string >m_fieldsName;  
    
    vector <bool> m_isCompress;
    
    //##ModelId=3F752FF70033
    ParameterLog *m_parameterLog; 
    
    //##ModelId=3F729E4F00F1
    //##Documentation
    //## 一张表的几种索引 (而每一个索引有可能有几个字段)
    vector<ParameterIndex *> m_parameterIndex;
    
    vector<ParameterIndex *> m_parameterIndexTmp;//added by zhanglong 2006-4-4 14:12
     
     //##ModelId=3F88BC320259
    ParameterBuffer *m_parameterBuffer; 
    
    //##ModelId=3F88BC320259
    ParameterBuffer *m_parameterBufferTmp; 
    
   //##ModelId=3FD3E1BC00A6
    //##Documentation
    //## 参数记录对象存放列表
    ParameterPool< ParameterCdr > *m_parameterCdrPool; 

    //## 参数记录对象存放列表，临时用
    ParameterPool< ParameterCdr > *m_parameterCdrPoolTmp; 

    //##ModelId=3FA07F4B035C
    vector<string> m_format;   
    
    //##ModelId=3FC59B06011F
    int m_count;
      
    //##ModelId=3FD58EE00339
    bool m_needUpdata;  
    
     int m_bufferUnit;
    
   //##ModelId=3FD66C670040
    string m_lastUpdateTimeStamp;
 
    //##ModelId=3FD66E2E00EE
    time_t m_updateIntervalTime;

    //##ModelId=3FD66E410222
    time_t m_lastUpdateTime;   
    
    //##ModelId=3FD6B9AA0113
    vector<string> m_tableName;
};



#endif /* PARAMETER_H_INCLUDED_C08D0711 */

