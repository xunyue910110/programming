
#ifndef __INPUTER_H__
#define __INPUTER_H__

#include "Inoutputer.h"
#include "base/Directory.h"
#include "InoutputRule.h"
#include "RateGlobalInfo.h"

//Added By Sunam 2009/8/10 19:21:56
#include "FieldsXX.h"
//文件中最大记录数
const int MAXRECORDS = 40000;
//记录最大长度
const int MAXLENGTH  = 1024;

class Inputer :public Inoutputer
{
  public:
    Inputer();
    virtual ~Inputer();
  public:
    bool  initialize(RateGlobalInfo *r_gRateInfo,InputRuleMgr *r_inputRule);
    int   getFile(PString &r_fileName,PString &r_fullFileName); //-1 错误 ， 0 无文件 ，1 有文件
    bool  open();
    // 1 OK  0 文件结束 -1 失败
    int   getCdr();
    //返回值：-1 失败  0 成功 1 成功不需后续处理
    int   formatCdr();
    void  setPatternType(const PATTERNTYPE &r_patterType);
    bool  fileCommit();
    //输出延迟话单
    bool  outputDelayCdr();
    //将延迟用户从延迟目录移至输入目录下.
    bool  dealDelayedFile();
    //将文件移到无效目录下
    bool  mvFileToInvalidPath();
    
    //根据数据源匹配输入话单格式 : OCS采用此方式
    bool setInputRule(const char *r_sourceType,const PATTERNTYPE &r_patternType=PATTERN_TYPE_SOURCETYPE);
    
    //Added By Sunam 2009/8/10 19:27:30 获取当前未处理用户
    void genNoDealUser();
    //--m_recordIndex
    void previousCdr();
  public:
    // 支持固定费话单 ： 一次性从文件中读取多条记录userid相同的
    int getCdr(T_SERVER_ID &r_userId,vector<PString> &r_rentCdrList);
    InputRule* getInputRule();
    PString                m_remainCdr; //缓冲话单
    bool                   m_remainFlag;//有无缓冲话单标记
  private:
    RateGlobalInfo       * m_gRateInfo;
    PATTERNTYPE            m_patternType;  //匹配方式：1文件名 2SourceType
    InputRuleMgr         * m_inputRuleMgr; //输入规则管理类
    InputRule            * m_inputRule   ; //当前输入规则
                         
    Directory              m_dir;
    char                 * m_buffer;        //输入话单字符串
    int                    m_buffLen;       //缓冲区长度
    Inoutputer             m_delayFile;     //延迟文件类
    
    //Added By Sunam 2009/8/10 19:21:46
    //缓存当前文件中所有记录
    char                   m_fileBuffer[MAXRECORDS][MAXLENGTH];
    //当前记录索引号
    int                    m_recordIndex;
    //文件中总记录数
    int                    m_recordNum;
    //存放当前文件中所有用户列表
    vector<T_SERVER_ID>    m_userList;
};

#endif

