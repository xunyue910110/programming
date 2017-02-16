// ############################################
// Source file : PublicFunc.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-4-23
// Update      : 2008-4-23
// Copyright(C): chen min, Linkage.
// ############################################

#include "PublicFunc.h"
#include "base/StringUtil.h"
#include "InputParams.h"
#include "MdbConstDef.h"

int PublicFunc::m_offSet = PublicFunc::isBigEndian() ? sizeof(int)- TRANS_FIELDS_LENTH_BYTES : 0;

//通用的解析,数据集的串,格式fileldNum,length(field1)field1,length(field2)field2,length(field3)field3
int PublicFunc::splitStr(const void *request,void ** &pValues)
{
	int length = 0;
	int fieldNum = 0;
	int offSet;
	
	if (request == NULL)
	{
		pValues = NULL;
		return 0;
	}

	memcpy((char*)&fieldNum+m_offSet,(char *)request,TRANS_FIELDS_LENTH_BYTES);
	offSet = TRANS_FIELDS_LENTH_BYTES;
	
	if( fieldNum>0 )
	{
		pValues = new void *[fieldNum];
	}
	else
	{
		pValues = NULL;
		return 0;	
	}

	for(int i=0;i<fieldNum;++i)
	{
		memcpy((char*)&length+m_offSet,(char *)request+offSet,TRANS_FIELDS_LENTH_BYTES);
		offSet += TRANS_FIELDS_LENTH_BYTES;
		if (length >0 )
		{
			pValues[i] = malloc(length+1);
			memset(pValues[i],0,length+1);
			memcpy(pValues[i],(char *)request+offSet,length);
		}
		else
		{
			pValues[i] = NULL;
		}
		offSet += length;
	}	

	return fieldNum;
}

//通用的解析,数据集的串,格式fileldNum,length(field1)field1,length(field2)field2,length(field3)field3
int PublicFunc::splitStr(const void *request,InputParams * &pInputParams)
{
	int length = 0;
	int fieldNum = 0;
	int offSet;
	
	if (request == NULL)
	{
		pInputParams = NULL;
		return 0;
	}

	memcpy((char*)&fieldNum+m_offSet,(char *)request,TRANS_FIELDS_LENTH_BYTES);
	offSet = TRANS_FIELDS_LENTH_BYTES;
	
	if(fieldNum>0)
	{
		pInputParams = new InputParams(fieldNum);
	}
	else
	{
		pInputParams = NULL;
		return 0;	
	}

	for(int i=0;i<fieldNum;++i)
	{
		memcpy((char*)&length+m_offSet,(char *)request+offSet,TRANS_FIELDS_LENTH_BYTES);
		offSet += TRANS_FIELDS_LENTH_BYTES;
		if (length > 0)
		{
			pInputParams->m_pValues[i] = malloc(length);
			memset(pInputParams->m_pValues[i],0,length);
			memcpy(pInputParams->m_pValues[i],(char *)request+offSet,length);
		}
		else
		{
			pInputParams->m_pValues[i] = NULL;	
		}
		pInputParams->m_iValueLenths[i] = length;
		offSet += length;
	}	

	return fieldNum;
}

//通用的解析,数据集的串,格式
/*
recordNum
record1|reord1Length(fileldNum,length(field1)field1,length(field2)field2,length(field3)field3)
record2|reord2Length(fileldNum,length(field1)field1,length(field2)field2,length(field3)field3)
...
*/
int PublicFunc::splitStr(const void *request,vector<InputParams *> &pInputParams)
{
	int length = 0;
	int recordNum = 0;
	int offSet;
	InputParams *p;
	
	if (request == NULL)
	{
		return 0;
	}

	memcpy((char*)&recordNum+m_offSet,(char *)request,TRANS_FIELDS_LENTH_BYTES);
	offSet = TRANS_FIELDS_LENTH_BYTES;

	for(int i=0;i<recordNum;++i)
	{
		memcpy((char*)&length+m_offSet,(char *)request+offSet,TRANS_FIELDS_LENTH_BYTES);
		offSet += TRANS_FIELDS_LENTH_BYTES;
		PublicFunc::splitStr((char *)request+offSet, p);
		pInputParams.push_back(p);
		offSet += length;
	}	

	return recordNum;
}

//通用的解析,数据集的串,格式fileldNum,length(field1)field1,length(field2)field2,length(field3)field3
int PublicFunc::splitStr(const char *inPutStr,vector<string> &outPutStrs)
{
	int length = 0;
	int filedNum = 0;
	int offSet;
	T_NAMEDEF tmp;

	memcpy((char*)&filedNum+m_offSet,(char *)inPutStr,TRANS_FIELDS_LENTH_BYTES);
	offSet = TRANS_FIELDS_LENTH_BYTES;

	for(int i=0;i<filedNum;++i)
	{
		memcpy((char*)&length+m_offSet,(char *)inPutStr+offSet,TRANS_FIELDS_LENTH_BYTES);
		offSet += TRANS_FIELDS_LENTH_BYTES;
		memset(tmp,0,sizeof(tmp));
		memcpy(tmp,(char *)inPutStr+offSet,length);
		outPutStrs.push_back(tmp);
		offSet += length;
	}	

	return filedNum;
}

//功能：大端字节返回true,否则返回false
bool PublicFunc::isBigEndian()
{
	int p=1;
	return (*(char *)&p ==1 ) ? false : true;
}

/*// 专用的解析--给select和deleteRec使用
void PublicFunc::parse1(const void *request,const int iStrLenth)
{
	semicolon;
	int iSemicolonCount = 0;
	int iLastPostion;
	for(int i=0;i<iStrLenth;++i)
	{
		if( memcmp(request[i],FIELD_TOKEN,1) == 0)
		{
			switch(iSemicolonCount)
			{
				case 0:
					memcpy(tableName,request,i);
					break;
				case 1:
					memcpy(indexName,request+iLastPostion,i-iLastPostion);
					break;			
				case 2:
					memcpy(expres,requestiLastPostion,i-iLastPostion);
					break;
				default:
					break;
			}
			iLastPostion = i;	
			++iSemicolonCount;			
		}	
		else if( iSemicolonCount == 3
			     && memcmp(request[i],VALUE_TOKEN,1) == 0
			    )
		{
			comma_end  [iSemicolonCount] = i;
			comma_begin[iSemicolonCount+1] = i+1;		
		}				
	}	
}*/

//通用的解析,数据集的串,格式fileldNum,length(field1)field1,length(field2)field2,length(field3)field3
// add by chenm 2009-7-9 增加pValuesLenth参数,记录server传过来的字段值的长度
int PublicFunc::splitStr(const void *request,void ** &pValues,int * &pValuesLenth)
{
	int length = 0;
	int fieldNum = 0;
	int offSet;
	
	if (request == NULL)
	{
		pValues = NULL;
		return 0;
	}

	memcpy((char*)&fieldNum+m_offSet,(char *)request,TRANS_FIELDS_LENTH_BYTES);
	offSet = TRANS_FIELDS_LENTH_BYTES;
	
	if( fieldNum>0 )
	{
		pValues = new void *[fieldNum];
		pValuesLenth = new int [fieldNum]; // by chenm 2009-7-9 和第一个splitStr的区别所在
	}
	else
	{
		pValues = NULL;
		return 0;	
	}

	for(int i=0;i<fieldNum;++i)
	{
		memcpy((char*)&length+m_offSet,(char *)request+offSet,TRANS_FIELDS_LENTH_BYTES);
		offSet += TRANS_FIELDS_LENTH_BYTES;
		if (length >0 )
		{
			pValues[i] = malloc(length+1);
			memset(pValues[i],0,length+1);
			memcpy(pValues[i],(char *)request+offSet,length);
			
			pValuesLenth[i] = length;                     // by chenm 2009-7-9 和第一个splitStr的区别所在
		}
		else
		{
			pValues[i] = NULL;
		}
		offSet += length;
	}	

	return fieldNum;
}
