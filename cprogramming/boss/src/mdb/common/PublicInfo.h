 #ifndef _PUBLICINFO_H_
#define _PUBLICINFO_H_

#include "config-all.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>

USING_NAMESPACE(std)
#include "MdbAddress.h"
#include "Mdb_Exception.h"


//Session和Table互相转送的数据类
class Index;

class TransData
{
	public:
		TransData() :  m_recLength(0), m_operType('3')
		{
			m_oldRecordValue = NULL;
			m_newRecordValue = NULL;
		}
		TransData(const TransData& transData) 
		{
			m_addr = transData.m_addr;
			m_recLength = transData.m_recLength;
			m_oldRecordValue = transData.m_oldRecordValue;
			m_newRecordValue = transData.m_newRecordValue;
			m_operType = transData.m_operType;
		}
		~TransData() 
		{
			if( m_oldRecordValue != NULL )
				free(m_oldRecordValue);
			if( m_newRecordValue != NULL )
				free( m_newRecordValue);
		}

		inline TransData& operator=(const TransData& transData)
		{
			if (this != &transData)
			{
				m_addr = transData.m_addr;
				m_recLength = transData.m_recLength;
				m_oldRecordValue = transData.m_oldRecordValue;
				m_newRecordValue = transData.m_newRecordValue;
				m_operType = transData.m_operType;
			}
			return *this;
		}
				
		inline void cloneRecordValue()
		{
			//这边初始化,由外面调用的Session负责delete清空
			//m_newRecordValue = new char[m_recLength];
			m_newRecordValue = (char *)calloc(m_recLength,sizeof(char));
			if (m_oldRecordValue != NULL)
			{
				memcpy(m_newRecordValue, m_oldRecordValue, m_recLength);
			}
			else
			{
				memset(m_newRecordValue, 0x0, m_recLength);
			}
		}

		
	public:
		MdbAddress		m_addr;					//记录地址
		int				m_recLength;			//记录长度
		void			*m_oldRecordValue;		//原记录
		void			*m_newRecordValue;		//新记录
		char			m_operType;				//操作类型  '0':删除 '1':插入 '2':更新 '3':未更改
};

//Table进行更新操作时对Index所做的修改日志类
class IndexChangeLog
{
	public:
		IndexChangeLog() : m_undoFlag(0), m_operType(0)
		{
			m_index = NULL;
			m_oldValue = NULL;
			m_newValue = NULL;
		}

		IndexChangeLog(const IndexChangeLog& indexChangeLog)
		{
			m_undoFlag = indexChangeLog.m_undoFlag;
			m_operType = indexChangeLog.m_operType;
			m_index = indexChangeLog.m_index;
			m_addr = indexChangeLog.m_addr;
			m_oldValue = indexChangeLog.m_oldValue;
			m_newValue = indexChangeLog.m_newValue;
		}
		~IndexChangeLog() {}
		
		inline IndexChangeLog& operator=(const IndexChangeLog& indexChangeLog)
		{
			if (this != &indexChangeLog)
			{
				m_undoFlag = indexChangeLog.m_undoFlag;
				m_operType = indexChangeLog.m_operType;
				m_index = indexChangeLog.m_index;
				m_addr = indexChangeLog.m_addr;
				m_oldValue = indexChangeLog.m_oldValue;
				m_newValue = indexChangeLog.m_newValue;
			}
			return *this;
		}	
		
	public:
		unsigned char	m_undoFlag;							//索引重做标志 0:Redo	1:Undo
		unsigned char	m_operType;							//索引操作类型 0:delete 1:insert
		//int             m_operType;							//索引操作类型 0:delete 1:insert
		Index			*m_index;							//索引实例地址
		MdbAddress		m_addr;								//记录地址
		void			*m_oldValue;						//记录中索引信息原值
		void			*m_newValue;						//记录中索引信息新值
};

class MyString
{
	public:
		T_NAMEDEF m_pStr;
		/*
		friend bool operator < (const MyString &left,const MyString &right)
		{
			return (strcmp(left.m_pStr,right.m_pStr)<0);	
		}
    */
    //modified by gaojf 2008-9-22 21:53
		friend bool operator < (const MyString &left,const MyString &right)
		{
			return (strcasecmp(left.m_pStr,right.m_pStr)<0);	
		}
		friend bool operator == (const MyString &left,const MyString &right)
		{
			return (strcasecmp(left.m_pStr,right.m_pStr)==0);	
		}
};

typedef map<MyString, int> COLOMN_POS_MAP;
typedef COLOMN_POS_MAP::iterator COLOMN_POS_MAP_ITR;

// 用来同一altibase、TT等商用数据库和自主MDB的对外接口
// 统一使用使用IndexDef来保留相关信息
class TableDef;
class IndexDef; 
	
typedef map<MyString,TableDef *> TABLE_DEF_MAP;
typedef TABLE_DEF_MAP::iterator TABLE_DEF_MAP_ITR;
	
typedef map<MyString,COLOMN_POS_MAP> TABLE_COLOMN_POS_MAP;
typedef TABLE_COLOMN_POS_MAP::iterator TABLE_COLOMN_POS_MAP_ITR;

typedef map<MyString,IndexDef *> INDEX_DEF_MAP;
typedef INDEX_DEF_MAP::iterator INDEX_DEF_MAP_ITR;
#endif
