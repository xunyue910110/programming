
#ifndef __TRANSFERAPP_H__
#define __TRANSFERAPP_H__


/**
 * \class TransferApp 
 * \brief 基于BUDP实现的文件传输应用
 *
 * 该应用通过调用文件传输组件实现相应的传输控制功能。应用根据配置信息动态调用一个
 * 或多个文件传输控制器，并顺序调度控制器执行实现一个或多个文件传输服务。
 *
 * 应用配置信息说明：每个应用包含一个或多个控制器所需要的配置描述信息，
 * 应用将每个控制配置信息描述为一个 task.(控制器配置信息在 @class FTController 描述)

@code
<0>
  task_count = 3
  <task0>
    protocal_type        = FTP
    action               = get
    ...
  </task0>  
  
  <task1>
    protocal_type        = FTP
    action               = get
    ...
  </task1>
  
  <task2>
    protocal_type        = FTP
    action               = put
    ...
  </task2>    
</0> 
@endcode

 */


#include "appfrm/Application.h"
#include "config-all.h"
#include <vector>

class FTController;

USING_NAMESPACE(std);


class TransferApp : public Application
{
  public:
    char m_errorMsg[200];
    int  m_errorCode;

  public:
    int writeLog(const string & log);
    int writeAlert(const string & msg);

  protected:
    virtual bool initialization();
    virtual bool loopProcess();
    virtual bool end();

  private:
    vector<FTController *> m_FTControllers;
    int m_TaskCount;
    int m_iTask;
    char m_cTemp[20];
    int m_rtn;

};
#endif // __TRANSFERAPP_H__





