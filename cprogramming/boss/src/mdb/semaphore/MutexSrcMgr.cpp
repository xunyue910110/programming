// ############################################
// Source file : MutexSrcMgr.cpp
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : Gao xi le
// E_mail      : gaoxl@lianchuang.com
// Create      : 2008-4-28
// Update      : 2008-4-28
// Copyright(C): Gaoxl, Linkage.
// ############################################

#include "MutexSrcMgr.h"
#include "DoubleLock.h"

MutexSrcMgr::MutexSrcMgr()
{
  m_attachFlag=false;
}
MutexSrcMgr::~MutexSrcMgr()
{
  if(m_attachFlag==true)
  {
    detachMutexShm();
    m_attachFlag=false;
  }
}

bool MutexSrcMgr::createMutexShm(const char * r_dbName,const int &r_srcNum)
{
  strcpy(m_dbName,r_dbName);
  m_srcNum=r_srcNum;
  
  MutexShmInfo  t_shmInfo;
  strcpy(t_shmInfo.m_dbName,m_dbName);
  t_shmInfo.m_maxNum=m_srcNum;
  if(MutexShmMgr::createMutexShm(t_shmInfo)==false)
  {
    #ifdef _DEBUG_
    #endif
    return false;
  }
  m_shmAddr=t_shmInfo.m_shmAddr;
  m_attachFlag=true;
  m_psMsrcInfo=(ProcessMSrcInfo*)((char*)m_shmAddr+MutexShmMgr::STATE_BIT_SIZE
                                    +sizeof(MutexNode)*m_srcNum);
  return true;
}
bool MutexSrcMgr::deleteMutexShm(const char * r_dbName,const int &r_srcNum)
{
  if(m_attachFlag==true)
  {
    detachMutexShm();
    m_attachFlag=false;
  }
  if(MutexShmMgr::deleteMutex(r_dbName,r_srcNum)==false)
  {
    #ifdef _DEBUG_ 
      cout<<"MutexShmMgr::deleteMuxtex false!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}
bool MutexSrcMgr::attachMutexShm(const char * r_dbName,const int &r_srcNum)
{
  if(m_attachFlag==true)
  {
    detachMutexShm();
    m_attachFlag=false;
  }
  return createMutexShm(r_dbName,r_srcNum);
}
bool MutexSrcMgr::detachMutexShm()
{
  if(m_attachFlag==false) return true;
    
  shmdt(m_shmAddr);  m_shmAddr=NULL;
  m_attachFlag=false;
  return true;
}
bool MutexSrcMgr::getEProcessInfo(const int &r_timeOut,vector<ProcessMSrcInfo> &r_eMSrcInfo)
{
  ProcessMSrcInfo *t_pMsrcInfo=m_psMsrcInfo;
  MsrcInfo        *t_psrcInfo;
  MutexNode       *t_pMutexNode;
  time_t           t_nowTime;
  
  time(&t_nowTime);  
  r_eMSrcInfo.clear();
  for(int i=0;i<MutexShmMgr::MAX_PROCESS_NUM;i++)
  {
    //if(i<5) cout<<"r_timeOut = "<<r_timeOut<<" m_state="<<t_pMsrcInfo->m_state;
    if(t_pMsrcInfo->m_state!=0) 
    {
      for(int j=0;j<t_pMsrcInfo->m_srcNum;j++)
      {
        t_psrcInfo = &(t_pMsrcInfo->m_srcInfo[j]);
        if(t_psrcInfo->m_rcount+t_psrcInfo->m_wcount+t_psrcInfo->m_wwait>0)
        {
          if(difftime(t_nowTime,t_psrcInfo->m_time)>r_timeOut)
          {
            r_eMSrcInfo.push_back(*t_pMsrcInfo);
            break;
          }
        }
      }
    }
    if(i<MutexShmMgr::MAX_PROCESS_NUM-1)
      t_pMsrcInfo++;
  }
  return true;
}
bool MutexSrcMgr::rollbackSrc(const ProcessMSrcInfo &r_mSrcInfo)
{
  MutexNode        * t_pMutexNode;
  pthread_mutex_t  * t_pMutex;  
  t_pMutexNode = (MutexNode*)((char*)m_shmAddr+MutexShmMgr::STATE_BIT_SIZE);
  t_pMutex     = &(t_pMutexNode->m_mutex);    
  //pthread_mutex_lock(t_pMutex); by chenm 2009-5-25 20:57:22
  int iLockCount=0;
  pthread_mutex_t  lastMutex;
  lastMutex = *t_pMutex;
  
  while (pthread_mutex_trylock(t_pMutex) != 0 )
  {
  	sleep(1);
  	if( memcmp(&lastMutex,t_pMutex,sizeof(pthread_mutex_t)) == 0)
  	{
  		++iLockCount;
  		if(iLockCount==3) // trylock 尝试三次都失败后,解锁
  		{
  			pthread_mutex_unlock(t_pMutex);	
  		}
  	}
  	else
  	{
  		lastMutex = *t_pMutex;
  		iLockCount = 0;
  	}	
  }
  // over 2009-5-25 20:57:27
  
  ProcessMSrcInfo *t_pMsrcInfo=m_psMsrcInfo;
  MsrcInfo        *t_psrcInfo;
  for(int i=0;i<MutexShmMgr::MAX_PROCESS_NUM;i++)
  {
    if((t_pMsrcInfo->m_state!=0) &&(*t_pMsrcInfo == r_mSrcInfo))
    { //释放该线程对应的资源
      for(int j=0;j<t_pMsrcInfo->m_srcNum;j++)
      {
        t_psrcInfo = &(t_pMsrcInfo->m_srcInfo[j]);
        
        // add by chenm  2009-11-1 22:36:29
        if(t_psrcInfo->m_sid<0 || 
        	t_psrcInfo->m_sid>DoubleLock::MAX_MUTEX_NUM)
        {
        	continue;	
        }
        // over 2009-11-1 22:36:34
        
        t_pMutexNode=(MutexNode*)((char*)m_shmAddr+MutexShmMgr::STATE_BIT_SIZE
                       +sizeof(MutexNode)*t_psrcInfo->m_sid);
        while(t_psrcInfo->m_rcount>0)
        {
          //pthread_mutex_lock(&(t_pMutexNode->m_mutex));by chenm 2009-5-25 20:57:22
 		  iLockCount=0;
		  lastMutex = t_pMutexNode->m_mutex;
		  
		  while (pthread_mutex_trylock(&(t_pMutexNode->m_mutex)) != 0 )
		  {
		  	sleep(1);
		  	if(memcmp(&lastMutex,&(t_pMutexNode->m_mutex),sizeof(pthread_mutex_t))==0)
		  	{
		  		++iLockCount;
		  		if(iLockCount==3) // trylock 尝试三次都失败后,解锁
		  		{
		  			pthread_mutex_unlock(&(t_pMutexNode->m_mutex));	
		  		}
		  	}
		  	else
		  	{
		  		lastMutex = t_pMutexNode->m_mutex;
		  		iLockCount = 0;
		  	}	
		  }
		  // over 2009-5-25 20:57:27
          
          if(t_pMutexNode->m_rdCount>0) // add by chenm 2009-3-3 17:24:59
          	--(t_pMutexNode->m_rdCount);
          if (0 == t_pMutexNode->m_rdCount)
          {
            //pthread_cond_signal(&(t_pMutexNode->m_cond));
            if(iLockCount!=3)
            {
            	pthread_cond_broadcast(&(t_pMutexNode->m_cond));
            }
            else  // 尝试了三次解锁后,把condition也重置？
            {
				pthread_condattr_t  t_condAttr;
           		pthread_condattr_init(&t_condAttr);
           		pthread_condattr_setpshared(&t_condAttr,PTHREAD_PROCESS_SHARED);
      			pthread_cond_init(&(t_pMutexNode->m_cond), &t_condAttr);
      			pthread_condattr_destroy(&t_condAttr);            	
            }
          }
          pthread_mutex_unlock(&(t_pMutexNode->m_mutex));
          --(t_psrcInfo->m_rcount);
        }
        while(t_psrcInfo->m_wcount>0)
        {
          //pthread_mutex_lock(&(t_pMutexNode->m_mutex)); by chenm 2009-5-25 20:57:22
		  iLockCount=0;
		  lastMutex = t_pMutexNode->m_mutex;
		  
		  while (pthread_mutex_trylock(&(t_pMutexNode->m_mutex)) != 0 )
		  {
		  	
		  	sleep(1);
		  	if(memcmp(&lastMutex,&(t_pMutexNode->m_mutex),sizeof(pthread_mutex_t))==0)
		  	{
		  		++iLockCount;
		  		if(iLockCount==3) // trylock 尝试三次都失败后,解锁
		  		{
		  			pthread_mutex_unlock(&(t_pMutexNode->m_mutex));	
		  		}
		  	}
		  	else
		  	{
		  		lastMutex = t_pMutexNode->m_mutex;
		  		iLockCount = 0;
		  	}	
		  }
		  // over 2009-5-25 20:57:27
		  
          if(t_pMutexNode->m_wtCount>0) // add by chenm 2009-3-3 17:24:59
          	--(t_pMutexNode->m_wtCount);
          if (0 == t_pMutexNode->m_wtCount)
          {
            //pthread_cond_signal(&(t_pMutexNode->m_cond));
           	if(iLockCount!=3)
           	{
           		pthread_cond_broadcast(&(t_pMutexNode->m_cond));
           	}
           	else // 尝试了三次解锁后,把condition也重置？
           	{
           		pthread_condattr_t  t_condAttr;
           		pthread_condattr_init(&t_condAttr);
           		pthread_condattr_setpshared(&t_condAttr,PTHREAD_PROCESS_SHARED);
      			pthread_cond_init(&(t_pMutexNode->m_cond), &t_condAttr);
      			pthread_condattr_destroy(&t_condAttr);	
           	}
          }
          pthread_mutex_unlock(&(t_pMutexNode->m_mutex));
         --(t_psrcInfo->m_wcount);
        }
        while(t_psrcInfo->m_wwait>0)
        {
          //pthread_mutex_lock(&(t_pMutexNode->m_mutex)); by chenm 2009-5-25 20:57:22
		  iLockCount=0;
		  lastMutex = t_pMutexNode->m_mutex;
		  
		  while (pthread_mutex_trylock(&(t_pMutexNode->m_mutex)) != 0 )
		  {
		  	
		  	sleep(1);
		  	if(memcmp(&lastMutex,&(t_pMutexNode->m_mutex),sizeof(pthread_mutex_t))==0)
		  	{
		  		++iLockCount;
		  		if(iLockCount==3) // trylock 尝试三次都失败后,解锁
		  		{
		  			pthread_mutex_unlock(&(t_pMutexNode->m_mutex));	
		  		}
		  	}
		  	else
		  	{
		  		lastMutex = t_pMutexNode->m_mutex;
		  		iLockCount = 0;
		  	}	
		  }
		  // over 2009-5-25 20:57:27
          if(t_pMutexNode->m_wtWait>0) // add by chenm 2009-3-3 17:24:59
          	--(t_pMutexNode->m_wtWait);
          if (0 == t_pMutexNode->m_wtWait)
          {
            //pthread_cond_signal(&(t_pMutexNode->m_cond));
            if(iLockCount!=3)
            {
            	pthread_cond_broadcast(&(t_pMutexNode->m_cond));
            }
            else  // 尝试了三次解锁后,把condition也重置？
            {
				pthread_condattr_t  t_condAttr;
           		pthread_condattr_init(&t_condAttr);
           		pthread_condattr_setpshared(&t_condAttr,PTHREAD_PROCESS_SHARED);
      			pthread_cond_init(&(t_pMutexNode->m_cond), &t_condAttr);
      			pthread_condattr_destroy(&t_condAttr);            	
            }
          }
          pthread_mutex_unlock(&(t_pMutexNode->m_mutex));
          --(t_psrcInfo->m_wwait);
        }
      }
      memset((char*)t_pMsrcInfo,0,sizeof(ProcessMSrcInfo));
      break;
    }
    
    if(i<MutexShmMgr::MAX_PROCESS_NUM-1)
        t_pMsrcInfo++;
  }
  pthread_mutex_unlock(t_pMutex);
  return true;
}

//获取进程已退出且进程信息未注销的信息。
bool MutexSrcMgr::getExitProcessInfo(vector<ProcessMSrcInfo> &r_eMSrcInfo)
{
  ProcessMSrcInfo *t_pMsrcInfo=m_psMsrcInfo;
  MsrcInfo        *t_psrcInfo;
  MutexNode       *t_pMutexNode;
  time_t           t_nowTime;
  
  time(&t_nowTime);  
  r_eMSrcInfo.clear();
  for(int i=0;i<MutexShmMgr::MAX_PROCESS_NUM;i++)
  {
    if(!(t_pMsrcInfo->m_state==0))
    {
      //判断该进程是否还存在
      if(isProcessExist(t_pMsrcInfo->m_pid)==false)
      {
        r_eMSrcInfo.push_back(*t_pMsrcInfo);
      }
    }
    if(i<MutexShmMgr::MAX_PROCESS_NUM-1)
      t_pMsrcInfo++;
  }
  return true;
}

bool MutexSrcMgr::isProcessExist(const pid_t &r_pid) const
{
/*  if(kill(r_pid,0) == -1) //发送空信号,测试进程是否存在
  {
    return false;
  } else
  {
    return true;
  }*/
    char cmd[200];
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "ps -ef |grep -v grep|awk {'print $2'}| grep %d", r_pid);
    
    FILE * handle;
    
    if((handle = popen(cmd,"r")) == NULL)
    {
        cout << "popen err" << endl;
        return true;
    }
	fflush(handle);
	
	bool flag=false;
	char line[300];
    memset(line, 0, sizeof(line));
    
	while(fgets(line, sizeof(line), handle) != (char *)NULL)
	{
		if(atoi(line)==r_pid)
		{
		    flag = true;
		    break;
		}
		memset(line, 0, sizeof(line));
	}
	
    if( pclose(handle) < 0)
    {
        cout << "pclose err" << endl;
        return true;
    }
    
	return flag;	  
}  


