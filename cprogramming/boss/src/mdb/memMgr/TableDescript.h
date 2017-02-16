#ifndef _TABLEDESCRIPT_H_INCLUDE_20080411_
#define _TABLEDESCRIPT_H_INCLUDE_20080411_

#include "TableDefParam.h"

class DescPageInfo
{
  public:
    int            m_slotSize;  ///<每个节点的大小（包含节点的所有信息）
    //表内存地址信息
    ShmPosition    m_firstPage;   ///<第一页
    ShmPosition    m_lastPage;    ///<最后一页
    ShmPosition    m_firstNotFullPage;  ///<第一个未满页
    ShmPosition    m_lastNotFullPage;   ///<最后一个未满页
  //#ifdef _USEDSLOT_LIST_
    ShmPosition    m_firstSlot; ///<第一条记录
    ShmPosition    m_lastSlot;  ///<最后一条记录
  //#endif 
  public:
    void  initPos();
};


class TableDesc//表定义信息
{
  public:
    TableDef       m_tableDef;
    int            m_indexNum;   ///<索引数
    size_t         m_indexPosList[MAX_INDEX_NUM]; ///<组成Index列表
    T_NAMEDEF      m_indexNameList[MAX_INDEX_NUM];///<索引名称
    DescPageInfo   m_pageInfo;
    size_t         m_recordNum;
 public:
    TableDesc(){m_recordNum=0;}
    virtual ~TableDesc(){}
  public:
    friend int operator<(const TableDesc &left,const TableDesc &right)
    {
      return (left.m_tableDef<right.m_tableDef);
    }

    friend int operator==(const TableDesc &left,const TableDesc &right)
    {
      return (left.m_tableDef==right.m_tableDef);
    }
    friend ostream& operator<<(ostream& os,const TableDesc& r_obj);
  public:
    //功能：将表空间和表描述符绑定
    bool addSpace(const char *r_spaceName);
    void getSpaceList(vector<string> &r_spaceList)
    {
      m_tableDef.getSpaceList(r_spaceList);
    }
    bool setSlotSize();
    bool addIndex(const size_t &r_indexPos,const char * r_indexName);
    bool deleteIndex(const size_t &r_indexPos);
    void initByTableDef(const TableDef &r_tableDef);
  public:
    void dumpInfo(ostream &r_os);
};

class IndexDesc
{
  public:
    IndexDef       m_indexDef; 
    size_t         m_tablePos;  ///<归属表位置
    DescPageInfo   m_pageInfo;  
    ShmPosition    m_header;    ///<索引头：hash索引为hash头，T-Tree为根
  public:
    friend int operator<(const IndexDesc &left,const IndexDesc &right)
    {
      return (left.m_indexDef<right.m_indexDef);
    }

    friend int operator==(const IndexDesc &left,const IndexDesc &right)
    {
      return (left.m_indexDef==right.m_indexDef);
    }
    friend ostream& operator<<(ostream& os,const IndexDesc& r_obj);
  public:
    IndexDesc(){}
    virtual ~IndexDesc(){}
    //前提：表空间已经创建
    //功能：将表空间和表描述符绑定
    bool addSpace(const char *r_spaceName);
    void getSpaceList(vector<string> &r_spaceList)
    {
      m_indexDef.getSpaceList(r_spaceList);
    }
    virtual bool setSlotSize();
    void initByIndeDef(const IndexDef &r_indexDef);
  public:
    void dumpInfo(ostream &r_os);
};


#endif //_TABLEDESCRIPT_H_INCLUDE_20080411_


