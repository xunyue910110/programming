// ############################################
// Source file : ExpOr.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20031124
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include "ExpOr.h"
#include "ParameterCdr.h"

//##ModelId=3FAF50F20386
ExpOr::ExpOr()
{   
	m_leftExp  = 0; 
	m_rightExp = 0;
	 
	strcpy(m_token,"or");
}

//##ModelId=3FAF50F400FE
ExpOr::~ExpOr()
{
}

//##ModelId=3FC1A2B2000F
bool ExpOr::evaluate(const vector<string> *right,const ParameterCdr *parameterCdr)
{ 
	return m_leftExp->evaluate(right,parameterCdr) || m_rightExp->evaluate(right,parameterCdr);	
}

//##ModelId=3FC1A2D80064
void ExpOr::setCdr(const ParameterCdr *parameterCdr)
{   
	m_leftExp->setCdr(parameterCdr); 
	m_rightExp->setCdr(parameterCdr);
}

