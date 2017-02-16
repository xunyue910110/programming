// ############################################
// Source file : ParameterList.h
// Version     : 0.2
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20031201
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef PARAMETERPOOL_H_INCLUDED_C08D4C79
#define PARAMETERPOOL_H_INCLUDED_C08D4C79

#include "util/PConfigureFile.h"

#include "Parameter.h" 
#include "DataSourceList.h"  
#include "ParameterLog.h" 
#include <pthread.h>


//##ModelId=3F727D6D0133
class ParameterList
{
  public:
    //##ModelId=3F7282C10329
    ParameterList();

    //##ModelId=3F7282C70287
    ~ParameterList();

    //##ModelId=3F7282B50372
    bool initialize();  
    
    //##ModelId=3F73E18901E1
    void setSysParam(PConfigureFile *p);  
    
    //##ModelId=3F73E875006F
    bool init_dataSourceList();

    //##ModelId=3F73E88F0211
    bool init_parameter();     
    
    //##ModelId=3F75210A0165
    void setSystemName(const string &name); 
    
    //##ModelId=3F753006030F
    void setDebug(int flag);  
    
    //##ModelId=3F75430903B8
    bool getData();

    //##ModelId=3FD3E674036A
    bool upData(PConfigureFile *sysParam);   

    //##ModelId=3FB45D2E01BA
    bool getParameterList(string &tables, vector<Parameter *> &parameterList);  
    
    //##ModelId=402878F800B5
    int getTables(const char *param, string &result);
    
    int getTablesName(const char *param, string &result);
    
    int getTableCount(const char *param, string &result);
    
    int dump(string &result);
    
    int dump(const string &p,const string &no,string &result);

	int m_idx;
  private:
    //##ModelId=3FD6E6B002BB
    bool needUpdata(const string &timeStamp, const string &tableList);

    //##ModelId=3F7280C90071
    //##Documentation
    //## 数据源列表对象(数据库,文件)
    DataSourceList m_dataSourceList;  
    
    //##ModelId=3F728F9401B7
    //##Documentation
    //## 保存全部参数对象
    vector<Parameter *> m_parameter; 
    
    Parameter *m_parameterDump;
    
    //##ModelId=3F73E1710331
    PConfigureFile *m_sysParam; 

    //##ModelId=3F752133004B
    string m_systemName; 
    
    //##ModelId=3F752FD4015F
    ParameterLog *m_parameterLog; 
    
    //##ModelId=3FCA9D9603BF
    vector<string> m_parameterList;//配置的参数表的集合

  public:
    
      typedef struct 
      {
          int       m_position;     //对应的数组下标
          int       m_bRet;         //返回值
          pthread_t m_pid;
          Parameter * plist;//指针变量；用来指向自身
      }ThreadParam;
      ThreadParam  m_subparam[100];       //多线程参数

    static void *  getData_t(ThreadParam &tparam);

    static void *  upData_t(ThreadParam &tparam);

};



#endif /* PARAMETERPOOL_H_INCLUDED_C08D4C79 */
