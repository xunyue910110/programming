#include "FileFilterInfoMgr.h"
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include "RrException.h"
#include "base/supdebug.h" 
#include <map>
#include <algorithm>
using namespace std;

FileFilterInfoMgr::FileFilterInfoMgr()
{
  m_openFlag = false;
  m_fullFileList.clear();
}

FileFilterInfoMgr::~FileFilterInfoMgr()
{
  if(m_openFlag==true)
  {
    fclose(m_fidFile);
    m_openFlag = false;
  }
  m_fullFileList.clear();
}

bool FileFilterInfoMgr::initialization(const RrConfig &r_config)
{
  m_filePath   = r_config.m_fidPath;
  m_channelNo  = r_config.m_channelNo;
  m_fidFileSize= r_config.m_fidFileSize;
  m_onefileTime= r_config.m_onefileTIme;
   
  memset(m_fileName1,0,sizeof(m_fileName1));
  m_nodeFixSize = sizeof(long)+sizeof(long)+sizeof(int);
  
	//add by chenm 2009-9-17 14:38:10
	m_fidFileNameList.clear();
    
    struct dirent *dirp;
    DIR   *dp;

    if ((dp = opendir(m_filePath.c_str())) == 0)
        RR_THROW_CP1(E_RR_FILE_OPEN,"打开目录失败:{%s}!",m_filePath.c_str());;

    while ((dirp = readdir(dp)) != 0)
    {
        if (strcmp(dirp->d_name, ".") == 0
                || strcmp(dirp->d_name, "..") == 0)
            continue;

        char cTmpFileName[200];
        sprintf(cTmpFileName,"%s/%s",m_filePath.c_str(),dirp->d_name);
        m_fidFileNameList.insert(cTmpFileName);  
    }
  return true;
}
bool FileFilterInfoMgr::setFileType(const PString &r_fileType)
{
  m_fileType = r_fileType;
  return true;
}

bool FileFilterInfoMgr::isRepeatFid(const int        &r_rrType,
                                    const FilterInfo &r_fidInfo,
                                    const size_t     &r_timeDuration)
{
  int t_subFileNo=0;
  if(r_rrType == E_RR_RRTYPE_EQUAL )
  {
    generateFileName(r_fidInfo.m_beginTime);
    for(t_subFileNo=0;t_subFileNo<100;++t_subFileNo)
    {
      sprintf(m_fileName2,"%s/%s%02d",m_filePath.c_str(),m_filePrefix,t_subFileNo);
      //判断文件是否存在
      //if(access(m_fileName2, F_OK) != 0)  modified by chenm 2009-9-17 14:57:55
      if(m_fidFileNameList.find(m_fileName2)==m_fidFileNameList.end())
      { //文件不存在，表示不重复
    		break;
    	}else
    	{
    	  if(isRepeatFidInFile(r_rrType,r_fidInfo,m_fileName2)==true)
    	  {
    	    return true;
    	  }
    	} 
    }
  }else //E_RR_RRTYPE_CONTAIN 包容排重或者文件级排重added by hanyu
  {
    //在允许的最大跨度中，均搜索一下
    vector<long>  t_timeList;
    getTimeLists(r_fidInfo.m_beginTime,r_timeDuration,t_timeList);
    
    for(vector<long>::iterator t_timeItr=t_timeList.begin();
        t_timeItr!=t_timeList.end();++t_timeItr)
    {
      generateFileName(*t_timeItr);
      for(t_subFileNo=0;t_subFileNo<100;++t_subFileNo)
      {
        sprintf(m_fileName2,"%s/%s%02d",m_filePath.c_str(),m_filePrefix,t_subFileNo);
        //判断文件是否存在
        if(access(m_fileName2, F_OK) != 0)
        { //文件不存在，表示不重复
      		break;
      	}else
      	{
      	  if(isRepeatFidInFile(r_rrType,r_fidInfo,m_fileName2)==true)
      	  {
      	    return true;
      	  }
      	} 
      }    
    }
  }
  return false;
}
bool FileFilterInfoMgr::queryFidInFile(const FilterInfo& r_fidInfo
	                                  ,const string& r_fileName)
{
	//判断文件是否存在
	/* by chenm 2009-9-17 1:41:52
	if(access(r_fileName.c_str(), F_OK) != 0)
	{ //文件不存在，表示不重复
		return false;
	}*/
	if(openFile(r_fileName.c_str())==false)
	{ //文件存在，但打不开，则需要报错，并退出
		RR_THROW_CP1(E_RR_FILE_OPEN,"打开文件失败:{%s}!",r_fileName);
	}
	readFileHeader();
	m_hashSize = m_fileHeader.m_hashSize;
	unsigned t_keyValue  = hashKey(r_fidInfo.m_fid.c_str()) % m_hashSize;
	size_t   t_bucketPos = m_fileHeader.m_indexOffSet+SIZE_T_SIZE*t_keyValue;
	size_t   t_nodePos  = 0;
	t_nodePos=getBucketPosValue(t_bucketPos);
	if (t_nodePos == 0)
		return false;
	
	FileBucket t_node;
	while(1)
	{
		readBucketNode(t_nodePos,t_node);
		HisFidInfo t_fidNode;
		HisFilterInfoMgr::getNodeValue(m_fidFile,t_node.m_valuePos,t_fidNode);
		if (r_fidInfo.m_fid == t_fidNode.m_fid)
		{
			if (t_fidNode.m_beginTime == r_fidInfo.m_beginTime
			&&  t_fidNode.m_endTime   == r_fidInfo.m_endTime)
			{
				return true;
			}
			if (t_node.m_sameNext != 0)
			{
				t_nodePos = t_node.m_sameNext;
			}
			else
			{
				return false;
			}
		}
		else if (t_node.m_diffNext != 0)
		{
			t_nodePos = t_node.m_diffNext;
		}
		else
		{
			return false;
		}
	}
	return false;	
}
bool FileFilterInfoMgr::queryFidInfo(const int &r_rrType
					 ,const FilterInfo & r_filter
					 ,const size_t  &r_timeDuration
					 ,string& r_fileName)
{
	int t_subFileNo=0;
	if(r_rrType == E_RR_RRTYPE_EQUAL )
	{
		generateFileName(r_filter.m_beginTime);
		for(t_subFileNo=0;t_subFileNo<100;++t_subFileNo)
		{
			sprintf(m_fileName2,"%s/%s%02d",m_filePath.c_str(),m_filePrefix,t_subFileNo);
			//判断文件是否存在
			if(access(m_fileName2, F_OK) != 0)
			{ //文件不存在，表示不重复
				r_fileName="";
				break;
			}
			else
			{
				if (queryFidInFile(r_filter,m_fileName2))
				{
					r_fileName=m_fileName2;
					break;
				}
			} 
		}			
	}
	else
	{
		r_fileName="";
		//在允许的最大跨度中，均搜索一下
		vector<long>  t_timeList;
		getTimeLists(r_filter.m_beginTime,r_timeDuration,t_timeList);
		
		for(vector<long>::iterator t_timeItr=t_timeList.begin();
		t_timeItr!=t_timeList.end();++t_timeItr)
		{
			generateFileName(*t_timeItr);
			bool bFound=false;
			for(t_subFileNo=0;t_subFileNo<100;++t_subFileNo)
			{
				sprintf(m_fileName2,"%s/%s%02d",m_filePath.c_str(),m_filePrefix,t_subFileNo);
				//判断文件是否存在
				if(access(m_fileName2, F_OK) != 0)
				{ //文件不存在，表示不重复
					bFound=false;
					break;
				}
				else
				{
					if (queryFidInFile(r_filter,m_fileName2))
					{
						bFound=true;
						r_fileName=m_fileName2;
						break;
					}
				}
			}
			if (bFound) 
				break;
		}
	}
	return true;
}
bool FileFilterInfoMgr::queryFidInfo(const int &r_rrType
						             ,const vector<FilterInfo*> &r_filterList
						             ,const size_t  &r_timeDuration
						             ,vector<string>& r_fileNameList)
{
	r_fileNameList.clear();
	string t_fileName;
	vector<FilterInfo*>::const_iterator t_it=r_filterList.begin();
	for (; t_it != r_filterList.end(); t_it ++)
	{
		queryFidInfo(r_rrType,*(*t_it),r_timeDuration,t_fileName);
		r_fileNameList.push_back(t_fileName);
	}
	return true;
}	  
//根据偏移量判断是否重单
bool FileFilterInfoMgr::isRepeatFidByIdx(FILE* r_fileFile,
                                         const int        &r_rrType,
                                         const FilterInfo &r_fidInfo,
                                         const FileBucket &r_bucketNode)
{
  HisFilterInfoMgr::getNodeValue(r_fileFile,r_bucketNode.m_valuePos,m_node);
  if(strcmp(r_fidInfo.m_fid.c_str(),m_node.m_value) == 0)
  {
    if(r_rrType == E_RR_RRTYPE_EQUAL || r_rrType == E_RR_RRTYPE_FILE)
    { //完全排重
      return true;
    }else
    { //包容排重
	    if(m_node.m_beginTime>=r_fidInfo.m_beginTime &&
	       m_node.m_beginTime< r_fidInfo.m_endTime ||
	       m_node.m_beginTime<=r_fidInfo.m_beginTime &&
	       m_node.m_endTime  > r_fidInfo.m_beginTime )
	    {
	      return true;
	    }
	    FileBucket t_bucketNode1=r_bucketNode;
	    while(t_bucketNode1.m_sameNext != 0)
	    {
	      getBucketValue(r_fileFile,t_bucketNode1.m_sameNext,t_bucketNode1);
	      HisFilterInfoMgr::getNodeValue(r_fileFile,t_bucketNode1.m_valuePos,m_node);
  	    if(m_node.m_beginTime>=r_fidInfo.m_beginTime &&
  	       m_node.m_beginTime< r_fidInfo.m_endTime ||
  	       m_node.m_beginTime<=r_fidInfo.m_beginTime &&
  	       m_node.m_endTime  > r_fidInfo.m_beginTime )
  	    {
  	      return true;
  	    }
	    }
	    return false;
  	}
  }else
  {
    if(r_bucketNode.m_diffNext == 0) 
      return false;
    FileBucket t_bucketNode2;
    getBucketValue(r_fileFile,r_bucketNode.m_diffNext,t_bucketNode2);
    return isRepeatFidByIdx(r_fileFile,r_rrType,r_fidInfo,t_bucketNode2);
  }
}
bool FileFilterInfoMgr::readFileHeader(FidFileHeader * r_header)
{
	if (!m_openFlag) return false;
	if (r_header == NULL)
		r_header=&m_fileHeader;
	fseek(m_fidFile,0,SEEK_SET);
	fread(r_header,sizeof(FidFileHeader),1,m_fidFile);
	return true;
}
bool FileFilterInfoMgr::writeFileHeader(FidFileHeader * r_header)
{
	if (!m_openFlag) return false;
	if (r_header == NULL)
		r_header=&m_fileHeader;
	fseek(m_fidFile,0,SEEK_SET);	
	fwrite(r_header,sizeof(FidFileHeader),1,m_fidFile);
	return true;
}
size_t FileFilterInfoMgr::getBucketPos(const char *r_fid)
{
	m_hashSize=m_fileHeader.m_hashSize;
	unsigned t_keyValue  = hashKey(r_fid) % m_hashSize;
	return m_fileHeader.m_indexOffSet+SIZE_T_SIZE*t_keyValue;
}
size_t FileFilterInfoMgr::getBucketPosValue(size_t r_bucketPos)
{
	fseek(m_fidFile,r_bucketPos,SEEK_SET);
	size_t t_bucketNodePos;
	fread(&t_bucketNodePos,SIZE_T_SIZE,1,m_fidFile);
	return t_bucketNodePos;
}
bool FileFilterInfoMgr::writeBucketPosValue(size_t r_bucketPos,size_t r_bucketPosValue)
{
	fseek(m_fidFile,r_bucketPos,SEEK_SET);
	size_t t_value=r_bucketPosValue;
	fwrite(&t_value,SIZE_T_SIZE,1,m_fidFile);
	return true;
}
void FileFilterInfoMgr::readBucketNode(size_t r_bucketNodePos,FileBucket &r_node)
{
	return getBucketValue(m_fidFile,r_bucketNodePos,r_node);	
}
void FileFilterInfoMgr::writeBucketNode(size_t r_bucketNodePos,FileBucket &r_node)
{
	return writeBucketValue(m_fidFile,r_bucketNodePos,r_node);
}

//bool FileFilterInfoMgr::getFidInfo(size_t r_fidPos,string& r_fid,long& r_beginTime,long&r_endTime)
//{
//	InnerNode t_node;
//	HisFilterInfoMgr::getNodeValue(m_fidFile,r_fidPos,t_node);
//	r_fid.assign(t_node.m_value);
//	r_beginTime=t_node.m_beginTime;
//	r_endTime=t_node.m_endTime;
//	return true;
//}
bool FileFilterInfoMgr::isRepeatFidInFile(const int        &r_rrType,
                                          const FilterInfo &r_fidInfo,
                                          const char       *r_fileName)
{
  //判断文件是否存在
  /*by chenm 2009-9-17 1:41:18 
  if(access(r_fileName, F_OK) != 0)
  { //文件不存在，表示不重复
		return false;
	}*/ 
  if(openFile(r_fileName)==false)
  { //文件存在，但打不开，则需要报错，并退出
    RR_THROW_CP1(E_RR_FILE_OPEN,"打开文件失败:{%s}!",r_fileName);
  }
  //读取文件内容判断是否重复
  fseek(m_fidFile,0,SEEK_SET);
  fread(&m_fileHeader,sizeof(FidFileHeader),1,m_fidFile);
  //add by chenzm@2009-5-6
  if (m_fileHeader.m_nodeNumber == 0)
   return false;
  //add end
  //计算该FID对应的桶的位置
  m_hashSize = m_fileHeader.m_hashSize;
  unsigned t_keyValue  = hashKey(r_fidInfo.m_fid.c_str()) % m_hashSize;
  size_t   t_bucketPos = m_fileHeader.m_indexOffSet+SIZE_T_SIZE*t_keyValue;
  size_t   t_bnodePos  = 0;
  FileBucket t_bucketNode;

  fseek(m_fidFile,t_bucketPos,SEEK_SET);
  fread(&t_bnodePos,SIZE_T_SIZE,1,m_fidFile);
  //add by chenzm@2009-5-6
  if (t_bnodePos == 0)  return false;
  //add end
  fseek(m_fidFile,t_bnodePos,SEEK_SET);
  fread(&t_bucketNode,BUCKET_SIZE,1,m_fidFile);
  #ifdef FP_DEBUG
    FP_BEGIN(isRepeatFidByIdx)
  #endif
  if(isRepeatFidByIdx(m_fidFile,r_rrType,r_fidInfo,t_bucketNode)==true)
  {
    #ifdef FP_DEBUG
      FP_END(isRepeatFidByIdx)
    #endif
    return true;
  }
  #ifdef FP_DEBUG
    FP_END(isRepeatFidByIdx)
  #endif
  return false;
}
//插入HASH索引
//r_hashPos     hash桶起始地址
//r_hashNodePos 新BOCKET位置
//r_nodePos     FID位置
void FileFilterInfoMgr::insertIdxNode(const int  &r_rrtype,
                                      FILE*       r_fileFile,
                                const size_t     &r_hashPos,
                                const size_t     &r_hashNodePos,
                                const size_t     &r_nodePos,
                                const FilterInfo &r_fidInfo)
{
  FileBucket  t_bucketNode1;
  unsigned t_keyValue  = hashKey(r_fidInfo.m_fid.c_str()) % m_hashSize;
  size_t t_hashpos = r_hashPos+SIZE_T_SIZE*t_keyValue;
  size_t t_bnodePos= 0;

  fseek(r_fileFile,t_hashpos,SEEK_SET);
  fread(&t_bnodePos,SIZE_T_SIZE,1,r_fileFile);
  
  t_bucketNode1.m_valuePos = r_nodePos;
  t_bucketNode1.m_diffNext = t_bucketNode1.m_sameNext = 0;
  if(t_bnodePos == 0)
  { //第一个节点
    writeBucketValue(r_fileFile,r_hashNodePos,t_bucketNode1);
    fseek(r_fileFile,t_hashpos,SEEK_SET);
    t_bnodePos = r_hashNodePos;
    fwrite(&t_bnodePos,SIZE_T_SIZE,1,r_fileFile);
    return ;
  }else
  { 
    if(r_rrtype == E_RR_RRTYPE_EQUAL || r_rrtype == E_RR_RRTYPE_FILE)
    { //插到第一个节点(因为不存在FID相同的节点)
      t_bucketNode1.m_diffNext = t_bnodePos;
      writeBucketValue(r_fileFile,r_hashNodePos,t_bucketNode1);
      fseek(r_fileFile,t_hashpos,SEEK_SET);
      t_bnodePos = r_hashNodePos;
      fwrite(&t_bnodePos,SIZE_T_SIZE,1,r_fileFile);
    }else
    { //查到第相同的第一个节点
      FileBucket t_bucketNode2;
      HisFidInfo t_fidInfo;
      for(;;)
      {
        getBucketValue(r_fileFile,t_bnodePos,t_bucketNode2);
        HisFilterInfoMgr::getNodeValue(r_fileFile,t_bucketNode2.m_valuePos,t_fidInfo);
        if(t_fidInfo.m_fid == r_fidInfo.m_fid)
        { //插入该节点的same下
          t_bucketNode1.m_sameNext = t_bucketNode2.m_sameNext;
          t_bucketNode2.m_sameNext = r_hashNodePos;
          writeBucketValue(r_fileFile,r_hashNodePos,t_bucketNode1);
          writeBucketValue(r_fileFile,t_bnodePos,t_bucketNode2);
          break;
        }else if(t_bucketNode2.m_diffNext == 0)
        { //插入最后一个节点
          writeBucketValue(r_fileFile,r_hashNodePos,t_bucketNode1);
          t_bucketNode2.m_diffNext = r_hashNodePos;
          writeBucketValue(r_fileFile,t_bnodePos,t_bucketNode2);
          break;
        }else
        { //继续找
          t_bnodePos = t_bucketNode2.m_diffNext;
        }
      } 
    }
  }
  return;
}
bool FileFilterInfoMgr::deleteFidInfo(const int &r_rrtype,const vector<FilterInfo*>& r_filterList)
{
	int t_subFileNo=0;
	
	string t_fid;
	long t_beginTime,t_endTime;
	size_t t_fidPos;
	
	FileBucket t_node,t_leftNode,t_upNode,t_rightNode;
	
  	for(vector<FilterInfo*>::const_iterator t_itr=r_filterList.begin()
  		;t_itr!=r_filterList.end()
  		;++t_itr)
  	{
	    if((*t_itr)->m_uniqFlag!=E_RR_REPFLAG_UNIQ)
	    { //不是唯一信息，则不需要提交
	      continue;
	    }
	    generateFileName((*t_itr)->m_beginTime);
	    for(t_subFileNo=0;t_subFileNo<100;t_subFileNo++)
	    {
			sprintf(m_fileName2,"%s/%s%02d",m_filePath.c_str(),m_filePrefix,t_subFileNo);
			if(access(m_fileName2, F_OK) != 0)
			{
				break;
			}			
			if(openFile()==false)
			{
				//异常处理
				RR_THROW_CP1(E_RR_FILE_OPEN,"打开文件失败:{%s}!",m_fileName2);
			}
			size_t t_bucketPos=getBucketPos((*t_itr)->m_fid.c_str());
			size_t t_nodePos=getBucketPosValue(t_bucketPos);
			if (t_nodePos == 0)
				continue;
			size_t t_leftPos=0,t_rightPos=0,t_upPos=0,t_downPos=0;
			bool t_bFound=false;
			while(1)
			{
				readBucketNode(t_nodePos,t_node);
				
				t_rightPos=t_node.m_sameNext;
				t_downPos=t_node.m_diffNext;
				
				t_fidPos=t_node.m_valuePos;	
				HisFidInfo t_fidNode;
				HisFilterInfoMgr::getNodeValue(m_fidFile,t_node.m_valuePos,t_fidNode);
				if ((*t_itr)->m_fid == t_fidNode.m_fid)
				{
					if (t_fidNode.m_beginTime == (*t_itr)->m_beginTime
					&&  t_fidNode.m_endTime   == (*t_itr)->m_endTime)
					{
						t_bFound=true;
						break;
					}
					if (t_node.m_sameNext != 0)
					{
						t_leftPos=t_nodePos;
						t_nodePos = t_node.m_sameNext;
					}
					else
					{
						t_bFound=false;
						break;
					}
				}
				else if (t_node.m_diffNext != 0)
				{
					t_upPos=t_nodePos;
					t_nodePos = t_node.m_diffNext;
				}
				else
				{
					t_bFound=false;
					break;
				}
			}
			if (!t_bFound) continue;
			
			readFileHeader();
			m_fileHeader.m_nodeNumber--;
			writeFileHeader();
			
			if (t_leftPos != 0)
				readBucketNode(t_leftPos,t_leftNode);
			if (t_upPos != 0)
				readBucketNode(t_upPos,t_upNode);
			if (t_rightPos != 0)
				readBucketNode(t_rightPos,t_rightNode);

			//仅1个节点
			if (t_upPos==0 && t_leftPos == 0 && t_rightPos == 0 && t_downPos == 0)
			{
				writeBucketPosValue(t_bucketPos,0);
				continue;
			}
			if (t_leftPos != 0)//有左节点
			{
				t_leftNode.m_sameNext = t_rightPos;
				writeBucketNode(t_leftPos,t_leftNode);
				continue;
			}
			if (t_upPos != 0)//有上节点
			{
				if (t_rightPos == 0)//无横向子节点
				{
					t_upNode.m_diffNext = t_downPos;
					writeBucketNode(t_upPos,t_upNode);
				}
				else//有横向子节点
				{
					t_upNode.m_diffNext = t_rightPos;
					writeBucketNode(t_upPos,t_upNode);
					t_rightNode.m_diffNext=t_downPos;
					writeBucketNode(t_rightPos,t_rightNode);
				}
				continue;
			}
			if (t_leftPos == 0)//无左节点
			{
				if (t_rightPos != 0)//有右节点
				{
					writeBucketPosValue(t_bucketPos,t_rightPos);
					if (t_downPos != 0)//有下节点
					{
						t_rightNode.m_diffNext=t_downPos;
						writeBucketNode(t_rightPos,t_rightNode);
					}
				}
				else//无右节点
				{
					writeBucketPosValue(t_bucketPos,t_downPos);
				}
				continue;
			}
			if (t_upPos == 0)//无上节点
			{
				if (t_rightPos != 0)//有右节点
				{
					writeBucketPosValue(t_bucketPos,t_rightPos);
					if (t_downPos != 0)//有下节点
					{
						t_rightNode.m_diffNext=t_downPos;
						writeBucketNode(t_rightPos,t_rightNode);
					}
				}
				else //无右节点
				{
					writeBucketPosValue(t_bucketPos,t_downPos);
				}
			}
	    }
  	}
  	return true;
}
bool FileFilterInfoMgr::getFidPathFileList(vector<string> &r_vFileList,string& r_path)
{
	char t_filePrefix[30];
	sprintf(t_filePrefix,"%03d_%s_",m_channelNo,m_fileType.c_str());
	
	r_path=m_filePath.c_str();
	DIR * dir = opendir(m_filePath.c_str());
	if (dir == NULL)
	{
		RR_THROW_CP1(E_RR_FILE_OPEN,"打开FID文件存储目录 %s 失败!",m_filePath.c_str());
	}
	struct dirent* pDirent = 0;
	while ((pDirent = readdir(dir)) != NULL)
	{
		if ((strcmp(pDirent->d_name,".")  == 0) || (strcmp(pDirent->d_name,"..") == 0))
			continue;
		string t_fileName=r_path+string("/")+string(pDirent->d_name);
		struct stat t_type;
		if (lstat(t_fileName.c_str(),&t_type) < 0)
			continue;
		if ((t_type.st_mode & S_IFMT) == S_IFLNK)
			continue;
		if ((t_type.st_mode & S_IFMT) == S_IFDIR)
			continue;
		if (strncmp(pDirent->d_name,t_filePrefix,strlen(t_filePrefix)) != 0)
			continue;
		if (strlen(pDirent->d_name) != strlen(t_filePrefix) + 14)
			continue;
		r_vFileList.push_back(pDirent->d_name);
	}
	closedir(dir);
	return true;
}
bool FileFilterInfoMgr::showFileHeader(const string& r_filename)
{
	string t_fileName = r_filename;
	if (strstr(t_fileName.c_str(),m_filePath.c_str()) == NULL)
	{
		t_fileName  = m_filePath.c_str();
		t_fileName += "/";
		t_fileName += r_filename;
	}
	if(access(t_fileName.c_str(), F_OK) != 0)
	{
		cout<<"文件："<<t_fileName<<"不存在!"<<endl;
		return false;
	}
	strcpy(m_fileName2,t_fileName.c_str());
	if(openFile()==false)
	{
		return false;
		//异常处理
		//RR_THROW_CP1(E_RR_FILE_OPEN,"打开文件失败:{%s}!",m_fileName2);
	}
	readFileHeader();
	cout<<"---------- 文件头信息 ----------"<<endl;
	cout<<"文件是否已满    (m_fullFlag     )=" << m_fileHeader.m_fullFlag<<"(0-未满,1-已满)"<<endl;
	cout<<"文件大小        (m_fileSize     )=" << m_fileHeader.m_fileSize<<endl;
	cout<<"最大记录数      (m_maxNumber    )=" << m_fileHeader.m_maxNumber<<endl;
	cout<<"有效记录数      (m_nodeNumber   )=" << m_fileHeader.m_nodeNumber<<endl;
	cout<<"下一索引开始    (m_indexOffSet  )=" << m_fileHeader.m_indexOffSet<<endl;
	cout<<"下一索引节点开始(m_nextBucketSet)=" << m_fileHeader.m_nextBucketSet<<endl;
	cout<<"下一FID存放开始 (m_nextFidOffSet)=" << m_fileHeader.m_nextFidOffSet<<endl;
	cout<<"HASH桶的总数    (m_hashSize     )=" << m_fileHeader.m_hashSize<<endl;
	cout<<"最晚的时间点    (m_lastTime     )=" << m_fileHeader.m_lastTime<<endl;
	return true;
}
bool FileFilterInfoMgr::showFidFileCalInfo(const string& r_filename)
{
	string t_fileName = r_filename;
	if (strstr(t_fileName.c_str(),m_filePath.c_str()) == NULL)
	{
		t_fileName  = m_filePath.c_str();
		t_fileName += "/";
		t_fileName += r_filename;
	}
	if(access(t_fileName.c_str(), F_OK) != 0)
	{
		cout<<"文件："<<t_fileName<<"不存在!"<<endl;
		return false;
	}
	strcpy(m_fileName2,t_fileName.c_str());
	if(openFile()==false)
	{
		return false;
		//异常处理
		//RR_THROW_CP1(E_RR_FILE_OPEN,"打开文件失败:{%s}!",m_fileName2);
	}
	readFileHeader();	
	cout<<"---------- HASH索引区 ----------"<<endl;
	cout<<"有效记录数  ="<<m_fileHeader.m_nodeNumber<<endl;
	int iCount=0;
	fseek(m_fidFile,sizeof(FidFileHeader),SEEK_SET);
	size_t t_posValue;
	for (size_t i=0;i<m_fileHeader.m_hashSize;i++)
	{
		t_posValue = 0;
		fread(&t_posValue,sizeof(size_t),1,m_fidFile);
		if (t_posValue == 0)
			continue;
		iCount++;
	}
		
	cout<<"有效Hash桶数="<<iCount<<endl;
	cout<<"Hash桶总数  ="<<m_fileHeader.m_hashSize<<endl;
	
	long t_valid=iCount;
	long t_total=m_fileHeader.m_nodeNumber;
	float t_percent=(float)t_valid/(float)t_total;
	printf("Hash命中率:%.2f%%(有效Hash桶数/有效记录数)\n",t_percent*100);
	
	t_total = m_fileHeader.m_hashSize;
	t_percent=(float)t_valid/(float)t_total;
	printf("索引区空间利用率:%.2f%%(有效Hash桶数/Hash桶总数)\n",t_percent*100);

	cout<<"---------- HASH索引节点区 ----------"<<endl;	
	cout<<"索引节点开始位置："<<sizeof(FidFileHeader)+SIZE_T_SIZE*m_fileHeader.m_hashSize<<endl;
	cout<<"索引节点当前位置："<<m_fileHeader.m_nextBucketSet<<endl;
	cout<<"索引节点总长度  ："<<BUCKET_SIZE*m_fileHeader.m_maxNumber<<endl;
	t_valid=m_fileHeader.m_nextBucketSet - (sizeof(FidFileHeader)+SIZE_T_SIZE*m_fileHeader.m_hashSize);
	t_total=BUCKET_SIZE*m_fileHeader.m_maxNumber;
	t_percent=(float)t_valid/(float)t_total;
	printf("索引区空间利用率:%ld/%ld=%.2f%%(已使用索引节点空间/索引节点总空间)\n"
	       ,t_valid,t_total,t_percent*100);

	cout<<"---------- FID存储区 ----------"<<endl;
	cout<<"过滤信息存储开始位置:"<<sizeof(FidFileHeader)+SIZE_T_SIZE*m_fileHeader.m_hashSize+BUCKET_SIZE*m_fileHeader.m_maxNumber<<endl;
	cout<<"过滤信息存储当前位置:"<<m_fileHeader.m_nextFidOffSet<<endl;
	cout<<"过滤信息存储区总长度:"<<m_fileHeader.m_fileSize - (sizeof(FidFileHeader)+SIZE_T_SIZE*m_fileHeader.m_hashSize + BUCKET_SIZE*m_fileHeader.m_maxNumber)<<endl;
	t_valid=m_fileHeader.m_nextFidOffSet - (sizeof(FidFileHeader)+SIZE_T_SIZE*m_fileHeader.m_hashSize + BUCKET_SIZE*m_fileHeader.m_maxNumber);
	t_total=m_fileHeader.m_fileSize - (sizeof(FidFileHeader)+SIZE_T_SIZE*m_fileHeader.m_hashSize + BUCKET_SIZE*m_fileHeader.m_maxNumber);
	t_percent=(float)t_valid/(float)t_total;
	printf("过滤信息存储区空间利用率:%ld/%ld=%.2f%%(已使用过滤信息存储空间/过滤信息存储总空间)\n\n"
	       ,t_valid,t_total,t_percent*100);	
	return true;
}
bool FileFilterInfoMgr::calFidFileDetail(const string& r_filename,int iShowCount)
{
	string t_fileName = r_filename;
	
	if (strstr(t_fileName.c_str(),m_filePath.c_str()) == NULL)
	{
		t_fileName  = m_filePath.c_str();
		t_fileName += "/";
		t_fileName += r_filename;
	}
	if(access(t_fileName.c_str(), F_OK) != 0)
	{
		cout<<"文件："<<t_fileName<<"不存在!"<<endl;
		return false;
	}
	strcpy(m_fileName2,t_fileName.c_str());
	if(openFile()==false)
	{
		return false;
		//异常处理
		//RR_THROW_CP1(E_RR_FILE_OPEN,"打开文件失败:{%s}!",m_fileName2);
	}
	readFileHeader();
	map<size_t,size_t> t_mHashIndex;
	fseek(m_fidFile,m_fileHeader.m_indexOffSet,SEEK_SET);
	size_t t_posValue;
	for (size_t i=0;i<m_fileHeader.m_hashSize;i++)
	{
		t_posValue = 0;
		fread(&t_posValue,sizeof(size_t),1,m_fidFile);
		if (t_posValue == 0)
			continue;
		t_mHashIndex.insert(map<size_t,size_t>::value_type(i,t_posValue));
	}
	size_t t_pos;
	map<size_t,size_t>::iterator t_it = t_mHashIndex.begin();
	map<int,int> t_mCal;
	map<int,int>::iterator t_cal;
	for (; t_it != t_mHashIndex.end(); t_it ++)//所有hash索引节点
	{
		t_pos=t_it->first;
		t_posValue=t_it->second;
		//cout<<"HASH桶位置["<<t_pos<<"]------------------------------"<<endl;
		FileBucket t_node,t_node2;
		size_t t_lastDiffPos;
		int k=0;
		vector<size_t> t_vSameList;
		while (1)//处理一个hash桶
		{
			readBucketNode(t_posValue,t_node);
			t_vSameList.push_back(t_node.m_valuePos);
			k++;
			t_lastDiffPos=t_posValue;
			while (t_node.m_sameNext != 0)//处理所有横向节点
			{
				readBucketNode(t_node.m_sameNext,t_node2);
				t_vSameList.push_back(t_node2.m_valuePos);
				k++;
				t_node.m_sameNext = t_node2.m_sameNext;
				t_node.m_diffNext = t_node2.m_diffNext;
				t_node.m_valuePos = t_node2.m_valuePos;
			}
			//cout<<"m_sameNext列表,共("<<t_vSameList.size()<<")"<<endl;
			//for (int i=0;i<t_vSameList.size();i++)
			//{
			//	cout<<"["<<t_vSameList[i]<<"]";
			//	HisFidInfo t_fidNode;
			//	HisFilterInfoMgr::getNodeValue(m_fidFile,t_vSameList[i],t_fidNode);					
			//	cout<<"beginTime:"<<t_fidNode.m_beginTime
			//	    <<",endTime:"<<t_fidNode.m_endTime
			//	    <<",FID:"<<t_fidNode.m_fid.c_str()<<endl;
			//}
			readBucketNode(t_lastDiffPos,t_node);
			if (t_node.m_diffNext != 0)
			{
				t_posValue=t_node.m_diffNext;
				t_vSameList.push_back(0);
				//cout<<"m_diffNext="<<t_posValue<<endl;
			}
			else
			{
				break;
			}
		}
		if (iShowCount != -1 && iShowCount == k)
		{
			//------------------------------
			cout<<"HASH桶位置["<<t_pos<<"]"<<endl;
			for (int i=0;i<t_vSameList.size();i++)
			{
				if (t_vSameList[i] != 0)
				{
					HisFidInfo t_fidNode;
					HisFilterInfoMgr::getNodeValue(m_fidFile,t_vSameList[i],t_fidNode);					
					if (i!=0 && t_vSameList[i - 1] == 0)
						cout<<"   * ";
					else
						cout<<"     ";
					cout<<t_fidNode.m_beginTime<<" "
					    <<t_fidNode.m_endTime<<" "
					    <<t_fidNode.m_fid.c_str()<<endl;
				}
				//else
				//{
				//	cout<<"    "<<"diffNext . . . . . ."<<endl;
				//}
			}
		}
		t_cal=t_mCal.find(k);
		if (t_cal == t_mCal.end())
		{
			t_mCal.insert(map<int,int>::value_type(k,1));
		}
		else
		{
			t_cal->second ++;
		}
	}
	cout<<"------ Hash节点散列情况统计:"<<endl;
	for (t_cal=t_mCal.begin(); t_cal != t_mCal.end(); t_cal++)
	{
		printf("HASH链表内元素个数:%5d,该类型Hash桶数量:%5d\n",t_cal->first,t_cal->second);
	}
	return true;
}
bool FileFilterInfoMgr::insertFidInfo(const int &r_rrtype,
                      const vector<FilterInfo*> &r_filterList)
{
  int t_subFileNo=0;
  for(vector<FilterInfo*>::const_iterator t_itr=r_filterList.begin();
      t_itr!=r_filterList.end();++t_itr)
  {
    if((*t_itr)->m_uniqFlag!=E_RR_REPFLAG_UNIQ)
    { //不是唯一信息，则不需要提交
      continue;
    }
    generateFileName((*t_itr)->m_beginTime);
    //#ifdef _DEBUG_
      if((*t_itr)->m_beginTime/1000000 <20000101)
      {
        cout<<"时间信息不对!fid="<<(*t_itr)->m_fid<<" "
            <<(*t_itr)->m_beginTime<<" "
            <<__FILE__<<__LINE__<<endl;
      }
    //#endif
    for(t_subFileNo=0;t_subFileNo<100;t_subFileNo++)
    {
      sprintf(m_fileName2,"%s/%s%02d",m_filePath.c_str(),m_filePrefix,t_subFileNo);
      //add by gaojf 2009-2-13 15:17 begin
      if(isFullFile(m_fileName2)==true)
      { //文件已满
        continue;
      }
      //end 2009-2-13 15:17
  	  if(openFile()==false)
  	  {
  	    //异常处理
  	    RR_THROW_CP1(E_RR_FILE_OPEN,"打开文件失败:{%s}!",m_fileName2);
  	  }
  	  //判断文件是否已满
      fseek(m_fidFile,0,SEEK_SET);
      fread(&m_fileHeader,sizeof(FidFileHeader),1,m_fidFile);
      if(m_fileHeader.m_fullFlag == 1)
      {//满，则continue
        #ifdef _DEBUG_
          cout<<"文件{"<<m_fileName2<<"} 已满!"
              <<__FILE__<<__LINE__<<endl;
        #endif
        insertFullFileName(m_fileName2);//add by gaojf 2009-2-13 15:17
  	    continue;
  	  }else
  	  {//未满，则插入,break;
  	    //插入节点
  	    fseek(m_fidFile,m_fileHeader.m_nextFidOffSet,SEEK_SET);
  	    m_fidSize=strlen((*t_itr)->m_fid.c_str());
  	    fwrite(&m_fidSize,sizeof(int),1,m_fidFile);
  	    fwrite((*t_itr)->m_fid.c_str(),sizeof(char)*m_fidSize,1,m_fidFile);
  	    fwrite(&((*t_itr)->m_beginTime),sizeof(long),1,m_fidFile);
  	    fwrite(&((*t_itr)->m_endTime),sizeof(long),1,m_fidFile);
  	    m_nodeSize = m_nodeFixSize+m_fidSize;
  	    m_hashSize = m_fileHeader.m_hashSize;
  #ifdef FP_DEBUG
    FP_BEGIN(insertIdxNode)
  #endif
  	    //插入索引信息
        insertIdxNode(r_rrtype,m_fidFile,
                      m_fileHeader.m_indexOffSet,
                      m_fileHeader.m_nextBucketSet,
                      m_fileHeader.m_nextFidOffSet,
                      **t_itr);
  #ifdef FP_DEBUG
    FP_END(insertIdxNode)
  #endif

  	    //修改文件头信息
  	    m_fileHeader.m_nextBucketSet+=BUCKET_SIZE;
  	    m_fileHeader.m_nextFidOffSet+=m_nodeSize;
  	    m_fileHeader.m_nodeNumber++;
  	    if(m_fileHeader.m_lastTime<(*t_itr)->m_beginTime)
  	    {m_fileHeader.m_lastTime = (*t_itr)->m_beginTime; }
  	    if((m_fileHeader.m_nextFidOffSet>=
  	        m_fileHeader.m_fileSize - RR_FIDFILE_RESERVESIZE)||
  	        m_fileHeader.m_nodeNumber>=m_fileHeader.m_maxNumber)
        {
          m_fileHeader.m_fullFlag = 1;
        }
        fseek(m_fidFile,0,SEEK_SET);
    	  fwrite(&m_fileHeader,sizeof(FidFileHeader),1,m_fidFile);
  	    break;
  	  }
    }  
  }
  if(m_openFlag==true)
  {
    fclose(m_fidFile);
    m_openFlag = false;
  }
  return true;
}

//根据时间生成对应的文件名前缀
//r_time格式:YYYYMMDDHHMISS
//文件命名规则：ZZZ_XXX_YYYYMMDD_NN.nn
//    ZZZ为通道号,XXX为业务,YYYYMMDD 为日，NN为序号，nn为子序号
void FileFilterInfoMgr::generateFileName(const long r_time)
{ 
  return HisFilterInfoMgr::generateFileName(r_time,m_fileType,m_filePrefix);
}
bool FileFilterInfoMgr::openFile()
{
  return openFile(m_fileName2);
}
bool FileFilterInfoMgr::openFile(const char *r_fileName)
{
  if(m_openFlag == true)
  {
    if(strcmp(m_fileName1,r_fileName)==0 )
    {
      return true;
    }else
    {
      fclose(m_fidFile);
      m_openFlag=false;
    }
  }  
  //if(access(r_fileName, F_OK) != 0) modified by chenm 2009-9-17 17:05:11
  	if(m_fidFileNameList.find(r_fileName)==m_fidFileNameList.end())
  {
    if(createFile(r_fileName,m_fidFileSize)==false)
    {
      #ifdef _DEBUG_
        cout<<"createFile("<<r_fileName<<") false!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
	// add by chenm 2009-9-17 14:50:40
	m_fidFileNameList.insert(r_fileName);
	// over 2009-9-17 14:50:45    
  }
  //打开该文件
  strcpy(m_fileName1,r_fileName);
  m_fidFile=fopen(m_fileName1,"rb+");
  if(!m_fidFile)
  {
    return false;
  }
  m_openFlag=true;  
  return true;
}

bool FileFilterInfoMgr::createFile(const char * r_fileName,const size_t &r_fileSize)
{
  //创建该文件
  int t_fd;
  if ((t_fd=creat(r_fileName,S_IRUSR|S_IWUSR))<0)
  {
    #ifdef _DEBUG_
      cout<<"open file {"<<r_fileName<<"} error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  if(lseek(t_fd,r_fileSize-1,SEEK_SET)<0)
  {
    #ifdef _DEBUG_
      cout<<"lseek file {"<<r_fileName<<","<<r_fileSize-1<<"} error!"<<__FILE__<<__LINE__<<endl;
    #endif
    close(t_fd);
    unlink(r_fileName);
    return false;
  }
  char t_buf[2]="0";
  if (write(t_fd, t_buf, 1) != 1)
  {
    #ifdef _DEBUG_
      cout<<"write file {"<<r_fileName<<") false!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  close(t_fd);
  //////初始化该文件/////////////////
  memset(&m_fileHeader,0,sizeof(FidFileHeader));
  m_fileHeader.m_fileSize      = r_fileSize;
  m_fileHeader.m_maxNumber     = r_fileSize/RR_FID_SIZE; //按照RR_FID_SIZE个字节一个估算
  m_fileHeader.m_nodeNumber    = 0;
  m_fileHeader.m_hashSize      = m_fileHeader.m_maxNumber/2+1; //HASH桶大小
  m_fileHeader.m_indexOffSet   = sizeof(FidFileHeader);
  m_fileHeader.m_nextBucketSet = sizeof(FidFileHeader)+                        //头大小
                                 SIZE_T_SIZE*m_fileHeader.m_hashSize;          //桶空间;
  m_fileHeader.m_nextFidOffSet = m_fileHeader.m_nextBucketSet + 
                                 BUCKET_SIZE*m_fileHeader.m_maxNumber ; //索引空间
  m_fileHeader.m_lastTime      = 0; //最晚时间点，初始化为0
  if(m_fileHeader.m_nextFidOffSet>=r_fileSize - RR_FIDFILE_RESERVESIZE)
  {
    m_fileHeader.m_fullFlag=1;
    #ifde  memset(&m_fileHeader,0,sizeof(FidFileHeader));
  m_fileHeader.m_fileSize      = r_fileSize;
  m_fileHeader.m_maxNumber     = r_fileSize/RR_FID_SIZE; //鎸夌収RR_FID_SIZE涓