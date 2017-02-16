#include "MemManager.h"
#include <iostream>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "DoubleLock.h"
#include "InstanceFactory.h"

bool MemManager::createTable(const TableDef &r_tableDef,TableDesc* &r_tableDesc)
{
  bool t_existFlag = false;
  try
  {
    //0. 检查是否有同名表存在
    if(getTableDescByName(r_tableDef.m_tableName,r_tableDesc)==true)
    {
      #ifdef _DEBUG_
        cout<<"表："<<r_tableDef.m_tableName<<" 已经存在!创建失败!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      t_existFlag=true;
      throw Mdb_Exception(__FILE__, __LINE__, "重名表已经存在!");
    }
  }catch(Mdb_Exception &e)
  {
    if(t_existFlag==true)
    {
      throw e;
    }else
    {
      //不存在为正常情况。
    }
  }
  //1. 根据表结构定义，生成一个TableDesc对象
  TableDesc  t_tableDesc;
  t_tableDesc.initByTableDef(r_tableDef);
  //2. 将TableDesc对象加入控制区表空间中
  if(m_ctlMemMgr.addTableDesc(t_tableDesc,r_tableDesc)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_ctlMemMgr.addTableDesc(t_tableDesc,r_tableDesc) false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    throw Mdb_Exception(__FILE__, __LINE__, "创建表描述符失败!");
    return false;
  }
  return true;
}
bool MemManager::getTableDescList(vector<TableDesc>& r_tableDescList)
{
  if(m_ctlMemMgr.getTableDescList(r_tableDescList)==false)
  {
    throw Mdb_Exception(__FILE__, __LINE__, "取表描述符列表失败!");
    return false;
  }
  return true;
}
bool MemManager::getTableDefList(vector<TableDef>& r_tableDefList)
{
  vector<TableDesc> t_tableDescList;
  r_tableDefList.clear();
  if(m_ctlMemMgr.getTableDescList(t_tableDescList)==false)
  {
    throw Mdb_Exception(__FILE__, __LINE__, "取表定义列表失败!");
    return false;
  }
  for(vector<TableDesc>::iterator t_itr=t_tableDescList.begin();
      t_itr!=t_tableDescList.end();t_itr++)
  {
    r_tableDefList.push_back(t_itr->m_tableDef);
  }
  return true;
}
bool MemManager::getTableDescByName(const char *r_tableName,TableDesc* &r_pTableDesc)
{
  if(m_ctlMemMgr.getTableDescByName(r_tableName,r_pTableDesc)==false)
  {
    char t_errMsg[256];
    sprintf(t_errMsg, "根据表名{%s}取表描述符失败!",r_tableName);
    throw Mdb_Exception(__FILE__, __LINE__,t_errMsg);
    return false;
  }
  return true;
}

bool MemManager::getIndexDescByName(const char* r_indexName,IndexDesc* &r_pIndexDesc)
{
  if(m_ctlMemMgr.getIndexDescByName(r_indexName,r_pIndexDesc)==false)
  {
    throw Mdb_Exception(__FILE__, __LINE__, "根据索引名取索引描述符失败!");
    return false;
  }
  return true;
}
bool MemManager::createIndex(const IndexDef &r_idxDef,IndexDesc* &r_idxDesc)
{
  bool t_existFlag = false;
  try
  {
    //检查重名索引是否存在
    getIndexDescByName(r_idxDef.m_indexName,r_idxDesc);
    #ifdef _DEBUG_
      cout<<"索引："<<r_idxDef.m_indexName<<" 已经存在!创建失败!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    t_existFlag=true;
    throw Mdb_Exception(__FILE__, __LINE__, "重名索引已经存在!创建失败!");
  }catch(Mdb_Exception &e)
  {
    //重名索引不存在，表示正常情况
    if(t_existFlag==true)
    {
      throw e;
    }else
    {
      //不存在为正常情况。
    }
  }
  
  bool t_bRet=true;
  IndexDesc t_indexDesc;
  //0.取对应的表描述符
  TableDesc * t_pTableDesc=NULL;
    getTableDescByName(r_idxDef.m_tableName,t_pTableDesc);
    //1. 创建索引描述符
    t_indexDesc.initByIndeDef(r_idxDef);
    if(m_ctlMemMgr.addIndexDesc(t_indexDesc,r_idxDesc)==false)
    {
      #ifdef _DEBUG_
        cout<<"m_ctlMemMgr.addIndexDesc(t_tableDesc,r_idxDesc) false!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      t_bRet=false;
      throw Mdb_Exception(__FILE__, __LINE__, "创建索引描述符失败!");
    }
    
    //2. 如果是Hash索引，则创建Hash结构
    if(r_idxDef.m_indexType==HASH_INDEX ||
       r_idxDef.m_indexType==HASH_TREE)
    { 
      //r_shmPos是指向Hash中ShmPositioin[]的首地址
      if(m_indexMemMgr.createHashIndex(r_idxDesc,r_idxDesc->m_header)==false)
      {
        m_ctlMemMgr.deleteIndexDesc(t_indexDesc);
        #ifdef _DEBUG_
          cout<<"m_indexMemMgr.createHashIndex(r_idxDesc,t_IndexHashPos) false!"
              <<__FILE__<<__LINE__<<endl;
        #endif
        t_bRet=false;
        throw Mdb_Exception(__FILE__, __LINE__, "创建Hash索引区失败!");
      }
    }else
    {
      r_idxDesc->m_header=NULL_SHMPOS;
    }
    //3. 在对应的表描述符中增加该索引信息
    size_t  t_indexOffset=m_ctlMemMgr.getOffset((char*)r_idxDesc);
    r_idxDesc->m_tablePos=m_ctlMemMgr.getOffset((char*)t_pTableDesc);
    t_pTableDesc->addIndex(t_indexOffset,r_idxDef.m_indexName);
  return t_bRet;
}

bool MemManager::dropTable(const char *r_tableName)
{
  bool t_bRet=true;
  //0.获取表描述符信息TableDesc
  TableDesc *t_pTableDesc=NULL,t_tableDesc;
    getTableDescByName(r_tableName,t_pTableDesc);
    memcpy(&t_tableDesc,t_pTableDesc,sizeof(TableDesc));
    //1.如果有索引，删除索引
    for(int i=0;i<t_tableDesc.m_indexNum;i++)
    {
      dropIndex(t_tableDesc.m_indexNameList[i],r_tableName);
    }
    //2.truncateTable 
    truncateTable(r_tableName);
    //3.删除表描述符
    if(m_ctlMemMgr.deleteTableDesc(*t_pTableDesc)==false)
    {
      t_bRet = false;
      throw Mdb_Exception(__FILE__, __LINE__, "删除表描述符失败!");
    } 
  return t_bRet;
}

bool MemManager::dropIndex(const char *r_idxName,const char * r_tableName)
{
  bool t_bRet=true;
  T_NAMEDEF t_tableName;
  IndexDesc *t_pIndexDesc=NULL;
    getIndexDescByName(r_idxName,t_pIndexDesc);
    if(r_tableName==0)
    {
      strcpy(t_tableName,t_pIndexDesc->m_indexDef.m_tableName);
    }else
    {
      strcpy(t_tableName,r_tableName);
    }
    //0. truncateIndex 
    truncateIndex(r_idxName);
    //1. 获取表描述符和索引描述符
    TableDesc *t_pTableDesc=NULL;
    getTableDescByName(t_tableName,t_pTableDesc);
    size_t t_indexOffSet=-1;
    for(int i=0;i<t_pTableDesc->m_indexNum;i++)
    {
      if(strcasecmp(t_pTableDesc->m_indexNameList[i],r_idxName)==0)
      {
        t_indexOffSet=t_pTableDesc->m_indexPosList[i];
      }
    }
    if(t_indexOffSet==-1)
    {
      #ifdef _DEBUG_
        cout<<"取对应的索引标识符失败!"<<endl;
      #endif
      t_bRet = false;
      throw Mdb_Exception(__FILE__, __LINE__, "取对应的索引标识符失败!");
    }
    //2. 删除表中对应的索引位置信息
    if(t_pTableDesc->deleteIndex(t_indexOffSet)==false)
    {
      t_bRet = false;
      throw Mdb_Exception(__FILE__, __LINE__, "表删除对应的索引信息失败!");
    }
    //3. 如果是Hash索引,释放Hash结构空间
    if(t_pIndexDesc->m_indexDef.m_indexType==HASH_INDEX||
       t_pIndexDesc->m_indexDef.m_indexType==HASH_TREE)
    {
      if(m_indexMemMgr.dropHashIdex(t_pIndexDesc->m_header)==false)
      {
        t_bRet = false;
        // add by chenm 2009-1-8 23:29:13 直接删除索引描述符
        t_pIndexDesc->m_header=NULL_SHMPOS;
        m_ctlMemMgr.deleteIndexDesc(*t_pIndexDesc);
        throw Mdb_Exception(__FILE__, __LINE__, "索引描述符删除成功,但释放Hash索引空间失败!");
      }
      t_pIndexDesc->m_header=NULL_SHMPOS;
    }
    //4. 删除索引描述符
    if(m_ctlMemMgr.deleteIndexDesc(*t_pIndexDesc)==false)
    {
      t_bRet = false;
      throw Mdb_Exception(__FILE__, __LINE__, "删除索引描述符失败!");
    }
  return t_bRet;
}

bool MemManager::truncateIndex(const char *r_idxName,const char * r_tableName)
{
  bool t_bRet=true;
  IndexDesc *t_pIndexDesc=NULL;
  
    //0.获取索引描述符
    getIndexDescByName(r_idxName,t_pIndexDesc);
    //1. 释放对应的索引数据Page给表空间
    ShmPosition  t_pagePos,t_nextPagePos;
    PageInfo * t_pPageInfo;
    char     * t_phAddr;
    t_pagePos=t_pIndexDesc->m_pageInfo.m_firstPage;
    while(!(t_pagePos==NULL_SHMPOS))
    {
      getPhAddr(t_pagePos,t_phAddr);
      t_pPageInfo = (PageInfo*)t_phAddr;
      t_nextPagePos=t_pPageInfo->m_next;
      if(m_tableMemMgr.freePage(t_pagePos)==false)
      {
        #ifdef _DEBUG_
          cout<<"freePage("<<t_pagePos<<") false!"
              <<__FILE__<<__LINE__<<endl;
        #endif
        t_bRet=false;
        throw Mdb_Exception(__FILE__, __LINE__, "释放PAGE失败!");
      }
      t_pagePos=t_nextPagePos;
    }
  //2. 清除索引描述符中的位置信息
  t_pIndexDesc->m_pageInfo.initPos();
  //3. 清除Hash索引中的Hash结构信息
  if(t_pIndexDesc->m_indexDef.m_indexType==HASH_INDEX ||
     t_pIndexDesc->m_indexDef.m_indexType==HASH_TREE)
  {
    m_indexMemMgr.initHashSeg(t_pIndexDesc->m_header);
  }else
  {
    t_pIndexDesc->m_header = NULL_SHMPOS;
  }
  return t_bRet;
}


bool MemManager::truncateTable(const char *r_tableName)
{
  bool t_bRet=true;
  //1. 取对应的表描述符
  //1. 获取表描述符和索引描述符
  TableDesc *t_pTableDesc=NULL;
    getTableDescByName(r_tableName,t_pTableDesc);
    //2. truncate所有索引
    for(int i=0;i<t_pTableDesc->m_indexNum;i++)
    {
      truncateIndex(t_pTableDesc->m_indexNameList[i],r_tableName);
    }
    //3. 释放表PAGE
    ShmPosition  t_pagePos,t_nextPagePos;
    PageInfo * t_pPageInfo;
    char     * t_phAddr;
    t_pagePos=t_pTableDesc->m_pageInfo.m_firstPage;
    while(!(t_pagePos==NULL_SHMPOS))
    {
      getPhAddr(t_pagePos,t_phAddr);
      t_pPageInfo = (PageInfo*)t_phAddr;
      t_nextPagePos=t_pPageInfo->m_next;
      if(m_tableMemMgr.freePage(t_pagePos)==false)
      {
        #ifdef _DEBUG_
          cout<<"m_tableMemMgr.freePage("<<t_pagePos<<") false!"
              <<" "<<__FILE__<<__LINE__<<endl;
        #endif
        t_bRet=false;
        throw Mdb_Exception(__FILE__, __LINE__, "释放PAGE失败!");
      }
      t_pagePos=t_nextPagePos;
    }
    //4. 清除表描述符中的位置信息
    t_pTableDesc->m_pageInfo.initPos();
    //5. m_recordNum置零 add by chenm 2008-5-23 
    t_pTableDesc->m_recordNum=0;
  return t_bRet;
}

bool MemManager::allocateTableMem(TableDesc* &r_tableDesc,const int &r_num,
                                  vector<MdbAddress>& r_slotAddrList) 
{
    allocateMem(r_tableDesc->m_tableDef.m_spaceNum,r_tableDesc->m_tableDef.m_spaceList,
                 r_tableDesc->m_pageInfo,r_num,r_slotAddrList,0);
  
  return true;
}
bool MemManager::allocateIdxMem(IndexDesc &r_indexDesc,const int &r_num,
                                vector<MdbAddress> &r_addrList)
{
    allocateMem(r_indexDesc.m_indexDef.m_spaceNum,r_indexDesc.m_indexDef.m_spaceList,
                r_indexDesc.m_pageInfo,r_num,r_addrList,1);
  
  return true;
}
//要求r_slotAddrList中即有物理地址，也有ShmPosition
bool MemManager::freeTableMem(TableDesc* &r_tableDesc,const vector<MdbAddress>& r_slotAddrList)
{
  bool t_bRet=true;
    for(vector<MdbAddress>::const_iterator t_itr=r_slotAddrList.begin();
        t_itr!=r_slotAddrList.end();t_itr++)
    {
      freeMem(r_tableDesc->m_pageInfo,t_itr->m_pos,0);
    }
  return t_bRet;
}

//要求r_slotAddrList中即有物理地址，也有ShmPosition
// by chenm 2008-6-25 单条释放
bool MemManager::freeTableMem(TableDesc* &r_tableDesc,const MdbAddress &r_slotAddr)
{
    freeMem(r_tableDesc->m_pageInfo,r_slotAddr.m_pos,0);
 	return true;
}

bool MemManager::freeIdxMem(IndexDesc &r_indexDesc,const vector<ShmPosition> &r_addrList)
{
  bool t_bRet=true;
    for(vector<ShmPosition>::const_iterator t_itr=r_addrList.begin();
        t_itr!=r_addrList.end();t_itr++)
    {
      freeMem(r_indexDesc.m_pageInfo,*t_itr,1);
    }
  return t_bRet;
}
bool MemManager::freeIdxMem(IndexDesc &r_indexDesc,const ShmPosition &r_addr)
{
  bool t_bRet=true;
    freeMem(r_indexDesc.m_pageInfo,r_addr,1);
  return t_bRet;
}

bool MemManager::allocateMem(const int &r_spaceNum,const T_NAMEDEF r_spaceList[MAX_SPACE_NUM],
                             DescPageInfo &r_descPageInfo,const int &r_num,
                             vector<MdbAddress>& r_slotAddrList,
                             const int &r_falg) 
{
  bool t_bRet = true;
  int  t_needNum=r_num,t_newNum=0;
  ShmPosition  t_pagePos;
  PageInfo    *t_pPage=NULL;
  try
  {
    updateMgrInfo();//更新表空间信息,因为表空间有可能变化
    r_slotAddrList.clear();
    if(r_num>r_slotAddrList.capacity())
    {
     r_slotAddrList.reserve(r_num); 
    }
    t_pagePos = r_descPageInfo.m_firstNotFullPage;
    //1.从未满页中先分配
    if(!(t_pagePos == NULL_SHMPOS))
    {
      while(t_needNum>0)
      {
        if(getPageInfo(t_pagePos,t_pPage)==false)
        {
          t_bRet = false;
          throw Mdb_Exception(__FILE__, __LINE__, "根据页面位置取页面失败!");
        }
        t_pPage->allocateNode((char*)t_pPage,t_needNum,r_slotAddrList,t_newNum);
        t_needNum = t_needNum-t_newNum;
        t_pagePos = t_pPage->m_nextNotNullPage;
        if(t_pPage->m_idleNum == 0)
        { //将t_pPage从未满队列中移出
          r_descPageInfo.m_firstNotFullPage=t_pagePos;
          t_pPage->m_nextNotNullPage = NULL_SHMPOS;
          if(t_pagePos == NULL_SHMPOS)
          {
            r_descPageInfo.m_lastNotFullPage=t_pagePos;
            break;
          }      
        }
      }
    }
  }catch(Mdb_Exception &e)
  {
    #ifdef _USEDSLOT_LIST_
      addSlotIntoDesc(r_descPageInfo,r_slotAddrList);
    #endif
    //将已申请到的节点释放掉
    for(vector<MdbAddress>::iterator t_addrItr=r_slotAddrList.begin();
        t_addrItr!=r_slotAddrList.end();t_addrItr++)
    {
      freeMem(r_descPageInfo,t_addrItr->m_pos,r_falg);
    }
    r_slotAddrList.clear();
    throw e;
  }
  if(t_needNum<=0)
  {
    #ifdef _USEDSLOT_LIST_
      addSlotIntoDesc(r_descPageInfo,r_slotAddrList);
    #endif
    return t_bRet;
  }

  int  t_oldNum=0;///<老页面所占部分申请到的节点数
  t_oldNum = r_slotAddrList.size();
  /////////////////////////////////////////
  PageInfo    *t_prevPage;
  vector<InnerPageInfo> t_pageList; ///<新申请到的页面列表
  vector<InnerPageInfo>::iterator t_pPageItr;
  InnerPageInfo          t_innerPageInfo;
  t_newNum=0;
  try
  {
    //2.如果没有分配到足够的空间，则从表对应的表空间列表中分配
    for(int i=0;i<r_spaceNum && t_needNum>0;i++)
    {
      while(t_needNum>0)
      {
        //申请一页面
        if(m_tableMemMgr.allocatePage(r_spaceList[i],t_pPage,t_pagePos)==true)
        {
          t_innerPageInfo.m_page=t_pPage;
          t_innerPageInfo.m_pos=t_pagePos;
          t_pageList.push_back(t_innerPageInfo);
          t_pPage->initSlotList((char*)t_pPage,r_descPageInfo.m_slotSize);
          t_newNum=0;
          t_pPage->allocateNode((char*)t_pPage,t_needNum,r_slotAddrList,t_newNum);
          t_needNum = t_needNum-t_newNum;
        }else
        { //表空间已满
          break;
        }
      }
    }
    if(t_needNum>0) //回滚
    { //该描述符对应的表空间已满，此次分配不成功
      t_bRet = false;
      throw Mdb_Exception(__FILE__, __LINE__, "没有足够的表空间分配!");
    }else
    {
      ///将新申请到的页面串起来
      if(addNewPageToDesc(r_descPageInfo,t_pageList)==false)
      {
        throw Mdb_Exception(__FILE__, __LINE__, "串页面过程失败!");
      }
      //将申请到的节点串起来
      #ifdef _USEDSLOT_LIST_
        addSlotIntoDesc(r_descPageInfo,r_slotAddrList);
      #endif
      t_pageList.clear();
    }
  }catch (Mdb_Exception &e)
  {
    //需要将新申请到的页面和节点还原
    vector<MdbAddress> t_slotList;
    int i;
    for(i=0;i<t_oldNum;i++)
    { //老页面申请到的节点需要一个个释放
      t_slotList.push_back(r_slotAddrList[i]);
    }
    #ifdef _USEDSLOT_LIST_
      addSlotIntoDesc(r_descPageInfo,t_slotList);
    #endif
    for(i=0;i<t_oldNum;i++)
    { //老页面申请到的节点需要一个个释放
      freeMem(r_descPageInfo,t_slotList[i].m_pos,r_falg);
    }
    r_slotAddrList.clear();
    //释放新申请的页面.........
    for(t_pPageItr=t_pageList.begin();
        t_pPageItr!=t_pageList.end();
        t_pPageItr++)
    {
      m_tableMemMgr.freePage(t_pPageItr->m_pos);
    }
    t_pageList.clear();
    throw e;
  }
  return t_bRet;
}


bool MemManager::freeMem(DescPageInfo &r_descPageInfo,
                         const ShmPosition &r_addr,
                         const int &r_falg)
{
  bool t_bRet = true;
  bool t_flag;
  PageInfo *  t_pPage,*t_prevPage;
  ShmPosition t_pagePos;
    updateMgrInfo(); //更新表空间信息,因为表空间有可能变化
    if(m_tableMemMgr.getPageBySlot(r_addr,t_pagePos,t_pPage)==false)
    {
      t_bRet = false;
      throw Mdb_Exception(__FILE__, __LINE__, "根据页面位置取页面失败!");
    }
#ifdef _USEDSLOT_LIST_
  if(r_falg == 0)
  {//将该Slot从表记录列表中移出
    delSlotFromDesc(r_descPageInfo,r_addr);
  }
#endif
  t_flag = (t_pPage->m_idleNum > 0);
  t_pPage->freeNode((char*)t_pPage,r_addr.getOffSet());
  if(t_flag == false)
  { //需将该页加入空闲队列中
    if(r_descPageInfo.m_lastNotFullPage==NULL_SHMPOS)
    { //第一页
      r_descPageInfo.m_firstNotFullPage = t_pagePos;
      r_descPageInfo.m_lastNotFullPage  = t_pagePos;
    }else
    { //追加在最后一页
      getPageInfo(r_descPageInfo.m_lastNotFullPage,t_prevPage);
      t_prevPage->m_nextNotNullPage=t_pagePos;
      r_descPageInfo.m_lastNotFullPage = t_pagePos;
    }
  }
  return t_bRet;
}

bool MemManager::getIndexListByTable(const TableDesc &r_tablDesc,vector<IndexDesc*> &r_pIndexList)
{
  TableDesc * t_pTableDesc=NULL;
  IndexDesc * t_pIndexDesc=NULL;
  r_pIndexList.clear();
  //1. 找到TableDesc
  if(m_ctlMemMgr.getTableDescByName(r_tablDesc.m_tableDef.m_tableName,t_pTableDesc)==false)
  {
    throw Mdb_Exception(__FILE__, __LINE__, "表不存在!");
    return false;
  }
  for(int i=0;i<t_pTableDesc->m_indexNum;i++)
  {
    m_ctlMemMgr.getIndexDescByPos(t_pTableDesc->m_indexPosList[i],t_pIndexDesc);
    r_pIndexList.push_back(t_pIndexDesc);
  }
  return true;
}
bool MemManager::getIndexListByTable(const char* r_tableName,vector<IndexDef> &r_pIndexList)
{
  TableDesc           t_tableDesc;
  vector<IndexDesc*>  t_pIndexDescList;
  r_pIndexList.clear();
  strcpy(t_tableDesc.m_tableDef.m_tableName,r_tableName);
    getIndexListByTable(t_tableDesc,t_pIndexDescList);
  for(vector<IndexDesc*>::iterator t_itr=t_pIndexDescList.begin();
      t_itr!=t_pIndexDescList.end();t_itr++)
  {
    r_pIndexList.push_back((*t_itr)->m_indexDef);
  }
  return true;
}

bool MemManager::addTableSpace(const char *r_spaceName,const char *r_tableName,const T_TABLETYPE &r_tableType)
{
  DoubleLock *t_pdbLock;
    //1.如果是表，则找到表描述符
    if(r_tableType==SYSTEM_TABLE ||
       r_tableType==DATA_TABLE ||
       r_tableType==LOCK_TABLE)
    {
      TableDesc *t_pTableDesc=NULL;
      getTableDescByName(r_tableName,t_pTableDesc);
      t_pdbLock=InstanceFactory::getDoubLockInstance(SYS_SEMAPORE, m_mdbName);
      t_pdbLock->open(T_DBLK_TABLE,(char*)r_tableName);
      t_pdbLock->getWriteLock();
      if(t_pTableDesc->addSpace(r_spaceName)==false)
      {
        t_pdbLock->releaseWriteLock();
        delete t_pdbLock;
        throw Mdb_Exception(__FILE__, __LINE__, "对表增加表空间失败!");
        return false;
      }
      t_pdbLock->releaseWriteLock();
      delete t_pdbLock;
      return true;  
    }else if(r_tableType==INDEX_TABLE)
    {
      IndexDesc *t_pIndexDesc=NULL;
      //2.如果是索引，则找到索引描述符
      getIndexDescByName(r_tableName,t_pIndexDesc);
      t_pdbLock=InstanceFactory::getDoubLockInstance(SYS_SEMAPORE, m_mdbName);
      t_pdbLock->open(T_DBLK_TABLE,(char*)(t_pIndexDesc->m_indexDef.m_tableName));
      t_pdbLock->getWriteLock();
      if(t_pIndexDesc->addSpace(r_spaceName)==false)
      {
        t_pdbLock->releaseWriteLock();
        delete t_pdbLock;
        throw Mdb_Exception(__FILE__, __LINE__, "对索引增加表空间失败!");
        return false;
      }
      t_pdbLock->releaseWriteLock();
      delete t_pdbLock;
      return true; 
    }else
    {
      #ifdef _DEBUG_
        cout<<"表类型错误!r_tableType="<<r_tableType
            <<" "<<__FILE__<<__LINE__<<endl;
      #endif
      throw Mdb_Exception(__FILE__, __LINE__, "表类型错误!");
      return false;
    }
  return true;
}
bool MemManager::getSpaceListByTable(const char *r_tableName,vector<string> &r_spaceList)
{
  r_spaceList.clear();
  //1. 找到表描述符
  TableDesc *t_pTableDesc=NULL;
    getTableDescByName(r_tableName,t_pTableDesc);
    t_pTableDesc->getSpaceList(r_spaceList);
  return true;
}
bool MemManager::getSpaceListByIndex(const char *r_indexName,vector<string> &r_spaceList)
{
  r_spaceList.clear();
  //1. 找到表描述符
  IndexDesc *t_pIndexDesc=NULL;
  //2.如果是索引，则找到索引描述符
    getIndexDescByName(r_indexName,t_pIndexDesc);
    t_pIndexDesc->getSpaceList(r_spaceList);
  return true;
}

//根据表或索引取对应的内存使用状况
bool MemManager::getTableMemInfo(const char *r_tableName,TbMemInfo &r_tbMemInfo)
{
  r_tbMemInfo.clear();
  T_TABLETYPE t_tableType;
  TableDesc *t_pTableDesc=NULL;
  IndexDesc *t_pIndexDesc=NULL;
  DescPageInfo * t_pDescPageInfo=NULL;
  PageInfo     * t_pPageInfo=NULL;
  try
  {
    getTableDescByName(r_tableName,t_pTableDesc);
    t_tableType = t_pTableDesc->m_tableDef.m_tableType;
    t_pDescPageInfo = &(t_pTableDesc->m_pageInfo);
  }catch (Mdb_Exception &e)
  {
    try
    {
      //2.如果是索引，则找到索引描述符
      getIndexDescByName(r_tableName,t_pIndexDesc);
      t_tableType = INDEX_TABLE;
      t_pDescPageInfo = &(t_pIndexDesc->m_pageInfo);
    }catch(Mdb_Exception &e)
    {
      throw Mdb_Exception(__FILE__, __LINE__, "表名或索引名不存在!");
      return false;
    }
  }
  strcpy(r_tbMemInfo.m_tbName,r_tableName);
  r_tbMemInfo.m_tbType = t_tableType;
  r_tbMemInfo.m_slotSize = t_pDescPageInfo->m_slotSize;
  //1.取占用页面的状态
  ShmPosition  t_pagePos;
  char     * t_phAddr;
  t_pagePos=t_pDescPageInfo->m_firstPage;
//  try
//  {
    while(!(t_pagePos==NULL_SHMPOS))
    {
      getPhAddr(t_pagePos,t_phAddr);
      t_pPageInfo = (PageInfo*)t_phAddr;
      r_tbMemInfo.setPageInfo(t_pPageInfo);
      t_pagePos=t_pPageInfo->m_next;
    }  
    //2.取未满页面状态
    t_pagePos=t_pDescPageInfo->m_firstNotFullPage;
    while(!(t_pagePos==NULL_SHMPOS))
    {
      getPhAddr(t_pagePos,t_phAddr);
      t_pPageInfo = (PageInfo*)t_phAddr;
      r_tbMemInfo.m_notFullPages.push_back(t_pagePos);
      t_pagePos=t_pPageInfo->m_nextNotNullPage;
    }  
//  }catch(Mdb_Exception &e)
//  {
//    throw e;
//    return false;
//  }
  return true;
}

bool MemManager::allocateIdxMem(IndexDesc* &r_pIndexDesc,MdbAddress &r_addr)
{
  int t_num=1;
  vector<MdbAddress> t_addrList;
//  try
//  {
    allocateMem(r_pIndexDesc->m_indexDef.m_spaceNum,r_pIndexDesc->m_indexDef.m_spaceList,
                 r_pIndexDesc->m_pageInfo,t_num,t_addrList,1);
    r_addr = t_addrList[0];
    return true;
//  }catch (Mdb_Exception &e)
//  {
//    throw e;
//    return false;
//  }
}
#ifdef _USEDSLOT_LIST_
void MemManager::addSlotIntoDesc(DescPageInfo       &r_descPageInfo,
                                 vector<MdbAddress> &r_addrList)
{
  if(r_addrList.empty()) return;
  UsedSlot *t_pSlot=NULL;
  UsedSlot *t_prevSlot=NULL;
  vector<MdbAddress>::iterator t_addrItr,t_prevItr=r_addrList.begin();
  //1. 将r_addrList中对应Slot先串起来
  for(t_addrItr=r_addrList.begin();
      t_addrItr!=r_addrList.end();t_addrItr++)
  {
    t_pSlot = (UsedSlot*)(t_addrItr->m_addr);
    if(t_addrItr==r_addrList.begin())
    {//第一个元素
      t_pSlot->m_prev=r_descPageInfo.m_lastSlot;
    }else
    {
      t_pSlot->m_prev=t_prevItr->m_pos;
      t_prevSlot->m_next=t_addrItr->m_pos;
    }
    t_prevItr=t_addrItr;t_prevSlot=t_pSlot;
  }
  //2. 将头和尾位置更新
  if(r_descPageInfo.m_lastSlot!=NULL_SHMPOS)
  {//非第一批
    getSlotByShmPos(r_descPageInfo.m_lastSlot,t_pSlot);
    t_pSlot->m_next = (r_addrList.begin())->m_pos;
  }else 
  {
    r_descPageInfo.m_firstSlot= (r_addrList.begin())->m_pos;
  }
  t_prevSlot->m_next=NULL_SHMPOS;//最后一个的next置为空
  r_descPageInfo.m_lastSlot=t_prevItr->m_pos;//尾位置
  return;
}

void MemManager::delSlotFromDesc(DescPageInfo &r_descPageInfo,const ShmPosition &r_addr)
{
  UsedSlot *t_pSlot=NULL,*t_pOtherSlot=NULL;
  getSlotByShmPos(r_addr,t_pSlot);
  if(t_pSlot->m_prev==NULL_SHMPOS)
  {//第一个
    r_descPageInfo.m_firstSlot=t_pSlot->m_next;
  }else
  {
    getSlotByShmPos(t_pSlot->m_prev,t_pOtherSlot);
    t_pOtherSlot->m_next=t_pSlot->m_next;
  }
  if(t_pSlot->m_next==NULL_SHMPOS)
  { //最后一个
    r_descPageInfo.m_lastSlot=t_pSlot->m_prev;
  }else
  {
    getSlotByShmPos(t_pSlot->m_next,t_pOtherSlot);
    t_pOtherSlot->m_prev = t_pSlot->m_prev;
  }
  return;
}
#endif


bool MemManager::addNewPageToDesc(DescPageInfo &r_descPageInfo,vector<InnerPageInfo> &t_innerPages)
{
  PageInfo *t_pLastPage=NULL;
  PageInfo *t_pLNotFullPage=NULL;
  if(r_descPageInfo.m_lastPage!=NULL_SHMPOS)
  {
    if(getPageInfo(r_descPageInfo.m_lastPage,t_pLastPage)==false)
    {
      #ifdef _DEBUG_
        cout<<"getPageInfo("<<r_descPageInfo.m_lastPage
            <<") false!"<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
  }
  if(r_descPageInfo.m_lastNotFullPage!=NULL_SHMPOS)
  {
    if(getPageInfo(r_descPageInfo.m_lastNotFullPage,t_pLNotFullPage)==false)
    {
      #ifdef _DEBUG_
        cout<<"getPageInfo("<<r_descPageInfo.m_lastNotFullPage
            <<") false!"<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
  }
  
  for(vector<InnerPageInfo>::iterator t_pageItr=t_innerPages.begin();
      t_pageItr!=t_innerPages.end();t_pageItr++)
  {
    t_pageItr->m_page->m_next=NULL_SHMPOS;
    if(r_descPageInfo.m_lastPage==NULL_SHMPOS)
    { //第一页
      r_descPageInfo.m_firstPage = t_pageItr->m_pos;
      r_descPageInfo.m_lastPage  = t_pageItr->m_pos;
      t_pLastPage=t_pageItr->m_page;
    }else
    { //非第一页
      t_pLastPage->m_next = t_pageItr->m_pos;
      r_descPageInfo.m_lastPage=t_pageItr->m_pos;
      t_pLastPage=t_pageItr->m_page;
    }
    t_pageItr->m_page->m_nextNotNullPage=NULL_SHMPOS;
    if(t_pageItr->m_page->m_idleNum <= 0)
    {
      continue;
    }
    if(r_descPageInfo.m_lastNotFullPage==NULL_SHMPOS)
    { //第一页
      r_descPageInfo.m_firstNotFullPage = t_pageItr->m_pos;
      r_descPageInfo.m_lastNotFullPage  = t_pageItr->m_pos;
      t_pLNotFullPage=t_pageItr->m_page;
    }else
    {
      t_pLNotFullPage->m_nextNotNullPage= t_pageItr->m_pos;
      r_descPageInfo.m_lastNotFullPage=t_pageItr->m_pos;
      t_pLNotFullPage=t_pageItr->m_page;
    }
  }
  return true;
}


