#ifndef RRCONFIG_H_HEADER_INCLUDED_B7645DF4
#define RRCONFIG_H_HEADER_INCLUDED_B7645DF4
#include "FileRrRule.h"
#include "base/PString.h"


class SysParam;
class RrConfig
{
  public:
    RrConfig();
    virtual ~RrConfig();
  public:
    //r_rootPath为rr所在模块的配置根路径
    //r_channelNo为通道号 
    bool getSysParam(SysParam *r_sysParam,const char *r_rootPath,const int &r_channelNo);
    const FileRrRule* getRrRule(const char* r_fileName);
    bool  end();
    friend ostream& operator<<(ostream &r_os,const RrConfig &r_config);
    AbstractFieldSelector* generateSelect(char *r_strRule,const char r_delimiter,const size_t r_maxLen=0);
  	bool getFilePrefixList(vector<string>& r_vPrefixList);
  protected:
    //判断该文件目录是否有效
    bool  isValidPath(const char * r_path) const;
  public:
    vector<FileRrRule> m_rrRuleList;
    
    int     m_channelNo;    //通道号
    PString m_inputPath;    //输入目录
    PString m_workPath;     //工作目录
    PString m_uniqPath;     //非重单输出目录
    PString m_repeatPath;   //重单输出目录
    PString m_errPath;      //错单输出目录
    size_t  m_mergeCount;   //合并记录数限制
    size_t  m_waiteTimeOut; //合并超时限制
    
    PString m_fidPath;      //过滤信息文件目录
    size_t  m_fidMemTime;   //内存存放的过滤信息时间段跨度
    size_t  m_fidMaxNum;    //内存存放的过滤信息最大条数限制
    size_t  m_fidFileSize;  //一个过滤信息文件大小：字节数
    long    m_onefileTIme;  //一个文件存放的时间跨度
    
    bool    m_rrUpdateFlag; //是否更新rrFlag标记
};



#endif /* RRCONFIG_H_HEADER_INCLUDED_B7645DF4 */
