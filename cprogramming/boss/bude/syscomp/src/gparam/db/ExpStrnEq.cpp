// ############################################
// Source file : ExpStrnEq.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031128
// Update      : 20031128
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include "ExpStrnEq.h"

//##ModelId=3FC6D8470063
ExpStrnEq::ExpStrnEq()
{  
	m_leftExp  = 0; 
	m_rightExp = 0;
	 
	strcpy(m_token,"neq");
}

//##ModelId=3FC6D84A00B7
ExpStrnEq::~ExpStrnEq()
{
}

//##ModelId=3FC6D8D60285
bool ExpStrnEq::evaluate(const vector<string> *right,const ParameterCdr *parameterCdr)
{   
	int n,nl,nr;	
	char tmp[40] ;
	const char *r,*l;
	
	l = format( tmp,parameterCdr->at(m_leftId) );
	r = (*right)[m_rightId].c_str();
	
	
	nr = (*right)[m_rightId].size();
	nl = strlen(l);
	
	if( nr > nl)
	    n = nl;
    else
        n = nr;	    
		
	return strncmp( l , r , n) == 0;		
	//return  strncmp( (*right)[m_rightId].c_str() , m_parameterCdr->at(m_leftId) , n ) == 0;
}

//##ModelId=3FC6D8EC0114
void ExpStrnEq::setCdr(const ParameterCdr *parameterCdr)
{  
	m_parameterCdr = parameterCdr;
}

