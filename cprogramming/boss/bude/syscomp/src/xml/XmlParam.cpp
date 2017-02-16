#include "XmlParam.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>

XmlParam::XmlParam()
{
    m_filename = "";
    m_sectionPath = "";
}

XmlParam::~XmlParam()
{}

bool XmlParam::Initialization(char *filename)
{
    m_filename = filename;
    if ( !xmlApi.open(m_filename)) 
        return false;
        
    return true;
}

void XmlParam::CloseSection()
{
    xmlApi.close();    
}

bool XmlParam::GetValue(const string& sectionPath, ResultMap& value)
{
    ResultMap resultMultiMap;    
    
    m_sectionPath = sectionPath;  
    m_sectionPath += "/@*";
    
    cout<<"m_sectionPath: "<<m_sectionPath<<endl;
    if ( !xmlApi.setXPath(m_sectionPath.c_str()) ) 
    {
        //cout<<"set setXPath false"<<endl;
        return false;
    }
    
    if ( !xmlApi.getXPathResultSet(resultMultiMap) )
    {
        //cout<<"getXPathResultSet false"<<endl;
        return false;
    }
        
    if (resultMultiMap.size() > 0) 
    {
        value = resultMultiMap;
        return true;
    } 
   
    m_sectionPath = sectionPath;  
    m_sectionPath += "/*";
    
    if ( !xmlApi.setXPath(m_sectionPath.c_str())) 
    {
        //cout<<" set setXPath false"<<endl;
        return false;
    }    
    
    if ( !xmlApi.getXPathResultSet(resultMultiMap))
        return false;   
        
    if (resultMultiMap.size() > 0) 
    {
        value = resultMultiMap;        
    }        
   
    return true;
}

bool XmlParam::GetValue(const string& sectionPath, const string& name,
               ResultVector& value)
{
    ResultMap resultMultiMap;
        
    m_sectionPath = sectionPath;  
    m_sectionPath += "/@";
    m_sectionPath += name;
    
    if ( !xmlApi.setXPath(m_sectionPath.c_str())) 
        return false;            
        
    if ( !xmlApi.getXPathResultSet(resultMultiMap))
        return false;       

    ResultMap::iterator iter;
    for(iter=resultMultiMap.begin();iter!=resultMultiMap.end();iter++)
    {
        value.push_back(ResultVector::value_type(iter->second));
       // cout<<"{\""<< iter->first <<"\",\""<< iter->second <<"\"}"<<endl;
    }        
    return true;    
}


