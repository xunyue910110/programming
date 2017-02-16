// ############################################
// Source file : Index.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-4-23
// Update      : 2008-4-23
// Copyright(C): chen min, Linkage.
// ############################################

// 根据indexdef获取索引字段值的长度
#include "Index.h"
void Index::getHashKeyLenth()
{
	char *pColumnName; 
	TableDesc *pTableDesc;
	
	m_iHashKeyLenth = 0;
	m_memMgr->getTableDescByName(m_tableName.c_str(), pTableDesc);
	
	// 如果(m_indexDesc->m_indexDef).m_columnNum小于0,则认为索引创建有问题,什么也不做,直接返回
	if((m_indexDesc->m_indexDef).m_columnNum<0)
		return;
		
	m_iIndexColumnLenth = new int*[(m_indexDesc->m_indexDef).m_columnNum];
	for(int i=0;i<(m_indexDesc->m_indexDef).m_columnNum; ++i)
	{
		// indexDef里定义的索引字段名
		pColumnName = (m_indexDesc->m_indexDef).m_columnList[i];
		
		for(int j=0;j<(pTableDesc->m_tableDef).m_columnNum;++j)
		{
			if( strcasecmp( (pTableDesc->m_tableDef).m_columnList[j].m_name,pColumnName ) == 0 )
			{
				m_iHashKeyLenth += (pTableDesc->m_tableDef).m_columnList[j].m_len;
				// 记录表描述符中,字段长度的地址,以防万一别的进程修改表结构
				m_iIndexColumnLenth[i] = &((pTableDesc->m_tableDef).m_columnList[j].m_len); 
				break;
			}
		}	
	}
}	

Index::~Index()
{
 	delete [] m_iIndexColumnLenth;
}

IndexDesc * Index::getIndexDesc()
{
	return m_indexDesc;	
}
