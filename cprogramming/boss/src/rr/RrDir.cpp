#include "RrDir.h"
#include <fstream>
#include <cstring>
#include <cstdio>
#include <algorithm>

/**
 * 扫描当前目录取得文件列表
 * @param sortBy 使用的排序方法
 * @param level 处理深度
 * @return 0 - OK, other - error
 */
int RrDir::scanFiles(const vector<string> &r_dealedFileList,const SortFlags sortby,const int level)
{
  int t_iRet=Directory::scanFiles(sortby,level);
  bool t_flag = false;
  /*
  if(t_iRet!=0)
  {
    m_filesArray.clear();
    return t_iRet;
  }
  */
  m_dealFlagList.clear();
  m_curFileNum=m_filesArray.size();
  m_firstNoDealPos=0;
  for(size_t i=0;i<m_curFileNum;++i)
  {
    t_flag = false;
    for(vector<string>::const_iterator t_itr=r_dealedFileList.begin();
        t_itr!=r_dealedFileList.end();++t_itr)
    {
      if(strcmp(t_itr->c_str(),m_filesArray[i].fs_name)==0)
      {
        t_flag = true;break;
      }
    }
    //m_dealFlagList.push_back(false);
    m_dealFlagList.push_back(t_flag);
    if(m_dealFlagList[m_firstNoDealPos] == true)
    {
      m_firstNoDealPos = i;
    }
  }
  m_curFilePos    =0;
  return t_iRet;
}

// 0 - no file, >0 - get file success, <0 - something error
int RrDir::getFile(char *r_shortname, const int r_len ,time_t& r_fileTime)
{
  if (m_pos >= m_curFileNum)
  {
    return 0;
  }
  if (strlen(m_filesArray[m_pos].fs_name) >= r_len)
  {
    return -1;
  }
  if(m_dealFlagList[m_pos] == false)
  {
    m_curFilePos=m_pos;
    strcpy(r_shortname, m_filesArray[m_pos].fs_name);
    r_fileTime = m_filesArray[m_pos].fs_mtime;
    m_pos++;
    return m_curFileNum;
  }else
  {
    m_pos++;
    return getFile(r_shortname,r_len,r_fileTime);
  }
}

//是下一个处理的文件为第一个未处理文件
void  RrDir::rewind()
{
  m_pos = m_firstNoDealPos;
}
//将当前处理的文件打上已处理标记
void  RrDir::setDealFlag()
{
  m_dealFlagList[m_curFilePos] = true;
  if(m_firstNoDealPos==m_curFilePos)
  {
    for(;m_firstNoDealPos<m_curFileNum;++m_firstNoDealPos)
    {
      if(m_dealFlagList[m_firstNoDealPos]==false)
      {
        break;
      }
    }
  } 
} 

