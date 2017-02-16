// ############################################
// Source file : FileDataSource.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20030925
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################


#ifndef FILEDATASOURCE_H_INCLUDED_C08D9ACA
#define FILEDATASOURCE_H_INCLUDED_C08D9ACA 

#include "DataSource.h"    
#include "util/PFileProcess.h" 
#include "Config.h"


//##ModelId=3F727051021D
//##Documentation
//## 文件数据源类
class FileDataSource : public DataSource
{
  public:
    //##ModelId=3F72753A0329
    FileDataSource();
    //add by gaojf 2006-6-7 14:36
    FileDataSource(const FileDataSource &right);
    void operator=(const FileDataSource& right);

    //##ModelId=3F727543039A
    virtual ~FileDataSource();  
    
    //##ModelId=3F72721700CC
    virtual bool initialize(const vector<string> &name);     
    
    //##ModelId=3F7278EB0093
    //##Documentation
    //## 从数据源读取一条记录
    virtual int getRecord(vector <string> &cdr); 
    
    //##ModelId=3F754EA20078
    virtual long reset(const string &attribute);      
    
    //##ModelId=3F9F4F3B0140
    virtual void close();       
    
    //##ModelId=3FD7CA720345
    virtual bool needUpdata(const string &srcFile);

  private:
    //##ModelId=3F72710B0383
    //##Documentation
    //## 参数文件存放路径ffsdfd
    string m_filePath;
    
    //##ModelId=3F74DC2A002B
    PFileProcess m_PFileProcess;
};



#endif /* FILEDATASOURCE_H_INCLUDED_C08D9ACA */
