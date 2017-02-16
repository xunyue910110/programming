// ############################################
// Source file : InputParam.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-6-4 16:35:16
// Update      : 2008-6-4 16:35:21
// Copyright(C): chen min, Linkage.
// ############################################

#include "InputParams.h"
#include "Mdb_Exception.h"
#include <memory.h>

//##ModelId=48464B970251
void InputParams::setValue(const COLUMN_VAR_TYPE type,void * pValue)
{
    if(m_iSequnce > m_iNum-1)
		throw Mdb_Exception(__FILE__, __LINE__, "缓存已满!");
		
		
	//初始化的意思就是分配内存空间存储对应数据类型的长度
	// 调用者第一轮参数赋值,需要初始化;调用reset函数后,就认为是客户端第二轮赋值,那就不需要再初始化了,而是直接赋值
	if( !m_isReseted )
	{
		m_varTypes[m_iSequnce] = type;
		switch(type)
		{
			case VAR_TYPE_INT2:
				m_iValueLenths[m_iSequnce] = 2;
				break;
			case VAR_TYPE_INT:
				m_iValueLenths[m_iSequnce] = sizeof(int);
				break;
			case VAR_TYPE_DATE:
				m_iValueLenths[m_iSequnce] = 2*sizeof(int);
				break;
			case VAR_TYPE_LONG:
				m_iValueLenths[m_iSequnce] = sizeof(long);
				break;
			case VAR_TYPE_REAL:
				m_iValueLenths[m_iSequnce] = sizeof(float);
				break;
			case VAR_TYPE_NUMSTR:
			case VAR_TYPE_VSTR:
				m_iValueLenths[m_iSequnce] = strlen( (char *)pValue )+1;			
				break;
			default:
				throw Mdb_Exception(__FILE__, __LINE__, "未知数据类型!");				
		}
		// 开辟空间

		m_pValues[m_iSequnce] = malloc(m_iValueLenths[m_iSequnce]);
	}
	else
	{
		
		//这种情况只会出现在下面这两种变长的数据类型上，定长的只要分配一次内存空间就没问题了
		// 当reset后,再次set的字符串比第一次的长,那么需要重新申请大的空间来存储
		if( (type ==VAR_TYPE_NUMSTR || type==VAR_TYPE_VSTR )
			&& m_iValueLenths[m_iSequnce]<(strlen( (char *)pValue )+1) 
			)
		{
			m_iValueLenths[m_iSequnce] = strlen( (char *)pValue )+1;
			free(m_pValues[m_iSequnce]);
			m_pValues[m_iSequnce] = malloc(m_iValueLenths[m_iSequnce]);
			
		}
	}
	
	//把分配过的用来存储相应数据类型的空间清0
	memset(m_pValues[m_iSequnce],0,m_iValueLenths[m_iSequnce]);
	
	//把分配过的用来存储相应数据类型的空间置上对应的数据
	memcpy(m_pValues[m_iSequnce],pValue,m_iValueLenths[m_iSequnce]);
	
	++m_iSequnce;
}


void InputParams::setValue(const COLUMN_VAR_TYPE type, long l)
{
    switch(type)
	{
		case VAR_TYPE_LONG:
		    
			setValue(VAR_TYPE_LONG, &l);
			break;
		case VAR_TYPE_REAL:
		case VAR_TYPE_INT2:
		case VAR_TYPE_INT:
		case VAR_TYPE_DATE:
		case VAR_TYPE_NUMSTR:
		case VAR_TYPE_VSTR:
		default:
			throw Mdb_Exception(__FILE__, __LINE__, "setValue类型不匹配!");				
	}
}


//##ModelId=48464BE703B9
void InputParams::reset()
{
	m_iSequnce = 0;	
	m_isReseted = true;
	
	for(int i=0;i<m_iNum;++i)
		memset(m_pValues[i],0,m_iValueLenths[i]);
	
}

//##ModelId=48464CB7008C
InputParams::InputParams(int iNum)
{
	m_iNum = iNum;
	m_iSequnce = 0;
	m_isReseted = false;
	
	//用来指向将要存储对应数据类型的数据地址区的指针
	//m_pValues = (void **)malloc( sizeof(void *) * m_iNum);//rem by chenzm@2008-12-25
	m_pValues = (void **)calloc( m_iNum,sizeof(void *));    //modify by chenzm@2008-12-25
	m_iValueLenths   = new int[m_iNum];
	m_varTypes       = new COLUMN_VAR_TYPE[m_iNum];
}

//##ModelId=48464CBE033C
InputParams::~InputParams()
{
	delete [] m_iValueLenths;
	delete [] m_varTypes;
	
	for(int i=0;i<m_iNum;++i)
	{
		if( m_pValues[i] != NULL )
			free(m_pValues[i]);	
	}
	
	free( m_pValues);
}

InputParams & InputParams::operator = (const InputParams &input)
{
	if( this->m_iNum != input.m_iNum)
		throw Mdb_Exception(__FILE__, __LINE__, "缓存数目不符,不能赋值!");
	
	for(int i=0;i<input.m_iNum;++i)	
	{
		this->setValue(input.m_varTypes[i],input.m_pValues[i]);	
	}
	return *this;
}
