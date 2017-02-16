// ############################################
// Source file : MutexShmMgr.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#include <sys/errno.h>
#include "MutexShmMgr.h"
#include <algorithm>
#include "Mdb_Exception.h"


vector<MutexShmInfo> MutexShmMgr::m_mutexShmList;
pthread_mutex_t      MutexShmMgr::m_innerMutex=PTHREAD_MUTEX_INITIALIZER;
MutexShmMgr::MutexShmMgr()
{
  
}  
MutexShmMgr::~MutexShmMgr()
{
}
bool MutexShmMgr::getMutexShmAddr(const char * r_dbName,const int &r_mutexNum,void * &r_shmAddr,ProcessMSrcInfo* &r_pMsrcInfo)
{
  bool t_bRet=true;
  
  //加进程内，线程互斥锁  begin
  //1. 从m_mutexShmList中找r_dbName对应的信息
  pthread_mutex_lock(&(MutexShmMgr::m_innerMutex));
  MutexShmInfo t_mutexShmInfo;
  vector<MutexShmInfo>::iterator t_itr;
  strcpy(t_mutexShmInfo.m_dbName,r_dbName);
  t_itr=::lower_bound(m_mutexShmList.begin(),m_mutexShmList.end(),t_mutexShmInfo);
  if(t_itr==m_mutexShmList.end() || !(*t_itr==t_mutexShmInfo))
  {
    t_mutexShmInfo.m_maxNum = r_mutexNum;
    if(createMutexShm(t_mutexShmInfo)==false)
    {
      #ifdef _DEBUG_
        cout<<"createMutexShm false!"<<__FILE__<<__LINE__<<endl;
      #endif
      t_bRet=false;
    }else
    {
      if(setThreadSrcPos(t_mutexShmInfo,r_pMsrcInfo)==false)
      {
        t_bRet=false;
        #ifdef _DEBUG_
          cout<<"setThreadSrcPos false!"<<__FILE__<<__LINE__<<endl;
        #endif
      }
      m_mutexShmList.insert(t_itr,t_mutexShmInfo);
      r_shmAddr  = t_mutexShmInfo.m_shmAddr;
    }
  }else
  {
    r_shmAddr  = t_itr->m_shmAddr;
    if(getThreadSrcPos(*t_itr,r_pMsrcInfo)==false)
    {
      if(setThreadSrcPos(*t_itr,r_pMsrcInfo)==false)
      {
        t_bRet=false;
        #ifdef _DEBUG_
          cout<<"setThreadSrcPos false!"<<__FILE__<<__LINE__<<endl;
        #endif
      }
    }
  }
  //加进程内，线程互斥锁  end
  pthread_mutex_unlock(&(MutexShmMgr::m_innerMutex));
  return t_bRet;
}
  
bool MutexShmMgr::createMutexShm(MutexShmInfo &r_mutexShmInfo)
{
  T_FILENAMEDEF  t_fileName;
  size_t         t_shmSize;
  bool           t_createFlag=false;
  int            file_des;
  
  sprintf(t_fileName,"%s/ctl/%s.lock",getenv(MDB_HOMEPATH.c_str()),r_mutexShmInfo.m_dbName);
  if(access(t_fileName, F_OK) != 0)
  {
    file_des = creat(t_fileName,S_IRUSR|S_IWUSR);
    if(access(t_fileName, F_OK) != 0)
    {
      throw Mdb_Exception(__FILE__, __LINE__, "文件{%s}不存在并无法创建!",t_fileName);
    }
    close(file_des);
  }
  t_shmSize=STATE_BIT_SIZE+(r_mutexShmInfo.m_maxNum)*sizeof(MutexNode);
  t_shmSize+=sizeof(ProcessMSrcInfo)*MAX_PROCESS_NUM;
  key_t  t_pkey = ftok(t_fileName,MUTEX_FTOK_ID);
  
  if(t_pkey<0)
  {
    #ifdef _DEBUG_
      cout<<"t_pkey = "<<t_pkey<<endl;
      cout<<"t_fileName is {" << t_fileName <<"} "<<__FILE__<<__LINE__<< endl;
      cout<<"errno="<<errno<<" "<<strerror(errno)<<endl;
    #endif
    return false;
  }
    
  int    t_shmid;
  if((t_shmid = shmget(t_pkey, t_shmSize, SHM_MODEFLAG))==-1)
  {
	// add by chenm 2009-2-19 15:40:21 如果mdb名称不存在 则不创建锁区共享内存
	T_FILENAMEDEF  t_fileName;
	sprintf(t_fileName,"%s/config/%s.conf",getenv(MDB_HOMEPATH.c_str()),r_mutexShmInfo.m_dbName);
	if(access(t_fileName, F_OK) != 0)
	{
	    throw Mdb_Exception(__FILE__, __LINE__, "数据库{%s}不存在!",t_fileName);
	}	
	// over 2009-2-19 15:40:29

    t_shmid = shmget(t_pkey, t_shmSize, IPC_CREAT | 0666);  
    if(t_shmid < 0)
    {
      if(errno == EEXIST)
      {
        sleep(1);
        if((t_shmid = shmget(t_pkey, t_shmSize, SHM_MODEFLAG))==-1)
        {
            #ifdef _DEBUG_
              cout << "error here " << __FILE__<<__LINE__<< endl;
            #endif
          return false;
        }
      }else
      {
        #ifdef _DEBUG_
          cout << "error here " << __FILE__<<__LINE__<< endl;
        #endif
        return false;
      }
    }else
    {
      t_createFlag=true;
    }
  }
  r_mutexShmInfo.m_shmAddr = (void *)shmat(t_shmid, NULL, 0); 
  if( r_mutexShmInfo.m_shmAddr == (void*)-1)
  {
    r_mutexShmInfo.m_shmAddr=NULL;
    #ifdef _DEBUG_
      cout << "error here " << __FILE__<<__LINE__<< endl;
    #endif
    return false;
  }
  //状态判断
  if(t_createFlag==true)
  {
    memset(r_mutexShmInfo.m_shmAddr,0,t_shmSize);
    //初始化资源信息
    MutexNode* t_pMutex;
    pthread_mutexattr_t t_attr;
    pthread_condattr_t  t_condAttr;
    for(int i=0;i<r_mutexShmInfo.m_maxNum;i++)
    {
      t_pMutex = (MutexNode*)((char*)(r_mutexShmInfo.m_shmAddr)
                              +STATE_BIT_SIZE
                              +sizeof(MutexNode)*i);
      //初始化*t_pMutex
      if(pthread_mutexattr_init(&t_attr)!=0)
      {
	    #ifdef _DEBUG_
	      cout << "error here " << __FILE__<<__LINE__<<"  " <<errno<<endl;
	    #endif
      	return false;
      }
      pthread_mutexattr_setpshared(&t_attr, PTHREAD_PROCESS_SHARED);
      pthread_mutex_init(&(t_pMutex->m_mutex), &t_attr);
      pthread_mutexattr_destroy(&t_attr);
      pthread_condattr_init(&t_condAttr);
      pthread_condattr_setpshared(&t_condAttr,PTHREAD_PROCESS_SHARED);
      pthread_cond_init(&(t_pMutex->m_cond), &t_condAttr);
      pthread_condattr_destroy(&t_condAttr);
      t_pMutex->m_rdCount=0;
      t_pMutex->m_wtCount=0;
      t_pMutex->m_wtWait=0;
    }
    memcpy(r_mutexShmInfo.m_shmAddr,(char*)"1",1);
  }else
  {
    while(memcmp(r_mutexShmInfo.m_shmAddr,(char*)"1",1)!=0)
    {
      usleep(1);
    }
  }
  return true;
}
bool MutexShmMgr::deleteMutex(const  char * r_dbName,const int &r_mutexNum)
{
  MutexShmInfo t_mutexShmInfo;
  vector<MutexShmInfo>::iterator t_itr;
  strcpy(t_mutexShmInfo.m_dbName,r_dbName);
  t_itr=::lower_bound(m_mutexShmList.begin(),m_mutexShmList.end(),t_mutexShmInfo);
  if(t_itr==m_mutexShmList.end() || !(*t_itr==t_mutexShmInfo))
  {
  }else
  {
    //先detach
    shmdt(t_itr->m_shmAddr);
    m_mutexShmList.erase(t_itr);
  }
  key_t t_key;
  int   t_shmid,t_ret;
  void * t_shmAddr;
  MutexNode *t_pNode;
  //获取t_key
  T_FILENAMEDEF  t_fileName;
  size_t         t_shmSize;
  sprintf(t_fileName,"%s/ctl/%s.lock",getenv(MDB_HOMEPATH.c_str()),r_dbName);
  t_shmSize=STATE_BIT_SIZE+(r_mutexNum)*sizeof(MutexNode);
  t_shmSize+=sizeof(ProcessMSrcInfo)*MAX_PROCESS_NUM;
  
  t_key = ftok(t_fileName,MUTEX_FTOK_ID);
  if(t_key<0) return false;
  //获取t_shmid
  t_shmid = shmget(t_key,t_shmSize,0666);
  if(t_shmid<0) return false;
  //获取shmAddr
  t_shmAddr = (void *)shmat(t_shmid, NULL, 0); 
  int t_intRet;
  //将信号量destory
  for(int i=0;i<r_mutexNum;i++)
  {
    t_pNode=(MutexNode*) ((char*)t_shmAddr+STATE_BIT_SIZE+sizeof(MutexNode)*i);
    pthread_mutex_destroy(&(t_pNode->m_mutex));
    pthread_cond_destroy(&(t_pNode->m_cond));
  }
  shmdt(t_shmAddr);
  //delete
  t_ret   = shmctl(t_shmid,IPC_RMID,(struct shmid_ds*)0);
  if(t_ret < 0)
  {
    #ifdef _DEBUG_
      cout<<"释放共享内存失败!"
          <<" "<<__FILE__<<":"<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}

bool MutexShmMgr::setThreadSrcPos(MutexShmInfo &r_mutexShmInfo,ProcessMSrcInfo* &r_pMsrcInfo)
{
  MutexNode        * t_pMutexNode;
  pthread_mutex_t  * t_pMutex;
  ProcessMSrcInfo  * t_pMsrcInfo;
  ProcessMSrcInfo    t_mSrcInfo;
  
  t_mSrcInfo.m_state = 1;
  t_mSrcInfo.m_pid   = getpid();
  t_mSrcInfo.m_thId  = pthread_self();
  time(&(t_mSrcInfo.m_pTime));
  t_mSrcInfo.m_srcNum= 0;
  t_pMutexNode = (MutexNode*)((char*)(r_mutexShmInfo.m_shmAddr)+STATE_BIT_SIZE);
  t_pMutex     = &(t_pMutexNode->m_mutex);
  t_pMsrcInfo  = (ProcessMSrcInfo*)((char*)(r_mutexShmInfo.m_shmAddr)+STATE_BIT_SIZE
                                    +sizeof(MutexNode)*r_mutexShmInfo.m_maxNum);
  
  pthread_mutex_lock(t_pMutex);
  for(int i=0;i<MAX_PROCESS_NUM;i++)
  {
    if(t_pMsrcInfo->m_state == 0)
    {
      memcpy(t_pMsrcInfo,&t_mSrcInfo,sizeof(ProcessMSrcInfo));
      r_pMsrcInfo = t_pMsrcInfo;
      pthread_mutex_unlock(t_pMutex);  
      return true;
    }else
    {
      t_pMsrcInfo++;
      continue;
    }
  }
  pthread_mutex_unlock(t_pMutex);    
  return false;
}
bool MutexShmMgr::getThreadSrcPos(const MutexShmInfo &r_mutexShmInfo,ProcessMSrcInfo* &r_pMsrcInfo)
{
  MutexNode        * t_pMutexNode;
  pthread_mutex_t  * t_pMutex;
  ProcessMSrcInfo  * t_pMsrcInfo;
  
  pid_t     t_pid   = getpid();
  pthread_t t_thId  = pthread_self();
  
  t_pMutexNode = (MutexNode*)((char*)(r_mutexShmInfo.m_shmAddr)+STATE_BIT_SIZE);
  t_pMutex     = &(t_pMutexNode->m_mutex);
  t_pMsrcInfo  = (ProcessMSrcInfo*)((char*)(r_mutexShmInfo.m_shmAddr)+STATE_BIT_SIZE
                                    +sizeof(MutexNode)*r_mutexShmInfo.m_maxNum);
  
  pthread_mutex_lock(t_pMutex);
  for(int i=0;i<MAX_PROCESS_NUM;i++)
  {
    if(t_pMsrcInfo->m_pid == t_pid &&
       t_pMsrcInfo->m_thId== t_thId&&
       t_pMsrcInfo->m_state!=0)
    {
      r_pMsrcInfo = t_pMsrcInfo;
      pthread_mutex_unlock(t_pMutex);   
      return true;
    }else
    {
      t_pMsrcInfo++;
      continue;
    }
  }  
  pthread_mutex_unlock(t_pMutex);    
  return false;  
}
bool MutexShmMgr::delThreadSrcInfo(const char *r_dbName)
{
  MutexShmInfo  t_shmInfo;
  bool          t_flag = false;
  //1. 找到现有的列表中找到对应内存信息
  pthread_mutex_lock(&(MutexShmMgr::m_innerMutex));
  for(vector<MutexShmInfo>::iterator t_itr=m_mutexShmList.begin();
      t_itr!=m_mutexShmList.end();t_itr++)
  {
    if(t_itr->m_dbName == r_dbName)
    {
      t_shmInfo=*t_itr;t_flag=true;break;
    }
  }
  pthread_mutex_unlock(&(MutexShmMgr::m_innerMutex));
  if(t_flag==false) return false;
    
  MutexNode        * t_pMutexNode;
  pthread_mutex_t  * t_pMutex;
  ProcessMSrcInfo  * t_pMsrcInfo;
  
  pid_t     t_pid   = getpid();
  pthread_t t_thId  = pthread_self();
  
  t_pMutexNode = (MutexNode*)((char*)(t_shmInfo.m_shmAddr)+STATE_BIT_SIZE);
  t_pMutex     = &(t_pMutexNode->m_mutex);
  t_pMsrcInfo  = (ProcessMSrcInfo*)((char*)(t_shmInfo.m_shmAddr)+STATE_BIT_SIZE
                                    +sizeof(MutexNode)*t_shmInfo.m_maxNum);
  
  pthread_mutex_lock(t_pMutex);
  for(int i=0;i<MAX_PROCESS_NUM;i++)
  {
    if(t_pMsrcInfo->m_pid == t_pid &&
       t_pMsrcInfo->m_thId== t_thId)
    {
      memset(t_pMsrcInfo,0,sizeof(ProcessMSrcInfo));
      pthread_mutex_unlock(t_pMutex);    
      return true;
    }else
    {
      t_pMsrcInfo++;
      continue;
    }
  }
  pthread_mutex_unlock(t_pMutex);    
  return false;
}
bool MutexShmMgr::lockRdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId)
{
  for(int i=0;i<r_pMsrcInfo->m_srcNum;i++)
  {
    if(r_pMsrcInfo->m_srcInfo[i].m_sid == r_srcId)
    {
      ++(r_pMsrcInfo->m_srcInfo[i].m_rcount);
      time(&(r_pMsrcInfo->m_srcInfo[i].m_time));
      return true;
    }
  }
  if(r_pMsrcInfo->m_srcNum >= MAX_PSRC_NUM)
  {
    #ifdef _DEBUG_
      cout<<"r_pMsrcInfo->m_srcNum="<<r_pMsrcInfo->m_srcNum
          <<" "<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  r_pMsrcInfo->m_srcInfo[r_pMsrcInfo->m_srcNum].m_sid = r_srcId;
  r_pMsrcInfo->m_srcInfo[r_pMsrcInfo->m_srcNum].m_rcount=1;
  r_pMsrcInfo->m_srcInfo[r_pMsrcInfo->m_srcNum].m_wcount=0;
  r_pMsrcInfo->m_srcInfo[r_pMsrcInfo->m_srcNum].m_wwait=0;
  time(&(r_pMsrcInfo->m_srcInfo[r_pMsrcInfo->m_srcNum].m_time));
  ++(r_pMsrcInfo->m_srcNum);
  return true;
}
bool MutexShmMgr::lockWdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId)
{
  for(int i=0;i<r_pMsrcInfo->m_srcNum;i++)
  {
    if(r_pMsrcInfo->m_srcInfo[i].m_sid == r_srcId)
    {
      ++(r_pMsrcInfo->m_srcInfo[i].m_wcount);
      time(&(r_pMsrcInfo->m_srcInfo[i].m_time));
      return true;
    }
  }
  if(r_pMsrcInfo->m_srcNum >= MAX_PSRC_NUM)
  {
    #ifdef _DEBUG_
      cout<<"r_pMsrcInfo->m_srcNum="<<r_pMsrcInfo->m_srcNum
          <<" "<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  r_pMsrcInfo->m_srcInfo[r_pMsrcInfo->m_srcNum].m_sid = r_srcId;
  r_pMsrcInfo->m_srcInfo[r_pMsrcInfo->m_srcNum].m_rcount=0;
  r_pMsrcInfo->m_srcInfo[r_pMsrcInfo->m_srcNum].m_wcount=1;
  r_pMsrcInfo->m_srcInfo[r_pMsrcInfo->m_srcNum].m_wwait=0;
  time(&(r_pMsrcInfo->m_srcInfo[r_pMsrcInfo->m_srcNum].m_time));
  ++(r_pMsrcInfo->m_srcNum);
  return true;
}
bool MutexShmMgr::unlockRdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId)
{
  int i;
  for(i=0;i<r_pMsrcInfo->m_srcNum;i++)
  {
    if(r_pMsrcInfo->m_srcInfo[i].m_sid == r_srcId)
    {
      --(r_pMsrcInfo->m_srcInfo[i].m_rcount);
      break;
    }
  }
  if(i >= r_pMsrcInfo->m_srcNum)
  {
    return false;
  }
  if(r_pMsrcInfo->m_srcInfo[i].m_rcount+
     r_pMsrcInfo->m_srcInfo[i].m_wcount+
     r_pMsrcInfo->m_srcInfo[i].m_wwait> 0)
  {
    return true;
  }
  for(;i<r_pMsrcInfo->m_srcNum-1;i++)
  {
    memcpy(&(r_pMsrcInfo->m_srcInfo[i]),&(r_pMsrcInfo->m_srcInfo[i+1]),sizeof(MsrcInfo));
  }
  --(r_pMsrcInfo->m_srcNum);
  return true;
}
bool MutexShmMgr::unlockWdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId)
{
  int i;
  for(i=0;i<r_pMsrcInfo->m_srcNum;i++)
  {
    if(r_pMsrcInfo->m_srcInfo[i].m_sid == r_srcId)
    {
      --(r_pMsrcInfo->m_srcInfo[i].m_wcount);
      break;
    }
  }
  if(i >= r_pMsrcInfo->m_srcNum)
  {
    return false;
  }
  if(r_pMsrcInfo->m_srcInfo[i].m_rcount+
     r_pMsrcInfo->m_srcInfo[i].m_wcount+
     r_pMsrcInfo->m_srcInfo[i].m_wwait> 0)
  {
    return true;
  }
  for(;i<r_pMsrcInfo->m_srcNum-1;i++)
  {
    memcpy(&(r_pMsrcInfo->m_srcInfo[i]),&(r_pMsrcInfo->m_srcInfo[i+1]),sizeof(MsrcInfo));
  }
  --(r_pMsrcInfo->m_srcNum);
  return true;
}


bool MutexShmMgr::lockWWaitSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId)
{
  for(int i=0;i<r_pMsrcInfo->m_srcNum;i++)
  {
    if(r_pMsrcInfo->m_srcInfo[i].m_sid == r_srcId)
    {
      ++(r_pMsrcInfo->m_srcInfo[i].m_wwait);
      time(&(r_pMsrcInfo->m_srcInfo[i].m_time));
      return true;
    }
  }
  if(r_pMsrcInfo->m_srcNum >= MAX_PSRC_NUM)
  {
    #ifdef _DEBUG_
      cout<<"r_pMsrcInfo->m_srcNum="<<r_pMsrcInfo->m_srcNum
          <<" "<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  r_pMsrcInfo->m_srcInfo[r_pMsrcInfo->m_srcNum].m_sid = r_srcId;
  r_pMsrcInfo->m_srcInfo[r_pMsrcInfo->m_srcNum].m_rcount=0;
  r_pMsrcInfo->m_srcInfo[r_pMsrcInfo->m_srcNum].m_wcount=0;
  r_pMsrcInfo->m_srcInfo[r_pMsrcInfo->m_srcNum].m_wwait=1;
  time(&(r_pMsrcInfo->m_srcInfo[r_pMsrcInfo->m_srcNum].m_time));
  ++(r_pMsrcInfo->m_srcNum);
  return true;
}

bool MutexShmMgr::unlockWWaitSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId)
{
  int i;
  for(i=0;i<r_pMsrcInfo->m_srcNum;i++)
  {
    if(r_pMsrcInfo->m_srcInfo[i].m_sid == r_srcId)
    {
      --(r_pMsrcInfo->m_srcInfo[i].m_wwait);
      break;
    }
  }
  if(i >= r_pMsrcInfo->m_srcNum)
  {
    return false;
  }
  if(r_pMsrcInfo->m_srcInfo[i].m_rcount+
     r_pMsrcInfo->m_srcInfo[i].m_wcount+
     r_pMsrcInfo->m_srcInfo[i].m_wwait> 0)
  {
    return true;
  }
  for(;i<r_pMsrcInfo->m_srcNum-1;i++)
  {
    memcpy(&(r_pMsrcInfo->m_srcInfo[i]),&(r_pMsrcInfo->m_srcInfo[i+1]),sizeof(MsrcInfo));
  }
  --(r_pMsrcInfo->m_srcNum);
  return true;
}

