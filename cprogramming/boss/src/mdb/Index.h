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

#ifndef INDEX_H_INCLUDED
#define INDEX_H_INCLUDED

#include "RecordConvert.h"
#include "base/config-all.h"
#include "MdbAddress.h"
#include "MemManager.h"
#include "Mdb_Exception.h"
#include "InputParams.h"


// Index 抽象类
class Index
{
	public:
		Index(const char *cTableName,const char *cIndexName
				,MemManager* memMgr
				,RecordConvert *pRecordConvert )
		{
			m_tableName = cTableName;
			m_indexName = cIndexName;
			m_memMgr    = memMgr;
			m_pRecordConvert = pRecordConvert;
		}
		virtual ~Index();
		
		virtual bool create(const IndexDef &theIndexDef) =0;
		virtual bool drop()                        =0;
		virtual bool truncate()                    =0;
		virtual bool initialization()              =0; //初始化一个索引（attach内存）		
		
		virtual int select(const InputParams *pInputParams   //查询关键字
						,vector<MdbAddress> &result
						,int iRowCount) =0;  

		virtual int select(const char *hashkey               //查询关键字
						,vector<MdbAddress> &result
						,int iRowCount) =0;  

		virtual bool deleteIdx(const char *hashkey       //查询关键字
						,const MdbAddress &theMdbAddress)  =0;  

		virtual bool insert(const char *hashkey       //查询关键字
						,const MdbAddress &theMdbAddress)  =0;
		virtual bool getHashPercent(int& iHashSize
		                           ,int& iUsedHash
		                           ,int& iRecordCount
		                           ,map<int,int>& mHashCal)=0;//Hash桶内元素个数，该类型Hash桶的个数		
		
		IndexDesc *getIndexDesc();
		virtual bool reIndex(Index * r_destIndex) = 0;//重新对	r_descIndex 索引
		
		// add by chenm 2009-6-3 不等值查询
		virtual int selectRange(const InputParams *pGtParams   //查询关键字
								,const InputParams *pLtParams
								,const int iRangeTag // 标示[]/[)/()/(]运算
								,vector<MdbAddress> &result	
								,const int iRowCount) {return 0;}
		// over 2009-6-3 
	protected:
		// 拼出多字段组成的索引键值 调用次数多 内联实现
		inline const char * getIndexKey(void * pRecord)
		{
			char columnValue[MAX_COLUMN_LEN];
			int  iColumnLenth,iOffSize=0 ;
			
			memset(m_indexKey,0,sizeof(m_indexKey));
			
			m_pRecordConvert->setRecordValue(pRecord);
			for(int i=0;i<(m_indexDesc->m_indexDef).m_columnNum;++i)
			{
				iColumnLenth = 0;
				memset(columnValue,0,sizeof(columnValue));
				m_pRecordConvert->getColumnValue( (m_indexDesc->m_indexDef).m_columnList[i]
				                                  ,columnValue,iColumnLenth);
				memcpy(m_indexKey+iOffSize,columnValue,iColumnLenth);
				iOffSize += iColumnLenth;
			}	
			return  m_indexKey;
		}
		
		inline void getSearchKey(const InputParams *pInputParams)
		{
			int iOffSize = 0;
			
			if( pInputParams->m_iNum == (m_indexDesc->m_indexDef).m_columnNum )	
			{
				memset(m_cSearchKey,0,sizeof(m_cSearchKey));
				
				for(int i=0;i<pInputParams->m_iNum;++i)
				{
					memcpy(m_cSearchKey+iOffSize,pInputParams->m_pValues[i],pInputParams->m_iValueLenths[i]);
					iOffSize += *m_iIndexColumnLenth[i];	
				}	
			}
			else
			{
				throw Mdb_Exception (__FILE__, __LINE__, "传入的索引字段数：[%s] 和索引描述信息不符 ", pInputParams->m_iNum );
			}
		}
		
		void getHashKeyLenth();
	
	protected:
		MemManager    *m_memMgr;
		string         m_tableName;
		string         m_indexName;
		IndexDesc     *m_indexDesc;
		RecordConvert *m_pRecordConvert;
		char           m_indexKey[MAX_COLUMN_LEN];
		int            m_iHashKeyLenth;
		// 传入查找索引的字符串
		char           m_cSearchKey[MAX_COLUMN_LEN];
		int           **m_iIndexColumnLenth;

};

#endif //INDEX_H_INCLUDED
