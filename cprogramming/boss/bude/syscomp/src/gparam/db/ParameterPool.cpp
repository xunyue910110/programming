// ############################################
// Source file : ParameterPool.cpp
// Version     : 0.2
// Language       : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030929
// Update      : 20031015
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include <stdio.h>    
#include <algorithm>

//##ModelId=3F77C7D400B2                     
template <class T>
ParameterPool<T>::ParameterPool()
{   
    m_redundancy      	  = 1;      // 冗余比率
    m_paramVector     	  = new vector<T>;   
    m_parameterLog        = ParameterLog::getInstance();
} 

//##ModelId=3F77A8E902F5
template <class T>
void ParameterPool<T>::resize(long sz)
{     
    m_paramVector->resize((unsigned int)(sz * m_redundancy + 1));
    m_paramVector->clear(); 
    m_capacity=m_paramVector->capacity();
}

//##ModelId=3F77C7D502E4
template <class T>
ParameterPool<T>::~ParameterPool()
{   
    delete m_paramVector;
}

//##ModelId=3F77A8BE00D7
template <class T>
void ParameterPool<T>::add(T & t)
{   
    char tmp[20],tmp1[20];    
    int size = m_capacity;  
    vector <T> *paramVector ; 
    
    if(m_paramVector->size()==m_capacity) { 
        paramVector = new vector<T>;  
        paramVector->resize((long)(m_capacity * m_redundancy));      
        paramVector->clear();
        m_capacity=paramVector->capacity(); 
        
        for( int i=0;i<size;i++)
        {
            paramVector->push_back(m_paramVector->at(i));
        } 
        delete m_paramVector;
        m_paramVector = paramVector;                 
    }        
    m_paramVector->push_back(t);
    m_capacity = m_paramVector->capacity();    
    
    if( m_parameterLog->m_debug>=2 && m_capacity>size) {
        sprintf(tmp,"%d",size);
        sprintf(tmp1,"%d",m_capacity);
        m_parameterLog->setMessage(string("*** old capacity=")+tmp+",new capacity="+tmp1+" ***\n",0);
    }        
}

//##ModelId=3F77A8C103B6
template <class T>
long ParameterPool<T>::size()
{ 
    return m_paramVector->size();
}

//##ModelId=3F77A8CF02D0
template <class T>
void ParameterPool<T>::sort()
{  
    std::partial_sort(m_paramVector->begin(),m_paramVector->end(),m_paramVector->end());  
}

//##ModelId=3F77A8DA0271
template <class T>
void ParameterPool<T>::clear()
{  
    m_paramVector->clear();
}

//##ModelId=3F77C202018B
template <class T>
long ParameterPool<T>::capacity()
{   
    return m_paramVector->capacity();    
}

//##ModelId=3F77D5B90265
template <class T>
T * ParameterPool<T>::at(long i)
{   
    return &m_paramVector->at(i);
}

//##ModelId=3F8CCF300111
template <class T>
int ParameterPool<T>::search(const T &record,vector<T> &result)
{   
    typename vector<T>::iterator itr; 
    
	result.clear();
    itr=lower_bound(m_paramVector->begin(),m_paramVector->end(),record);
    if(itr==m_paramVector->end()||!(*itr==record))return 0;

    result.push_back(*itr);
    return 1;
}
    
//##ModelId=3F8CCF30020B
template <class T>
int ParameterPool<T>::searchMulti(const T &record, vector<T> &result)
{
    int i=0;
    typename vector<T>::iterator itr,itr0; 

    result.clear();   
    itr0=lower_bound(m_paramVector->begin(),m_paramVector->end(),record); 

    for(itr=itr0;itr!=m_paramVector->end();itr++)
    {
        if(!(*itr==record))	break;
        result.push_back(*itr);
        i++;
    }  

    return i;
}

//##ModelId=3FBAC92303E4
template <class T>
int ParameterPool<T>::scanSearch(const T &record, vector<T> &result)
{   
    typename vector<T>::iterator itr; 
    
	result.clear(); 
    for(itr=m_paramVector->begin();itr!=m_paramVector->end();itr++)
    {
        if(*itr==record)  {
            result.push_back(*itr);
            return 1;
        }
    }
    return 0;
}

//##ModelId=3FBAC8D000B0
template <class T>
int ParameterPool<T>::scanSearchMulti(const T &record, vector<T> &result)
{  
	int i=0;
	typename vector<T>::iterator itr; 

	result.clear();
	for(itr=m_paramVector->begin();itr!=m_paramVector->end();itr++)
	{
		if(*itr==record)	{
			result.push_back(*itr);
			i++;
		}
	} 
	
	return i;	
}

