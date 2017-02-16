
#include <stdio.h>
#include "FILEImpl.h"
#include "Utility.h"
#include <iostream>


const int MAXFILELIMIT = 3000;     

FILEImpl::FILEImpl()
{
}

FILEImpl::~FILEImpl()
{
}

int FILEImpl::initialize()
{
  m_fSortMethod = Directory::SF_MODIFYTIME;
  m_maxFileLimit = MAXFILELIMIT;
  
  return 0;
}

int FILEImpl::connect(const string & host, const string & port,
                      const string & user, const string & passwd)
{
  return 0;
}

int FILEImpl::disconnect()
{
  return 0;
}

int FILEImpl::getFile(const string & source, const string & dest)
{
  if (Utility::linkFile(source, dest) < 0)
    return -1;
    
  return 0;
}

int FILEImpl::putFile(const string & dest, const string & source)
{
  if (Utility::linkFile(dest, source) < 0)
    return -1;
    
  return 0;
}

int FILEImpl::setMode(const int & mode)
{
  switch(mode) {
    default:
//      printf("mode does not exits\n");
      return -1;
  }
        
  return 0;
}

int FILEImpl::setOption(const int & type, const int & value)
{
  switch(type) {
    //传输文件数设置
    case MAXTRANSFILE:
      m_maxFileLimit = value;
      m_dir.setMaxScan(value); 
    default:
//      printf("option does not exits\n");
      return -1;
  }

  return 0;
}

int FILEImpl::changeDirectory(const string & dir)
{
  m_dir.setPath(dir.c_str());
  return m_dir.scanFiles(m_fSortMethod);
}

int FILEImpl::listDirectory(const string & dir, vector<string> & files, 
                            const string & pattern)
{
  int i,count;
  char fileName[80];
  vector<string> fileVector;
    
  changeDirectory(dir);
  files.clear();

  count = getFile(fileName, 80);

  if (count > 0) {
    fileVector.push_back(fileName);
  }
  
  for (i = 1; i < count && getFile(fileName, 80) > 0; i++)
  {
    fileVector.push_back(fileName);
  }
  
  Utility::regx(fileVector, files, pattern);

  if ((int)files.size() > m_maxFileLimit)
    files.resize(m_maxFileLimit);
      
  return 0;
}

int FILEImpl::getFileInfo(const string & file, unsigned long & size)
{
  struct stat myStat;
  
  if (::stat(file.c_str(), &myStat) < 0) {
    return -1;
  }
  
  size = myStat.st_size;
  return 0;
}
//新增根据特定结束文件标识,获取此文件时间前的文件列表  add by xiel  2009.10.30
int FILEImpl::getFileTime(const string & file, time_t & time)
{
  struct stat myStat;
  
  if (::stat(file.c_str(), &myStat) < 0) {
    return -1;
  }
  
  time = myStat.st_mtime;
  return 0;
}

int FILEImpl::delFile(const string & file)
{
  if ( Utility::unLinkFile(file) < 0 )
    return -1;
    
  return 0;
}

int FILEImpl::rename(const string & old_file, const string & new_file)
{
  if (Utility::renameFile(old_file.c_str(), new_file.c_str()) < 0)
    return -1;
  
  return 0;
}

int FILEImpl::end()
{
  return 0;
}

void FILEImpl::getErrInfo(int & code, string & msg)
{

}

int FILEImpl::getFile(char *path, const int length)
{
    unsigned long size;
    return getFile(path, length, size);
}

int FILEImpl::getFile(char *path, const int length , unsigned long & filesize)
{
    int ret;
  
    if ((ret = m_dir.getFile(path, length, filesize)) != 0)
        return ret;

    m_dir.scanFiles(m_fSortMethod);
    return m_dir.getFile(path, length, filesize);
}

void FILEImpl::setFileSortMethod(Directory::SortFlags sortmethod)
{
    m_fSortMethod = sortmethod;
    m_dir.scanFiles(m_fSortMethod);
}

