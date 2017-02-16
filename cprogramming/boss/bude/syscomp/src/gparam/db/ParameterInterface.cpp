// ############################################
// Source file : ParameterInterface.h
// Version     : 0.1.1
// Language       : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030930
// Update      : 20040109
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################



#include "ParameterLog.h"
#include "QueryManager.h"  
#include "QueryUseExpression.h"//added by cy 2004.12.09

#include "ParameterList.h"   
#include "util/PConfigureFile.h"   
#include "ParameterInterface.h" 

ParameterInterface::state ParameterInterface::m_systemState = NOUSE_STATE;//added by zhanglong 2006-4-2 22:33
int ParameterInterface::m_cli_FindNum = 0; //初始化为0 added by gaojf 2006-4-7 20:58
	
//##ModelId=3F78EA820089
ParameterInterface::ParameterInterface()
{   
		pthread_mutex_init(&g_Cli_FindNum_mutex,NULL);//初始化互斥信号量 by gaojf 2006-4-7 21:18
    m_systemState      = NOUSE_STATE;
    m_parameterLog     = ParameterLog::getInstance();     
    
    m_sysParam         = new PConfigureFile; 
    m_queryManager     = new QueryManager ; 
    m_parameterList    = new ParameterList;
    
}

//##ModelId=3F78EA83024D
ParameterInterface::~ParameterInterface()
{   
    delete m_sysParam;
    delete m_queryManager;
    delete m_parameterList;
    pthread_mutex_destroy(&g_Cli_FindNum_mutex);
} 

//##ModelId=3FD80FCC03DA 在server端执行的 客户请求的命令
int ParameterInterface::serverCommand(const char *param, string &result)
{   
    char *p;
    int flag;
    
    p = (char *)param;    
    result="";
   
    if( strncmp(param,"updata",6) == 0 ) { 	//(2)调用 updata 方法
        // param举例: updata 
        flag 	= updata();
        result 	= "update ok";
    } else if( strncmp(param,"getquery",8) == 0 ) {	//(3)调用 getQuery 方法
        // param举例: updata 
        flag 	= getQuery( ( p + 9 ) ,result);
    } else if( strncmp(param,"gettable",8) == 0 ) {	//(4) 调用 getTables 方法 
        // param举例: gettable   
        flag 	= getTables( ( p + 8 ) ,result); //得到param所指表s的 一些信息
    } else if( strncmp(param,"dump",4) == 0 ) {	//(4) 调用 getTables 方法 
        // param举例: dump,ServInfoPnIncre	
        // param举例: dump,TD_THUNK_ID	
        if( strlen(param) > 4 ) { 
        	flag = dump( ( p + 5 ) ,result); 
        } else {
        	flag = dump( p ,result); 
        }
    } else  {
    	result = "unknown data command.";
        flag = -1;
    }  
    
    return flag;                  
}//serverCommand   

//##ModelId=3FB870F5033E
//param如:getServInfoByImsi,460003013700536,20031001000000
int ParameterInterface::find(const char *param,char *result)
{          
    int n;
    char *p=(char *)param; 
    
    if( m_systemState == READY_STATE )  {  
    	//n =  m_queryManager->find(( p + 5 ),result); 
    	//增加信号量控制 add by gaojf 2006-4-7 21:07
    	pthread_mutex_lock (&g_Cli_FindNum_mutex);//锁定互斥信号量
    	m_cli_FindNum++;
    	pthread_mutex_unlock(&g_Cli_FindNum_mutex);//解锁互斥信号量
    	if ( m_systemState == READY_STATE )
    		n =  m_queryManager->find(( p + 5 ),result);  
    	else
    	{
    		pthread_mutex_lock (&g_Cli_FindNum_mutex);//锁定互斥信号量
	    	m_cli_FindNum--;
	    	pthread_mutex_unlock(&g_Cli_FindNum_mutex);//解锁互斥信号量
	    	strcpy(result,"更新状态...");
	    	return -10;
    	}	
    	pthread_mutex_lock (&g_Cli_FindNum_mutex);//锁定互斥信号量
    	m_cli_FindNum--;
    	pthread_mutex_unlock(&g_Cli_FindNum_mutex);//解锁互斥信号量
    	return n;
    }
    
    if( m_systemState == UPDATA_STATE )  {  
    		strcpy(result,"更新状态...");

    		return -10;
    }
    
    if( m_systemState == NOUSE_STATE )  {
        strcpy(result,"未知状态...");  
    } else if( m_systemState == INITIALIZE_STATE )  {
        strcpy(result,"正在初始化...");  
    }  
    
    return -1;
} 

void ParameterInterface::setBufferSize(const int &size)
{
	m_queryManager->setBufferSize(size);
}
	
int ParameterInterface::getQuery(const char *param, string &result)
{
    if( m_systemState == NOUSE_STATE )  {
        result = "未知状态...";    
        return -1;
    } else if( m_systemState == INITIALIZE_STATE )  {
        result = "正在初始化..."; 
       	return -1; 
    } 
    
	m_queryManager->getQuery(param,result);    
	return 0;   
}

//##ModelId=4028779A026A
int ParameterInterface::getTables(const char *param, string &result)
{   
    if( m_systemState == READY_STATE )  {  
        return m_parameterList->getTables(param,result);       
    }
        
    if( m_systemState == UPDATA_STATE )  {
        result = "正在更新..."; 
    } else if( m_systemState == NOUSE_STATE )  {
        result = "未知状态...";  
    } else if( m_systemState == INITIALIZE_STATE )  {
        result = "正在初始化...";  
    }  
    
    return -1;    
}

int ParameterInterface::dump(const char *param,string &result)
{
    int count;
    vector<string> fields;
    
	if( m_systemState == UPDATA_STATE )  {
        result = "正在更新..."; 
       	return -1;  
    } else if( m_systemState == NOUSE_STATE )  {
        result = "未知状态..."; 
        return -1;   
    } else if( m_systemState == INITIALIZE_STATE )  {
        result = "正在初始化...";  
        return -1;   
    } 
    
    if( m_systemState != READY_STATE )  {  
    	result = "未知状态..."; 
        return -1;   
    }
    
    split(",",param,false,fields);  
 
 	if( fields[0].compare("dump") == 0 ) {
		count =  m_parameterList->getTablesName(param,result);     
		return count;
	}  
	
	if( fields[0].compare("getCdr") == 0 ) {
		count =  m_parameterList->dump(fields[1],fields[2],result); 
		return count;
	}

	// 得到表记录数
	count = m_parameterList->getTableCount(param,result); 
	return count;
}

//##ModelId=3FD572490190            
bool ParameterInterface::updata()
{  
    bool flag ;  
    
//    m_systemState    = UPDATA_STATE ;//modified by zhanglong 2006-4-2 22:34
    
    flag = m_parameterList->upData(m_sysParam);        
    m_sysParam->closeAllSection();    // 释放配置文件占用内存 
    
    m_systemState    = READY_STATE;  

    return flag;
}  

//##ModelId=3F78EB210055 sysname:\\filter; configName: 配置文件名
bool ParameterInterface::initialize(const char *sysname,const char *configName)
{   
    m_systemState    = INITIALIZE_STATE;  

    if(!m_sysParam->initialization(configName)) {
        if( m_parameterLog->m_debug>=1) { 
            m_parameterLog->setMessage("sysParam 初始化失败\n",1); 
        }
        return false;
    }     
    //(1) 初始化 m_parameterList    
    m_parameterList->setSysParam(m_sysParam);
    m_parameterList->setSystemName(sysname);
    if(!m_parameterList->initialize()) {
        if( m_parameterLog->m_debug>=1) { 
            m_parameterLog->setMessage("m_parameterList 初始化失败\n",1); 
        }
        return false;
    } 
                
    //(2) 初始化 m_queryManager
    m_queryManager->setParam(m_sysParam,sysname,m_parameterList);  
    if(!m_queryManager->initialize()) {
        if( m_parameterLog->m_debug>=1) { 
            m_parameterLog->setMessage("m_queryManager 初始化失败\n",1); 
        }
        return false;
    }  
    // 取全部参数记录到内存
    if(!m_parameterList->getData()) { 
        if( m_parameterLog->m_debug>=1) { 
            m_parameterLog->setMessage("getData 失败\n",1); 
        }
        return false;
    } 
             
    m_sysParam->closeAllSection();    // 释放配置文件占用内存   
    
    if( m_parameterLog->m_debug>=2) {  
        m_parameterLog->setMessage("\n",0); 
        m_parameterLog->setMessage("==================== 通用参数代理初始化成功 ====================\n",1); 
    }  
    
    m_systemState    = READY_STATE;         
        
    return true; 
}



//##ModelId=3F796B8000E4
void ParameterInterface::setDebug(int debug)
{ 
    m_parameterList->setDebug(debug);  
    //m_parameterList->setDebug(2);     // 调试信息输出级别：0:无输出；1:告警信息；2:少量输出；3:大量输出
}

