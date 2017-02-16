#ifndef _HISFILTERINFOMGR_H_INCLUDE_20080815_
#define _HISFILTERINFOMGR_H_INCLUDE_20080815_
#include "FilterInfo.h"
#include "RrConfig.h"
#include "RrConstDef.h"
#include <iostream>

//定义过滤信息文件，文件头结构
const size_t IDXADD_MAXNUM = 5000;//二级时间窗存放的最大量
const int SIZE_T_SIZE = sizeof(size_t);

class FidFileHeader
{
  public:
    int    m_fullFlag;      //文件是否已满标记:0 未满，1已满
    size_t m_fileSize;      //文件大小
    size_t m_maxNumber;     //允许最大记录数
    size_t m_nodeNumber;    //有效记录数
    size_t m_indexOffSet;   //索引起始位置 Hash桶
    size_t m_nextBucketSet; //下个索引节点的起始偏移量
    size_t m_nextFidOffSet; //下个FID存放的起始偏移量       
    size_t m_hashSize;      //HASH桶数
    long   m_lastTime;      //最晚的时间点
    char   m_reserve[1024]; //预留，用于以后扩充
                            //便于在已有数据后的调整
  public:
    friend ostream& operator<<(ostream &r_os,const FidFileHeader &r_obj)
    {
      r_os<<"m_fullFlag      = "<<r_obj.m_fullFlag<<endl
          <<"m_fileSize      = "<<r_obj.m_fileSize<<endl
          <<"m_maxNumber     = "<<r_obj.m_maxNumber<<endl
          <<"m_nodeNumber    = "<<r_obj.m_nodeNumber<<endl
          <<"m_indexOffSet   = "<<r_obj.m_indexOffSet<<endl
          <<"m_nextFidOffSet = "<<r_obj.m_nextFidOffSet<<endl;
      return r_os;
    }
};

//Hash索引 BUCKET结构
class FileBucket
{
  public:
    size_t m_sameNext; //FID相同的下个bucket位置
    size_t m_diffNext; //FID不同的下个bucket位置
    size_t m_valuePos; //该索引对应的值位置
  public:
    FileBucket()
    {
      m_sameNext=m_diffNext=0;
      m_valuePos=0;
    }
    
};

const int BUCKET_SIZE = sizeof(FileBucket);

//文件头 FidFileHeader: sizeof(FidFileHeader);
//中间为索引：偏移量索引m_maxNumber个。每个索引为sizeof(int)。
//            按照m_fid排序
//内容：存放FID信息。
//      FID信息结构: 
//              m_fidSize   :  m_fid所占字节数,m_fidSize本身字节数为sizeof(int)
//              m_fid       :  内容 ，占 m_fidSize 个字节
//              m_beginTime :  起始时间 , sizeof(long)
//              m_endTime   :  起始时间 , sizeof(long)
//

class HisFilterInfoMgr
{
  public:
    HisFilterInfoMgr();
    virtual ~HisFilterInfoMgr();
  public:
    class InnerNode
    {
      public:
        int  m_valueLen;
        char m_value[RR_FIDFILE_RESERVESIZE];
        long m_beginTime;
        long m_endTime;
    };
    class CmpFileName_Desc
    {
      public:
        virtual bool operator()(const string &r_left,const string &r_right)
        {
          return (strcmp(r_left.c_str(),r_right.c_str())>0);
        }
    };
  public:
    static long getCurTime() ;
    //根据时间点和增加时长，取对应的时间点
    static long getTime(const long &r_stdTime,const long r_add) ;
    //根据时间点、允许的最大时间跨度，求取时间点区域(每半个小时一个时间点)
    void getTimeLists(const long &r_stdTime,const size_t  &r_timeDuration,
                      vector<long> &r_timeList) const ; 
    //从指定文件中，根据偏移量取对应的节点值
    void getNodeValue(FILE* r_fidFile,const size_t &r_offSet,InnerNode &r_node) const;
    //从指定文件中，根据偏移量取对应的节点值
    void getNodeValue(FILE* r_fidFile,const size_t &r_offSet,HisFidInfo &r_hisFidInfo) const;
    //取该业务最近的文件列表(最后半小时)
    bool getLastFileList(const PString &r_fileType,vector<PString> &r_fileNameList,long &r_fileTime) const;
    //根据时间生成对应的文件名前缀
    //r_time格式:YYYYMMDDHHMISS ,r_fileName存放生成的文件明前缀
    void generateFileName(const long r_time,const PString &r_fileType,char *r_fileName) const;
  private:
    //根据文件序号：YYYYMMDD_NN
    long getTimeByFileNo(const char * fileNo) const;
  public:
    //从指定文件中, 根据偏移量取或写对应的BUCKET值
    void getBucketValue(FILE* r_fileFile,const size_t &r_offSet,FileBucket &r_node) const;
    void writeBucketValue(FILE* r_fileFile,const size_t &r_offSet,const FileBucket &r_node);
    void getAllIdxList(FILE*    r_fidFile,  size_t &r_indexNum,
                       size_t* &r_indexList,size_t &r_buffSize);
    //取该文件中最晚节点的时间
    long getLastNodeTime(FILE*  r_fidFile);
  private:
    void getIdxList(FILE* r_fidFile,size_t* &r_indexList,size_t &r_idxPos,
                    const FileBucket &r_bucketNode);
  protected:
    PString           m_filePath;   //FID目录
    int               m_channelNo;  //排重通道号
    size_t            m_onefileTime;//一个文件的时间跨度
  public:
	  //取Hash值
		static unsigned hashKey(const char *r_key)
		{
			unsigned t_ret_val = 0;
			int t_i ;
			
			while (*r_key)
			{
				t_i = (*r_key);
				t_ret_val ^= t_i;
				t_ret_val <<= 1;
				r_key++;
			}
			return t_ret_val;
		};
};

#endif //_HISFILTERINFOMGR_H_INCLUDE_20080815_
