#ifndef __INPUTERCDR_H__
#define __INPUTERCDR_H__

#include "base/Directory.h"
#include "Inoutputer.h"
#include "FilterRule.h"
#include "FilterGlobalInfo.h"

const int MAXSCANNUM = 500;

class InputerCdr:public Inoutputer
{
  public:
    InputerCdr();
    virtual ~InputerCdr();
  public:
    bool  initialize(FilterGlobalInfo *r_gFilterInfo,InputRuleMgr *r_inputRuleMgr);

    int   getFile(PString &r_fileName); //-1 错误 ， 0 无文件 ，1 有文件
    bool  open();
    // 1 OK  0 文件结束 -1 失败
    int   getCdr();
    //返回值：-1 失败  0 成功 1 成功不需后续处理
    int   formatCdr();
    //将文件移至invalid目录
    int   moveErrFile();
    //文件结束处理
    bool  fileCommit();
  private:
    InputRuleMgr               * m_inputRuleMgr; //输入规则管理类
    InputRuleMgr::InputParam   * m_inputParam;   //子通道输入规则
    FilterGlobalInfo           * m_gFilterInfo;
    Directory                    m_dir;
    char                       * m_buffer;       //输入话单字符串
    int                          m_buffLen;      //缓冲区长度
  private:
    int                          m_fileSum;  //单目录处理文件个数
    int                          m_pathNo;   //子通道编号
    bool                         changeDirectory();
};

#endif
