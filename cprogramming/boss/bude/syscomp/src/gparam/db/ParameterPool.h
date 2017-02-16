// ############################################
// Source file : ParameterPool.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030929
// Update      : 20030929
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#ifndef PARAMETERPOOL_H_INCLUDED_C0883B64
#define PARAMETERPOOL_H_INCLUDED_C0883B64

#include "config-all.h"

#include <vector> 
 
#include "ParameterLog.h"

USING_NAMESPACE(std);

//##ModelId=3F77A3CB0265
template <class T>
class ParameterPool
{
  public:
    
    //##ModelId=3F77A8BE00D7
    void add(T & t);

    //##ModelId=3F77A8C103B6
    long size();
  
    //##ModelId=3F77A8CF02D0
    void sort();
  
    //##ModelId=3F77A8DA0271
    
    void clear();
  
    //##ModelId=3F77A8E902F5
    void resize(long sz);  
    
    //##ModelId=3F77C202018B
    long capacity();  
    
    //##ModelId=3F77C7D400B2
    ParameterPool();

    //##ModelId=3F77C7D502E4
    ~ParameterPool();  
    
    //##ModelId=3F77D5B90265
    T * at(long i); 
    
    //##ModelId=3F8CCF300111
    int search(const T &record, vector<T> &result); 
    
    //##ModelId=3F8CCF30020B
    int searchMulti(const T &record, vector<T> &result);  
    
    //##ModelId=3FBAC92303E4
    int scanSearch(const T &record, vector<T> &result);


    //##ModelId=3FBAC8D000B0
    int scanSearchMulti(const T &record, vector<T> &result);

  private:
    //##ModelId=3F77A3FB0341
   	vector <T> *m_paramVector ;   
   	          
    //##ModelId=3F77C6B901C3
    long m_capacity; 
    
    //##ModelId=3F77E94503B5
    double m_redundancy;
    
    //##ModelId=3F77ED28015D
    ParameterLog *m_parameterLog;   
    
    

};

int index_binary_search( vector <Index> &indexList, Index& index) ;

int static index_binary_search( vector <Index> &indexList,const Index& index) 
{
	int lo,hi,mid;
	vector<Index>::iterator itr,itr_begin; 

	lo = 0;
	hi = indexList.size()-1;  
    
    itr_begin = indexList.begin();
    	
	while (lo <= hi)
	{  
		mid = (lo + hi) / 2;
		if (  *(itr_begin+mid) == index   ) {
			return mid;
		}
    
		if ( !(*(itr_begin+mid) < index)   ) {
			hi = --mid;
		} else {
			lo = ++mid;
		}
		
	}
    
	return -1;
}


#include "ParameterPool.cpp"

#endif /* PARAMETERPOOL_H_INCLUDED_C0883B64 */
