#include "TableDescript.h"

void  DescPageInfo::initPos()
{
  m_firstPage= NULL_SHMPOS;
  m_lastPage = NULL_SHMPOS;
  m_firstNotFullPage=NULL_SHMPOS;
  m_lastNotFullPage =NULL_SHMPOS;
//#ifdef _USEDSLOT_LIST_
  m_firstSlot=NULL_SHMPOS;
  m_lastSlot =NULL_SHMPOS;
//#endif 
  
}
//前提：表空间已经创建
//功能：将表空间和表描述符绑定
bool TableDesc::addSpace(const char *r_spaceName)
{
  return m_tableDef.addSpace(r_spaceName);
}


bool TableDesc::setSlotSize()
{
  int  t_slotSize=m_tableDef.getSlotSize();
  if(t_slotSize>sizeof(ShmPosition))
      m_pageInfo.m_slotSize = t_slotSize;
  else
      m_pageInfo.m_slotSize = sizeof(ShmPosition);
  return true;
}

bool TableDesc::addIndex(const size_t &r_indexPos,const char * r_indexName)
{
  m_indexPosList[m_indexNum]=r_indexPos;
  strcpy(m_indexNameList[m_indexNum],r_indexName);
  m_indexNum++;
  return true;
}
bool TableDesc::deleteIndex(const size_t &r_indexPos)
{
  int t_pos;
  for(t_pos=0;t_pos<m_indexNum;t_pos++)
  {
    if(m_indexPosList[t_pos]==r_indexPos)
    {
      break;
    }
  }
  if(t_pos>=m_indexNum)
  {
    #ifdef _DEBUG_
      cout<<"没有找到该索引存在!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  for(;t_pos<m_indexNum-1;t_pos++)
  {
    m_indexPosList[t_pos]=m_indexPosList[t_pos+1];
    memcpy(m_indexNameList[t_pos],m_indexNameList[t_pos+1],sizeof(T_NAMEDEF));
  }
  m_indexNum--;
  return true;
}

void TableDesc::initByTableDef(const TableDef &r_tableDef)
{
  m_tableDef = r_tableDef;
  m_indexNum = 0;
  m_pageInfo.initPos();
  setSlotSize();
  return ;
}
bool IndexDesc::addSpace(const char *r_spaceName)
{
  return m_indexDef.addSpace(r_spaceName);
}

void IndexDesc::initByIndeDef(const IndexDef &r_indexDef)
{
  m_indexDef = r_indexDef;
  m_pageInfo.initPos();
  setSlotSize();
  m_tablePos=0;
  return ;
}
bool IndexDesc::setSlotSize()
{
  int  t_slotSize=m_indexDef.getSlotSize();
  if(t_slotSize>sizeof(ShmPosition))
      m_pageInfo.m_slotSize = t_slotSize;
  else
      m_pageInfo.m_slotSize = sizeof(ShmPosition);
  return true;
}
ostream& operator<<(ostream& os,const TableDesc& r_obj)
{
  string out;
  char cLine[1024];
  os<<"-------------table desc begin-----------------------"<<endl;
  os<<r_obj.m_tableDef;
  sprintf(cLine,"索引数(m_indexNum):%d\n",r_obj.m_indexNum);out+=cLine;
  if (r_obj.m_indexNum > 0)
  {
  	  sprintf(cLine,"%-40s %26s\n","索引名称(m_indexNameList[])","索引位置(m_indexPosList[])");out+=cLine;
  for(int i=0;i<r_obj.m_indexNum;i++)
  {
	  	sprintf(cLine,"%-40s %26ld\n",r_obj.m_indexNameList[i],r_obj.m_indexPosList[i]);out+=cLine;
	    //os<<r_obj.m_indexNameList[i]<<","<<r_obj.m_indexPosList[i]<<endl;
  }
  }
  out += "\n";
  sprintf(cLine,"记录数(m_recordNum):%ld\n",r_obj.m_recordNum);out+=cLine;
  out += "Page信息(m_pageInfo):\n";
  os<<out;
  os<<"  节点的大小    (m_pageInfo.m_slotSize        ) = "<<r_obj.m_pageInfo.m_slotSize<<endl;
  os<<"  第一页        (m_pageInfo.m_firstPage       ) = "<<r_obj.m_pageInfo.m_firstPage<<endl;
  os<<"  最后一页      (m_pageInfo.m_lastPage        ) = "<<r_obj.m_pageInfo.m_lastPage<<endl;
  os<<"  第一个未满页  (m_pageInfo.m_firstNotFullPage) = "<<r_obj.m_pageInfo.m_firstNotFullPage<<endl;
  os<<"  最后一个未满页(m_pageInfo.m_lastNotFullPage ) = "<<r_obj.m_pageInfo.m_lastNotFullPage<<endl;
  os<<"  第一条记录    (m_pageInfo.m_firstSlot       ) = "<<r_obj.m_pageInfo.m_firstSlot<<endl;
  os<<"  最后一条记录  (m_pageInfo.m_lastSlot        ) = "<<r_obj.m_pageInfo.m_lastSlot<<endl;
  os<<"-------------table desc end-----------------------"<<endl;
  return os;
}
ostream& operator<<(ostream& os,const IndexDesc& r_obj)
{
  os<<"-------------index desc begin-----------------------"<<endl;
  os<<r_obj.m_indexDef;
  os<<"归属表位置(m_tablePos)="<<r_obj.m_tablePos<<endl;
  os<<"索引头位置(m_header  )="<<r_obj.m_header<<endl;
  os<<"节点的大小(m_slotSize)="<<r_obj.m_pageInfo.m_slotSize<<endl;
  os<<"Page信息(m_pageInfo):"<<endl;
  os<<"    第一页        (m_pageInfo.m_firstPage       )="<<r_obj.m_pageInfo.m_firstPage<<endl;
  os<<"    最后一页      (m_pageInfo.m_lastPage        )="<<r_obj.m_pageInfo.m_lastPage<<endl;
  os<<"    第一个未满页  (m_pageInfo.m_firstNotFullPage)="<<r_obj.m_pageInfo.m_firstNotFullPage<<endl;
  os<<"    最后一个未满页(m_pageInfo.m_lastNotFullPage )="<<r_obj.m_pageInfo.m_lastNotFullPage<<endl;
  os<<"-------------index desc end-----------------------"<<endl;
  return os;
}

void TableDesc::dumpInfo(ostream &r_os)
{
  r_os<<*this;
}

void IndexDesc::dumpInfo(ostream &r_os)
{
  r_os<<*this;
}
