// ############################################
// Source file : DbDataSource.h
// Version     : 0.2
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20031029
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef DBDATASOURCE_H_INCLUDED_C08DA516
#define DBDATASOURCE_H_INCLUDED_C08DA516 

#include "Config.h"
#include "DataSource.h"

//##ModelId=3F7270630205
//##Documentation
//## 数据库数据源类
class DbDataSource : public DataSource
{
  public:
    //##ModelId=3F72721C006F
    virtual bool initialize(const vector<string> &name);   
    
    //##ModelId=3F72754F0077
    DbDataSource();

    //add by gaojf 2006-6-7 14:36
    DbDataSource(const DbDataSource &right);
    void operator=(const DbDataSource& right);
    //##ModelId=3F727555038C
    virtual ~DbDataSource(); 
    
    //##ModelId=3F7278EE00A2
    //##Documentation
    //## 从数据源读取一条记录
    virtual int getRecord(vector <string> &cdr);  
    
    //##ModelId=3F754EA5018B
    virtual long reset(const string &attribute);  
    
    //##ModelId=3F9F4F4E0206
    virtual void close();    
    
    //##ModelId=3FD7D0640203
    virtual bool needUpdata(const string &srcFile);

  private:
    //##ModelId=3F9F55FD0347
    string getSqlCount(const string &sql);   
    
    //##ModelId=3F9F57E10248
    int getFieldCount(const string &sql);

  private:
    //##ModelId=3F72713A001A
    //##Documentation
    //## 数据库服务名
    string m_serviceName;

    //##ModelId=3F72714900BC
    //##Documentation
    //## 数据库用户名
    string m_userName;

    //##ModelId=3F7271510013
    //##Documentation
    //## 数据库用户密码
    string m_userPwd; 
    
    //##ModelId=3F9F507F01C8
    DbInterface m_dbInterface;  
    
    //##ModelId=3F9F51D2000D
    string m_sql;

    //##ModelId=3F9F51E00298
    string m_countSql; 
    
    //##ModelId=3F9F54BE0366
    int m_recordCount;

    //##ModelId=3F9F54C401A2
    int m_fieldCount;
    
    //##ModelId=3F9F554C0298
    StringVector * m_recordSet;

    //##ModelId=3F9F555B036C
    StringVector * m_errorSet;  
    
    //##ModelId=3F9F6726000E
    int m_recordTotal;       
    
    //##ModelId=3F9F6851000E
    int m_subCount;  
    
    //##ModelId=3F9F6AF502CD
    int m_subTotal;

};



#endif /* DBDATASOURCE_H_INCLUDED_C08DA516 */
