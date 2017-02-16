#ifndef _SUMBILLPROC_UNICOM_H_INCLUDE_20081211_
#define _SUMBILLPROC_UNICOM_H_INCLUDE_20081211_
#include "Procedure.h"
#include "RateConstDef.h"

class NullProc:public Procedure
{
  public:
    virtual int execute();
};

//普通增量合张过程
class SumBillProc: public Procedure
{
  public:
    virtual int execute();
  public:
    void setBillInfos();
  private:
    char  m_errmsg[128+1];
    //账单轨迹信息为:{用户ID|账单类型|帐期|资费实例|起始时间|账单项|值}
    // 多个信息间 以 分号 分割
    char  m_billTrack[TRACK_INFO_MAXLEN+1]; //计费账单轨迹
    int   m_billTrackLen;        //账单轨迹长度
};
//异常过程
class ErrorProc:public Procedure
{
  public:
    virtual int execute();
  private:
    char  m_errmsg[128+1];
};
//以账单项为单位全量覆盖式合张过程
class SumBillProc_Total : public Procedure
{
  public:
    virtual int execute();
  public:
    void setBillInfos();
  private:
    char  m_errmsg[128+1];
    //账单轨迹信息为:{用户ID|账单类型|帐期|资费实例|起始时间|账单项|值}
    // 多个信息间 以 分号 分割
    char  m_billTrack[TRACK_INFO_MAXLEN+1]; //计费账单轨迹
    int   m_billTrackLen;        //账单轨迹长度
};
//以账单项为单位全量覆盖式合张过程(不求帐目项)
class SumBillProc_OT : public SumBillProc_Total
{
  public:
    virtual int execute();
};
#endif //_SUMBILLPROC_UNICOM_H_INCLUDE_20081211_


