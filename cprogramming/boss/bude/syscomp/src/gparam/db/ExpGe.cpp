// ############################################
// Source file : ExpGe.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20031124
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include "ExpGe.h"
#include "ParameterCdr.h"

//##ModelId=3FAF4B4F0377
ExpGe::ExpGe()
{  
	m_leftExp  = 0; 
	m_rightExp = 0;
	 
	strcpy(m_token,">=");
}

//##ModelId=3FAF4B510059
ExpGe::~ExpGe()
{
}

//##ModelId=3FC1A29D02CC
bool ExpGe::evaluate(const vector<string> *right,const ParameterCdr *parameterCdr)
{ 
	char tmp[40] ;
	const char *r,*l;
	
	l = format( tmp,parameterCdr->at(m_leftId) );
	//cout<<"l="<<l<<endl;
	
	r = (*right)[m_rightId].c_str();//来源于客户端传进的参数
	//cout<<"r="<<r<<endl;
	
	return strcmp( r , l ) >= 0;
	//return  strcmp( (*right)[m_rightId].c_str() , m_parameterCdr->at(m_leftId) ) >= 0;
}

//##ModelId=3FC1A2CE0019
void ExpGe::setCdr(const ParameterCdr *parameterCdr)
{  
	m_parameterCdr = parameterCdr;
}

