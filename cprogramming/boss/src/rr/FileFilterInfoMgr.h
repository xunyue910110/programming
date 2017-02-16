#ifndef FILEFILTERINFOMGR_H_HEADER_INCLUDED_B7643042
#define FILEFILTERINFOMGR_H_HEADER_INCLUDED_B7643042
#include "HisFilterInfoMgr.h"
#include <set>

//过滤信息管理
class FileFilterInfoMgr : public HisFilterInfoMgr
{
  public:
    FileFilterInfoMgr();
    virtual ~FileFilterInfoMgr();
  public:
    bool initialization(const RrConfig &r_config);

    bool setFileType(const PString &r_fileType);
    bool isRepeatFid(const int &r_rrType,const FilterInfo& r_fidInfo,
                     const size_t  &r_timeDuration);
    bool insertFidInfo(const int &r_rrtype,const vector<FilterInfo*> &r_filterList);

    bool deleteFidInfo(const int &r_rrtype,const vector<FilterInfo*> &r_filterList);
	  bool queryFidInfo(const int &r_rrType
  		             ,const vector<FilterInfo*> &r_filterList
  		             ,const size_t  &r_timeDuration
  		             ,vector<string>& r_fileNameList);//如果存在，则返回文件名，否则返回空字符串
	  bool queryFidInfo(const int &r_rrType
					 ,const FilterInfo & r_filter
					 ,const size_t  &r_timeDuration
					 ,string& r_fileName);
  	//针对单个文件的统计信息展示	
  	//显示文件头信息
  	bool showFileHeader(const string& r_filename);
      //显示文件的统计信息
  	bool showFidFileCalInfo(const string& r_filename);
	  //统计hash节点散列程度明晰数据
    bool calFidFileDetail(const string& r_filename,int iShowCount=-1);
    bool getFidPathFileList(vector<string> &r_vFileList,string& r_path);
    
    string getFidFilePath(){return m_filePath.c_str();}
  protected:
    bool isRepeatFidInFile(const int  &r_rrType,const FilterInfo& r_fidInfo,
                           const char *r_fileName);
    //根据时间生成对应的文件名前缀
    //r_time格式:YYYYMMDDHHMISS
    void generateFileName(const long r_time);
    //创建文件：并按照过滤信息文件格式初始化文件  
    bool createFile(const char * r_fileName,const size_t &r_fileSize);
    //打开指定文件
    bool openFile();
    bool openFile(const char *r_fileName);
    //根据偏移量取对应的节点值
    void getNodeValue(const size_t &r_offSet);
    
    //根据偏移量判断是否重单
    bool isRepeatFidByIdx(FILE* r_fileFile,
                          const int        &r_rrType,
                          const FilterInfo &r_fidInfo,
                          const FileBucket &r_bucketNode);
    //插入HASH索引
    //r_hashPos     hash桶起始地址
    //r_hashNodePos 新BOCKET位置
    //r_nodePos     FID位置
    void insertIdxNode(const int  &r_rrtype,
                       FILE*       r_fileFile,
                 const size_t     &r_hashPos,
                 const size_t     &r_hashNodePos,
                 const size_t     &r_nodePos,
                 const FilterInfo &r_fidInfo);

    //add by chenzm
    bool readFileHeader(FidFileHeader * r_header=NULL);
    bool writeFileHeader(FidFileHeader * r_header=NULL);
    size_t getBucketPos(const char *r_fid);
    size_t getBucketPosValue(size_t r_bucketPos);
    bool writeBucketPosValue(size_t r_bucketPos,size_t r_bucketPosValue);
    void readBucketNode(size_t r_bucketNodePos,FileBucket &r_node);
    void writeBucketNode(size_t r_bucketNodePos,FileBucket &r_node);
    //bool getFidInfo(size_t r_fidPos,string& r_fid,long& r_beginTime,long&r_endTime);
	  bool queryFidInFile(const FilterInfo& r_fidInfo,const string& r_fileName);
    //add end
  private:
    PString m_fileType;            //文件类型
    char    m_fileName1[PATH_MAX]; //已打开的文件
    char    m_fileName2[PATH_MAX]; //将要处理的文件
    char    m_filePrefix[50];      //文件名前缀
    FILE*   m_fidFile;             //文件处理句柄
    bool    m_openFlag;            //句柄打开标记
  private:
    //以下为处理过程的临时变量
    FidFileHeader m_fileHeader;  
    int           m_fidSize;
    int           m_nodeFixSize;
    int           m_nodeSize;
    InnerNode     m_node;
    size_t        m_fidFileSize; //一个过滤文件大小
    size_t        m_hashSize;    //HASH桶大小
  private: //add by gaojf 2009-2-13 15:17
    vector<string> m_fullFileList; //存放已满文件列表
    bool isFullFile(const char *r_fileName); //判断文件是否已满
    void insertFullFileName(const char *r_fileName);
    
	// add by chenm 2009-9-17 for 替换access函数
	set<PString> m_fidFileNameList;   
};



#endif /* FILEFILTERINFOMGR_H_HEADER_INCLUDED_B7643042 */
