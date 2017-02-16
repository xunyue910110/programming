#include "RwlockShmMgr.h"
#include <algorithm>

vector<RwlockShmInfo> RwlockShmMgr::m_RwlockShmList;
pthread_rwlock_t      RwlockShmMgr::m_innerRwlock=PTHREAD_RWLOCK_INITIALIZER;
RwlockShmMgr::RwlockShmMgr()
{
  
}  
RwlockShmMgr::~RwlockShmMgr()
{
}
bool RwlockShmMgr::getRwlockShmAddr(const char * r_dbName,const int &r_RwlockNum,void * &r_shmAddr,ProcessMSrcInfo* &r_pMsrcInfo)
{
  bool t_bRet=true;
  
  //加进程内，线程读写锁  begin
  //1. 从m_RwlockShmList中找r_dbName对应的信息
  pthread_rwlock_wrlock(&(RwlockShmMgr::m_innerRwlock));
  RwlockShmInfo t_RwlockShmInfo;
  vector<RwlockShmInfo>::iterator t_itr;
  strcpy(t_RwlockShmInfo.m_dbName,r_dbName);
  t_itr=::lower_bound(m_RwlockShmList.begin(),m_RwlockShmList.end(),t_RwlockShmInfo);
  if(t_itr==m_RwlockShmList.end() || !(*t_itr==t_RwlockShmInfo))
  {
    t_RwlockShmInfo.m_maxNum = r_RwlockNum;
    if(createRwlockShm(t_RwlockShmInfo)==false)
    {
      #ifdef _DEBUG_
        cout<<"createRwlockShm false!"<<__FILE__<<__LINE__<<endl;
      #endif
      t_bRet=false;
    }else
    {
      if(setThreadSrcPos(t_RwlockShmInfo,r_pMsrcInfo)==false)
      {
        t_bRet=false;
        #ifdef _DEBUG_
          cout<<"setThreadSrcPos false!"<<__FILE__<<__LINE__<<endl;
        #endif
      }
      m_RwlockShmList.insert(t_itr,t_RwlockShmInfo);
      r_shmAddr  = t_RwlockShmInfo.m_shmAddr;
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
  //加进程内，线程读写锁  end
  pthread_rwlock_unlock(&(RwlockShmMgr::m_innerRwlock));
  return t_bRet;
}
  
bool RwlockShmMgr::createRwlockShm(RwlockShmInfo &r_RwlockShmInfo)
{
  T_FILENAMEDEF  t_fileName;
  size_t         t_shmSize;
  bool           t_createFlag=false;
  
  sprintf(t_fileName,"%s/ctl/%s.lock",getenv(MDB_HOMEPATH.c_str()),r_RwlockShmInfo.m_dbName);
  t_shmSize=STATE_BIT_SIZE+(r_RwlockShmInfo.m_maxNum)*sizeof(RwlockNode);
  t_shmSize+=sizeof(ProcessMSrcInfo)*MAX_PROCESS_NUM;
  key_t  t_pkey = ftok(t_fileName,RWLOCK_FTOK_ID);
  
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
  r_RwlockShmInfo.m_shmAddr = (void *)shmat(t_shmid, NULL, 0); 
  if( r_RwlockShmInfo.m_shmAddr == (void*)-1)
  {
    r_RwlockShmInfo.m_shmAddr=NULL;
    #ifdef _DEBUG_
      cout << "error here " << __FILE__<<__LINE__<< endl;
    #endif
    return false;
  }
  //状态判断
  if(t_createFlag==true)
  {
    memset(r_RwlockShmInfo.m_shmAddr,0,t_shmSize);
    //初始化资源信息
    RwlockNode* t_pRwlock;
    pthread_rwlockattr_t t_attr;
    pthread_condattr_t  t_condAttr;
    for(int i=0;i<r_RwlockShmInfo.m_maxNum;i++)
    {
      t_pRwlock = (RwlockNode*)((char*)(r_RwlockShmInfo.m_shmAddr)
                              +STATE_BIT_SIZE
                              +sizeof(RwlockNode)*i);
      //初始化*t_pRwlock
      pthread_rwlockattr_init(&t_attr);
      pthread_rwlockattr_setpshared(&t_attr, PTHREAD_PROCESS_SHARED);
      pthread_rwlock_init(&(t_pRwlock->m_rwlock), &t_attr);
      pthread_rwlockattr_destroy(&t_attr);
      //pthread_condattr_init(&t_condAttr);
      //pthread_condattr_setpshared(&t_condAttr,PTHREAD_PROCESS_SHARED);
      //pthread_cond_init(&(t_pRwlock->m_cond), &t_condAttr);
      //pthread_condattr_destroy(&t_condAttr);
      t_pRwlock->m_rdCount = 0;
      t_pRwlock->m_wtCount = 0;
      t_pRwlock->m_wtWait  = 0;
    }
    memcpy(r_RwlockShmInfo.m_shmAddr,(char*)"1",1);
  }else
  {
    while(memcmp(r_RwlockShmInfo.m_shmAddr,(char*)"1",1)!=0)
    {
      usleep(1);
    }
  }
  return true;
}
bool RwlockShmMgr::deleteRwlock(const  char * r_dbName,const int &r_RwlockNum)
{
  RwlockShmInfo t_RwlockShmInfo;
  vector<RwlockShmInfo>::iterator t_itr;
  strcpy(t_RwlockShmInfo.m_dbName,r_dbName);
  t_itr=::lower_bound(m_RwlockShmList.begin(),m_RwlockShmList.end(),t_RwlockShmInfo);
  if(t_itr==m_RwlockShmList.end() || !(*t_itr==t_RwlockShmInfo))
  {
  }else
  {
    //先detach
    shmdt(t_itr->m_shmAddr);
    m_RwlockShmList.erase(t_itr);
  }
  key_t t_key;
  int   t_shmid,t_ret;
  void * t_shmAddr;
  RwlockNode *t_pNode;
  //获取t_key
  T_FILENAMEDEF  t_fileName;
  size_t         t_shmSize;
  sprintf(t_fileName,"%s/ctl/%s.lock",getenv(MDB_HOMEPATH.c_str()),r_dbName);
  t_shmSize=STATE_BIT_SIZE+(r_RwlockNum)*sizeof(RwlockNode);
  t_shmSize+=sizeof(ProcessMSrcInfo)*MAX_PROCESS_NUM;
  
  t_key = ftok(t_fileName,RWLOCK_FTOK_ID);
  if(t_key<0) return false;
  //获取t_shmid
  t_shmid = shmget(t_key,t_shmSize,0666);
  if(t_shmid<0) return false;
  //获取shmAddr
  t_shmAddr = (void *)shmat(t_shmid, NULL, 0); 
  int t_intRet;
  //将信号量destory
  for(int i=0;i<r_RwlockNum;i++)
  {
    t_pNode=(RwlockNode*) ((char*)t_shmAddr+STATE_BIT_SIZE+sizeof(RwlockNode)*i);
    pthread_rwlock_destroy(&(t_pNode->m_rwlock));
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

bool RwlockShmMgr::setThreadSrcPos(RwlockShmInfo &r_RwlockShmInfo,ProcessMSrcInfo* &r_pMsrcInfo)
{
  RwlockNode        * t_pRwlockNode;
  pthread_rwlock_t  * t_pRwlock;
  ProcessMSrcInfo  * t_pMsrcInfo;
  ProcessMSrcInfo    t_mSrcInfo;
  
  t_mSrcInfo.m_state = 1;
  t_mSrcInfo.m_pid   = getpid();
  t_mSrcInfo.m_thId  = pthread_self();
  time(&(t_mSrcInfo.m_pTime));
  t_mSrcInfo.m_srcNum= 0;
  t_pRwlockNode = (RwlockNode*)((char*)(r_RwlockShmInfo.m_shmAddr)+STATE_BIT_SIZE);
  t_pRwlock     = &(t_pRwlockNode->m_rwlock);
  t_pMsrcInfo  = (ProcessMSrcInfo*)((char*)(r_RwlockShmInfo.m_shmAddr)+STATE_BIT_SIZE
                                    +sizeof(RwlockNode)*r_RwlockShmInfo.m_maxNum);
  
  pthread_rwlock_wrlock(t_pRwlock);
  for(int i=0;i<MAX_PROCESS_NUM;i++)
  {
    if(t_pMsrcInfo->m_state == 0)
    {
      memcpy(t_pMsrcInfo,&t_mSrcInfo,sizeof(ProcessMSrcInfo));
      r_pMsrcInfo = t_pMsrcInfo;
      pthread_rwlock_unlock(t_pRwlock);  
      return true;
    }else
    {
      t_pMsrcInfo++;
      continue;
    }
  }
  pthread_rwlock_unlock(t_pRwlock);    
  return false;
}
bool RwlockShmMgr::getThreadSrcPos(const RwlockShmInfo &r_RwlockShmInfo,ProcessMSrcInfo* &r_pMsrcInfo)
{
  RwlockNode        * t_pRwlockNode;
  pthread_rwlock_t  * t_pRwlock;
  ProcessMSrcInfo  * t_pMsrcInfo;
  
  pid_t     t_pid   = getpid();
  pthread_t t_thId  = pthread_self();
  
  t_pRwlockNode = (RwlockNode*)((char*)(r_RwlockShmInfo.m_shmAddr)+STATE_BIT_SIZE);
  t_pRwlock     = &(t_pRwlockNode->m_rwlock);
  t_pMsrcInfo  = (ProcessMSrcInfo*)((char*)(r_RwlockShmInfo.m_shmAddr)+STATE_BIT_SIZE
                                    +sizeof(RwlockNode)*r_RwlockShmInfo.m_maxNum);
  
  pthread_rwlock_wrlock(t_pRwlock);
  for(int i=0;i<MAX_PROCESS_NUM;i++)
  {
    if(t_pMsrcInfo->m_pid == t_pid &&
       t_pMsrcInfo->m_thId== t_thId&&
       t_pMsrcInfo->m_state!=0)
    {
      r_pMsrcInfo = t_pMsrcInfo;
      pthread_rwlock_unlock(t_pRwlock);   
      return true;
    }else
    {
      t_pMsrcInfo++;
      continue;
    }
  }  
  pthread_rwlock_unlock(t_pRwlock);    
  return false;  
}
bool RwlockShmMgr::delThreadSrcInfo(const char *r_dbName)
{
  RwlockShmInfo  t_shmInfo;
  bool          t_flag = false;
  //1. 找到现有的列表中找到对应内存信息
  pthread_rwlock_wrlock(&(RwlockShmMgr::m_innerRwlock));
  for(vector<RwlockShmInfo>::iterator t_itr=m_RwlockShmList.begin();
      t_itr!=m_RwlockShmList.end();t_itr++)
  {
    if(t_itr->m_dbName == r_dbName)
    {
      t_shmInfo=*t_itr;t_flag=true;break;
    }
  }
  pthread_rwlock_unlock(&(RwlockShmMgr::m_innerRwlock));
  if(t_flag==false) return false;
    
  RwlockNode        * t_pRwlockNode;
  pthread_rwlock_t  * t_pRwlock;
  ProcessMSrcInfo  * t_pMsrcInfo;
  
  pid_t     t_pid   = getpid();
  pthread_t t_thId  = pthread_self();
  
  t_pRwlockNode = (RwlockNode*)((char*)(t_shmInfo.m_shmAddr)+STATE_BIT_SIZE);
  t_pRwlock     = &(t_pRwlockNode->m_rwlock);
  t_pMsrcInfo  = (ProcessMSrcInfo*)((char*)(t_shmInfo.m_shmAddr)+STATE_BIT_SIZE
                                    +sizeof(RwlockNode)*t_shmInfo.m_maxNum);
  
  pthread_rwlock_wrlock(t_pRwlock);
  for(int i=0;i<MAX_PROCESS_NUM;i++)
  {
    if(t_pMsrcInfo->m_pid == t_pid &&
       t_pMsrcInfo->m_thId== t_thId)
    {
      memset(t_pMsrcInfo,0,sizeof(ProcessMSrcInfo));
      pthread_rwlock_unlock(t_pRwlock);    
      return true;
    }else
    {
      t_pMsrcInfo++;
      continue;
    }
  }
  pthread_rwlock_unlock(t_pRwlock);    
  return false;
}
bool RwlockShmMgr::lockRdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId)
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
bool RwlockShmMgr::lockWdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId)
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
bool RwlockShmMgr::unlockRdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId)
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
    memcpy(&(r_pMsrcInfo->m_srcInfo[i]),&(r_pMsrcInfo->m_srcInfo[i+1]),sizeof(RwSrcInfo));
  }
  --(r_pMsrcInfo->m_srcNum);
  return true;
}
bool RwlockShmMgr::unlockWdSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId)
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
    memcpy(&(r_pMsrcInfo->m_srcInfo[i]),&(r_pMsrcInfo->m_srcInfo[i+1]),sizeof(RwSrcInfo));
  }
  --(r_pMsrcInfo->m_srcNum);
  return true;
}


bool RwlockShmMgr::lockWWaitSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId)
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

bool RwlockShmMgr::unlockWWaitSrc(ProcessMSrcInfo* r_pMsrcInfo,int &r_srcId)
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
    memcpy(&(r_pMsrcInfo->m_srcInfo[i]),&(r_pMsrcInfo->m_srcInfo[i+1]),sizeof(RwSrcInfo));
  }
  --(r_pMsrcInfo->m_srcNum);
  return true;
}

