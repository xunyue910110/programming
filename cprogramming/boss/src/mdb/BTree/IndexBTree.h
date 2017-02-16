// ############################################
// Source file : IndexBTree.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2009-5-12
// Update      : 2009-5-12
// Copyright(C): chen min, Linkage.
// ############################################

#ifndef INDEXBTTREE_H_HEADER_INCLUDED_B5F77DDC
#define INDEXBTTREE_H_HEADER_INCLUDED_B5F77DDC

#include "Node.h"
#include "Index.h"

const int NODE_ELEM_NUM=256;
const string NODE_TABLE_PREFIX = "BTREE_NODE_";
const string CHAIN_ELEM_TABLE_PREFIX = "BTREE_CHAINELEM_";
//##ModelId=480861C9008C
class IndexBTree :public Index
{
	public:
		IndexBTree(const char *cTableName,const char *cIndexName,MemManager* memMgr,RecordConvert *pRecordConvert);
		virtual ~IndexBTree(){};
		
		virtual bool create(const IndexDef &theIndexDef) ;   // 创建一个新索引
		virtual bool drop()                        ;
		virtual bool truncate()                    ;
		virtual bool initialization()              ; //初始化一个索引（attach内存）		
		
		virtual int select(const InputParams *pInputParams   //查询关键字
						,vector<MdbAddress> &result
						,int iRowCount) ;  

		virtual int select(const char *hashkey               //查询关键字
						,vector<MdbAddress> &result
						,int iRowCount) {return 0;};  

		virtual bool deleteIdx(const char *key       //查询关键字
						,const MdbAddress &theMdbAddress);
  
		virtual bool insert(const char *key       //查询关键字
						,const MdbAddress &theMdbAddress)  ;

		virtual bool getHashPercent(int& iHashSize
		                           ,int& iUsedHash
		                           ,int& iRecordCount
		                           ,map<int,int>& mHashCal)
		{
			return true;
		}
		
		virtual bool reIndex(Index * r_destIndex){return true;}//重新对	r_descIndex 索引
		
		virtual int selectRange(const InputParams *pGtParams   //查询关键字
								,const InputParams *pLtParams								
								,const int iRangeTag // 标示[]/[)/()/(]运算								
								,vector<MdbAddress> &results	
								,const int iRowCount) ; 
		
		// 申请BTree节点/ChainElem空间
		MdbAddress allocateMem(int iMemType);
		// 释放BTree节点/ChainElem空间
		void freeMem(int iMemType,ShmPosition &posi);
		void freeMem(int iMemType,char *phAddr);
		void getPhAddr(MdbAddress &mdbAddr);
		char * getPhAddr(const ShmPosition &shmPosi);
		void getShmPos(MdbAddress &mdbAddr);
		
		void dump();
					
	private:
		int selectALL(vector<MdbAddress> &results);
		int selectLimited(vector<MdbAddress> &results,int iRowCount);
		int getKeyLenth(const IndexDef &theIndexDef);

	private:
		//##ModelId=4A080A070261
		Node *m_pRootNode;
		
		TableDesc *m_nodeTableDesc;
		TableDesc *m_chainElemTableDesc;
};



#endif /* INDEXBTTREE_H_HEADER_INCLUDED_B5F77DDC */
