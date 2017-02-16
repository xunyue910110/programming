// ############################################
// Source file : Node.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2009-5-12
// Update      : 2009-5-12
// Copyright(C): chen min, Linkage.
// ############################################

#ifndef NODE_H_HEADER_INCLUDED_B5F710A6
#define NODE_H_HEADER_INCLUDED_B5F710A6

//#pragma pack (1)
#include "base/config-all.h"

#include "Elem.h"
#include "Mdb_Exception.h"
#include <map>


// BTree的节点（root、mid、leaf）
//##ModelId=4A080CCE006D

typedef map<void *,int> ELEM_POS_IN_NODE_MAP; // 用于记录search时,elem在Node中的位置
typedef map<void *,int>::const_iterator ELEM_POS_IN_NODE_MAP_ITR;

const int NOT_IN_ELEMCHAIN=0;	

const int KEY_REPEATED=0;
const int KEY_NOT_REPEATED=1;

const int MEM_TYPE_NODE=0;
const int MEM_TYPE_CHAIN_ELEM=1;

class IndexBTree;
class MdbAddress;
class Node
{
  public:
  	void initial(const int iMaxNum,const int iKeyLenth);
  	
	//##ModelId=4A080CF2002E
	Node * insert(const char *key
					,const MdbAddress &theMdbAddress
					,const ELEM_POS_IN_NODE_MAP &elemPosiInNodes
					,const int isRepeatedKey
					,IndexBTree * pIndexBTree);

	//##ModelId=4A080CF60232
	bool deleteElem(int iElemPosi
						,int iElemChainPosi
						,ELEM_POS_IN_NODE_MAP &elemPosiInNodes
						,IndexBTree * pIndexBTree);

	//##ModelId=4A080D1002EE
	bool search(const char *key
					,vector<MdbAddress> &results
					,Node *&pLeafNode
					,ELEM_POS_IN_NODE_MAP &elemPosiInNodes
					,IndexBTree * pIndexBTree);

	// 设置节点为叶子节点
	void setNodeTypeLeaf();
	
	void setBrotherNode(const ShmPosition &pNode);
	void setParentNode(const ShmPosition &pNode); 
	
	void setElem(const Elem *pElem);
	void setKeyLenth(const int iKeyLenth);
	// 父节点分裂,需要向子节点注册m_pParentNode
	void reg2child(IndexBTree * pIndexBTree,MdbAddress &parentMdbAddr);
	
	bool isLeafNode();
	
	inline Elem * getElemt(int iSequnce);
	
	void getChainPosi(const MdbAddress &theMdbAddress
						,const int &iElemPosi
						,IndexBTree * pIndexBTree
						,int &iElemChainPosi);
						
	int getElemPosiInNode(const ELEM_POS_IN_NODE_MAP &elemPosiInNodes
								,void *p
								,void *pChild = NULL
								,IndexBTree * pIndexBTree=NULL);
								
	// 在 m_elemts 找到第一个不大于key的 Elem
	//##ModelId=4A08D801008C
	bool lower_bound(const char *key
					,const int iRangeTag // 标示[]/[)/()/(]运算	
					,Node *&pLeafNode
					,int &iElemtPosi
					,IndexBTree * pIndexBTree);

	// 在 m_elemts 找到第一个不小于等于key的 Elem
	//##ModelId=4A08D91B003E
	bool upper_bound(const char *key
					,const int iRangeTag // 标示[]/[)/()/(]运算	
					,Node *&pLeafNode
					,int &iElemtPosi
					,IndexBTree * pIndexBTree);		
					
	const ShmPosition & getBrotherNode();
	
	const int getCurrentNum();
	
	void  dump(IndexBTree  *pIndexBTree,int iLayer);
	
	// 获取本节点下指定起止位置的所有元素(包括元素下同key值的链表值)
	void getElemsInNode(const int &iFirstElemPosi
							,const int &iLastElemPosi
							,vector<MdbAddress> &results
							,IndexBTree * pIndexBTree);
							
	void getFirstLeafNode(Node *&pNode,IndexBTree * pIndexBTree);
	void getLastLeafNode(Node *&pNode,IndexBTree * pIndexBTree);

  protected:
	// insert操作时会被调用,从叶子节点向根节点递归,分裂节点,并向其父节点注册
	//##ModelId=4A0825790196
	Node * splitNode(const char *key
						,const MdbAddress &theMdbAddress
						,const ELEM_POS_IN_NODE_MAP &elemPosiInNodes
						,const int isRepeatedKey
						,IndexBTree * pIndexBTree);

	// delete时会被调用,可能也会向根节点递归
	//##ModelId=4A08DF3F007D
	bool getNextBrotherNode(Node *&pBrotherNode
								,Node *&pParentOfBrotherNode
								,int &iBrotherNodePosi
								,const ELEM_POS_IN_NODE_MAP &elemPosiInNodes
								,const Node * pChildNode
								,IndexBTree * pIndexBTree);

  private:
	// 下一个水平节点,为非NULL时,标示为叶子节点,(正常情况下,叶子节点的该值为右边的叶子节点,若为最右侧的叶子节点,则该值为-1)
	//##ModelId=4A080EEF02BF
	ShmPosition m_pBrotherNode;  // 该值,永久存放在库中
	
	// 父节点
	//##ModelId=4A080F5F036B
	ShmPosition m_pParentNode;   // 该值,永久存放在库中
	
	// 索引键值:Elem类中,m_keyValue的长度
	//##ModelId=4A0818C4038A
	int m_iKeyLenth;              // 该值,永久存放在库中
	// Node下能容纳的最多Elem
	//##ModelId=4A08DDA70251
	int m_iMaxNum;                // 该值,永久存放在库中
	// Node下当前的Elem个数
	//##ModelId=4A08DDD10261
	int m_pCurrentNum;	          // 该值,永久存放在库中 
	
}; 
// Node中,首位elem的偏移量
const int FIRST_ELEM_OFFSIZE=sizeof(Node);

Elem * Node::getElemt(int iSequnce)
{
	if(this!=NULL)
	{
		return (Elem*) ((char *)this + FIRST_ELEM_OFFSIZE +iSequnce*(sizeof(Elem)+m_iKeyLenth));
	}
	else
	{
		throw Mdb_Exception(__FILE__, __LINE__, "Node may not be initiated!");	
	}
}	



#endif /* NODE_H_HEADER_INCLUDED_B5F710A6 */




