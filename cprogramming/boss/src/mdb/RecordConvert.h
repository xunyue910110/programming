#ifndef _RecordConvert_H_
#define _RecordConvert_H_


#include "PublicInfo.h"
#include "TableDefParam.h"
#include "TableDescript.h"
#include "InputParams.h"
#include "Mdb_Exception.h"

/**
* RecordConvert:基于表描述信息,对表记录内容的分析,提供记录内容中列值信息的设置和获取功能。
* 	RecordConvert:		构造函数
*	~RecordConvert:		析构函数
*	setRecordValue:		设置记录值
*	getRecordValue:		获取记录值
*	getColumnsValue:	根据字段名称集,获取记录值
*	getPkValue:			获取表主键值
*	getPkColumnPos:		根据主键中字段位置获取表字段位置
*	setColumnValue:		根据列名称设置指定列的列值
*	getColumnValue:		根据列名称获取指定列的列值
*/
typedef map<MyString, int> COLOMN_POS_POOL;
typedef COLOMN_POS_POOL::iterator COLOMN_POS_POOL_ITR;

class RecordConvert
{
	public:

		/**
		* RecordConvert:构造函数
		* @param	tableDesc	表描述符指针
		* @return
		*/
		RecordConvert(TableDesc* tableDesc);

		/**
		* ~RecordConvert:析构函数
		* @param
		* @return
		*/		
		~RecordConvert();

		/**
		* setRecordValue:设置记录值
		* @param	value		记录值指针
		* @return
		*/		
		void setRecordValue(void* value);
		
		/**
		* getRecordValue:获取记录值
		* @param	value		记录值指针
		* @param	len			记录值长度
		* @return
		*/
		void getRecordValue(void* &value, int &len);
		
		/**
		* getColumnsValue:根据字段名称集,获取记录值
		* @param	columnNames	列名称集
		* @param	value		记录值指针
		* @param	len			记录值长度
		* @return
		*/
		void getColumnsValue(const vector<string>& columnNames, void* value, int& len);

		/**
		* getPkValue:获取表主键值
		* @param	value		记录值指针
		* @param	len			记录值长度
		* @return
		*/
		void getPkValue(void* value, int& len);

		/**
		* getPkColumnPos:根据主键中字段位置获取表字段位置
		* @param	pkPos		主键中字段位置
		* @param	colPos		表字段位置
		* @return
		*/
		void getPkColumnPos(const int& pkPos, int& colPos);
		
		/**
		* setColumnValue:根据列名称设置指定列的列值
		* @param	columnName		列名称
		* @param	value			指定列值
		* @return
		*/		
		template<class T>
		void setColumnValue(const char* columnName, const T* value)
		{
			MyString s;
			
			strcpy(s.m_pStr,columnName);
			COLOMN_POS_POOL_ITR itr = m_columnPos.find(s);
			if (itr != m_columnPos.end())
			{
				// add by chenm 2008-6-19 17:01:43 如果是字符串
				if( (m_tableDesc->m_tableDef).m_columnList[itr->second].m_type == VAR_TYPE_NUMSTR 
					|| (m_tableDesc->m_tableDef).m_columnList[itr->second].m_type ==VAR_TYPE_VSTR )
				{
					memset((char *)m_value+(m_tableDesc->m_tableDef).m_columnList[itr->second].m_offSet, 0x0, (m_tableDesc->m_tableDef).m_columnList[itr->second].m_len);
					memcpy((char *)m_value+(m_tableDesc->m_tableDef).m_columnList[itr->second].m_offSet, value, strlen( (char *)value ) );
				}
				else
				{
					memcpy((char *)m_value+(m_tableDesc->m_tableDef).m_columnList[itr->second].m_offSet, value, (m_tableDesc->m_tableDef).m_columnList[itr->second].m_len);
				}
			}
		}

		/**
		* setColumnsValue:根据列名称集合设置指定列集合的列值
		* @param	sFieldNames		列名称集合
		* @param	pUpdateValues	指定列值集合
		* @return
		*/		
		void setColumnsValue(vector<string> &sFieldNames, const InputParams *pUpdateValues)
		{
			if( sFieldNames.size() > pUpdateValues->m_iNum )
			{
				throw Mdb_Exception(__FILE__, __LINE__, "update的字段名称个数和值个数不符!");	
			}
			for(int i=0;i<sFieldNames.size();++i)
			{
				this->setColumnValue(sFieldNames[i].c_str(),pUpdateValues->m_pValues[i]);
			}
		}

		/**
		* assembleRec:根据传入的列值，组合成一条内存存放的记录
		* @param	pValues		列值
		* @param	
		* @return
		*/		
		void * assembleRec(const void **pValues)
		{
			void *pDesValue;
			//pDesValue = new char[m_tableDesc->m_pageInfo.m_slotSize];
			pDesValue = malloc(m_tableDesc->m_pageInfo.m_slotSize);
			
			memset(pDesValue,0,m_tableDesc->m_pageInfo.m_slotSize);
			for(int i=0;i<(m_tableDesc->m_tableDef).m_columnNum;++i)
			{
				if(pValues[i]!=NULL)
				{
					// add by chenm 2008-6-19 17:01:43 如果是字符串
					if( (m_tableDesc->m_tableDef).m_columnList[i].m_type == VAR_TYPE_NUMSTR 
						|| (m_tableDesc->m_tableDef).m_columnList[i].m_type ==VAR_TYPE_VSTR )
					{
						memcpy((char *)pDesValue+(m_tableDesc->m_tableDef).m_columnList[i].m_offSet
					        , pValues[i]
					        , strlen( (char *)pValues[i] ));	
					}
					else
					{
						memcpy((char *)pDesValue+(m_tableDesc->m_tableDef).m_columnList[i].m_offSet
					        , pValues[i]
					        , (m_tableDesc->m_tableDef).m_columnList[i].m_len);	
					}
				}
				else
				{
					//delete [] pDesValue;
					free(pDesValue);
					throw Mdb_Exception(__FILE__, __LINE__, "值个数和表定义字段数不符!");
				}
			}
			
			return pDesValue;
		}

		/**
		* assembleRecByColumnName:根据传入的列值，组合成一条内存存放的记录
		* @param	pValues		列值
		* @param	
		* @return
		*/		
		void * assembleRecByColumnName(const vector<string> &cFieldNames,const void **pValues)
		{
			void *pDesValue;
			//pDesValue = new char[m_tableDesc->m_pageInfo.m_slotSize];
			pDesValue = malloc(m_tableDesc->m_pageInfo.m_slotSize);
			
			memset(pDesValue,0,m_tableDesc->m_pageInfo.m_slotSize);
			
			bool isMatched = false;
			for(int j=0;j<cFieldNames.size();++j)
			{
				for(int i=0;i<(m_tableDesc->m_tableDef).m_columnNum;++i)
				{
					// 字段名匹配
					if( strcasecmp(cFieldNames[j].c_str(),(m_tableDesc->m_tableDef).m_columnList[i].m_name)==0 )
					{
						if(pValues[j]!=NULL)
						{
							// add by chenm 2008-6-19 17:01:43 如果是字符串
							if( (m_tableDesc->m_tableDef).m_columnList[i].m_type == VAR_TYPE_NUMSTR 
								|| (m_tableDesc->m_tableDef).m_columnList[i].m_type ==VAR_TYPE_VSTR )
							{
								memcpy((char *)pDesValue+(m_tableDesc->m_tableDef).m_columnList[i].m_offSet
							        , pValues[j]
							        , strlen( (char *)pValues[j] ));	
							}
							else
							{
								memcpy((char *)pDesValue+(m_tableDesc->m_tableDef).m_columnList[i].m_offSet
							        , pValues[j]
							        , (m_tableDesc->m_tableDef).m_columnList[i].m_len);	
							}
						}
						else
						{
							free(pDesValue);
							throw Mdb_Exception(__FILE__, __LINE__, "值个数和表定义字段数不符!");
						}
						isMatched = true;
						break;
					}
				}
				if( isMatched==false )
				{
					free(pDesValue);
					throw Mdb_Exception(__FILE__, __LINE__, "传入的字段名:%s 在表:%s 里没有定义!",cFieldNames[j].c_str(),m_tableDesc->m_tableDef.m_tableName);
				}
				else
					isMatched=false;
			}
			
			return pDesValue;
		}
		
		/**
		* getColumnValue:根据列名称获取指定列的列值
		* @param	columnName		列名称
		* @param	value			列值
		* @return
		*/
		template<class T>
		bool getColumnValue(const char* columnName, T* value)
		{
			MyString s;
			
			strcpy(s.m_pStr,columnName);
			COLOMN_POS_POOL_ITR itr = m_columnPos.find(s);
			if (itr != m_columnPos.end())
			{
				memcpy(value, (char *)m_value+(m_tableDesc->m_tableDef).m_columnList[itr->second].m_offSet, (m_tableDesc->m_tableDef).m_columnList[itr->second].m_len);
				return true;
			}
			else
			{
				//throw Mdb_Exception(__FILE__, __LINE__, "无效的字段名:%s",columnName);
				return false;
			}
		}
		
		/**
		 *多返回一个列长度lenth
		 */
		template<class T>
		bool getColumnValue(const char* columnName, T* value, int& lenth)
		{
			MyString s;
			
			strcpy(s.m_pStr,columnName);
			COLOMN_POS_POOL_ITR itr = m_columnPos.find(s);
			if (itr != m_columnPos.end())
			{
				memcpy(value, (char *)m_value+(m_tableDesc->m_tableDef).m_columnList[itr->second].m_offSet, (m_tableDesc->m_tableDef).m_columnList[itr->second].m_len);
			    lenth = (m_tableDesc->m_tableDef).m_columnList[itr->second].m_len;
			    return true;
			}
			else
			{
				//throw Mdb_Exception(__FILE__, __LINE__, "无效的字段名:%s",columnName);
				return false;
			}
		}

		/**
		 *按照字段顺序,返回字段值
		 */
		template<class T>
		bool getColumnValue(const int iSequnce, T* value, int& lenth)
		{
			if (iSequnce < (m_tableDesc->m_tableDef).m_columnNum)
			{
				memcpy(value, (char *)m_value+(m_tableDesc->m_tableDef).m_columnList[iSequnce].m_offSet, (m_tableDesc->m_tableDef).m_columnList[iSequnce].m_len);
			    lenth = (m_tableDesc->m_tableDef).m_columnList[iSequnce].m_len;
			    return true;
			}
			else
			{
				//throw Mdb_Exception(__FILE__, __LINE__, "字段序号[%d]错误,正确范围[0 ~ %d]",iSequnce,(m_tableDesc->m_tableDef).m_columnNum - 1);
				return false;	
			}
		}
		
	private:
		/**
		* RecordConvert:构造函数
		* @param	recordConvert	RecordConvert类的实例
		* @return
		*/		
		RecordConvert(const RecordConvert& recordConvert);


		/**
		* =:类赋值操作函数
		* @param	recordConvert	RecordConvert类的实例
		* @return	RecordConvert类的实例
		*/
		RecordConvert& operator=(const RecordConvert& recordConvert);


	private:
		TableDesc			*m_tableDesc;			//表描述定义指针
		COLOMN_POS_POOL   	m_columnPos;			//表列名称和列位置对应关系
		vector<string>		m_pkColumnNames;		//表主键列名称集
		void				*m_value;				//表记录地址
};


#endif

