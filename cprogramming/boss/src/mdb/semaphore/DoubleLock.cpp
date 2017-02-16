// ############################################
// Source file : DoubleLock.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#include "DoubleLock.h"
#include <stdlib.h>
#include <fcntl.h>
#include "Mdb_Exception.h"

DoubleLock::DoubleLock()
{
}
//##ModelId=481027F403BA
DoubleLock::DoubleLock(const char * dbname)
{
    strcpy(m_dbName,dbname);
    sprintf(m_fileName,"%s/ctl/%s.lock",getenv(MDB_HOMEPATH.c_str()),dbname);
}

//##ModelId=481027F403BD
DoubleLock::~DoubleLock()
{
}

int DoubleLock::getInnerSrcId(const char * fileName,int resourceType, char *resourceID)
{ //该函数一台主机只允许同时，只有一个进程中的一个线程执行

  T_NAMEDEF    t_lockFile;
  sprintf(t_lockFile,"%s.lock",m_dbName);
  int t_innerId=-1;
  //加写锁
  try
  {
    //1. 如果文件不存在,则创建文件
    int file_des;
    if(access(m_fileName, F_OK) != 0)
    {
      file_des = creat(m_fileName,S_IRUSR|S_IWUSR);
      if(access(m_fileName, F_OK) != 0)
      {
        throw Mdb_Exception(__FILE__, __LINE__, "文件{%s}不存在并无法创建!",m_fileName);
      }
      close(file_des);
    }
    
    int t_fd;
    //if((t_fd=::open(m_fileName,O_RDWR|O_NSHARE|O_DELAY))<0) //HP和linux不支持O_NSHARE|O_DELAY
    if((t_fd=::open(m_fileName,O_RDWR))<0)
    {
      throw Mdb_Exception(__FILE__, __LINE__, "打开文件{%s}失败!",m_fileName);
    }
    struct flock *ret ;
    ret = new struct flock();//add struct key word,by chenzm for hp-ux
    ret->l_type = F_WRLCK;
    if( fcntl(t_fd, F_SETLKW, ret)!= 0) //如果他人加上文件锁，则阻塞等待他人解锁
    {
    	delete 	ret;
    	throw Mdb_Exception(__FILE__, __LINE__, "锁文件{%s}失败!",m_fileName);
    }
    
    InnerSrc t_innsrc;
    int      t_maxInnerSrcId = 1;
    while(::read(t_fd,&t_innsrc,sizeof(InnerSrc))==sizeof(InnerSrc))
    {
      if(t_innsrc.m_sourceType==resourceType &&
         strcasecmp(t_innsrc.m_sourceName,resourceID)==0)
      {
        t_innerId = t_innsrc.m_sourceId;
        break;
      }else
      {
        if(t_innsrc.m_sourceId>t_maxInnerSrcId)
          t_maxInnerSrcId=t_innsrc.m_sourceId;
      }
    };
    if(t_innerId<0)
    {
      //如果不存在，则记录最大的内部资源ID，并将新的资源加入
      t_innerId = t_maxInnerSrcId+1;
      if(lseek(t_fd,0,SEEK_END)<0)
      {
        close(t_fd);
        throw Mdb_Exception(__FILE__, __LINE__, "lseek{%s}失败!",m_fileName);
      }
      t_innsrc.m_sourceType = resourceType;
      strcpy(t_innsrc.m_sourceName,resourceID);
      t_innsrc.m_sourceId=t_innerId;
      if(write(t_fd,&t_innsrc,sizeof(InnerSrc))!=sizeof(InnerSrc))
      {
        close(t_fd);
        throw Mdb_Exception(__FILE__, __LINE__, "write{%s}失败!",m_fileName);
      }
    }
    delete 	ret;
    close(t_fd);
  }catch(LINKAGE_MDB::Mdb_Exception e)
  {
    t_innerId=-1;
    #ifdef _DEBUG_
      cout<<e.GetString()<<endl;
    #endif
  }
  return t_innerId;
}
