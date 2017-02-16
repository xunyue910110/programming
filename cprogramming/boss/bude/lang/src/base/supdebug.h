// Language:    C++
//
// OS Platform: UNIX
//
// Authors:     Lu Zhong Liang <luzl@lianchuang.com>
//
// History:
// Copyright (C) 2001 by Linkage. All Rights Reserved.
//
// Comments:    定义调试的宏及性能测试
//
//------------------------------------------------------------------------------

#ifndef SUP_DEBUG_H
#define SUP_DEBUG_H

#ifdef  DEBUG_COUT
extern  ofstream DEBUG_COUT;
#define COUT DEBUG_COUT
#else   //DEBUG_COUT
#define COUT cout
#endif  //DEBUG_COUT 

#ifdef FP_DEBUG

#include "config-all.h"
#include <sys/time.h>
#include <iostream>
#include <iomanip>
#include <string>

USING_NAMESPACE(std)

class FunctionPerformance
{
public:
  FunctionPerformance(const string& name) 
    : m_seconds(0), m_microseconds(0), m_functionName(name), m_counter(0) 
  {
  }
  
  ~FunctionPerformance()
  {
    COUT << "Function <" << m_functionName << ">: " << endl 
      << "Total: "<< m_seconds << "." 
      << setw(6) << setfill('0') << m_microseconds << setw(0) << setfill(' ') << "s" << endl
      << "Count: "<< m_counter << endl;

    if(m_counter > 0)
      COUT << "Average: " << ((double)(m_seconds * 1000000 + m_microseconds))/m_counter/1000000 << "s" << endl;
   }
  
  void begin() {    
    gettimeofday(&m_beginPoint, NULL);        
  }
  
  void end() {
    struct timeval endPoint;
    gettimeofday(&endPoint, NULL);                
    m_microseconds += (endPoint.tv_sec - m_beginPoint.tv_sec) * 1000000 + (endPoint.tv_usec - m_beginPoint.tv_usec);    
    m_seconds += m_microseconds/1000000;
    m_microseconds %= 1000000;
    ++m_counter;
  }
  
  private:
    int m_seconds;
    int m_microseconds;
    int m_counter;
    string m_functionName;
    struct timeval m_beginPoint;
};

#define FP_BEGIN(name) static FunctionPerformance FP##name(#name); FP##name.begin();
#define FP_END(name) FP##name.end();
#define __FP(name, statement) static FunctionPerformance FP##name(#name); FP##name.begin(); statement FP##name.end();

#else  //FP_DEBUG

#define FP_BEGIN(name)
#define FP_END(name)
#define __FP(name, statement) statement

#endif  //FP_DEBUG

#ifdef  TRACE_DEBUG

#define TR_POS() COUT << "FILE:" << __FILE__ << " LINE:" << __LINE__ << endl << flush;
#define TR_OUT(statement) COUT statement
#define __TR(statement)  COUT << #statement << endl; statement


#else   //TRACE_DEBUG

#define TR_POS() 
#define TR_OUT(statement)
#define __TR(statement) statement


#endif  //TRACE_DEBUG

#endif  //SUP_DEBUG_H
