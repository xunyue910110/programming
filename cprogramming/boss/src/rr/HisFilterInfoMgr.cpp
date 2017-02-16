#include "HisFilterInfoMgr.h"
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>

HisFilterInfoMgr::HisFilterInfoMgr()
{
  m_onefileTime = RR_ONEFILE_TIME;
}
HisFilterInfoMgr::~HisFilterInfoMgr()
{
}
 
long HisFilterInfoMgr::getCurTime()
{
  static long t_time;
  char      t_timeStr[14+1];
  time_t    t_nowTime;
  struct tm t_tmTime;
  
  time(&t_nowTime);
  localtime_r(&t_nowTime,&t_tmTime);
  sprintf(t_timeStr,"%04d%02d%02d%02d%02d%02d",
           t_tmTime.tm_year+1900,
           t_tmTime.tm_mon+1,
           t_tmTime.tm_mday,
           t_tmTime.tm_hour,
           t_tmTime.tm_min,
           t_tmTime.tm_sec);
  t_time = atol(t_timeStr);
  return t_time;
}
long HisFilterInfoMgr::getTime(const long &r_stdTime,const long r_add)
{
  static long t_time;
  char t_timeStr[14+1];
  long t_year=r_stdTime/10000000000;
  long t_month=(r_stdTime/100000000)%100;
  long t_day=(r_stdTime/1000000)%100;
  long t_hour=(r_stdTime/10000)%100;
  long t_min=(r_stdTime/100)%100;
  long t_second=r_stdTime%100;
  
  struct tm t_tmValue;
  t_tmValue.tm_year = t_year -1900;
  t_tmValue.tm_mon  = t_month -1;
  t_tmValue.tm_mday = t_day;
  t_tmValue.tm_hour = t_hour;
  t_tmValue.tm_min  = t_min;
  t_tmValue.tm_sec  = t_second+r_add;
  t_tmValue.tm_isdst = -1;
  mktime(&t_tmValue);
  
  sprintf(t_timeStr,"%04d%02d%02d%02d%02d%02d",
           t_tmValue.tm_year+1900,
           t_tmValue.tm_mon+1,
           t_tmValue.tm_mday,
           t_tmValue.tm_hour,
           t_tmValue.tm_min,
           t_tmValue.tm_sec);
  t_time = atol(t_timeStr);
  return t_time;
}

//根据时间点、允许的最大时间跨度，求取时间点区域(每m_onefileTime秒一个时间点)
//r_stdTime: YYYYMMDDHHMISS
void HisFilterInfoMgr::getTimeLists(const long   &r_stdTime,
                                    const size_t &r_timeDuration,
                                    vector<long> &r_timeList) const
{
  r_timeList.clear();
  //特殊处理，对于日期为00的包月业务，不作前后判断处理
  long t_day=(r_stdTime/1000000)%100;
  if(t_day==0)
  {
    r_timeList.push_back(r_stdTime);
    return; 
  }
  int  t_count=(r_timeDuration+m_onefileTime-1)/m_onefileTime;
  int  t_i ;
  for(t_i=-1*t_count;t_i<=t_count;++t_i)
  {
    r_timeList.push_back(getTime(r_stdTime,t_i*m_onefileTime));
  }
}
//根据偏移量取对应的节点值
void HisFilterInfoMgr::getNodeValue(FILE* r_fidFile,const size_t &r_offSet,InnerNode &r_node) const
{
  int  t_fidSize=0;
  fseek(r_fidFile,r_offSet,SEEK_SET);  
  fread(&t_fidSize,sizeof(int),1,r_fidFile);
  r_node.m_valueLen = t_fidSize;
  if(t_fidSize > 0)
  {
    fread(r_node.m_value,sizeof(char)*t_fidSize,1,r_fidFile);
  }
  r_node.m_value[t_fidSize]=0;
  fread(&(r_node.m_beginTime),sizeof(long),1,r_fidFile);
  fread(&(r_node.m_endTime),sizeof(long),1,r_fidFile);
}
//从指定文件中，根据偏移量取对应的节点值
void HisFilterInfoMgr::getNodeValue(FILE* r_fidFile,
                            const size_t &r_offSet,
                              HisFidInfo &r_hisFidInfo) const
{
  InnerNode t_node;
  getNodeValue(r_fidFile,r_offSet,t_node);
  r_hisFidInfo.m_fid = t_node.m_value;
  r_hisFidInfo.m_beginTime=t_node.m_beginTime;
  r_hisFidInfo.m_endTime=t_node.m_endTime;
}
//取该业务最近的文件列表(最后半小时)
bool HisFilterInfoMgr::getLastFileList(const PString &r_fileType,
                                       vector<PString> &r_fileNameList,
                                       long &r_fileTime) const
{
  struct stat    t_statbuf;
  struct dirent *t_dirp;
  DIR           *t_dp;
  char           t_filePrefix[64];
  char           t_fileNo[64];
  vector<string> t_fileNameList;
  int            t_prefixLen;
  PString        t_fileName;
  
  sprintf(t_filePrefix,"%03d_%s_",m_channelNo,r_fileType.c_str());
  t_prefixLen = strlen(t_filePrefix);
  //////////////////////////////////////////////////////////
  //1. 获取该目录下的文件列表
  if (lstat(m_filePath.c_str(), &t_statbuf) < 0)
  {
    #ifdef _DEBUG_
      cout<<"lstat("<<m_filePath<<", &t_statbuf) false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  if (S_ISDIR(t_statbuf.st_mode) == 0) 
  {
    #ifdef _DEBUG_
      cout<<m_filePath<<" is not dir!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }

  if ((t_dp = opendir(m_filePath.c_str())) == 0)
  {
    // cann't read directory
    #ifdef _DEBUG_
      cout<<"cann't read directory:{"<<m_filePath<<"}!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }

  while ((t_dirp = readdir(t_dp)) != 0)
  {
    if (strncmp(t_dirp->d_name,t_filePrefix,t_prefixLen) != 0)
    {
      continue;
    }else if(strlen(t_dirp->d_name) != t_prefixLen+14)
    { //ZZZ_XXX_YYYYMMDD_NN.nn
      continue;
    }
    t_fileNameList.push_back(string(t_dirp->d_name));
  }
  closedir(t_dp);
  ::sort(t_fileNameList.begin(),t_fileNameList.end(),CmpFileName_Desc());
  for(vector<string>::const_iterator t_itr=t_fileNameList.begin();
      t_itr!=t_fileNameList.end();++t_itr)
  {
    if(t_itr==t_fileNameList.begin())
    {//取：YYYYMMDD_NN
      strcpy(t_fileNo,t_itr->c_str()+t_prefixLen);
      t_fileNo[11]=0;
      r_fileTime=getTimeByFileNo(t_fileNo);
      t_fileName=t_itr->c_str();
      r_fileNameList.push_back(t_fileName);
    }else if(strncmp(t_fileNo,t_itr->c_str()+t_prefixLen,11)==0)
    {
      t_fileName=t_itr->c_str();
      r_fileNameList.push_back(t_fileName);
    }else break;
  }
  return true;
}
//根据文件序号：YYYYMMDD_NN
//时间： YYYYMMDDHHMISS
long HisFilterInfoMgr::getTimeByFileNo(const char * r_fileNo) const
{
  int t_timeSec=0;
  char t_datStr[8+1];
  char t_fileNo[2+1];
  char t_timeStr[14+1]; 
  if(strlen(r_fileNo)!=11) return 0;
  strncpy(t_datStr,r_fileNo,8);t_datStr[8]=0;
  strncpy(t_fileNo,r_fileNo+9,2);t_fileNo[2]=0;
  t_timeSec = atoi(t_fileNo)*m_onefileTime;
  sprintf(t_timeStr,"%08d%02d%02d00",atoi(t_datStr),
          t_timeSec/3600 ,    //小时
          (t_timeSec/60)%60); //--分钟
  return atol(t_timeStr);
}

//根据时间生成对应的文件名前缀
//r_time格式:YYYYMMDDHHMISS ,r_fileName存放生成的文件明前缀
//  ZZZ_XXX_YYYYMMDD_NN.
void HisFilterInfoMgr::generateFileName(const long r_time,
                                        const PString &r_fileType,
                                        char *r_fileName) const
{
  int t_day,t_hour,t_min,t_fileno;
  t_day  = r_time/1000000;
  t_hour = (r_time%1000000/10000);
  t_min  = (r_time%10000/100);
  t_fileno = (t_hour*3600+t_min*60)/m_onefileTime;
  sprintf(r_fileName,"%03d_%s_%08d_%02d.",
          m_channelNo,r_fileType.c_str(),t_day,t_fileno);
}
//从指定文件中, 根据偏移量取或写对应的BUCKET值
void HisFilterInfoMgr::getBucketValue(FILE* r_fileFile,const size_t &r_offSet,FileBucket &r_node) const
{
  fseek(r_fileFile,r_offSet,SEEK_SET);  
  fread(&r_node,BUCKET_SIZE,1,r_fileFile);
}
void HisFilterInfoMgr::writeBucketValue(FILE* r_fileFile,const size_t &r_offSet,const FileBucket &r_node)
{
  fseek(r_fileFile,r_offSet,SEEK_SET);  
  fwrite(&r_node,BUCKET_SIZE,1,r_fileFile);
}

//取该文件中最晚节点的时间
long HisFilterInfoMgr::getLastNodeTime(FILE*  r_fidFile)
{
  FidFileHeader t_fileHeader;
  fseek(r_fidFile,0,SEEK_SET);
  fread(&t_fileHeader,sizeof(FidFileHeader),1,r_fidFile);
  return t_fileHeader.m_lastTime;
}

void HisFilterInfoMgr::getAllIdxList(FILE*    r_fidFile,  size_t &r_indexNum,
                                     size_t* &r_indexList,size_t &r_buffSize)
{
  FidFileHeader t_fileHeader;
  fseek(r_fidFile,0,SEEK_SET);
  fread(&t_fileHeader,sizeof(FidFileHeader),1,r_fidFile);
  fseek(r_fidFile,t_fileHeader.m_indexOffSet,SEEK_SET);
  r_indexNum=t_fileHeader.m_nodeNumber;
  if(r_buffSize < r_indexNum || r_indexList==NULL)
  {
    if(r_indexList!=NULL) delete[] r_indexList;
    r_buffSize = r_indexNum;
    r_indexList = new size_t[r_buffSize];
  }
  //遍历所有Hash节点,将所有的位置信息填入r_indexList中
  //1.读取Hash桶所有信息
  size_t* t_hashInfo = new size_t[t_fileHeader.m_hashSize];
  size_t  t_idxPos   = 0;
  FileBucket t_bucketNode;
  fread(t_hashInfo,sizeof(size_t)*t_fileHeader.m_hashSize,1,r_fidFile);
  for(size_t i=0;i<t_fileHeader.m_hashSize;++i)
  {  
    if(t_hashInfo[i] == 0) continue; //该桶为空
    //取该桶下的所有索引信息
    getBucketValue(r_fidFile,t_hashInfo[i],t_bucketNode);
    getIdxList(r_fidFile,r_indexList,t_idxPos,t_bucketNode);
  }
  delete[] t_hashInfo;
}

void HisFilterInfoMgr::getIdxList(FILE*       r_fidFile,
                                  size_t*    &r_indexList,
                                  size_t     &r_idxPos,
                            const FileBucket &r_bucketNode)
{
  //将该节点本身值压入
  r_indexList[r_idxPos]=r_bucketNode.m_valuePos;
  ++r_idxPos;
  FileBucket t_bucketNode;
  //取FID相同的节点
  if(r_bucketNode.m_sameNext != 0)
  {
    getBucketValue(r_fidFile,r_bucketNode.m_sameNext,t_bucketNode);
    getIdxList(r_fidFile,r_indexList,r_idxPos,t_bucketNode);
  }
  //取FID不同的节点
  if(r_bucketNode.m_diffNext != 0)
  {
    getBucketValue(r_fidFile,r_bucketNode.m_diffNext,t_bucketNode);
    getIdxList(r_fidFile,r_indexList,r_idxPos,t_bucketNode);
  }
}

