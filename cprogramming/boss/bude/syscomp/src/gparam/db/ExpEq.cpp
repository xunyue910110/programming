// ############################################
// Source file : ExpEq.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031124
// Update      : 20031124
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include "ExpEq.h"
#include "util/Util.h" 
#include "ParameterCdr.h"

//##ModelId=3FAF4A820065
ExpEq::ExpEq()
{  
	m_leftExp  = 0; 
	m_rightExp = 0;
	 
	strcpy(m_token,"==");
}

//##ModelId=3FAF4A83008F
ExpEq::~ExpEq()
{
}

//##ModelId=3FC1A2990103
bool ExpEq::evaluate(const vector<string> *right,const ParameterCdr *parameterCdr)//right 放客户端传进的参数 
{   	
	char tmp[40] ;
	const char *r,*l,*m;
		
	m = parameterCdr->at(m_leftId);
	l = format( tmp,m );
	r = (*right)[m_rightId].c_str();

        if( strncmp(l,"*",1) == 0 )
		return true;
		
	return strcmp( r , l ) == 0;
	//return  (strcmp(m_parameterCdr->at(m_leftId),(*right)[m_rightId].c_str()) == 0);
}

//##ModelId=3FC1A2DE0301
void ExpEq::setCdr(const ParameterCdr *parameterCdr)
{  
	m_parameterCdr = parameterCdr;
}

