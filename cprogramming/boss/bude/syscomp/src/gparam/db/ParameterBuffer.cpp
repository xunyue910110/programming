// ############################################
// Source file : ParameterBuffer.cpp
// Version     : 0.2
// Language       : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031012
// Update      : 20031127
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include <stdio.h>

#include "ParameterBuffer.h"


//##ModelId=3F88BB760078
ParameterBuffer::ParameterBuffer()
{       
    m_sum                 = 0;
    m_buffer              = 0;
    m_indexBuffer         = 0;  
    m_bufferUnit          = 0;
    
    m_buffers.clear();
    m_indexBuffers.clear(); 
}

//##ModelId=3F88BB7F0324
ParameterBuffer::~ParameterBuffer()
{               
    clear();
} 

//##ModelId=3F88BB4E0052
char *ParameterBuffer::getBuffer(int size)
{
    char *buffer;
    
    if( m_buffer==0 || size>(m_bufferUnit-m_position) ) //必须开辟一段新的内存
    {          
        createBuffer();
        if(m_buffer==0) 
        {               
            return 0; 
        }
    } 
    m_sum += size;
    
    buffer          = m_buffer + m_position;
    m_position     += size; 
    
    return buffer;
}  

//##ModelId=3F8CC1A102AA
char *ParameterBuffer::getIndexBuffer(int size)
{   
    char *buffer=0;
    
    if( m_indexBuffer == 0 || size > (1024 - m_indexPosition) ) 
    {  
        createIndexBuffer();   
        if(m_indexBuffer==0) 
        {               
            return 0; 
        }
    } 
    buffer                   = m_indexBuffer + m_indexPosition;
    m_indexPosition         += size; 
    
    return buffer;
}

//##ModelId=3F88BBAB0088
bool ParameterBuffer::createBuffer()
{   
    m_buffer = 0;                                 
    m_buffer = new char[m_bufferUnit];    
    if(m_buffer == 0)
        return false;
    
    memset(m_buffer,0,sizeof(m_buffer));   
    m_buffers.push_back(m_buffer);   
    
    m_position = 0;  
     
    return true;
}

//##ModelId=3F8CC1A1034A
bool ParameterBuffer::createIndexBuffer()
{       
    m_indexBuffer = 0; 
    m_indexBuffer = new char[1024];   
    if(m_indexBuffer == 0)
        return false;
    memset(m_indexBuffer,0,sizeof(m_indexBuffer)); 
    m_indexBuffers.push_back(m_indexBuffer);   
    
    m_indexPosition = 0;  
     
    return true;
}

//##ModelId=3F88C0D002BC
void ParameterBuffer::setBufferUnit(const int unit)
{   
    m_bufferUnit = unit;    
}

//##ModelId=3F8B495C03A7 得到内存分配的利用率
string ParameterBuffer::getMessage(int & mem)
{    
    float ratio; 
    char t1[20],t2[20],t3[20];
    string message;    
     
    if( m_bufferUnit*m_buffers.size()==0)
        return "";  
     
    ratio = (float)m_sum/(m_bufferUnit*m_buffers.size())*100; 
    mem   = m_bufferUnit*m_buffers.size()/1024; 
    
    sprintf(t1,"%f",ratio); 
    sprintf(t2,"%d",mem);  //已分配多少K
    sprintf(t3,"%d",m_buffers.size());
    
    message =string(" 已分配内存: ") + t2 + "k, 缓存块数: " + t3 + ", 内存利用率为: " + t1 + "%\n";     
                
    return  message;
}

//##ModelId=3FD3E02000CB
bool ParameterBuffer::clear()
{   
    int size,i;
    
    size=m_buffers.size();             
    for(i=0;i<size;i++)
    {
        delete [] m_buffers[i];
    } 
     
    size=m_indexBuffers.size();             
    for(i=0;i<size;i++)
    {
        delete [] m_indexBuffers[i];
    } 
    
    m_sum                 = 0;
    m_buffer              = 0;
    m_indexBuffer         = 0;  
    
    m_buffers.clear();
    m_indexBuffers.clear();   
    
    return true;    
}

