// ############################################
// Source file : ELEM.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2009-5-12
// Update      : 2009-5-12
// Copyright(C): chen min, Linkage.
// ############################################

#ifndef ELEM_H_HEADER_INCLUDED_B5F73BB7
#define ELEM_H_HEADER_INCLUDED_B5F73BB7

//#pragma pack (1)

#include "MdbConstDef.h"
// key<-->record 映射关系类 由于keyValue的类型不同 此类需为模板
//##ModelId=4A0819810213
class Elem
{
  public:
	//##ModelId=4A081A6102FD
	Elem();
	void initial();
	
	void clone(const Elem *left,const int &iKeyLenth);

  public:
	//##ModelId=4A08199600BB
	ShmPosition m_pRecordAddrs;  // 指向不重复记录的指针
	ShmPosition m_pNext;         // 指向重复记录链表的指针
	//##ModelId=4A0819A10399
	char m_pKeyValue[1];
};

// 相同key值的单向链表
class ChainElem
{
	public:
		ChainElem()
		{
			this->initial();	
		}
		void initial()
		{
			m_record = NULL_SHMPOS;
			m_next   = NULL_SHMPOS;		
		}
	public:
		ShmPosition m_record;
		ShmPosition m_next;	
};

#endif /* ELEM_H_HEADER_INCLUDED_B5F73BB7 */
