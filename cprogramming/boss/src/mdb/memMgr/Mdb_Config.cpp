#include "Mdb_Config.h"
#if defined(_TYPE_CMCC)
	#include "uconfig/SysParam.h"
#else
	#include "base/SysParam.h"
#endif

ostream& operator<<(ostream& os,const MdbCtlInfo& r_obj)
{
  os<<"m_dbName        ="<<r_obj.m_dbName<<endl;
  os<<"m_ctlSpaceName  ="<<r_obj.m_ctlSpaceName<<endl;
  os<<"m_ctlDiskFile   ="<<r_obj.m_ctlDiskFile<<endl;
  os<<"m_size          ="<<r_obj.m_size<<endl;
  os<<"m_pageSize      ="<<r_obj.m_pageSize<<endl;
  os<<"m_gparamMaxNum  ="<<r_obj.m_gparamMaxNum<<endl;
  os<<"m_gparamMaxSpNum="<<r_obj.m_gparamMaxSpNum<<endl;
  os<<"m_tableMaxNum   ="<<r_obj.m_tableMaxNum<<endl;
  os<<"m_indexMaxNum   ="<<r_obj.m_indexMaxNum<<endl;
  os<<"m_sessionMaxNum ="<<r_obj.m_sessionMaxNum<<endl;
  os<<"m_idxHashSpName ="<<r_obj.m_idxHashSpaceName<<endl;
  os<<"m_logPath       ="<<r_obj.m_logPath<<endl;
  os<<"m_lockfile      ="<<r_obj.m_lockfile<<endl;
  return os;
}
bool MdbCtlInfo::getCtlInfo(const char * r_dbName,SysParam* pSysParam)
{
  strcpy(m_dbName,r_dbName);
  char   t_tmp[128+1];
  string t_rootpath;
  map<string,string> t_keyValues;
  sprintf(t_tmp,"%c%s%cMdbCtlInfo",SECTDELIM,m_dbName,SECTDELIM);
  t_rootpath=t_tmp;
  if(pSysParam->getValue(t_rootpath,t_keyValues)==false)
  {
    #ifdef _DEBUG_
      cout<<"getConfigValue false!{"<<t_rootpath<<"}"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }  
  sprintf(m_ctlSpaceName,"%s",t_keyValues["CTLSPACENAME"].c_str());
  sprintf(m_ctlDiskFile,"%s",t_keyValues["CTLDISKFILE"].c_str());
  m_size = atol(t_keyValues["CTLSPACESIZE"].c_str())*1024*1024;
  m_pageSize = atol(t_keyValues["PAGESIZE"].c_str())*1024*1024;
  m_gparamMaxNum = atoi(t_keyValues["GPARAMMAXNUM"].c_str());
  m_gparamMaxSpNum = atoi(t_keyValues["SPACEMAXNUM"].c_str());
  m_tableMaxNum = atoi(t_keyValues["TABLEMAXNUM"].c_str());
  m_indexMaxNum = atoi(t_keyValues["INDEXMAXNUM"].c_str());
  m_sessionMaxNum = atoi(t_keyValues["SESSIONMAXNUM"].c_str());
  sprintf(m_logPath,"%s",t_keyValues["LOGPATH"].c_str());
  sprintf(m_lockfile,"%s/%s.lock",t_keyValues["DBLOCKPATH"].c_str(),r_dbName);
  sprintf(m_idxHashSpaceName,"%s",t_keyValues["INDEXSPACE_HASH"].c_str());
  if(strlen(m_ctlDiskFile)<=0 || m_size<=0           || 
     m_pageSize<=0            || m_gparamMaxNum<=0   || 
     m_gparamMaxSpNum<=0      || m_tableMaxNum<=0    ||
     m_indexMaxNum<=0         || m_sessionMaxNum<=0  ||
     strlen(m_logPath)<=0     || strlen(m_idxHashSpaceName)<=0)
  {
    #ifdef _DEBUG_
      cout<<"配置项缺失或值不规范 false!"<<t_rootpath<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  return true;
}
bool Mdb_Config::getConfigInfo(const char * r_dbName)
{
  //获取配置文件名称
  T_FILENAMEDEF t_configFile;
  sprintf(t_configFile,"%s/config/%s.conf",getenv(MDB_HOMEPATH.c_str()),r_dbName);
  //读取配置文件
  SysParam *t_pSysParam=NULL;
#if defined(_TYPE_CMCC)
	t_pSysParam =SysParam::getNewInstance();
#else
	t_pSysParam = new SysParam();
#endif	
  t_pSysParam->initialize(t_configFile);
  bool   t_bRet=getConfigInfoLocal(r_dbName,t_pSysParam);
  t_pSysParam->closeSection();
  delete t_pSysParam;
  t_pSysParam=NULL;
  return t_bRet;
}

//从配置文件中读取配置信息
//配置文件$MDB_HOMEPATH/config/dbName.conf
bool Mdb_Config::getConfigInfoLocal(const char * r_dbName,SysParam* r_pSysParam)
{
  bool   t_bRet=true;
  string t_path,t_name,t_value;
  map<string,string> t_keyValues;
  if(m_ctlInfo.getCtlInfo(r_dbName,r_pSysParam)==false)
  {
    #ifdef _DEBUG_
      cout<<"m_ctlInfo.getCtlInfo(r_pSysParam) false!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  /*
  cout<<"-----m_ctlInfo---------------"<<endl;
  cout<<m_ctlInfo;
  cout<<"-----------------------------"<<endl;
  */
  char  t_tmp[128+1];
  //表空间定义信息（包括索引表空间定义信息和数据表空间定义信息）
  sprintf(t_tmp,"%c%s%cSpaceDefInfos",SECTDELIM,r_dbName,SECTDELIM);
  string t_spaceRoot=t_tmp;
  string t_spaceName;
  vector<string> t_spaceList;
  SpaceInfo t_spaceInfo;
  int       t_spaceType;
  m_spaceInfoList.clear();
  strcpy(t_spaceInfo.m_dbName,r_dbName);
  t_spaceInfo.m_pageSize = m_ctlInfo.m_pageSize;
  if(r_pSysParam->setSectionPath(t_spaceRoot)==false)
  {
    #ifdef _DEBUG_
      cout<<"r_pSysParam->setSectionPath("<<t_spaceRoot
          <<") false!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }  
  while(r_pSysParam->getSubSection(t_spaceName)==true)
  {
    t_spaceList.push_back(t_spaceName);
  }
  for(vector<string>::iterator t_spSItr=t_spaceList.begin();
      t_spSItr!=t_spaceList.end();t_spSItr++)
  {
    t_path=t_spaceRoot+SECTDELIM+*t_spSItr;
    t_keyValues.clear();
    if(r_pSysParam->getValue(t_path,t_keyValues)==false)
    {
      #ifdef _DEBUG_
        cout<<"getConfigValue false!{"<<t_path<<"}"<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }  
    sprintf(t_spaceInfo.m_spaceName,"%s",t_spSItr->c_str());
    t_spaceInfo.m_size = atol(t_keyValues["SPACESIZE"].c_str())*1024*1024;
    t_spaceType = atoi(t_keyValues["SPACETYPE"].c_str());
    t_spaceInfo.m_type = (T_SPACETYPE)t_spaceType;
    strcpy(t_spaceInfo.m_fileName,t_keyValues["DISKFILE"].c_str());
    m_spaceInfoList.push_back(t_spaceInfo);
  }
  //*
  for(vector<SpaceInfo>::iterator t_spItr=m_spaceInfoList.begin();
      t_spItr!=m_spaceInfoList.end();t_spItr++)
  {
    cout<<"--------------------------------------"<<endl;
    cout<<"spaceName = "<<t_spItr->m_spaceName<<endl;
    cout<<"m_fileName= "<<t_spItr->m_fileName<<endl;
    cout<<"spaceSize = "<<t_spItr->m_size<<endl;
    cout<<"spaceType = "<<t_spItr->m_type<<endl;
    cout<<"--------------------------------------"<<endl;
  }
  //*/
  if(check()==false)
  {
    return false;
  }
  return true;
}
bool Mdb_Config::check()
{
  bool t_bRet = false;
  //1.检查索引hash表空间是否定义
  for(vector<SpaceInfo>::iterator t_spItr=m_spaceInfoList.begin();
      t_spItr!=m_spaceInfoList.end();t_spItr++)
  {
    if(strcasecmp(m_ctlInfo.m_idxHashSpaceName,t_spItr->m_spaceName)==0)
    {
      t_bRet=true;
      break;
    }
  }
  return t_bRet;
}

