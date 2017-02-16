#ifndef __OUTPUTERCDR_H__
#define __OUTPUTERCDR_H__
#include "Inoutputer.h"
#include "FilterRule.h"
#include "FilterGlobalInfo.h"

class OutputerCdr:public Inoutputer
{
  public:
    OutputerCdr();
    virtual ~OutputerCdr();

  public:
    bool  initialize(FilterGlobalInfo *r_gFilterInfo,OutputRuleMgr *r_outputRuleMgr);
    bool  open();
    bool  open(const int &r_mode);
    bool  close();
    bool  output();       //输出话单
    bool  fileCommit();   //文件级提交
    bool  fileRollback(); //文件回退，删除工作文件

  private:
    PString          m_workfullName;  //工作全路径文件
    PString          m_outfullName;   //输出全路径文件
    PString          m_prefixName;    //话单前缀
    PString          m_outpathName;   //路径名
    int              m_recordNum;     //记录数
    bool             m_isOpen;        //是否已经打开 true:打开 false:关闭

    Inoutputer                   m_errorFile;      //错单文件
    OutputRuleMgr                * m_outputRuleMgr;  //输出规则管理类
    OutputRuleMgr::OutputParam   * m_outputParam; //子通道输出规则
    FilterGlobalInfo             * m_gFilterInfo;

    int           outputfile_num;    //最大创建输出类个数
    OutputerCdr   *map_outputerCdr;  //用于创建多个输出类
    map<PString,OutputerCdr*>                m_map_outputer;    //first为通道名(prov trash...)
    map<PString,map<PString,OutputerCdr*> >  m_map_outputerMgr; //first 子通道名(channel_1...)
    void mapOutclean();  //清空m_map_outputer状态
  private:
    ///以下为单个类成员调用
    void setOutName(const char * r_prefixName,const char *r_fileName,const char *r_path); //设置输出文件名、路径
    bool initialdetail(FilterGlobalInfo *r_gFilterInfo,
             OutputRuleMgr::OutputParam * r_outputParam); //重新获取全局参数地址
    bool outputdetail(const string &r_channelName); //具体输出记录级话单
    bool  renameFile();  //work文件->out文件
    bool  unlinkWorkFile() const;

};

#endif //__OUTPUTERCDR_H__
