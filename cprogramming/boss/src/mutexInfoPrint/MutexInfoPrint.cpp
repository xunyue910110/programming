#include <iostream>
#include <sys/shm.h>
#include <map>
#include <string>

#include "Mdb_Exception.h"
#include "MutexShmMgr.h"
#include "MSrcInfo.h"
#include "DoubleLock.h"

using namespace std;

typedef map<int,string> RESOURCE_ID2NAME_MAP; // 资源id和资源名称的映射关系
typedef map<int,string>::iterator RESOURCE_ID2NAME_MAP_ITR; 
typedef map<string,int> RESOURCE_NAME2ID_MAP; // 资源id和资源名称的映射关系
typedef map<string,int>::iterator RESOURCE_NAME2ID_MAP_ITR; 

struct InnerInfo
{
  int         m_sourceType;
  T_NAMEDEF   m_sourceName;
  int         m_sourceId;
};

class MutexInfoPrint
{
	public:
		MutexInfoPrint(string sDbName);
		virtual ~MutexInfoPrint();
		
		void attachMutexShm();
		void printMutexRC(bool isReset2Zero);
		void printProcessOccupiedRC();
		void resetOneMuexRc();
	
	private:
		ProcessMSrcInfo      *m_pProcessMSrcInfo;
		void                 *m_pShmAddr; 
		string                m_dbName;  
		RESOURCE_ID2NAME_MAP  m_RC_ID2Name_map; // 资源id和资源名称的映射关系
		RESOURCE_NAME2ID_MAP  m_RC_Name2ID_map; // 资源名称和资源id的映射关系
		
};

MutexInfoPrint::MutexInfoPrint(string sDbName)
{
	m_dbName           = sDbName;
	m_pProcessMSrcInfo = NULL;
	m_pShmAddr         = NULL;
	m_RC_ID2Name_map.clear();
	m_RC_Name2ID_map.clear();
}

MutexInfoPrint::~MutexInfoPrint()
{
	// detach share memory
	shmdt(m_pShmAddr);  
	m_pShmAddr = NULL;
}

// attache MUTEX共享内存
void MutexInfoPrint::attachMutexShm()
{
	MutexShmInfo  shmInfo;
	strcpy(shmInfo.m_dbName,m_dbName.c_str());
	shmInfo.m_maxNum=DoubleLock::MAX_MUTEX_NUM;
	if(MutexShmMgr::createMutexShm(shmInfo)==false)
	{
		throw Mdb_Exception(__FILE__, __LINE__,"连接MUTEX共享内存失败!");;
	}
	m_pShmAddr = shmInfo.m_shmAddr;
}

// 打印锁资源信息
void MutexInfoPrint::printMutexRC(bool isReset2Zero)
{
	char cFileName[128];
	int iFd;
	
	// 打开dbname.lock文件
	sprintf(cFileName,"%s/ctl/%s.lock",getenv(MDB_HOMEPATH.c_str()),m_dbName.c_str());
	if((iFd=::open(cFileName,O_RDWR))<0)
    {
    	throw Mdb_Exception(__FILE__, __LINE__, "打开文件{%s}失败!",cFileName);
    }
	
	// 读取dbname.lock文件中的信息	
	InnerInfo innsrc;
	while(::read(iFd,&innsrc,sizeof(InnerInfo))==sizeof(InnerInfo))
    {
    	m_RC_ID2Name_map.insert(RESOURCE_ID2NAME_MAP::value_type(innsrc.m_sourceId,innsrc.m_sourceName));
    	m_RC_Name2ID_map.insert(RESOURCE_NAME2ID_MAP::value_type(innsrc.m_sourceName,innsrc.m_sourceId));
    }
    ::close(iFd);	
    	
   	// 共享内存中的锁资源
	MutexNode * t_pMutex;
	RESOURCE_ID2NAME_MAP_ITR itr;
	for(itr=m_RC_ID2Name_map.begin();itr!=m_RC_ID2Name_map.end();++itr)
	{
		t_pMutex=(MutexNode*)((char*)m_pShmAddr+MutexShmMgr::STATE_BIT_SIZE)+itr->first;
		cout<<"resource:"<<itr->second<<" has "<<endl;
		cout<<"     read lock :"<<t_pMutex->m_rdCount<<endl;
		cout<<"     write lock:"<<t_pMutex->m_wtCount<<endl;
		cout<<"     waiting   :"<<t_pMutex->m_wtWait<<endl;
		
		if(t_pMutex->m_wtCount<0 
			&& isReset2Zero==true)
		{
			pthread_mutex_lock(&(t_pMutex->m_mutex));
			if(t_pMutex->m_wtCount<0)
			{
				cout<<"!!!!!!!!!!!!!"<<endl;
				t_pMutex->m_wtCount = 0;	
			}			
			pthread_mutex_unlock(&(t_pMutex->m_mutex));	
		}
	}
	cout<<"----------------------------------------------------"<<endl;
}

// 打印进程所占用着的资源
void MutexInfoPrint::printProcessOccupiedRC()
{
	ProcessMSrcInfo *pProcessMSrcInfo;
	MsrcInfo        *pMsrcInfo;	
	
	// 共享内存中的进程申请到的资源
	pProcessMSrcInfo=(ProcessMSrcInfo*)((char*)m_pShmAddr+MutexShmMgr::STATE_BIT_SIZE
	                                  +sizeof(MutexNode)*DoubleLock::MAX_MUTEX_NUM);
	                                 
	for(int i=0;i<MutexShmMgr::MAX_PROCESS_NUM;i++)
	{
		if(pProcessMSrcInfo->m_state!=0) 
		{
			cout<<"process:"<<pProcessMSrcInfo->m_pid<<" thread:"<<pProcessMSrcInfo->m_thId<<" Occupied lock on:"<<endl;
			for(int j=0;j<pProcessMSrcInfo->m_srcNum;j++)
			{
				pMsrcInfo = &(pProcessMSrcInfo->m_srcInfo[j]);
				if(pMsrcInfo->m_rcount+pMsrcInfo->m_wcount+pMsrcInfo->m_wwait>0)
				{
					cout<<"     resource  :"<<m_RC_ID2Name_map[pMsrcInfo->m_sid]<<endl;
					cout<<"     read lock :"<<pMsrcInfo->m_rcount<<endl;
					cout<<"     write lock:"<<pMsrcInfo->m_wcount<<endl;
					cout<<"     waiting   :"<<pMsrcInfo->m_wwait<<endl;
					cout<<"     time      :"<<pMsrcInfo->m_time<<endl;
				}
			}
			cout<<"****************************"<<endl;
		}
		if(i<MutexShmMgr::MAX_PROCESS_NUM-1)
			pProcessMSrcInfo++;
	}		
}

// ulock一个资源的mutex锁
void MutexInfoPrint::resetOneMuexRc()
{
	char cRcName[30];
	cout<<"pls input resource name:(q to exist)"<<endl;
	cin>>cRcName;
	
	printMutexRC(false);
	
	while(strcasecmp(cRcName,"q")!=0)
	{
		RESOURCE_NAME2ID_MAP_ITR itr = m_RC_Name2ID_map.find(cRcName);
		
		if(itr!=m_RC_Name2ID_map.end())
		{
			MutexNode *  pMutexNode=(MutexNode*)((char*)m_pShmAddr+MutexShmMgr::STATE_BIT_SIZE)+itr->second;

			int iLockCount = 0;
			while (pthread_mutex_trylock(&(pMutexNode->m_mutex)) != 0 )
			{
				sleep(1);
				
				++iLockCount;
				if(iLockCount==3) // trylock 尝试三次都失败后,解锁
				{
					pthread_mutex_unlock(&(pMutexNode->m_mutex));
					iLockCount = 0;	
				}	
			}	
			if(pMutexNode->m_rdCount >0)
			{
				--pMutexNode->m_rdCount;	
			}
			if(pMutexNode->m_wtCount >0)
			{
				--pMutexNode->m_wtCount;	
			}		
			pthread_mutex_unlock(&(pMutexNode->m_mutex));	
			cout<<"resource "<<cRcName<<" unlocked sccessfully!"<<endl;
		}
		else
		{
			cout<<"resource "<<cRcName<<" not found!"<<endl;	
		}
		
		cin>>cRcName;	
	}
	
	return;
}

// 主函数 命令行格式为:MutexInfoPrint billing [1--归零/0--不归零]
int main(int argc, char *argv[])
{
	bool isReset2Zero = false; // 是否对负的锁计数器进行归零处理
	bool isResetOneMutex = false;
	
	
	if(argc<2)
	{
		cout<<"Please input MDB Name!"<<endl;
		return -1;	
	}
	else if( (strcasecmp(argv[1],"-h")==0)
				||(strcasecmp(argv[1],"-help")==0)
			)
	{
		cout<<"------------------使用说明--------------------------------"<<endl;
		cout<<"---------mutex_manager dbName:打印锁信息------------------"<<endl;
		cout<<"---------mutex_manager dbName 1:负值锁计数器清零----------"<<endl;
		cout<<"---------mutex_manager dbName 2:单一资源解锁    ----------"<<endl;
	}
	
	if( argc==3 )
	{
		if(strcmp(argv[2],"1")==0)
		{
			isReset2Zero = true;	
		}
		else if(strcmp(argv[2],"2")==0)
		{
			isResetOneMutex = true;	
		}	
	}
	
	try
	{
		MutexInfoPrint mutexInfoPrint(argv[1]);
		mutexInfoPrint.attachMutexShm();
		if(isResetOneMutex==true)
		{
			mutexInfoPrint.resetOneMuexRc();
		}
		else	
		{
			mutexInfoPrint.printMutexRC(isReset2Zero);
			mutexInfoPrint.printProcessOccupiedRC();
		}
	}
	catch(Mdb_Exception &e)
	{
		e.toString();	
	}
	                              
	return 0;
}
