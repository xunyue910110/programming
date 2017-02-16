// ############################################
// Source file : TransDataMgr.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-5-4
// Update      : 2008-5-4
// Copyright(C): chen min, Linkage.
// ############################################


#include "TransDataMgr.h"

TransDataMgr::TransDataMgr()
{
	this->clearTransData();	
}

TransDataMgr::~TransDataMgr()
{
	clearTransData();
}

bool TransDataMgr::clearTransData()
{
	TRANSDATA_POOL_ITR itr;
	for(itr=m_transDataPool.begin();itr!=m_transDataPool.end();++itr)
	{
		if( itr->second != NULL)
			delete itr->second;	
	}		
	m_transDataPool.clear();
	m_transDataVector.clear();
	
	return true;	
}

TRRANSDATA_VECTOR & TransDataMgr::getTransDataPool()
{
	return this->m_transDataVector;	
}

bool TransDataMgr::insertTransData(TransData *pTransData)
{
	m_transDataPool.insert( TRANSDATA_POOL::value_type( pTransData->m_addr.m_addr,pTransData) );
	m_transDataVector.push_back(pTransData);
	
	return true;
}

// 根据输入TransData中的记录地址,查找是否缓存中有匹配
TransData *TransDataMgr::matchTransData(TransData *pTransData)
{
	TRANSDATA_POOL_ITR itr;
	
	itr = m_transDataPool.find( pTransData->m_addr.m_addr );

	if(itr != m_transDataPool.end())
	{
		// 对于insert操作,pTransData结构中的m_addr是空的
		if( pTransData->m_addr.m_addr == NULL )
		{
			while( (itr->first == NULL)
			       &&(itr != m_transDataPool.end())
			     )
			{
				if( strcmp((const char *)pTransData->m_oldRecordValue,(const char *)itr->second->m_oldRecordValue) == 0
					|| strcmp((const char *)pTransData->m_newRecordValue,(const char *)itr->second->m_newRecordValue) == 0 )
				{
					break;	
				}
				++itr;	
			}
			
			if(itr == m_transDataPool.end())
				return NULL;
		}	
		return 	itr->second;
	}
	else
	{
		return NULL;	
	}	
}
