// ############################################
// Source file : ParameterInterface.h
// Version     : 0.2.0
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030930
// Update      : 20031114
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef PARAMETERINTERFACE_H_INCLUDED_C0875BD1
#define PARAMETERINTERFACE_H_INCLUDED_C0875BD1  

class PConfigureFile; 
class ParameterList; 
class QueryManager; 
class ParameterLog;

USING_NAMESPACE(std)

//add by gaojf 访问线程正在查询数互斥信号量 2006-4-7 21:17
static  pthread_mutex_t g_Cli_FindNum_mutex; 

//##ModelId=3F78EA65009B
class ParameterInterface
{
	
  public:
    //##ModelId=3F78EA820089
    ParameterInterface(); 
    
    //##ModelId=3F78EA83024D
    ~ParameterInterface();
            
    //##ModelId=3F796B8000E4
    //##Documentation
    //## 调试信息输出级别：0:无输出；1:告警信息；2:少量输出；3:大量输出
    void setDebug(int debug); 
    
    //##ModelId=3FB870F5033E
    int find(const char *param, char *result);   
    
    //##ModelId=3FD572490190
    bool updata();   
    
    //##ModelId=3FD80FCC03DA
    int serverCommand(const char *param, string &result); 
    
    //##ModelId=3F78EB210055
    bool initialize(const char *sysname,const char *configName);  
    
    //##ModelId=4028779A026A
    int getTables(const char *param, string &result);
    
    int getQuery(const char *param, string &result);
    
    int dump(const char *param,string &result);
    
    void setBufferSize(const int &size);


  private:
  	friend class Parameter;//added by zhanglong 2006-4-2 22:34
    //##ModelId=3FD807930102
    //##Documentation
    //##  更新状态 初始化 未知状态 可用状态    
    enum state { UPDATA_STATE, INITIALIZE_STATE, NOUSE_STATE, READY_STATE };

    //##ModelId=3F78EA7E001F
    PConfigureFile *m_sysParam; 
    
    //##ModelId=3F78F27F03AB
    ParameterLog *m_parameterLog; 
     
    //##ModelId=3FB447BF0124
    QueryManager * m_queryManager;  
    
    //##ModelId=3FB447DD02D6
    ParameterList *m_parameterList;   
      
    //##ModelId=3FD7FC6A02C7
    //##Documentation 更新状态 初始化 未知状态 可用状态      
    static state m_systemState;   //modified by zhanglong 2006-4-2 22:34
    
    //add by gaojf 2006-4-7 为了建立访问线程和服务线程之间的机制
		static int	 m_cli_FindNum;   //在线正在执行查询的线程数
};

#endif /* PARAMETERINTERFACE_H_INCLUDED_C0875BD1 */
