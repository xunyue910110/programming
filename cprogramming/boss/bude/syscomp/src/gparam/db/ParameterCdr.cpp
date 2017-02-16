// ############################################
// Source file : ParameterCdr.cpp
// Version     : 0.1.1
// Language       : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20030925
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include <stdlib.h> 
 
#include "ParameterCdr.h" 

  

//##ModelId=3F8B491300C7
ParameterCdr::ParameterCdr()
{
    m_index              = 0;
    m_fields             = 0;
    
    m_count              = 0;    
    m_parameterBuffer    = 0;    
}              
          
//##ModelId=3F8B491300D1
ParameterCdr::~ParameterCdr()
{   
}  

//##ModelId=3F72A2DF02D1 向 m_fields 中添加一些字段
bool ParameterCdr::insert(const vector<string> & field)
{       
    char *t; 
    int i,j;
    char tmp[1024]; 
     
    j=0;
    t=tmp;
    m_count = field.size();  
    memset(tmp,0,sizeof(tmp)); 
    
    if(m_count<=0)
        return true; 
    //m_index 可用的起始位置    
    if( (m_index = m_parameterBuffer->getIndexBuffer(m_count)) == 0 )
        return false;
         
    for(i=0;i<m_count;i++)
    {   
        m_index[i]=j;   
        memcpy(t,field[i].c_str(),field[i].size());  
        j+=strlen(t)+1;            // 下一个字段的偏移量
        t=tmp+j;                   // 下一个字段的起始地址
    }
     
    if( (m_fields = m_parameterBuffer->getBuffer(j)) == 0 ) 
    {   // 申请长度为j的存储空间
        return false;
    } 
    memcpy(m_fields,tmp,j);    
    
    return true;
}

//##ModelId=3F77D965014E
const char *ParameterCdr::at(const int i) const
{  
    int m = 0;
        
    m = (int)m_index[i];
    
    return m_fields + m;
}

//##ModelId=3F77DAA703B4
const int ParameterCdr::getCount() const 
{   
    return (int)m_count;
}



//##ModelId=3F88BCA90322
void ParameterCdr::setParameterBuffer(ParameterBuffer *buffer)
{    
    m_parameterBuffer = buffer;
}

