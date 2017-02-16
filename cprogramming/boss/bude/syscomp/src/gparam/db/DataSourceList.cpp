// ############################################
// Source file : DataSourceList.cpp
// Version     : 0.1.1
// Language       : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20030925
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include "DataSourceList.h"

//##ModelId=3F72756B00C7
DataSourceList::DataSourceList()
{ 
    m_parameterLog=ParameterLog::getInstance();
}

//##ModelId=3F72757302DB
DataSourceList::~DataSourceList()
{    
    for(int i=0;i<m_dataSource.size();i++) 
    {
        delete m_dataSource[i];
    }
    m_dataSource.clear();
    m_sourceTypeList.clear();
}

//##ModelId=3F72750D0112 两个都是输入参数,type 代表了从数据库还是文件
bool DataSourceList::initialize(const int &type,const vector <string> & param)
{  
    DataSource *dataSource;
    
    if(type == 0) 
    {
        dataSource = new  DbDataSource;
    } 
    else if( type == 1) 
    {
        dataSource = new  FileDataSource;
    } 
    else 
    {
        return false;
    } 
        
    if(!dataSource->initialize(param))
        return false; 
            
    m_dataSource.push_back(dataSource);
    m_sourceTypeList.push_back(type); //add by gaojf 2006-6-7 14:25
    return true;    
}

//##ModelId=3F72764501E3
DataSource *DataSourceList::getInstance(string name,int &type)
{  
    for(int i=0;i<m_dataSource.size();i++)
    {
        if((m_dataSource[i]->getSourceID()).compare(name)==0)
        {
            type = m_sourceTypeList[i]; //add by gaojf 2006-6-7 14:25
            return m_dataSource[i];
        }
    }   
    
    return 0;        
}

//##ModelId=3F9F52A70212
void DataSourceList::close()
{   
    for(int i=0;i<m_dataSource.size();i++)
    {
        m_dataSource[i]->close();
    }
}

