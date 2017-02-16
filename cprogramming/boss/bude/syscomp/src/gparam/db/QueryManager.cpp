// ############################################
// Source file : QueryManager.h
// Version     : 0.2
// Language       : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031113
// Update      : 20031201
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include <stdio.h>

#include "ParameterCdr.h" 
#include "ParameterLog.h" 
#include "ParameterList.h" 
#include "QueryUseExpression.h" 

#include "util/Util.h"
#include "util/PConfigureFile.h"  

#include "QueryManager.h"

//##ModelId=3FB095FA01A5
QueryManager::QueryManager()
{     
    m_sysParam         = 0; 
    m_parameterLog    = ParameterLog::getInstance();
}

//##ModelId=3FB095FC0111
QueryManager::~QueryManager()
{    
    for(int i=0;i<m_queryUseExpression.size();i++)
    {
        delete m_queryUseExpression[i];
    }    
}

//##ModelId=3FB09C950106
//param 如:getServInfoByImsi,460003013700536,20031001000000
// 第一个参数:查询对象，在<query>段中定义
// 后面的参数：输入查询字段值(在<query>段中的 in_fields 中定义)
int QueryManager::find(const char *param, char *result)
{   
    int n;
    vector<string> fields; 
        
    split(",",param,false,fields);
    //得到 query 名 getServInfoByImsi 在 m_queryList 中的坐标  n  
    if( ( n = getQueryId(fields[0]) ) < 0 ) 
    { 
        if( m_parameterLog->m_debug>=1) {  
            m_parameterLog->setMessage("参数对象ID"+fields[0]+"错误！",1);
        }              
        return -1;
    }    
    fields.erase(fields.begin());   // 删除第一个参数(查询对象)   
    
    //fields:460003013700536,20031001000000
    n =   m_queryUseExpression[n]->find(fields,result);        
        
    return n;
}

void QueryManager::setBufferSize(const int &size)
{
	for(int i=0;i<m_queryUseExpression.size();i++)
	{
		m_queryUseExpression[i]->setBufferSize(size);
	}
}

//##ModelId=3FB3380F01AE
bool QueryManager::initialize()
{   
    int order;
    char tmp[20];
    string list;    
    string path,name,value;   
    vector <string> queryList; 
    QueryUseExpression *queryUseExpression;   
    
    // 根据配置文件的<query>段，创建 m_queryUseExpression 对象 
    if( m_parameterLog->m_debug>=2) {
        m_parameterLog->setMessage("\n",0); 
        m_parameterLog->setMessage("--------------------- query 初始化 --------------------\n",1); 
    }
    //m_systemName=\\filter 
    path = string("\\") + m_systemName + "\\query";    
    //path = m_systemName + "\\query";    
    
    if(! m_sysParam->getValue(path,"query_list",value) )  {
        if( m_parameterLog->m_debug>=1) {
         	m_parameterLog->setMessage("参数错误，没有发现" + path + "\n",1); 
        }
        return false;
    } 
                  
    split(",",value.c_str(),false,queryList);           
    for(int i=0;i<queryList.size();i++)
    {    
    	if( m_parameterLog->m_debug>=2) {  
        	m_parameterLog->setMessage("\n",0); 
        	m_parameterLog->setMessage("["+queryList[i]+"]\n",0); 
    	}
        path = string("\\") + m_systemName + "\\query\\" + queryList[i]; 
        
        if(! m_sysParam->getValue(path,"parameter_table",value) )  
        {
            continue;  
        } 
        m_queryList.push_back(queryList[i]);
                  
        if( m_parameterLog->m_debug>=2) {
            m_parameterLog->setMessage("parameter_table = "+value+"\n",0); 
     	} 
            
        // 创建、初始化参数对象，属性==================================     
        queryUseExpression     = new QueryUseExpression;
        
        // 初始化 queryUseExpression的属性：m_parameterList
        if(!m_parameterList->getParameterList( value,*( queryUseExpression->getList() ) ))
            return false;
             
        // 取、初始化    in_fields 参数    
        if( !m_sysParam->getValue(path,"in_fields",value) )  {
            if( m_parameterLog->m_debug>=1) {
                m_parameterLog->setMessage("参数 in_fields 取不到！\n",1); 
         	}
          	return false; 
        }  
        
   		if( !queryUseExpression->setInFields(value)) { 
     		if( m_parameterLog->m_debug>=1) {
                m_parameterLog->setMessage("参数错误("+value+")\n",1); 
         	}
           	return false;
    	}  
           
           // 取、初始化    out_fields 参数    
        if( !m_sysParam->getValue(path,"out_fields",value) )  {
            if( m_parameterLog->m_debug>=1) {
                m_parameterLog->setMessage("参数 out_fields 取不到！\n",1); 
         	}
        	return false; 
        }  
        
  		if( !queryUseExpression->setOutFields(value)) { 
         	if( m_parameterLog->m_debug>=1) {
                m_parameterLog->setMessage("参数错误("+value+")\n",1); 
      		}
       		return false;
     	}
            
    	if( !m_sysParam->getValue(path,"query_mode",value) )  {
       	 	if( m_parameterLog->m_debug>=1) {
                m_parameterLog->setMessage("参数 query_mode取不到！\n",1); 
         	}
          	return false; 
        }  
                
        order = atoi(value.c_str());                      
       	if( !initializeExpression(path,order,queryUseExpression) ) { 
        	return false;
     	}
           
        m_queryUseExpression.push_back(queryUseExpression);
    } 
        
    return true;
}//initialize

//##ModelId=3FB9C4EC006B
bool QueryManager::initializeExpression(string &path, 
                                        int     order, 
                                        QueryUseExpression *queryUseExpression)
{           
    string name,value;       
    string eq;
        
    //path += "\\expression";  
       if( !m_sysParam->setSectionPath(path) )  { 
           if( m_parameterLog->m_debug>=1) {
            m_parameterLog->setMessage("参数错误，没有发现"+path +"\n",1); 
           }
           return false;
       } 
                  
       while( m_sysParam->getSectionValue(name,value) ) 
       {
           if(name.compare("where")==0) {
               eq         += value + " ";         
           }                              
       }
       //解析配置文件,得到一个表达形 m_exp (如有必要,可以形成一个树结构)
       if(!queryUseExpression->setExpression(eq,order)) { 
           return false;
       } 
       
       return true;                                          
}//initializeExpression

//##ModelId=3FB33EBA00E0
void QueryManager::setParam(PConfigureFile *sysParam, const string &name, ParameterList *list)
{   
    string desStr;
    string::size_type bpos, epos;
    const string trimchar("/");

    bpos = name.find_first_of(trimchar);   
    
    if (bpos != string::npos)
          m_systemName     = name.substr(bpos+1);
    else 
        m_systemName    = name; 
          
    m_sysParam         = sysParam;  
    m_parameterList    = list;         
    
}

//##ModelId=3FCAA443004B
int QueryManager::getQueryId(const string &queryName)
{
    int size = m_queryList.size(); 
    
    for(int i=0;i<size;i++)
    {
        if(m_queryList[i].compare(queryName)==0) {
            return i;
        }
    }
    
    return -1;    
}

void QueryManager::getQuery(const char *param, string &result)
{
	char t[10];
    result = "";
    for(int i=0;i<m_queryList.size();i++)
    {
    	sprintf(t,"查询%d: ",i+1);
    	result += string(t) + " find " + m_queryList[i] + " " + m_queryUseExpression[i]->getQuery() + "\n";
    }
}

