// ############################################
// Source file : ParameterCdr.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20030925
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef PARAMETERCDR_H_INCLUDED_C08D075F
#define PARAMETERCDR_H_INCLUDED_C08D075F  

#include <vector>
#include <string>  
#include <pthread.h>

#include "ParameterBuffer.h" 
#include "Config.h"


//##ModelId=3F729C24005D
//##Documentation
//## 参数记录类
class ParameterCdr
{
  public: 
  	//##ModelId=3F8B491300C7
	ParameterCdr();
	
	//##ModelId=3F8B491300D1
	~ParameterCdr(); 

    //##ModelId=3F72A2DF02D1
    //##Documentation
    //## 向 m_fields 中添加一些字段
    bool insert(const vector<string> & field);
    
    //##ModelId=3F77D965014E
    const char *at(const int i) const;
    
    //##ModelId=3F77DAA703B4
    const int getCount() const ;
        
    //##ModelId=3F88BCA90322
    void setParameterBuffer(ParameterBuffer *buffer);

  private:
    //##ModelId=3F729C3800E8
    //##Documentation
    //## 参数记录字段保存列表
    char *m_fields;
    
    //##ModelId=3F862542034B
    //char *m_index;一个列表,放的是各个字段的起始位置
    char *m_index;  
    
    //##ModelId=3F8B49130027
    char m_count;
        
    //##ModelId=3F88BCDF003B 负责一段一段不连续内存的分配
    ParameterBuffer *m_parameterBuffer;

    //pthread_mutex_t m_cdrMutex;    // 互斥量

};



#endif /* PARAMETERCDR_H_INCLUDED_C08D075F */
