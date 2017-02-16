// ##########################################
// Source file : FieldConvertRule_3.cpp
// System      : Mobile Billing System
// Version     : 0.1.8
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20020316
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "FieldConvertRule_3.h"

//##ModelId=3BAAEAC2029D
bool FieldConvertRule_3::initialize(string ruleText)
{
    char *tok;
	int len,ndx;
	char cTmp[100];
	vector<string> tmpStrs,tmpStrs1;

	// example :   rule_3;ALL;MSRN_FLAG,MSRN,1
	splitStrByToken(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 3)
	{
		// 小于3个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
	m_callType = tmpStrs[1]; //跳过 'rule_3'
	
	// 提取 MSRN_FLAG,MSRN,1
	splitStrByToken(tmpStrs[2],",",tmpStrs1);
	if( tmpStrs1.size() == 3)
	{
		m_phoneFlag = tmpStrs1[0];
		m_phone     = tmpStrs1[1];
		m_order     = atoi(tmpStrs1[2].c_str());
	}
	else if( tmpStrs1.size() == 2 )
	{
		m_phoneFlag = tmpStrs1[0];
		m_phone     = tmpStrs1[1];
		m_order     = 1;
	}	
	else
	{
		return false;	
	}

	return true;
}

//##ModelId=3BAAEB020321
bool FieldConvertRule_3::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
	int i,len;
	int zeroCount;
	char src[30],des[30];
	FieldInfo *otherP,*srcP;
	static string distanceCode;
	static string callType("CALL_TYPE"),all("ALL");
	
	if( (otherP=getFieldMap(theFieldInfoMaps,callType))==0 )
		return false;
	
	// 条件：通话类型，“包括：国内、国际、本地”
	if(m_callType.compare(all)!=0) 
	{
	   if(otherP->m_FieldDesValue.compare(m_callType)!=0) 
	   {
	      return false;
	   }
	}
	
	if( (srcP=getFieldMap(theFieldInfoMaps,m_phoneFlag)) == 0 ) 
	{
		return false;
	}
	if ( (otherP=getFieldMap(theFieldInfoMaps,m_phone))  == 0 )   
	{
	   return false;
	}

	if(srcP->m_FieldDesValue.compare("91")==0)          // 国际电话
	{
	   distanceCode = "00";
	} 
	else if(srcP->m_FieldDesValue.compare("A1")==0)   // 国内电话
	{
	   distanceCode = "0";
	} 
	else 
	{
	   return true;
	}

    // modified by chenm 2003-11-3 对国际长途标识的单独处理
    if( (srcP->m_FieldDesValue.compare("91")==0) 
    	&& (otherP->m_FieldDesValue.size() >= 2) )
    {
    	//  不是'00'开头的号码，加'00'
    	if( (otherP->m_FieldDesValue.size() >= 2)
    		&& (otherP->m_FieldDesValue.compare(0,2,"00")!=0) )
    	{
    		otherP->m_FieldDesValue	= "00" + otherP->m_FieldDesValue;
    	}
    }
    else if(srcP->m_FieldDesValue.compare("A1")==0)
    {
    	zeroCount = mvZero(otherP->m_FieldDesValue,des);       // 去掉开始的"0"
    	
		if( (strncmp(des,"13",2)==0)||(strncmp(des,"15",2)==0)||(strncmp(des,"18",2)==0) ) 
		{
		   otherP->m_FieldDesValue = des;
    	
		   return true;
		}
    	
		if( strncmp(des,"86",2)!=0 ) 
		{
			if( (zeroCount>0) && (zeroCount!=distanceCode.size()) )  
			{
			   if(m_order==1) 
			   {
			      otherP->m_FieldDesValue = distanceCode + des;
			   }
			} 
			else 
			{
			   otherP->m_FieldDesValue = distanceCode + des;
			}
		} 
		else 
		{
		   len = strlen(des) - 2;
		   if( len <= 0) 
		   {
		      return false;
		   }
		   memcpy(src,des+2,len);                          // 去掉"86"
		   src[len] = 0;
    	   mvZero(src,des);                                // 去掉开始的"0"
		   if( (strncmp(des,"13",2)!=0)&&(strncmp(des,"15",2)!=0)&&(strncmp(des,"18",2)!=0) ) 
		   {
		      otherP->m_FieldDesValue = string("0") + des; // is 国内长途
		   } 
		   else 
		   {
		      otherP->m_FieldDesValue = des;
		   }
		}
	}

	return true;
}

//##ModelId=3C2D434F014B
int FieldConvertRule_3::mvZero(string src,char *des)
{
    int i,len;
    char tmp[40];

	len=src.size();
	strcpy(tmp,src.c_str());
	for(i=0;i<len;i++)
	{
	   if(tmp[i]!='0') 
	   {
	      break;
	   }
	}
	len -= i;
	memcpy(des,tmp+i,len);          // 去掉开始的字符"0"
	des[len]=0;

    return i;
}

