// ############################################
// Source file : IndexHash.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-4-23
// Update      : 2008-4-23
// Copyright(C): chen min, Linkage.
// ############################################

#ifndef INDEXHASH_H_INCLUDED
#define INDEXHASH_H_INCLUDED

#include <vector>

#include "Index.h"
#include "Bucket.h"

// IndexHash 基于hash表的索引结构实现
class IndexHash : public Index
{
	public:
		IndexHash(const char *cTableName,const char *cIndexName,MemManager* memMgr,RecordConvert *pRecordConvert);
		virtual ~IndexHash(){};
		
		virtual bool create(const IndexDef &theIndexDef) ;   // 创建一个新索引
		virtual bool drop()                        ;
		virtual bool truncate()                    ;
		virtual bool initialization()              ; //初始化一个索引（attach内存）		
		
		virtual int select(const InputParams *pInputParams   //查询关键字
						,vector<MdbAddress> &result
						,int iRowCount) ;  

		virtual int select(const char *hashkey               //查询关键字
						,vector<MdbAddress> &result
						,int iRowCount) ;  

		virtual bool deleteIdx(const char *hashkey       //查询关键字
						,const MdbAddress &theMdbAddress);
  
		virtual bool insert(const char *hashkey       //查询关键字
						,const MdbAddress &theMdbAddress)  ;
		virtual bool getHashPercent(int& iHashSize
		                           ,int& iUsedHash
		                           ,int& iRecordCount
		                           ,map<int,int>& mHashCal);//Hash桶内元素个数，该类型Hash桶的个数
		virtual bool reIndex(Index * r_destIndex);//重新对	r_descIndex 索引				
	private:
		int selectALL(vector<MdbAddress> &results);
		int selectLimited(vector<MdbAddress> &results,int iRowCount);
		int selectRowByRow(vector<MdbAddress> &results,int iRowCount);

	private:
		int         m_size;        //hash表分配大小
		ShmPosition *m_hashTable;   //共享内存hash索引区的首地址
		int         m_iHashPosi;    // by chenm 2009-5-18 23:07:02 逐条扫描的计数器
};

#endif //INDEXHASH_H_INCLUDED
