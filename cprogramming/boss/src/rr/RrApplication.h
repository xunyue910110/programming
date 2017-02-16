/**
*   排重模块APP类.
*   @author 高将飞 <gaojf@lianchuang.com>
*           
*   @version 1.0
*   @date 2008-08-08
*/
#ifndef RRAPPLICATION_H_HEADER_INCLUDED_B7642005
#define RRAPPLICATION_H_HEADER_INCLUDED_B7642005

#include "appfrm/Application.h"
#include "RrDir.h"
#include "FilterInfo.h"
#include "RrConfig.h"
#include "RrLog.h"
#include "FilterInfoMgr.h"
#include "RrConstDef.h"
#include <fstream> 
#include <sys/time.h> //added by hanyu 08-11-25

class RrApplication:public Application
{
  public:
    RrApplication();
    virtual ~RrApplication();
  protected:
    virtual bool initialization();
    virtual bool beforeLoop();
    virtual bool recovery();
    virtual bool loopProcess();
    virtual bool end();
    
  public:
    bool getSysParam();
    bool getNameFilterInfo(const char * r_fileName);//added by hanyu 08-12-02
    bool getCdrFilterInfo(const char * r_fileName);
    long getCdrNum(const char * r_fileName); //added by hanyu 08-12-02
    long getDealTime();//added by hanyu 08-12-01
    bool doProcess();
  protected:    
    bool isSameFileType(const char * r_fileName,const FileRrRule* &r_fileRrRule);
    void generateFileName(const char *r_fileName);
    //判断是否为已经处理过的文件
    bool isDealedFile(const char * r_fileName);
    void rewind();
    void addFilterInfo(const FilterInfo &r_filterInfo);
    bool outputCdr();
    bool outputFile();//added by hanyu 08-11-27
    bool commitFile();
    bool writeLog();
    void warning(const int &r_warncode,const string &r_warnMsg);
    void updateRtStatus(int r_state, const char* r_objectName, const char *r_remark);
    void unlinkTmpFile();
    static bool cpFile(const char *r_srcFileName,const char *r_desFileName);
  private:
    RrConfig           m_config;        //配置信息对象
    vector<FilterInfo> m_fidInfoList;   //过滤信息缓冲区
    size_t             m_fidCapcity;    //过滤信息容量
    size_t             m_fidCount;      //有效记录数
    FilterInfoMgr      m_filterInfoMgr; //过滤信息排重管理器
    RrLog              m_rrLog;         //合并文件排重日志
    RrMergLog          m_mergLog;       //文件合并日志
    RrDir              m_dir;           //目录管理
    char               m_lineBuff[E_RR_LINEMAXLEN+1];//话单缓冲区
    char               m_fieldValue[E_RR_LINEMAXLEN+1];//字段缓冲区
    
    vector<string>        m_srcFileNames;  //本次处理的文件
    Directory::SortFlags  m_sortMethod;    //排序方式
    /////////////////////////////////////////
    //原文件
    char             m_srcFile_fullName[PATH_MAX];
    //排重前合并后的中间文件
    char             m_workFile_fullName[PATH_MAX];
    //以下为目标中间文件
    char             m_uniqueFileName_tmp[PATH_MAX];
    char             m_repeatFileName_tmp[PATH_MAX];
    char             m_errorFileName_tmp[PATH_MAX];
    //以下为目标文件
    char             m_uniqueFileName[PATH_MAX];
    char             m_repeatFileName[PATH_MAX];
    char             m_errorFileName[PATH_MAX];
  private:
    //为了合并文件增加上个文件的类型等属性
    const FileRrRule * m_lastRrRule;     //当前处理的文件排重类型
    int                m_lastFileType_merge; //当前处理的文件类型
    bool               m_firstFileFlag;      //第一个未处理文件标记
    time_t             m_firstFileTime;      //第一个未处理文件时间
    time_t             m_curDealTime;        //当前文件处理的时间
    const FileRrRule  *m_fileRrRule;         //当前文件排重规则
    fstream            m_mergFile;           //合并后的中间文件
    
    //为了排重连续相同告警而增加的属性
    int                m_lastWarnCode; //最近一次告警代码
    size_t             m_lastWarnTime; //最近一次告警时间
    string             m_lastWarnMsg;  //最近一次告警信息
    //日志打开标记
    bool               m_openFlag;     
};
#endif // RRAPPLICATION1_H_HEADER_INCLUDED_B7642005 
