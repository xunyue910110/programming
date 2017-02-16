// ##########################################
// Source file : FieldValidateRule.cpp
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20020108
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "FieldValidateRule.h"

//##ModelId=3BD62966007F
FieldValidateRule::FieldValidateRule()
{
   m_SendLog_	 = SendLog::getInstance();
   m_cdrFlag = CdrFlag::getInstance();
}

//##ModelId=3BCC237C007E
string FieldValidateRule::lrTrim(string theString)
{
	int len,size;

	size	= theString.size();
	len	= theString.find_first_not_of(" \t");
	if(len>0)
		theString	= theString.substr(len,size-len);	// 去掉头部空格和Tab
	len	= theString.find_last_not_of(" \t");
	if(len>0)
		theString	= theString.substr(0,len+1);		   // 去掉尾部空格和Tab

	return theString;
}

//##ModelId=3BFC6A250375
void FieldValidateRule::substr(string &srcString,string &desString)
{
	int len,size;
	
	size=srcString.size();
	
	if(m_subLength==0) 
	{
		if(size>m_substart)
		{
		   len=size - m_substart;
		}
		else
		{
		   return ;
		}
	} 
	else 
	{
		len=m_subLength;
	}
	
	desString=srcString.substr(m_substart,len);

}

//##ModelId=3BFC6C320366
void FieldValidateRule::substr(char * src,char *des)
{
	int len;
	
	if(m_subLength==0)
	{
	   len=strlen(src)-m_substart;
	}
	else
	{
	   len=m_substart;
	}
	
	if(len<=0)
	   return ;
	memcpy(des,src+m_substart,len);
	des[len]=0;
}

//##ModelId=3BFC782F0387
bool FieldValidateRule::setSubstr(string &src)
{
	string tmp;
	int len,ndx1,ndx2;
	
	// example :   substr(1,8)
	if ( ( ndx1  = src.find("(") ) < 0 )
		return false;
	if ( ( ndx2  = src.find(")") ) < 0 )
		return false;
	
	tmp=lrTrim( src.substr(ndx1+1,ndx2-ndx1-1) );
	
	if ( ( ndx1  = tmp.find(",") ) < 0 )
		return false;
	
	m_substart  = atoi(tmp.substr(0,ndx1).c_str())-1;
	m_subLength = atoi(tmp.substr(ndx1+1,tmp.size()-ndx1-1).c_str());
	
	if(m_substart<0 || m_subLength<0) 
	{
		return false;
	}
	
	return true;
}

//##ModelId=3C242F0603D6
FieldInfo * FieldValidateRule::getFieldMap(multimap<MyString,FieldInfo *> &theFieldInfoMaps,string &theFieldName)
{
    multimap<MyString,FieldInfo *>::iterator itr;
    itr = theFieldInfoMaps.find(theFieldName.c_str());      
    if( (itr != theFieldInfoMaps.end() )
             && ((itr->second)->m_FieldName_.compare(theFieldName) == 0)
       )
    {
        return itr->second;
    }

	return 0;
}

