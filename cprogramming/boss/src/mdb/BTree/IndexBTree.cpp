// ############################################
// Source file : IndexBTree.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2009-5-12
// Update      : 2009-5-12
// Copyright(C): chen min, Linkage.
// ############################################

#include "IndexBTree.h"

IndexBTree::IndexBTree(const char *cTableName,const char *cIndexName,MemManager* memMgr,RecordConvert *pRecordConvert)
	                           :Index(cTableName,cIndexName,memMgr,pRecordConvert)
{
	m_pRootNode          = NULL;
	m_nodeTableDesc      = NULL;
	m_chainElemTableDesc = NULL;
}

// 创建一个新索引
bool IndexBTree::create(const IndexDef &theIndexDef)
{
	// 0.首先获取key长度
	int iKeyLenth = this->getKeyLenth(theIndexDef);
	// 1.创建索引
	m_memMgr->createIndex(theIndexDef,m_indexDesc);
	
	// 2.创建Node所用表
	TableDef  t_tableDef;
	sprintf(t_tableDef.m_tableName,"%s%s",NODE_TABLE_PREFIX.c_str(),theIndexDef.m_indexName);
	t_tableDef.m_tableType = DATA_TABLE;

	t_tableDef.m_columnNum = 1;
	sprintf(t_tableDef.m_columnList[0].m_name,"%s","NODE");
	t_tableDef.m_columnList[0].m_type=VAR_TYPE_VSTR;
	t_tableDef.m_columnList[0].m_len=FIRST_ELEM_OFFSIZE + NODE_ELEM_NUM*(iKeyLenth +sizeof(Elem));
	// 指定和索引用一个表空间
	t_tableDef.m_spaceNum = theIndexDef.m_spaceNum;
	for(int j=0;j<theIndexDef.m_spaceNum;++j)
	{
		strcpy(t_tableDef.m_spaceList[j],theIndexDef.m_spaceList[j]);
	}
	m_memMgr->createTable(t_tableDef, m_nodeTableDesc);
	
	// 3.创建chainElem所用表
	sprintf(t_tableDef.m_tableName,"%s%s",CHAIN_ELEM_TABLE_PREFIX.c_str(),theIndexDef.m_indexName);
	t_tableDef.m_tableType = DATA_TABLE;
	
	t_tableDef.m_columnNum = 1;
	sprintf(t_tableDef.m_columnList[0].m_name,"%s","CHAINELEM");
	t_tableDef.m_columnList[0].m_type=VAR_TYPE_VSTR;
	t_tableDef.m_columnList[0].m_len = sizeof(ChainElem);
	// 指定和索引用一个表空间
	t_tableDef.m_spaceNum = theIndexDef.m_spaceNum;
	for(int j=0;j<theIndexDef.m_spaceNum;++j)
	{
		strcpy(t_tableDef.m_spaceList[j],theIndexDef.m_spaceList[j]);
	}
	m_memMgr->createTable(t_tableDef, m_chainElemTableDesc);
	
	//4. 生成第一个根节点,认为是叶子
	MdbAddress tmpAddr;
	tmpAddr = this->allocateMem(MEM_TYPE_NODE);
	m_indexDesc->m_header = tmpAddr.m_pos;      // 在indexdesc中注册根节点的地址
	m_pRootNode = (Node *)tmpAddr.m_addr;
	m_pRootNode->initial(NODE_ELEM_NUM,iKeyLenth);
	m_pRootNode->setNodeTypeLeaf();
	
	// 5.根据indexdef获取索引字段值的长度
	getHashKeyLenth();
}

bool IndexBTree::drop()
{
	// 1.删除Node所用表
	m_memMgr->dropTable( m_nodeTableDesc->m_tableDef.m_tableName);
	// 2.删除ChainElem所用表
	m_memMgr->dropTable( m_chainElemTableDesc->m_tableDef.m_tableName);
	// 3.删除索引
	m_memMgr->dropIndex( m_indexName.c_str(),m_tableName.c_str() );	

	return true;
}

bool IndexBTree::truncate()
{
	// 1. truncate Node所用表
	m_memMgr->truncateTable( m_nodeTableDesc->m_tableDef.m_tableName );	
	// 2.truncate  ChainElem所用表
	m_memMgr->truncateTable( m_chainElemTableDesc->m_tableDef.m_tableName);
	// 3.truncate Index
	m_memMgr->truncateIndex( m_indexName.c_str(),m_tableName.c_str() );		

	return true;
}

// 初始化一个索引（attach内存）
bool IndexBTree::initialization()
{
	//1. 初始化index/初始化root node
	m_memMgr->getIndexDescByName(m_indexName.c_str(),m_indexDesc);
	m_memMgr->getPhAddr(m_indexDesc->m_header,(char *&)m_pRootNode);  // 获取指向BTree根节点地址
	
	//2.初始化Node表
	string tableName;
	tableName = NODE_TABLE_PREFIX+m_indexDesc->m_indexDef.m_indexName;
	m_memMgr->getTableDescByName(tableName.c_str(), m_nodeTableDesc);
	
	//3.初始化ChainElem表
	tableName = CHAIN_ELEM_TABLE_PREFIX+m_indexDesc->m_indexDef.m_indexName;
	m_memMgr->getTableDescByName(tableName.c_str(), m_chainElemTableDesc);
	
	// 4.根据indexdef获取索引字段值的长度
	getHashKeyLenth();
	
	return true;
}

bool IndexBTree::insert(const char *key       //查询关键字
				,const MdbAddress &theMdbAddress)  
{
	vector<MdbAddress> results;
	Node *pNewParent;
	Node *pLeafNode;
	ELEM_POS_IN_NODE_MAP elemPosiInNodes;
	elemPosiInNodes.clear();
	
	if( m_pRootNode->search(key,results,pLeafNode,elemPosiInNodes,&(*this)) )
	{
		if( m_indexDesc->m_indexDef.m_isUnique )
		{
			// 如果是唯一索引,则抛出主键重复异常	
			throw Mdb_Exception (__FILE__, __LINE__, "值 [%s] 主键重复",key ); 
		}
		pNewParent = pLeafNode->insert(key,theMdbAddress,elemPosiInNodes,KEY_REPEATED,&(*this));
	}
	else
	{
		pNewParent = pLeafNode->insert(key,theMdbAddress,elemPosiInNodes,KEY_NOT_REPEATED,&(*this)); // 从叶子节点自下而上插入
	}
	// 插入新元素导致父节点变化了
	if(pNewParent!=NULL)
	{
		m_pRootNode = pNewParent;	
		
		MdbAddress tmpMdbAddr;
		tmpMdbAddr.m_addr = (char *)m_pRootNode;
		m_memMgr->getShmPos( tmpMdbAddr );
		m_indexDesc->m_header = tmpMdbAddr.m_pos;      // 在indexdesc中注册根节点的地址	
	}
	
	return true;	
}

int IndexBTree::select(const InputParams *pInputParams   //查询关键字
						,vector<MdbAddress> &result
						,int iRowCount) 
{
	Node *pLeafNode;
	ELEM_POS_IN_NODE_MAP elemPosiInNodes;
	elemPosiInNodes.clear();
	
	getSearchKey(pInputParams);
	
	return m_pRootNode->search(m_cSearchKey,result,pLeafNode,elemPosiInNodes,&(*this));	
} 

int IndexBTree::getKeyLenth(const IndexDef &theIndexDef)
{
    TableDesc* pTableDesc;
    m_memMgr->getTableDescByName(this->m_tableName.c_str(), pTableDesc);
    
    int iKeyLenth = 0;
    for(int i=0;i<theIndexDef.m_columnNum;++i)
	{
	    bool isMatchColumn = false;
	    for(int j=0;j<pTableDesc->m_tableDef.m_columnNum;++j)
	    {
	    	if(strcasecmp(theIndexDef.m_columnList[i]
	    					,pTableDesc->m_tableDef.m_columnList[j].m_name)==0)
	    	{
	    		switch(pTableDesc->m_tableDef.m_columnList[j].m_type)
			    {
				    case VAR_TYPE_INT2:
				        iKeyLenth+=2;
				        break;
				    case VAR_TYPE_INT:
				        iKeyLenth+=sizeof(int);
				        break;
				    case VAR_TYPE_DATE:
				        iKeyLenth+=2*sizeof(int);
				        break;
				    case VAR_TYPE_LONG:
				        iKeyLenth+=sizeof(long);
				        break;
				    case VAR_TYPE_REAL:
				        iKeyLenth+=sizeof(float);
				        break;
				    case VAR_TYPE_NUMSTR: //数字字符串，采用BCD码压缩
				        iKeyLenth+=(pTableDesc->m_tableDef.m_columnList[j].m_len+1)/2;
				        break;
				    case VAR_TYPE_VSTR: //字符串方式
				        iKeyLenth+=pTableDesc->m_tableDef.m_columnList[j].m_len;
				        break;
				    case VAR_TYPE_UNKNOW:
				    default:
				      	throw Mdb_Exception(__FILE__, __LINE__,"column %s's data type is unknow!",theIndexDef.m_columnList[i]);
			    }	
			    isMatchColumn = true;
			}
	    
	    }
	    if(isMatchColumn == false)
	    {
	    	throw Mdb_Exception(__FILE__, __LINE__,"column %s is not defined!",theIndexDef.m_columnList[i]);
	    }
	}
	return iKeyLenth;        
	
}

MdbAddress IndexBTree::allocateMem(int iMemType)
{
	vector<MdbAddress> addrs;
	addrs.clear();
	
	if(iMemType==MEM_TYPE_NODE)
	{
		m_memMgr->allocateTableMem(m_nodeTableDesc, 1, addrs);
		return addrs[0];
	}
	else if(iMemType==MEM_TYPE_CHAIN_ELEM)
	{
		m_memMgr->allocateTableMem(m_chainElemTableDesc, 1, addrs);
		return addrs[0];	
	}
}

// 根据相对地址释放表内存
void IndexBTree::freeMem(int iMemType,ShmPosition &posi)
{
	MdbAddress addr;
	
	addr.m_pos = posi;
	if(iMemType==MEM_TYPE_NODE)
	{
		m_memMgr->freeTableMem(m_nodeTableDesc, addr);
	}
	else if(iMemType==MEM_TYPE_CHAIN_ELEM)
	{
		m_memMgr->freeTableMem(m_chainElemTableDesc, addr);
	}
}

// 根据物理地址释放表内存
void IndexBTree::freeMem(int iMemType,char *phAddr)
{
	MdbAddress addr;
	
	addr.m_addr = phAddr;
	m_memMgr->getShmPos(addr);
	if(iMemType==MEM_TYPE_NODE)
	{
		m_memMgr->freeTableMem(m_nodeTableDesc, addr);
	}
	else if(iMemType==MEM_TYPE_CHAIN_ELEM)
	{
		m_memMgr->freeTableMem(m_chainElemTableDesc, addr);
	}
}

void IndexBTree::getPhAddr(MdbAddress &mdbAddr)
{
	m_memMgr->getPhAddr( mdbAddr ); 
	return;	
}

// 根据 ShmPosition 获得实际的物理地址
char * IndexBTree::getPhAddr(const ShmPosition &shmPosi)
{
	char *p;
	m_memMgr->getPhAddr( shmPosi,p );         // 根据相对地址,得到实际地址
	return 	p;	
}

// 根据 物理地址  得到 ShmPosition
void IndexBTree::getShmPos(MdbAddress &mdbAddr)
{
	m_memMgr->getShmPos( mdbAddr ); 
}

bool IndexBTree::deleteIdx(const char *key       //查询关键字
						,const MdbAddress &theMdbAddress)
{
	int iElemPosi,iElemChainPosi;
	Node *pLeafNode;
	vector<MdbAddress> results;
	ELEM_POS_IN_NODE_MAP elemPosiInNodes;
	elemPosiInNodes.clear();
	
	if( m_pRootNode->search(key,results,pLeafNode,elemPosiInNodes,&(*this)) )
	{
		if(pLeafNode!=NULL)
		{
			ELEM_POS_IN_NODE_MAP_ITR itr=elemPosiInNodes.find((void * )pLeafNode);
			
			if(itr!=elemPosiInNodes.end())
			{
				iElemPosi = itr->second;
				pLeafNode->getChainPosi(theMdbAddress,iElemPosi,&(*this),iElemChainPosi);
				pLeafNode->deleteElem(iElemPosi,iElemChainPosi,elemPosiInNodes,&(*this));		
			}
			else
			{
				throw Mdb_Exception(__FILE__, __LINE__, "search path was not passed this Node!");		
			}
			
		}	
	}
	return true;
}

int IndexBTree::selectRange(const InputParams *pGtParams   //查询关键字
								,const InputParams *pLtParams
								,const int iRangeTag // 标示[]/[)/()/(]运算	
								,vector<MdbAddress> &results
								,const int iRowCount) 
{
	Node *pGtLeafNode,*pLtLeafNode;
	int  iGtElemPosi,iLtElemPosi;
	MdbAddress result;	
	
	results.clear();
	
	if( pGtParams!=NULL 
		 &&pGtParams->m_iNum!=0 )
	{
		getSearchKey(pGtParams);
		m_pRootNode->lower_bound(m_cSearchKey,iRangeTag,pGtLeafNode,iGtElemPosi,&(*this));
	}
	else
	{
		m_pRootNode->getFirstLeafNode(pGtLeafNode,&(*this)); // 开区间,获取第最小节点	
		iGtElemPosi = 0;
	}	
	
	if( pLtParams!=NULL 
		 &&pLtParams->m_iNum!=0 )
	{
		getSearchKey(pLtParams);
		m_pRootNode->upper_bound(m_cSearchKey,iRangeTag,pLtLeafNode,iLtElemPosi,&(*this));
	}
	else
	{
		m_pRootNode->getLastLeafNode(pLtLeafNode,&(*this)); // 开区间,获取第最大节点	
		iLtElemPosi = pLtLeafNode->getCurrentNum();
	}
	
	if(pGtLeafNode==pLtLeafNode)
	{
		pGtLeafNode->getElemsInNode(iGtElemPosi
		                              ,iLtElemPosi
		                              ,results
		                              ,&(*this));
	}
	else
	{
		// 首节点
		pGtLeafNode->getElemsInNode(iGtElemPosi
		                              ,pGtLeafNode->getCurrentNum()
		                              ,results
		                              ,&(*this));

		// 遍历中间节点
		pGtLeafNode = (Node *)getPhAddr(pGtLeafNode->getBrotherNode());
		while(pGtLeafNode!=pLtLeafNode)
		{
			pGtLeafNode->getElemsInNode(0,pGtLeafNode->getCurrentNum()
			                              ,results
			                              ,&(*this));

			pGtLeafNode = (Node *)getPhAddr(pGtLeafNode->getBrotherNode());
		}
		// 尾节点
		pLtLeafNode->getElemsInNode(0,iLtElemPosi
		                              ,results
		                              ,&(*this));
	}
	return 	results.size();
}

void IndexBTree::dump()
{
	int i=0;
	m_pRootNode->dump(&(*this),i); 
}
