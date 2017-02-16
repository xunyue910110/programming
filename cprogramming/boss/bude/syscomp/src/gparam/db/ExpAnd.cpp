// ############################################
// Source file : ExpAnd.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20031124
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include "ExpAnd.h"
#include "ParameterCdr.h"

//##ModelId=3FAF50D00142
ExpAnd::ExpAnd()
{ 
	m_leftExp  = 0; 
	m_rightExp = 0;
	 
	strcpy(m_token,"and"); 	
}

//##ModelId=3FAF50D1028E
ExpAnd::~ExpAnd()
{
}

//##ModelId=3FC1A2AD017A
bool ExpAnd::evaluate(const vector<string> *right,const ParameterCdr *parameterCdr)
{     
	return m_leftExp->evaluate(right,parameterCdr) && m_rightExp->evaluate(right,parameterCdr);	
}

//##ModelId=3FC1A2D4018A
void ExpAnd::setCdr(const ParameterCdr *parameterCdr)
{   
	m_leftExp->setCdr(parameterCdr); 
	m_rightExp->setCdr(parameterCdr);
}

