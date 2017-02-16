/* Copyright (c) 2003,Linkage  All rights reserved. */

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "base/config-all.h"
#include <string>

USING_NAMESPACE(std)

/// 应用程序运行状态
#define PROC_READY '0'  /// 就绪
#define PROC_RUN   '1'  /// 运行
#define PROC_EXIT  '2'  /// 已退出
#define PROC_ABORT '3'  /// 异常退出
#define PROC_WAIT  '4'  /// 等待
#define PROC_IDLE  '5'  /// 空闲
#define PROC_FATAL '6'  /// 致命错误
#define PROC_WARN  '7'  /// 告警 
#define PROC_PAUSE '^'  /// 暂停


const int MAXAPPNAME = 255;

/// 应用程序框架错误常量定义
const int E_NOERROR           = 0; ///无错误              
const int E_SYSNOTIFY_STARTUP = 1; ///启动错误                
const int E_SYSNOTIFY_STOP    = 2; ///停止错误                
const int EAPPFRAME_BASE      = 200;               
const int EAPPFRM_SYSINIT     = EAPPFRAME_BASE + 1; ///初始化错误
const int EAPPFRM_SYSVAL      = EAPPFRAME_BASE + 2; ///系统变量错误
const int EAPPFRM_ARG         = EAPPFRAME_BASE + 3; ///应用参数错误
const int EAPPFRM_SCANF       = EAPPFRAME_BASE + 4; ///扫面错误
const int EAPPFRM_PROC        = EAPPFRAME_BASE + 5; ///处理错误


class SysParam;
class ErrorMessages;
class ServiceMonitor;
class Directory;
class VersionInfo;


///
/// @class Application
///
/// @brief 应用程序框架类。
///
/// 该类定义了BUDP环境中应用程序高级行为。BUDP中新的应用从该类继承并实现该类定义的
/// 以下方法 initialization(),recovery(),loopProcess(),end()等等。
///  

/**
@code
 下面是应用程序框架的方法调用次序:
     
      NOTE:
         [VF] - virtual member function
     
       main
        |-setopt()
        +-run()
           |- [VF] initialization()
           |         |- parseOpt()
           |         |    |- getopt()
           |         |    |- [VF]processopt()
           |         +- create/init ErrorMessages SysParam Log Alert State
           |
           |- [VF] recovery()
           |
           |- preLoop()
           |    |- if need run_onlyone()
           |    |- if need fork_child()
           |    |- if need to_background()
           |    |- update registerProcessInfo for maybe new pid
           |    +- close m_theSysParam all opened section
           |
           |
           |- [VF] beforeLoop();
           |
           |
           |- mainLoop()
           |    |-- updateStatus() once
           |    |
           |   while(1)
           |    |----> process control command
           |    |----> [VF] loopProcess()
           |    |----> write log
           |    +----> write alert
           |
           +- [VF] end()
                     |-- write alert
                     +-- end/delete ErrorMessages SysParam Log Alert State


 应用框架定义的虚拟成员:    
       VIRTUAL MEMBER FUNCTION CALL BY FRAMWORK            

       processopt()                                      
         -- current is no influence, reference below.    
         -- 处理应用程序的命令行

       initialization()                                  
         -- call base class initialization() first       
         -- 应用程序初始化(要求先调用框架中的初始化方法)

       recovery()                                        
         -- to be determined         
         -- 应用程序恢复
                              
       end()                                             
         -- add your code before call base class end()   
         -- 应用结束,释放相关的资源,并在最后调用框架中的end()方法。

       loopProcess()                                     
         -- write your owner process                     
            Application::loopProcess is only return true.
         -- 应用的处理部分，继承应用通过实现这个方法完成相关处理。


 可以由继承类调用的保护成员:
-------------------------------------------------------------------------//
 PROTECTED MEMBER FUNCTION CAN CALLED BY YOUR DERIVED APPLICATION CLASS
   void writeAlert();
   void updateStatus();
   void setoptstr(const char * optstring);
   bool get_asdaemon() const;
   void set_asdaemon(bool left);
   bool get_onlyone() const;
   void set_onlyone(bool left);
   bool get_runinbkg() const;
   void set_runinbkg(bool left);
---------------------------------------------`----------------------------//

-------------------------------------------------------------------------//
 PROTECTED MEMBER DATA CAN USED BY YOUR DERIVED APPLICATION CLASS
   int  m_channelNo;
   char m_name[MAXAPPNAME + 1];
   string m_logstr;
   string m_objectName;
   string m_reserved;
   char   m_status;
   SysParam * m_theSysParam;
   ErrorMessages *m_theErrorMessages;
-------------------------------------------------------------------------//
@endcode

*/

 
class Application {
  public:
    Application();
    ///
    ///Run 应用程序运行.
    ///该过程一次调用 初始化过程initialization()，预循环过程 preLoop(),循环处理过程
    ///mainLoop(),结束过程 end();
    ///
    ///@return 0>=0 表示成功
    int run();
    /// 应用程序命令行简单处理 设置应用名m_name 成员。
    void setopt(int argc, char **argv);

  public:
    /// 解析命令行选项
    virtual bool processopt(int optopt, const char *optarg);
    /// 初始化应用
    virtual bool initialization(); 
    /// 恢复应用
    virtual bool recovery();
    /// 应用结束
    virtual bool end();
    /// 循环前处理
    virtual bool beforeLoop();
    /// 循环处理
    virtual bool loopProcess();
    /// 系统信号发生
    virtual void onSignal(const int sig);

    /// 设置命令行选项
    void setoptstr(const char * optstring);
    /// 写出告警信息
    void writeAlert();

    /// 更新应用状态
    void updateStatus();
    void updateStatus(const char * status);

    /// 写出日志信息
    void writeLog();
    void writeLog(int typeId, const string& str);
    void writeLog(int typeId, const char* str);
    
    /// 应用等待一段时间
    /// @param sec 等待的秒数
    void ssleep(int sec);
    
    /// 是否为后台服务应用
    bool get_asdaemon() const;    
    /// 设置为后台服务应用
    void set_asdaemon(bool left);
    /// 是否仅仅运行一次
    bool get_onlyone() const;
    /// 设置仅仅运行一次     
    void set_onlyone(bool left);
    /// 是否后台执行
    bool get_runinbkg() const;
    /// 设置后台执行
    void set_runinbkg(bool left);


    /// 设置文件输入路径
    void setPathIn(const char *path);
    /// 文件匹配模式    
    void setFilter(const char *pattern);
    ///文件排序方法
    void setFileSortMethod(const int method);
    ///设置排序文件数
    void setMaxScan(const int num);
    /// 取得一个待处理的文件
    /// @param path 文件名(包含路径)
    /// @param length 文件名最大长度
    /// @return <0 没有得到相应的文件
    int  getFile(char *path, const int length);
    int  getFile(char *path, const int length , unsigned long& filesize);

    /// 设置版本信息
    void setVersion(const VersionInfo & version); 
    /// 显示版本信息
    void showVersion();

  private:
    void preLoop();
    int  mainLoop();
    static void term_proc(int sig);
    static void fork_child();
    static void to_background();
    static int  run_onlyone(const char *unifname);
    int  getopt(char *opts);
    bool parseOpt();


  protected:
    char m_name[MAXAPPNAME + 1];
    int  m_channelNo;
    string m_loggerDn;

    string m_logstr;
    int m_stop;
    
    string m_objectName;
    string m_reserved;
    int m_status;
    SysParam *m_theSysParam;
    ErrorMessages *m_theErrorMessages;

  private:
    static volatile char m_curcmd;
    char   m_optopt;
    char  *m_optarg;
    int    m_argc;
    char **m_argv;
    int    m_optind;
    int    m_sp;
    char   m_optstring[MAXAPPNAME + 1];
    
    long   m_pid;

    bool   m_onlyone;
    bool   m_asdaemon;
    bool   m_runinbkg;
    
    // application attributes
    string  m_lockpath;
    ServiceMonitor* m_theMonitor;
    int m_logTypeId;

    string m_configFile;
    
    bool m_showVersion;
    VersionInfo * m_version;
    
    Directory * m_dir;
    int         m_fsortmethod;
    string      m_pattern;

};

typedef Application FileApplication;

#include "globalobj.h"

#endif /* __APPLICATION_H__ */


