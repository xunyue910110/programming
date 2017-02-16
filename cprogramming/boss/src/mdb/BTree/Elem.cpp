// ############################################
// Source file : ELem.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2009-5-12
// Update      : 2009-5-12
// Copyright(C): chen min, Linkage.
// ############################################

#include "Elem.h"


//##ModelId=4A081A6102FD
Elem::Elem()
{
	initial();
}

void Elem::initial()
{
	m_pRecordAddrs = NULL_SHMPOS;
	m_pNext        = NULL_SHMPOS; 
	return;
}

void Elem::clone(const Elem *left,const int &iKeyLenth)
{
    m_pRecordAddrs = left->m_pRecordAddrs;
    m_pNext        = left->m_pNext;
    memcpy(m_pKeyValue,left->m_pKeyValue,iKeyLenth);
    return ;
}


