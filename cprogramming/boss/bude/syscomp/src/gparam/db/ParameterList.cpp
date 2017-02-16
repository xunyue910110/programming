// ############################################
// Source file : ParameterList.cpp
// Version     : 0.2
// Language    : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20031201
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include <stdio.h>
#include <unistd.h>

#include "util/Util.h"
#include "ParameterList.h"   

//##ModelId=3F7282C10329
ParameterList::ParameterList()
{       
    m_parameterLog            = ParameterLog::getInstance();    
}

//##ModelId=3F7282C70287
ParameterList::~ParameterList()
{     
    for(int i=0;i<m_parameter.size();i++) 
    {
        delete m_parameter[i];
    } 
}

//##ModelId=3F7282B50372
bool ParameterList::initialize()
{   
    if(!init_dataSourceList())
        return false;

    if(!init_parameter())
        return false;

    return true;        
}

//##ModelId=3F73E875006F
bool ParameterList::init_dataSourceList()
{   
    int i; 
    char tmp[8]; 
    string path,value;     
    vector <string> param; 
        
    path = string("\\") + m_systemName + "\\system";
    //path = m_systemName + "\\system";    
    if(! m_sysParam->getValue(path,"log_path",value) ) {
        if( m_parameterLog->m_debug>=2) {  
            m_parameterLog->setMessage("log_path没有找到,path="+path,0); 
        }
        return false;  
    }
            
    checkPath(value);
    value = value + m_systemName + "_";     
    m_parameterLog->setLogPath(value);        
    
    if( m_parameterLog->m_debug>=2) {  
        m_parameterLog->setMessage("\n",0); 
        m_parameterLog->setMessage("==================== 通用参数代理开始初始化 ====================\n",1); 
    }  
    
    // 根据配置文件的<data_source>段，创建m_dataSourceList对象
    //1. 初始化 DbDataSource。 
    if( m_parameterLog->m_debug>=2) {  
        m_parameterLog->setMessage("\n",0); 
        m_parameterLog->setMessage("--------------------- data_source 初始化 --------------------\n",1); 
    }   
        
    i=0;    
    do { 
        //m_systemName(test) data_source database db_0(db_1,...)
        param.clear(); 
        sprintf(tmp,"db_%d",i); 
        path = string("\\") + m_systemName + "\\data_source\\database\\" + tmp; 
        
        param.push_back(tmp);//param的内容 :路径 值 值 值    
        if(! m_sysParam->getValue(path,"server_name",value) ) 
            break; 
        param.push_back(value);    
        
        if(! m_sysParam->getValue(path,"user_name",value) ) 
            return false; 
        param.push_back(value);    
        
        if(! m_sysParam->getValue(path,"passwd",value) ) 
            return false;  
        param.push_back(value);
            
        if(!m_dataSourceList.initialize(0,param)) {
            return false;                             
        }
        i++;            
    } while(1);
        
    //2. 初始化 FileDataSource。
    i=0;    
    do { 
        param.clear(); 
        sprintf(tmp,"file_%d",i); 
        //m_systemName(test) data_source file file_0(file_1,...)
        path = string("\\") + m_systemName + "\\data_source\\file\\" + tmp; 
        
        param.push_back(tmp);    
        if(! m_sysParam->getValue(path,"file_path",value) ) 
            break; 
        param.push_back(value);  //param的内容 :路径 值 
        if(!m_dataSourceList.initialize(1,param)) {
            return false;                             
        }
        i++;            
    } while(1);    
    
    return true;    
}//init_dataSourceList

//##ModelId=3F73E88F0211 读配置文件 创建m_parameter对象
bool ParameterList::init_parameter()
{           
    int i,j; 
    char tmp[20];     
    Parameter *parameter;         
    DataSource *dataSource;
    string path_1,path_2,name,value,list;    
    vector <string> param_1,param_2,tableList; 
          
    // 根据配置文件的<parameter_table>段，创建 m_parameter 对象
    if( m_parameterLog->m_debug>=2) {  
        m_parameterLog->setMessage("\n",0); 
        m_parameterLog->setMessage("--------------------- parameter_table 初始化 --------------------\n",1); 
    }
    
    //得到各个表名
    path_1 = string("\\") + m_systemName + "\\parameter_table"; 
   
    if(! m_sysParam->getValue(path_1,"table_list",value) ) {
        if( m_parameterLog->m_debug>=1) {
           m_parameterLog->setMessage("参数错误，没有发现" + path_1 + "\n",1); 
        }
        return false;
    } 
                 
    split(",",value.c_str(),false,tableList);
    
    //扫描各表名在配置文件中的参数 创建 m_parameter       
    for(i=0;i<tableList.size();i++)    
    {                    
      //m_systemName parameter_table ServInfoPnIncre(TD_CHANNEL,...)
      if( m_parameterLog->m_debug>=2) {  
          m_parameterLog->setMessage("\n",0); 
          m_parameterLog->setMessage("["+tableList[i]+"]\n",0); 
      }
        path_1 = string("\\") + m_systemName + "\\parameter_table\\" + tableList[i];         
        if(! m_sysParam->getValue(path_1,"data_source",value) ) {
            return false;  
        }
        m_parameterList.push_back(tableList[i]);
        
        //创建、初始化一个参数对象 parameter 放入m_parameter
        //(1)得到 param_1  
        //changed by gaojf 2006-6-7 14:27
        int sourType = 0;
        parameter          = new Parameter; 
        //if ((dataSource    = m_dataSourceList.getInstance(value))==0)
        if ((dataSource    = m_dataSourceList.getInstance(value,sourType))==0)
            return false;    
        
        //parameter->setDataSource(dataSource);
        parameter->setDataSource(dataSource,sourType);
        parameter->setTableName(tableList[i]);

        param_1.clear(); 
        if(! m_sysParam->getValue(path_1,"name",value) ) 
            return false;
        param_1.push_back(value);//value:select 语句  or 文件名 
         
        if(! m_sysParam->getValue(path_1,"fields_name",value) ) 
            return false;
        param_1.push_back(value);                         
    
        if(! m_sysParam->getValue(path_1,"table_name",value) )  { 
            value = "";                                  
        }  
        param_1.push_back(value);  
        
        if(! m_sysParam->getValue(path_1,"buffer_unit",value) )  { 
            return false;                                         
        }
        param_1.push_back(value);      
        
        if(! m_sysParam->getValue(path_1,"updata_time",value) )  { 
            return false;                                         
        }
        param_1.push_back(value); 

        //与刷新相关的参数            
        path_2 = string("\\") + m_systemName + "\\updata" ;       
        if(! m_sysParam->getValue(path_2,"update_time_stamp",value) )  { 
            //return false;      // 允许为空，表示不进行时间戳判断 
            value="";                                  
        }
        param_1.push_back(value);    
                
        //(2)得到 param_2 (与该表相关的"索引" 配置)
        
        j=0;
        param_2.clear();  
        path_2 = path_1 + "\\index";
        do {
            sprintf(tmp,"index_%d",j);             
            
            if(! m_sysParam->getValue(path_2,tmp,value) ) 
                break;
            param_2.push_back(value);
            j++;
        } while(1);    
         
        //依据 param_1, param_2 初始化 parameter
        if(!parameter->initialize(param_1,param_2))
            return false;                                                                          
                                
        m_parameter.push_back(parameter);
        
    }  
    
    return true;                 
}//init_parameter

//##ModelId=3F73E18901E1
void ParameterList::setSysParam(PConfigureFile *p)
{  
    m_sysParam = p;
}  

//##ModelId=3F75210A0165
void ParameterList::setSystemName(const string &name)
{   
    string desStr;
    string::size_type bpos, epos;

    bpos = name.find_first_of("/"); //  去掉 '/filter'的/  
    if (bpos != string::npos)
        m_systemName     = name.substr(bpos+1);
    else 
        m_systemName     = name;
}

//##ModelId=3F753006030F
void ParameterList::setDebug(int flag)
{  
    m_parameterLog->setDebug(flag);
}

//##ModelId=3F75430903B8
bool ParameterList::getData()
{   
    int i,j;
    int size;   
        
    if( m_parameterLog->m_debug>=2) { 
        m_parameterLog->setMessage("\n",0);    
        m_parameterLog->setMessage("--------------------- 读取参数记录 --------------------\n",1); 
    }
    
    size = m_parameter.size();    
    for(i=0;i<size;i++) 
    {
      //每个表自已读参数    
      if (m_parameterList[i].substr(0,2) != "TD")
      {
          m_subparam[i].m_bRet= 0;
          m_subparam[i].m_position = i;
          m_subparam[i].plist = m_parameter[i];
          //pthread_t     pid;
          //pthread_create(&pid,NULL,(void *(*)(void *))getData_t, &m_subparam[i]);
          if(pthread_create(&(m_subparam[i].m_pid),NULL,(void *(*)(void *))getData_t, &(m_subparam[i]))!=0)
          {
            cout<<"Create Thread Error"<<endl;
            for (j = 0;j < i;j++)
            {
              if(m_subparam[j].m_position >= 0)
                pthread_join(m_subparam[j].m_pid,NULL);
            }
            return false;

          }
      }
      else
      {
        m_subparam[i].m_pid = 0;
        m_subparam[i].m_position = -1;
        if( !m_parameter[i]->getData() )
        {
            for (j = 0;j < i;j++)
            {
              if(m_subparam[j].m_position >= 0)
                pthread_join(m_subparam[j].m_pid,NULL);
            }
            return false;
          }
        else
          m_subparam[i].m_bRet= 1;
      }

    } 
    for (j = 0;j <size;j++)
    {
      if(m_subparam[j].m_position >= 0)
        pthread_join(m_subparam[j].m_pid,NULL);
    }
    for(i=0;i<size;i++) 
    {
     if (m_subparam[i].m_bRet != 1)
     {
         cout<<" 参数：第"<<i<<"个 导入失败"<<__FILE__<<__LINE__<<endl;
         return false;
     }
    }

    m_dataSourceList.close();
            
    return true;
}


//##ModelId=3FD3E674036A
bool ParameterList::upData(PConfigureFile *sysParam)
{ 
    int i,size,j;  
    bool flag=false;
    string path,name,value;
    string updateTimeStamp,updataTableList;
    
    
    //m_systemName ="\\filter"
    path =  string("\\") + m_systemName + "\\updata" ; 
    //得到时间戳， 如果时间戳为空，则不进行时间戳判断
    if(! sysParam->getValue(path,"update_time_stamp",updateTimeStamp) )   
    {
        //return false;  
        updateTimeStamp ="";  
    }
    //得到 需要更新的数据库参数表列表
    path += "\\updata_table_list"; 
    
    if( !sysParam->setSectionPath(path) )  { 
       if( m_parameterLog->m_debug>=1) {
           m_parameterLog->setMessage("参数错误，没有发现" + path + "\n",1); 
        }
        return false;
    }   
                  
    while( m_sysParam->getSectionValue(name,value) ) 
    {
        if(name.compare("updata_table_list")==0) 
        {  
          if( value.size() <= 0) {
            break;
          }
            value = lrtrim(value.c_str()); 
            if( value.compare(value.size()-1,1,",") != 0 )  {
                value += "," ;  
            }
            updataTableList  += value + " ";         
        }                              
    } 
    
    //置 每个Parameter 的 m_needUpdata标志       
    flag = needUpdata(updateTimeStamp,updataTableList);
    
    if( flag ) {
      if( m_parameterLog->m_debug>=2) { 
         m_parameterLog->setMessage("\n",0);    
         m_parameterLog->setMessage("--------------------- 开始参数更新 --------------------\n",1); 
      }
    }  
    size = m_parameter.size();    
    for(i=0;i<size;i++) 
    {
      //每张表刷新自已
      if (m_parameterList[i].substr(0,2) != "TD")
      {
          m_subparam[i].m_bRet= 0;
          m_subparam[i].m_position = i;
          m_subparam[i].plist = m_parameter[i];
          if(pthread_create(&(m_subparam[i].m_pid),NULL,(void *(*)(void *))upData_t, &(m_subparam[i]))!=0)
          {
            cout<<"Create Thread Error"<<endl;
            for (j = 0;j < i;j++)
            {
              if(m_subparam[j].m_position >= 0)
                pthread_join(m_subparam[j].m_pid,NULL);
            }
            return false;
          }
        }
      else 
      {
        m_subparam[i].m_position = -1;
        if( !m_parameter[i]->upData() )
        {
            for (j = 0;j < i;j++)
            {
              if(m_subparam[j].m_position >= 0)
                pthread_join(m_subparam[j].m_pid,NULL);
            }
            return false;
        }
        else
          m_subparam[i].m_bRet= 1;
      }
    } 
    for (j = 0;j <size;j++)
    {
      if(m_subparam[j].m_position >= 0)
        pthread_join(m_subparam[j].m_pid,NULL);
    }
    for(i=0;i<size;i++) 
    {
       if (m_subparam[i].m_bRet != 1)
         return false;
    }
    m_dataSourceList.close();  
    
    if( flag ) {
      if( m_parameterLog->m_debug>=2) { 
         m_parameterLog->setMessage("\n",0);    
         m_parameterLog->setMessage("--------------------- 参数更新成功 --------------------\n",1); 
      }
    }
               
    return true;    
    
}//upData


//##ModelId=3FB45D2E01BA 
// query 要用到   tables 输入参数;parameterList 输出参数(被设置了,有了内容)
bool ParameterList::getParameterList(string &tables, vector<Parameter *> &parameterList)
{  
    int n;
    vector<string> table;
    
    split(",",tables.c_str(),false,table); //输出参数 table 
    for(int i=0;i<table.size();i++)
    {   
        for(int j=0;j<m_parameterList.size();j++)
        {   
            if( table[i].compare(m_parameterList[j])==0 ) 
            {   //如果是上面配置的表中的一个
                parameterList.push_back(m_parameter[j]);                 
            } 
        }
    }  
    
    if(table.size() != parameterList.size())  {     
        if( m_parameterLog->m_debug>=1) {        
            m_parameterLog->setMessage("parameter_table 参数错误"+tables+"!\n",1); 
        }   
        
        return false;   
    }
    
    return true;
}//getParameterList

//##ModelId=3FD6E6B002BB 置 每个Parameter 的 m_needUpdata标志
bool ParameterList::needUpdata(const string &timeStamp, const string &tableList)
{   
    int size; 
    bool flag=false;
    vector<string> tables;  
        
    split(",",tableList.c_str(),true,tables);  
    size = m_parameter.size();    
    
    for(int i=0;i<size;i++)
    {     
        //置 每个Parameter 的 m_needUpdata标志
        if(m_parameter[i]->needUpdata(timeStamp,tables))
          flag = true;
    }     
    return flag;             
}

//##ModelId=402878F800B5 得到param所指表s的 一些信息
int ParameterList::getTables(const char *param, string &result)
{   
    int mem=0;
    int n,size; 
    string tmp;
    char m[10],t[10];
 
    for(int j=0;j<m_parameterList.size();j++)
    {   
      sprintf(t,"表%d: ",j+1);
      
       mem    += m_parameter[j]->getTable(tmp); //tmp:(1)文件名或select语句 \n (2) 记录数 =记录数 \n (3) message \n 
        result += string(t) + m_parameterList[j] + ":\n" + tmp + "----------------------------------------------------------------------------\n";                
    } 
    if( mem > 1024 ) {
      mem=(mem+1)/1024;
      sprintf(m,"%dM",mem);
    } else {
      sprintf(m,"%dK",mem);
    }
    result += string("      总共使用内存数：") + m + "\n"; 
     
    return 1;
}//getTables

int ParameterList::getTablesName(const char *param, string &result)
{
  int n = m_parameterList.size();
  
  result="";
  for(int j=0;j<n;j++)
  {   
    result +=   m_parameterList[j] + "," ;
  } 
  
  return n;
}

int ParameterList::getTableCount(const char *param, string &result)
{
  char tmp[10];
  int count = -1;
  Parameter *parameter;
  
  for(int j=0;j<m_parameterList.size();j++)
  {   
     if( m_parameterList[j].compare(param)==0) {
       parameter = m_parameter[j];
       m_parameterDump = parameter;        
       count = parameter->getCount();
       break;
     } 
  } 

  if(count>=0) {
    sprintf(tmp,"%d,%d", count, parameter); 
    result=string(tmp);
  } else {
    result="";
  }
        
  return count;
}

int ParameterList::dump(const string &p,const string &no,string &result)
{
  Parameter *parameter = (Parameter *) atoi(p.c_str());
       
  if(parameter <= 0 && m_parameterDump > 0) {
    parameter = m_parameterDump;
  }
  
  return parameter->dump(result,atoi(no.c_str()));  
}



int ParameterList::dump(string &result)
{
  if(m_parameterDump <= 0)
    return -1;
  
  return m_parameterDump->dump(result);  
}

void * ParameterList::getData_t(ThreadParam &tparam)
{
  if(!(tparam.plist->getData()))
    tparam.m_bRet=2;
  else
    tparam.m_bRet=1;

  return (NULL);
}


void * ParameterList::upData_t(ThreadParam &tparam)
{
  if(!(tparam.plist->upData()))
    tparam.m_bRet=2;
  else
    tparam.m_bRet=1;

  return (NULL);
}




