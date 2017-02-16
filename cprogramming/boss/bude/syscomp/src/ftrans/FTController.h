#ifndef __FTCONTROLLER_H__
#define __FTCONTROLLER_H__



/** 
 *  \class FTController  
 *  \brief 文件传输控制器类(file transfer controller)。
 *
 *  该控制器根据用户提供的配置信息控制文件的传输。
 *
 */



#include <string>

using namespace std;



class ControlInfo;
class FSystemAgent;

typedef int (*WriteLogHook)(const string & msg); 
typedef int (*WriteAlertHook)(const string & msg);
typedef int (*WriteStateHook)(const string & msg);


class FTController {
  public:
    /**
     * 初始化传输控制器。根据配置信息初始化控制器，连接相应的文件系统。
     * @param conf_file  指定配置文件路径.
     * @param section_path 配置信息在配置文件中的位置
     * @return >=0 初始化成功，否则失败。
     */
    int initialize(const string& conf_file,const string& section_path );
    
    /**
     * 执行传输动作
     * @return >=0 初始化成功，否则失败。
     */
    int execute();  
    
    /**
     * 结束传输控制器，释放资源以及外部连接
     */
    int end();

    bool m_isOk;

    FTController();
    ~FTController();

  public:
    /**
     * 传输控制器使用的日志回调钩子函数。
     * 通过该钩子控制器将传输的日志信息输出给外部调用者。
     */
    WriteLogHook writeLogHook;

    /**
     * 传输控制器使用的告警回调钩子函数。
     * 通过该钩子控制器将传输的告警信息输出给外部调用者。
     */
    WriteAlertHook writeAlertHook;

    /**
     * 传输控制器使用的状态回调钩子函数。
     * 通过该钩子控制器将传输的状态信息输出给外部调用者。
     */
    WriteStateHook writeStateHook;    

  private:
    int m_filenum;
    int get();
    int put();
    int writeLog(const string & msg) const;
    int writeAlert(const string & msg) const;
    int writeState(const string & msg) const;

  private:
    FSystemAgent * m_remote;
    FSystemAgent * m_local;
    ControlInfo  * m_controlInfo;

};

#endif //__FTCONTROLLER_H__






// -----------------------------------------------------------------------------
// 下列为 doxygen 自动化文档服务的部分


/**
 *  \mainpage FTP文件传输组件

\section sec_intro 综述
 FTP文件传输组件是BOSS1.5应用依赖的基础部件。该组件实现较完整文件传输控制功能，
 通过封装为使用者提供简洁的接口。本组件包括一套面向传输控制的API封装和一个完整的传输应用。
 

的实现类图如下：
\image HTML  relationships.jpg



对CBs或其他后台BOSS1.5应用程序,通过调用组件接口,实现文件传输功能。
组件以C++为实现方式,提供应用开发编程接口。使用方式有:

\arg 以链接库的方式给应用程序使用，即API方式。                                  

\arg 一个独立运行的文件传输应用，各个子系统直接使用。


\section sec_apply  引用
FTP文件传输组件将以静态库(.a)形式发布，静态库名字为libftp.a；同时提供一个基于BUDP框架的完整应用transfer。

使用该组件的应用需要做以下工作：

\arg 包含头文件FTController.h 获得带有控制功能的传输控制器。此时调用者要提供相信的
 控制器需要配置信息,并调度控制器运行。

@code 
  FTController ftcontroller;
  ftcontroller.initialize("/boss/ftp/config.cfg","\\tests\\1\\task0\\");
  ftcontroller.execute();
  ftcontroller.end();
@endcode

\arg 包含头文件FSystemAgent.h 通过调用getInstatnce获得远程或本地文件系统操作代理，
 实现相应的文件传输和管理操作。

@code 
  FSystemAgent * remote = FSystemAgent::getInstance("FTP");
  remote->initialize();
  if (remote->connect("192.168.0.1","21","ftp","") <0) {
  	// connect error 
  }
  
  remote->getFile("/ftp/D000000","/local/ftp/D000000");
  
  remote->putFile("/ftp/D000000","/local/ftp/D000000");
  
  remote->disconnect();
  
@endcode


\arg 编译时连接libftp.a ( -lftp )


*/
