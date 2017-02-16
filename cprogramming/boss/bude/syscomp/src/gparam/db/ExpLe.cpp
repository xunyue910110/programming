// ############################################
// Source file : ExpLe.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20031124
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include "ExpLe.h"
#include "ParameterCdr.h"

//##ModelId=3FAF4ADA0012
ExpLe::ExpLe()
{    
	m_leftExp  = 0; 
	m_rightExp = 0;
	 
	strcpy(m_token,"<=");
}

//##ModelId=3FAF4ADB0172
ExpLe::~ExpLe()
{
}

//##ModelId=3FC1A2A9002A
bool ExpLe::evaluate(const vector<string> *right,const ParameterCdr *parameterCdr)
{  
	char tmp[40] ;
	const char *r,*l;
	
	
	l = format( tmp,parameterCdr->at(m_leftId) );
	r = (*right)[m_rightId].c_str();
		
	return strcmp( r , l ) <= 0;
	//return  strcmp( (*right)[m_rightId].c_str() , m_parameterCdr->at(m_leftId) ) <= 0;
}

//##ModelId=3FC1A2D100D2
void ExpLe::setCdr(const ParameterCdr *parameterCdr)
{   
	m_parameterCdr = parameterCdr;
}

