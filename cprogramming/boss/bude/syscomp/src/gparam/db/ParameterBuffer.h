// ############################################
// Source file : ParameterBuffer.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031012
// Update      : 20031012
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef PARAMETERBUFFER_H_INCLUDED_C0772C6F
#define PARAMETERBUFFER_H_INCLUDED_C0772C6F  

#include "Config.h"


#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <string>
#include <vector>  
#include <stdlib.h>   


USING_NAMESPACE(std)

//##ModelId=3F88B9BA0192 负责一段一段不连续内存的分配
class ParameterBuffer
{
  public:
    //##ModelId=3F88BB4E0052
    char *getBuffer(int size);   
    
    //##ModelId=3F8CC1A102AA
    char *getIndexBuffer(int size);

    //##ModelId=3F88BB760078
    ParameterBuffer();

    //##ModelId=3F88BB7F0324
    ~ParameterBuffer();

    //##ModelId=3F88BBAB0088
    bool createBuffer();   
    
    //##ModelId=3F8CC1A1034A
    bool createIndexBuffer();  
    
    //##ModelId=3F88C0D002BC
    void setBufferUnit(const int unit);  
    
    //##ModelId=3F8B495C03A7
    string getMessage(int & mem);
    
    //##ModelId=3FD3E02000CB
    bool clear();


  private:
    //##ModelId=3F88B9C803D7
    vector <char *> m_buffers; 
    
    //##ModelId=3F8CC1A003CB
    vector <char *> m_indexBuffers;

    //##ModelId=3F88B9E10328
    short m_position; //这段内存未用的相对起始位置
    
    //##ModelId=3F8CC1A101E2
    short m_indexPosition;

    //##ModelId=3F88BBCB0070
    char *m_buffer;     //一段内存的起始位置 
    
    //##ModelId=3F8CC1A10246
    char *m_indexBuffer;//一段内存的起始位置 

    //##ModelId=3F88BC0902AA
    int m_bufferUnit;  
    
    //##ModelId=3F8B4A59027F
    unsigned int m_sum; //内存中已用的字节数

};



#endif /* PARAMETERBUFFER_H_INCLUDED_C0772C6F */
