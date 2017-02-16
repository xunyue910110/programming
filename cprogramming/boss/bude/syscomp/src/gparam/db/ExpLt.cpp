// ############################################
// Source file : ExpLt.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20031124
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include "ExpLt.h"
#include "ParameterCdr.h"

//##ModelId=3FAF49EE0121
ExpLt::ExpLt()
{   
	m_leftExp  = 0; 
	m_rightExp = 0;
	 
	strcpy(m_token,"<");
}

//##ModelId=3FAF49EF03C1
ExpLt::~ExpLt()
{
}

//##ModelId=3FC1A28F020D
bool ExpLt::evaluate(const vector<string> *right,const ParameterCdr *parameterCdr)
{  
	char tmp[40] ;
	const char *r,*l;
	
	l = format( tmp,parameterCdr->at(m_leftId) );
	r = (*right)[m_rightId].c_str();
	
	return strcmp( r , l ) < 0;
	//return  strcmp( (*right)[m_rightId].c_str() , m_parameterCdr->at(m_leftId) ) < 0;
}

//##ModelId=3FC1A2CA0190
void ExpLt::setCdr(const ParameterCdr *parameterCdr)
{   
	m_parameterCdr = parameterCdr;
}

