// ############################################
// Source file : Node.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2009-5-12
// Update      : 2009-5-12
// Copyright(C): chen min, Linkage.
// ############################################

#include "Node.h"
#include "IndexBTree.h"
#include "Mdb_Exception.h"

void Node::initial(const int iMaxNum,const int iKeyLenth)
{
	m_pBrotherNode   = NULL_SHMPOS;
	m_pParentNode = NULL_SHMPOS;
	m_iKeyLenth   = iKeyLenth;
	m_iMaxNum     = iMaxNum;
	m_pCurrentNum = 0;	
	
	for(int i=0;i<iMaxNum;++i)
	{
		this->getElemt(i)->initial();
	}
	return;
}

//##ModelId=4A080CF2002E
Node * Node::insert(const char *key
					,const MdbAddress &theMdbAddress
					,const ELEM_POS_IN_NODE_MAP &elemPosiInNodes
					,const int isRepeatedKey
					,IndexBTree * pIndexBTree )
{
	if(m_pCurrentNum > m_iMaxNum
		|| m_iMaxNum <=0)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "CurrentNum:%d,MaxElemNum:%d",m_pCurrentNum,m_iMaxNum);	
	}
	
	if(isRepeatedKey==KEY_REPEATED)
	{
		int iTmpPosi = getElemPosiInNode(elemPosiInNodes,(void *)this);
		
		// 申请链节点空间
		MdbAddress newMdbAddr = pIndexBTree->allocateMem(MEM_TYPE_CHAIN_ELEM);
		
		ChainElem * pNewChainElem = (ChainElem *)(newMdbAddr.m_addr);
		pNewChainElem->initial();
		pNewChainElem->m_record = theMdbAddress.m_pos;
		pNewChainElem->m_next   = this->getElemt(iTmpPosi)->m_pNext;
		this->getElemt(iTmpPosi)->m_pNext = newMdbAddr.m_pos; 
		
		getElemt(iTmpPosi)->m_pRecordAddrs = newMdbAddr.m_pos;
						
		return NULL;	//未产生父节点变化
	}
	
	// 如果当前节点还有剩余元素空间,则直接插入
	if(m_pCurrentNum < m_iMaxNum)
	{
		int i = m_pCurrentNum;

		while (i>0 && memcmp(getElemt(i-1)->m_pKeyValue,key,m_iKeyLenth)>0) 
		{
    		getElemt(i)->clone(getElemt(i-1),m_iKeyLenth);
	        --i;
	    }
	    getElemt(i)->initial();
	    getElemt(i)->m_pRecordAddrs = theMdbAddress.m_pos;
	    memcpy(getElemt(i)->m_pKeyValue,key,m_iKeyLenth); 	
		++m_pCurrentNum;
		
		// 如果是首节点,还需向其父节点注册
		if(i==0)
		{
			if(m_pParentNode!=NULL_SHMPOS)
			{
				Node *pTmpNode;
				pTmpNode = (Node *)pIndexBTree->getPhAddr(m_pParentNode);
				
				int iTmpPosi = getElemPosiInNode(elemPosiInNodes,(void *)pTmpNode);
				// 修改父节点对应的Elem的key值
				memcpy(pTmpNode->getElemt(iTmpPosi)->m_pKeyValue,key,m_iKeyLenth);
			}			
		}
	}
	else // 没有空间,则一拆二,并递归向父节点插入
	{
		//在splitNode里递归,走到这儿,已经不是重复键了  
		return this->splitNode(key,theMdbAddress,elemPosiInNodes,KEY_NOT_REPEATED,pIndexBTree);
	}	

	return NULL; //未产生父节点变化
}

//##ModelId=4A080CF60232
bool Node::deleteElem(int iElemPosi
						,int iElemChainPosi
						,ELEM_POS_IN_NODE_MAP &elemPosiInNodes
						,IndexBTree * pIndexBTree)
{
	// 0.如果是重复key的链表,则直接删除链元素
	if( isLeafNode() && iElemChainPosi>NOT_IN_ELEMCHAIN)
	{
		ChainElem *pChainElem,*pPrevChainElem;
		pChainElem = (ChainElem *)pIndexBTree->getPhAddr(this->getElemt(iElemPosi)->m_pNext);
		// 0.1在链表中定位
		for(int i=0;i<iElemChainPosi;++i)
		{
			if(pChainElem->m_next==NULL_SHMPOS)
			{
				throw Mdb_Exception(__FILE__, __LINE__, "ChainElem %d is out of Range",i);		
			}
			pPrevChainElem = pChainElem;
			pChainElem = (ChainElem*)pIndexBTree->getPhAddr(pChainElem->m_next);
		}
		// 0.2释放链表节点占用的内存空间
		pIndexBTree->freeMem(MEM_TYPE_CHAIN_ELEM,pPrevChainElem->m_next);
		return true;
		// 0.3释放链表节点
		if(pChainElem->m_next==NULL_SHMPOS)
		{
			pPrevChainElem->m_next=NULL_SHMPOS;
		}
		else
		{
			pPrevChainElem->m_next=pChainElem->m_next;	
		}
	}
	
	// 非重复key
	if(iElemPosi<m_pCurrentNum)
	{
		Node *pParentNode;
			
		pParentNode = (Node *)pIndexBTree->getPhAddr(m_pParentNode);

		// 1.首先删除元素
		for(int i=iElemPosi;i<m_pCurrentNum-1;++i)
		{
			getElemt(i)->clone(getElemt(i+1),m_iKeyLenth);	
		}
		--m_pCurrentNum;	
		
		// 1.0 如果是本节点的最后一个元素,则释放空间,并从其父节点删除
		if(m_pCurrentNum==0 )
		{
			if( pParentNode==NULL) // 如果根节点都没有元素了,则置为叶子节点
			{
				setNodeTypeLeaf();
			}
			else
			{
				int iTmpPosi = getElemPosiInNode(elemPosiInNodes,(void *)pParentNode);
				// 从父节点中把本节点删除
				pParentNode->deleteElem(iTmpPosi,NOT_IN_ELEMCHAIN,elemPosiInNodes,pIndexBTree);
				// 释放本节点内存空间 
				pIndexBTree->freeMem(MEM_TYPE_NODE,(char *)this);
			}
			return true;				
		}
		
		// 1.1 如果删除的是本节点的首元素,则要修改其父节点对应的key值
		if(iElemPosi==0)
		{
			if(pParentNode!=NULL) // 如果本节点不是根节点
			{
				//int iTmpPosi = getElemPosiInNode(elemPosiInNodes,(void *)pParentNode);
				int iTmpPosi = pParentNode->getElemPosiInNode(elemPosiInNodes
																	,NULL
																	,(void *)this
																	,pIndexBTree);;
				
				memcpy(pParentNode->getElemt(iTmpPosi)->m_pKeyValue
							,this->getElemt(0)->m_pKeyValue
							,m_iKeyLenth);
							
				// 如果修改的是父节点的首元素,那么还需要修改爷爷节点的相关key值,如此一直到根节点
				Node *pTmpParentNode = pParentNode;
				Node *pTmpGrandParentNode;
				while(iTmpPosi==0) 
				{
					if(pTmpParentNode->m_pParentNode!=NULL_SHMPOS)
					{
						pTmpGrandParentNode = (Node *)pIndexBTree->getPhAddr(pTmpParentNode->m_pParentNode);
						iTmpPosi = pTmpGrandParentNode->getElemPosiInNode(elemPosiInNodes
																			,NULL
																			,(void *)pTmpParentNode
																			,pIndexBTree);	
						memcpy(pTmpGrandParentNode->getElemt(iTmpPosi)->m_pKeyValue
								,pTmpParentNode->getElemt(0)->m_pKeyValue
								,m_iKeyLenth);
						pTmpParentNode = pTmpGrandParentNode;
					}
					else
					{
						break;	
					}
				}
			}
		}	
		
		// 2.如果该节点中元素利用率小于50%
		if(pParentNode!=NULL 
			//&& pParentNode->m_pParentNode != NULL_SHMPOS
			&& m_pCurrentNum < m_iMaxNum/2)
		{
			Node *pBrotherNode,*pParentOfBrotherNode;
			int iBrotherNodePosi;
			// 2.1从父节点申请得到和本节点相邻的右侧兄弟节点,可能会递归
			if(pParentNode->getNextBrotherNode(pBrotherNode
													,pParentOfBrotherNode
													,iBrotherNodePosi
													,elemPosiInNodes
													,this
													,pIndexBTree)==true)	
			{
				// 2.1.1如果本节点+兄弟节点的元素使用数之和小于等于最大元素数,则合并这两个节点
				if(pBrotherNode->m_pCurrentNum+this->m_pCurrentNum <=m_iMaxNum)	
				{
					if( isLeafNode() ) // 如果本节点是叶子节点
					{
						for(int i=0;i<pBrotherNode->m_pCurrentNum;++i)
						{
							this->setElem(pBrotherNode->getElemt(i));	
						}
						this->setBrotherNode( pBrotherNode->getBrotherNode() ); //如果是叶子节点,需要设置新的兄弟节点
					}
					else  // 如果本节点不是叶子节点,则需要向其子节点注册新的父节点
					{
						MdbAddress selfMdbAddr;
						selfMdbAddr.m_addr = (char *)this;
						pIndexBTree->getShmPos(selfMdbAddr);
						
						for(int i=0;i<pBrotherNode->m_pCurrentNum;++i)
						{
							// 注册新的父节点
							((Node *)(pIndexBTree->getPhAddr(pBrotherNode->getElemt(i)->m_pRecordAddrs)))->setParentNode(selfMdbAddr.m_pos);
							this->setElem(pBrotherNode->getElemt(i));	
						}
					}
					// 从父节点中把兄弟节点删除
					pParentOfBrotherNode->deleteElem(iBrotherNodePosi,NOT_IN_ELEMCHAIN,elemPosiInNodes,pIndexBTree);
					// 释放兄弟节点内存空间 
					pIndexBTree->freeMem(MEM_TYPE_NODE,(char *)pBrotherNode);
				}
				// 2.1.2从兄弟节点移动1个元素过来
				else
				{
					if( !isLeafNode() ) //如果本节点不是叶子节点,则需要向其子节点注册新的父节点
					{
						MdbAddress selfMdbAddr;
						selfMdbAddr.m_addr = (char *)this;
						pIndexBTree->getShmPos(selfMdbAddr);
						
						// 注册新的父节点
						((Node *)(pIndexBTree->getPhAddr(pBrotherNode->getElemt(0)->m_pRecordAddrs)))->setParentNode(selfMdbAddr.m_pos);
					}
					this->setElem(pBrotherNode->getElemt(0));
					for(int i=1;i<pBrotherNode->m_pCurrentNum;++i)
					{
						pBrotherNode->getElemt(i-1)->clone(pBrotherNode->getElemt(i),m_iKeyLenth);						
					}
					--pBrotherNode->m_pCurrentNum;	
					// 修改兄弟节点在其父节点的对应key
					memcpy(pParentOfBrotherNode->getElemt(iBrotherNodePosi)->m_pKeyValue
							,pBrotherNode->getElemt(0)->m_pKeyValue
							,m_iKeyLenth);
					
					// 如果修改的是父节点的首元素,那么还需要修改爷爷节点的相关key值,如此一直到根节点
					Node *pTmpParentNode = pParentOfBrotherNode;
					Node *pTmpGrandParentNode;
					while(iBrotherNodePosi==0) 
					{
						if(pTmpParentNode->m_pParentNode!=NULL_SHMPOS)
						{
							pTmpGrandParentNode = (Node *)pIndexBTree->getPhAddr(pTmpParentNode->m_pParentNode);
							iBrotherNodePosi = pTmpGrandParentNode->getElemPosiInNode(elemPosiInNodes
																				,NULL
																				,(void *)pTmpParentNode
																				,pIndexBTree);	
							memcpy(pTmpGrandParentNode->getElemt(iBrotherNodePosi)->m_pKeyValue
									,pTmpParentNode->getElemt(0)->m_pKeyValue
									,m_iKeyLenth);
							pTmpParentNode = pTmpGrandParentNode;
						}
						else
						{
							break;	
						}
					}
				}
				//2.1.3
			}		
		}	
	}
	else
	{
		throw Mdb_Exception(__FILE__, __LINE__, "Elem %d is out of index %d",iElemPosi,m_pCurrentNum);				
	}
	return true;
}

//##ModelId=4A080D1002EE
bool Node::search(const char *key
					,vector<MdbAddress> &results
					,Node *&pLeafNode
					,ELEM_POS_IN_NODE_MAP &elemPosiInNodes
					,IndexBTree * pIndexBTree)
{
    // binary search of the node
    int iFirst = 0;
    int iLast = m_pCurrentNum-1;
    MdbAddress result;
    int iCmpResult;

	// 1.在当前节点,查找
	if( iLast-iFirst == 1 )
	{
		iCmpResult = memcmp(key,getElemt(iLast)->m_pKeyValue,m_iKeyLenth);	
	}
    while( iLast-iFirst > 1 ) 
    {
        int iMid = iFirst+(iLast-iFirst)/2;

		iCmpResult = memcmp(key,getElemt(iMid)->m_pKeyValue,m_iKeyLenth);
        if( iCmpResult >=0 )
		{
            iFirst = iMid;
		}
        else 
		{
            iLast = iMid;
		}
    }
	// 2.如果当前节点是叶子节点
    if( this->isLeafNode() )
	{
        Elem *pElem;
        // 记录叶子节点
        pLeafNode = this;
        
        if( memcmp(key,getElemt(iFirst)->m_pKeyValue,m_iKeyLenth)==0 )
        {
        	elemPosiInNodes.insert(ELEM_POS_IN_NODE_MAP::value_type(this,iFirst));
        	pElem = getElemt(iFirst);
        	result.m_pos = pElem->m_pRecordAddrs;
        }
        else if( memcmp(key,getElemt(iLast)->m_pKeyValue,m_iKeyLenth)==0 )
        {
        	elemPosiInNodes.insert(ELEM_POS_IN_NODE_MAP::value_type(this,iLast));
        	pElem = getElemt(iLast);
        	result.m_pos = pElem->m_pRecordAddrs;
    	}
    	else
    	{
    		return false;	
    	}

		pIndexBTree->getPhAddr( result );         // 根据相对地址,得到实际地址
		results.push_back( result ); 
		// 查找该elem下,是否还有其他元素
		if( pElem->m_pNext == NULL_SHMPOS)
		{
			return true;
		}
		else
		{
			ChainElem *pChainElem;
			pChainElem = (ChainElem *)( pIndexBTree->getPhAddr(pElem->m_pNext) );	
			
			// 遍历key相同的chainElem 链表
	        while(pChainElem!=NULL)
	        {
	        	result.m_pos = pChainElem->m_record;	
	        	pIndexBTree->getPhAddr( result );
				results.push_back( result );
				
				pChainElem = (ChainElem *)(pIndexBTree->getPhAddr(pChainElem->m_next));
	        }
			return true;
		}
	}
	// 3.如果当前节点不是叶子节点,则向下查询
	else
	{
		//if( iCmpResult >=0 )
		if( memcmp(key,getElemt(iLast)->m_pKeyValue,m_iKeyLenth)>=0 )
		{
            elemPosiInNodes.insert(ELEM_POS_IN_NODE_MAP::value_type(this,iLast));
        	result.m_pos = getElemt(iLast)->m_pRecordAddrs;
		}
        else 
		{
            elemPosiInNodes.insert(ELEM_POS_IN_NODE_MAP::value_type(this,iFirst));
        	result.m_pos = getElemt(iFirst)->m_pRecordAddrs;
		}	
		pIndexBTree->getPhAddr( result );         // 根据相对地址,得到实际地址
		return ((Node*)result.m_addr)->search(key,results,pLeafNode,elemPosiInNodes,pIndexBTree);		
	}
}

//##ModelId=4A0825790196
Node * Node::splitNode(const char *key
						,const MdbAddress &theMdbAddress
						,const ELEM_POS_IN_NODE_MAP &elemPosiInNodes
						,const int isRepeatedKey
						,IndexBTree * pIndexBTree)
{
	MdbAddress newMdbAddr;   
	Node *pNewNode;
	
	newMdbAddr = pIndexBTree->allocateMem(MEM_TYPE_NODE);
	pNewNode = (Node*)newMdbAddr.m_addr;
	pNewNode->initial(m_iMaxNum,m_iKeyLenth);
	
	// 1.拆分本节点
	for(int i=(m_iMaxNum/2);i<m_iMaxNum;++i)
	{
		pNewNode->setElem(getElemt(i));
		getElemt(i)->initial();
		--m_pCurrentNum;	
	}
	// 2.插入新元素
	if(memcmp(key,getElemt(m_pCurrentNum-1)->m_pKeyValue,m_iKeyLenth)>0)
	{
		pNewNode->insert(key,theMdbAddress,elemPosiInNodes,isRepeatedKey,pIndexBTree);	
	}
	else
	{
		this->insert(key,theMdbAddress,elemPosiInNodes,isRepeatedKey,pIndexBTree);	
	}
	
	// 3.注册本层节点的brotherNode或下层节点的parentNode
	// 只有叶子节点的分裂,才置m_pBrotherNode,用以构成链表
	if(m_pBrotherNode==ShmPosition(-1,-1)
		||m_pBrotherNode!=NULL_SHMPOS)
	{
		pNewNode->setBrotherNode(m_pBrotherNode);
		m_pBrotherNode = newMdbAddr.m_pos;	
	}
	else
	{
		// 父节点分裂,需要向子节点注册m_pParentNode
		pNewNode->reg2child(pIndexBTree,newMdbAddr);	
	}
	// 4.向父节点注册
	Node *pNewParentNode1,*pNewParentNode2;
	pNewParentNode1 = NULL;
	pNewParentNode2 = NULL;
	if(m_pParentNode==NULL_SHMPOS) //根节点已满:申请新的根节点,当前根节点作为新根节点的子节点
	{
		MdbAddress parentMdbAddr;
		parentMdbAddr = pIndexBTree->allocateMem(MEM_TYPE_NODE);
		
		m_pParentNode = parentMdbAddr.m_pos;
		pNewParentNode1 = (Node*)parentMdbAddr.m_addr;
		pNewParentNode1->initial(m_iMaxNum,m_iKeyLenth);
		//向父节点注册本节点里最小key
		MdbAddress selfMdbAddr;
		selfMdbAddr.m_addr = (char *)this;
		pIndexBTree->getShmPos(selfMdbAddr);
		pNewParentNode1->insert(getElemt(0)->m_pKeyValue
												,selfMdbAddr
												,elemPosiInNodes
												,isRepeatedKey
												,pIndexBTree);
	}	
	//向父节点注册新节点里最小key
	pNewNode->setParentNode(m_pParentNode);
	Node *pParentNode;
	pParentNode = (Node *)pIndexBTree->getPhAddr(m_pParentNode);
	pNewParentNode2 = pParentNode->insert(pNewNode->getElemt(0)->m_pKeyValue
											,newMdbAddr
											,elemPosiInNodes
											,isRepeatedKey
											,pIndexBTree);	
	// 应该是先判断pNewParentNode2 再判断pNewParentNode1 ？
	if(pNewParentNode2!=NULL)
	{
		return 	pNewParentNode2;
	}
	else if(pNewParentNode1!=NULL)
	{
		return pNewParentNode1;	
	}
	else
	{
		return NULL;
	}	
}

//##ModelId=4A08D801008C
bool Node::lower_bound(const char *key
						,const int iRangeTag // 标示[]/[)/()/(]运算
						,Node *&pLeafNode
						,int &iElemtPosi
						,IndexBTree * pIndexBTree)
{
    // binary search of the node
    int iFirst = 0;
    int iLast = m_pCurrentNum-1;
    int iCmpResult;

	// 1.在当前节点,查找
	if( iLast-iFirst == 1 )
	{
		iCmpResult = memcmp(key,getElemt(iLast)->m_pKeyValue,m_iKeyLenth);	
	}
    while( iLast-iFirst > 1 ) 
    {
        int iMid = iFirst+(iLast-iFirst)/2;

		iCmpResult = memcmp(key,getElemt(iMid)->m_pKeyValue,m_iKeyLenth);
        if( iCmpResult >=0 )
		{
            iFirst = iMid;
		}
        else 
		{
            iLast = iMid;
		}
    }
	// 2.如果当前节点是叶子节点
    if( this->isLeafNode() )
	{
        Elem *pElem;
        // 记录叶子节点
        pLeafNode = this;
        
        // 返回第一个不小于key的elem
        if( memcmp(key,getElemt(iFirst)->m_pKeyValue,m_iKeyLenth)==0 
        	 && (iRangeTag==RANGE_L||iRangeTag==RANGE_LR) )
        {
        	iElemtPosi = iFirst;
        }
        else 
        {
        	iElemtPosi = iLast;
    	}
    	return true;
	}
	// 3.如果当前节点不是叶子节点,则向下查询
	else
	{
    	MdbAddress result;

		if( memcmp(key,getElemt(iLast)->m_pKeyValue,m_iKeyLenth)>=0 )
		{
        	result.m_pos = getElemt(iLast)->m_pRecordAddrs;
		}
        else 
		{
        	result.m_pos = getElemt(iFirst)->m_pRecordAddrs;
		}	
		pIndexBTree->getPhAddr( result );         // 根据相对地址,得到实际地址
		return ((Node*)result.m_addr)->lower_bound(key,iRangeTag,pLeafNode,iElemtPosi,pIndexBTree);		
	}
}

//##ModelId=4A08D91B003E
bool Node::upper_bound(const char *key
						,const int iRangeTag // 标示[]/[)/()/(]运算
						,Node *&pLeafNode
						,int &iElemtPosi
						,IndexBTree * pIndexBTree)
{
    // binary search of the node
    int iFirst = 0;
    int iLast = m_pCurrentNum-1;
    int iCmpResult;

	// 1.在当前节点,查找
	if( iLast-iFirst == 1 )
	{
		iCmpResult = memcmp(key,getElemt(iLast)->m_pKeyValue,m_iKeyLenth);	
	}
    while( iLast-iFirst > 1 ) 
    {
        int iMid = iFirst+(iLast-iFirst)/2;

		iCmpResult = memcmp(key,getElemt(iMid)->m_pKeyValue,m_iKeyLenth);
        if( iCmpResult >0 )
		{
            iFirst = iMid;
		}
        else 
		{
            iLast = iMid;
		}
    }
	// 2.如果当前节点是叶子节点
    if( this->isLeafNode() )
	{
        Elem *pElem;
        // 记录叶子节点
        pLeafNode = this;
        
        // 返回第一个不大于key的elem
        if( iRangeTag==RANGE_R||iRangeTag==RANGE_LR)
        {
        	iElemtPosi = iLast+1;
        }
        else 
        {
        	iElemtPosi = iLast;
    	}
    	return true;
	}
	// 3.如果当前节点不是叶子节点,则向下查询
	else
	{
    	MdbAddress result;

		if( memcmp(key,getElemt(iLast)->m_pKeyValue,m_iKeyLenth)>0 )
		{
            result.m_pos = getElemt(iLast)->m_pRecordAddrs;
		}
        else 
		{
            result.m_pos = getElemt(iFirst)->m_pRecordAddrs;
		}	
		pIndexBTree->getPhAddr( result );         // 根据相对地址,得到实际地址
		return ((Node*)result.m_addr)->upper_bound(key,iRangeTag,pLeafNode,iElemtPosi,pIndexBTree);		
	}
}


//##ModelId=4A08DF3F007D
bool Node::getNextBrotherNode(Node *&pBrotherNode
								,Node *&pParentOfBrotherNode
								,int &iBrotherNodePosi
								,const ELEM_POS_IN_NODE_MAP &elemPosiInNodes
								,const Node * pChildNode
								,IndexBTree * pIndexBTree)
{
	int iElemPosi;
	
	iElemPosi = this->getElemPosiInNode(elemPosiInNodes,NULL,(void *)pChildNode,pIndexBTree);
		
	// 1.本节点的最后一个元素了
	if( iElemPosi== m_pCurrentNum-1)
	{
		// 1.1 已经是根节点了
		if(m_pParentNode == NULL_SHMPOS)
		{
			return false;	
		}
		// 1.2找父节点的兄弟节点( 伯父节点:-) )			
		else
		{
			Node *pUncleNode;
			Node *pParentOfUncleNode;
			int iUncleNodePosi;

			//1.2.1没有伯父节点,说明本节点是本层的最后一个节点
			if( ( (Node*)(pIndexBTree->getPhAddr(m_pParentNode)) )->getNextBrotherNode(pUncleNode
																						,pParentOfUncleNode
																						,iUncleNodePosi
																						,elemPosiInNodes
																						,this
																						,pIndexBTree)==false)
			{
				return false;	
			}
			//1.2.2有伯父节点,返回伯父节点的第一个elem
			else
			{
				pParentOfBrotherNode = pUncleNode;
				pBrotherNode = (Node *)pIndexBTree->getPhAddr( (pUncleNode->getElemt(0)->m_pRecordAddrs) );
				iBrotherNodePosi = 0;	
				return true;
			}
		}
	}
	// 2.本节点不是其父节点的最后一个元素
	else
	{
		iBrotherNodePosi = iElemPosi+1;
		pParentOfBrotherNode = this;	
		pBrotherNode = (Node *)pIndexBTree->getPhAddr(getElemt(iBrotherNodePosi)->m_pRecordAddrs);
		return true;			
	}
}

void Node::setNodeTypeLeaf()
{
	m_pBrotherNode = ShmPosition(-1,-1);
	return;	
}

void Node::setKeyLenth(const int iKeyLenth)
{
	m_iKeyLenth = iKeyLenth;
	return;	
}

void Node::setBrotherNode(const ShmPosition &pNode)
{
	m_pBrotherNode = pNode;
	return;	
}

void Node::setParentNode(const ShmPosition &pNode)
{
	m_pParentNode = pNode;
	return;	
}

void Node::setElem(const Elem *pElem)
{
	getElemt(m_pCurrentNum)->clone(pElem,m_iKeyLenth);
	++m_pCurrentNum;
	
	return;	
}

void Node::reg2child(IndexBTree * pIndexBTree,MdbAddress &parentMdbAddr)
{
	Node *pChildNode;
	for(int i=0;i<m_pCurrentNum;++i)
	{
		pChildNode = (Node *)(pIndexBTree->getPhAddr(getElemt(i)->m_pRecordAddrs));
		pChildNode->setParentNode(parentMdbAddr.m_pos); 	
	}
	return;	
}

bool Node::isLeafNode()
{
	if(m_pBrotherNode==ShmPosition(-1,-1)
		||m_pBrotherNode!=NULL_SHMPOS)
	{
		return true;	
	}	
	else
	{
		return false;	
	}
}

void Node::getChainPosi(const MdbAddress &theMdbAddress
							,const int &iElemPosi
							,IndexBTree * pIndexBTree
							,int &iElemChainPosi)
{
	Elem *pElem;
	ChainElem *pChainElem;
	
	iElemChainPosi = 0;
	pElem = this->getElemt(iElemPosi);
	// 如果首节点就匹配上
	if(pElem->m_pRecordAddrs == theMdbAddress.m_pos)
	{
		return;	
	}
	else if(pElem->m_pNext != NULL_SHMPOS)
	{
		pChainElem = (ChainElem *)( pIndexBTree->getPhAddr(pElem->m_pNext) );
		
		while( pChainElem!=NULL )
		{
			if(pChainElem->m_record == theMdbAddress.m_pos)
			{
				break;	
			}	
			pChainElem = (ChainElem *)( pIndexBTree->getPhAddr(pChainElem->m_next) );	
			++iElemChainPosi;
		}
	}
	
	return;
}

int Node::getElemPosiInNode(const ELEM_POS_IN_NODE_MAP &elemPosiInNodes
								,void *p                        
								,void *pChild                 //默认参数 只有getNextBrotherNode调用,子节点地址       
								,IndexBTree * pIndexBTree)     //默认参数 只有getNextBrotherNode调用
{
	// 1. 先从search路径中查找
	if(p!=NULL)
	{
		ELEM_POS_IN_NODE_MAP_ITR itr=elemPosiInNodes.find(p);
	
		if(itr!=elemPosiInNodes.end())
		{
			return itr->second;
		}
	}
	
	// 2. 再从节点的elem中遍历
	if(pIndexBTree!=NULL)
	{
		for(int i=0;i<m_pCurrentNum;++i)
		{
			if( pChild == pIndexBTree->getPhAddr(getElemt(i)->m_pRecordAddrs) )
			{
				return i;	
			}	
		}
	}
	// 3. 都没找到 抛异常
	throw Mdb_Exception(__FILE__, __LINE__, "search path was not passed this Node's parent!");		
}

const ShmPosition & Node::getBrotherNode()
{
	return m_pBrotherNode;	
}

const int Node::getCurrentNum()
{
	return m_pCurrentNum;	
}

void  Node::dump(IndexBTree  *pIndexBTree,int iLayer)
{
	if(isLeafNode())
	{
		cout<<"leaf Node size is:"<<m_pCurrentNum<<" ";
		for(int i=0;i<m_pCurrentNum;++i)
		{
			cout<< getElemt(i)->m_pKeyValue<<"," ;
		}
		cout<<endl;
		return;
	}
	else
	{
		for(int i=0;i<m_pCurrentNum;++i)
		{
			cout<<"Layer:"<<iLayer<<" size is:"<<m_pCurrentNum<<" ";
			for(int j=0;j<iLayer;++j)
			{
				cout<<" ";	
			}
			cout<<getElemt(i)->m_pKeyValue <<endl;
			Node *p;
			p = (Node *)pIndexBTree->getPhAddr(getElemt(i)->m_pRecordAddrs);
			p->dump(pIndexBTree,iLayer);
		}
		++iLayer;	
	}
}

// 获取本节点下指定起止位置的所有元素(包括元素下同key值的链表值)
void Node::getElemsInNode(const int &iFirstElemPosi
							,const int &iLastElemPosi
							,vector<MdbAddress> &results
							,IndexBTree * pIndexBTree)
{
	MdbAddress result;
	Elem *pElem;

	for(int i=iFirstElemPosi;i<iLastElemPosi;++i)
	{
		//cout<<pGtLeafNode->getElemt(i)->m_pKeyValue<<endl;
		pElem  = getElemt(i);
		result.m_pos = pElem->m_pRecordAddrs;
		pIndexBTree->getPhAddr( result );         // 根据相对地址,得到实际地址
		results.push_back( result ); 
		
		// 遍历key相同的chainElem 链表
        if( pElem->m_pNext == NULL_SHMPOS)
        {
	        ChainElem *pChainElem;
			pChainElem = (ChainElem *)( pIndexBTree->getPhAddr(pElem->m_pNext) );
	        
	        while(pChainElem!=NULL)
	        {
	        	result.m_pos = pChainElem->m_record;	
	        	pIndexBTree->getPhAddr( result );
				results.push_back( result );
				
				pChainElem = (ChainElem *)(pIndexBTree->getPhAddr(pChainElem->m_next));
	        }
		}	
	}		
	
	return;	
}

void Node::getFirstLeafNode(Node *&pNode,IndexBTree * pIndexBTree)
{
	if( isLeafNode() )
	{
		pNode = this;
		return;	
	}
	else
	{
		Node *pTmpNode = (Node *)(pIndexBTree->getPhAddr(getElemt(0)->m_pRecordAddrs));
		if( pTmpNode!=NULL )
		{
			// 递归
			return pTmpNode->getFirstLeafNode(pNode,pIndexBTree);
		}
		else
		{
			throw Mdb_Exception(__FILE__, __LINE__, "E rror Node Address!");		
		}
	}
}

void Node::getLastLeafNode(Node *&pNode,IndexBTree * pIndexBTree)
{
	if( isLeafNode() )
	{
		pNode = this;
		return;	
	}
	else
	{
		Node *pTmpNode = (Node *)(pIndexBTree->getPhAddr(getElemt(m_pCurrentNum-1)->m_pRecordAddrs));
		if( pTmpNode!=NULL )
		{
			// 递归
			return pTmpNode->getLastLeafNode(pNode,pIndexBTree);
		}
		else
		{
			throw Mdb_Exception(__FILE__, __LINE__, "E rror Node Address!");		
		}
	}
}
		