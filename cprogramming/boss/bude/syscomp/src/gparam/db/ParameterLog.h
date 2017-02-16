// ############################################
// Source file : ParameterLog.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030927
// Update      : 20030927
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef PARAMETERLOG_H_INCLUDED_C08A8EC9
#define PARAMETERLOG_H_INCLUDED_C08A8EC9  

#include "Config.h"

#include <string>  
   
class PFileProcess;

USING_NAMESPACE(std);

//##ModelId=3F7529B603D4
class ParameterLog
{    
  public: 
    //##ModelId=3F752B6C01F3
	ParameterLog();
   
    //##ModelId=3F752B6C0207
   	~ParameterLog();
   
    //##ModelId=3F752B6C0225
   	static ParameterLog *getInstance(); 
   	
    //##ModelId=3F752B6C0257
    //##Documentation
    //## 显示调试信息。
   	void setMessage(string message, int flag);   
   	
    //##ModelId=3F752D7E01A5
    void setDebug(int flag);  
    
    //##ModelId=4026FD9E00FF
    void setLogPath(const string &logPath);
    
    string getMessage();

    
    //##ModelId=3F752B6C01AD
    //##Documentation
    //## 调试信息输出级别：0:无输出；1:告警信息；2:少量输出；3:大量输出
	int m_debug;
  
  private: 
    //##ModelId=3F752B6C0192
	static ParameterLog *m_parameterLog;  
	
    //##ModelId=4026FD870278
    string m_logPath;   
    
    string m_message;
    
    //##ModelId=4027252A0110
    PFileProcess *m_logFile;


	  
};  

#endif /* PARAMETERLOG_H_INCLUDED_C08A8EC9 */
