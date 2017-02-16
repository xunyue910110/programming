
/**
 * 此DialogTool对建立参数服务器进行监控管理
 *
 *   被Server直接引用作为server进程内交互管理处理类；
 *   也可以直接调用通过远程连接管理server进程。
 *
 */ 

#ifndef __DIALOGTOOL_H__
#define __DIALOGTOOL_H__

#include "Config.h"

#include "Thread.h"

#include <string>
class Manager;

class DialogTool {
  public:
    //远程管理模式构造为Client模式，通过通信连接通服务端交互
    DialogTool(); 
    
    //服务进程构造模式。交互过程直接调用Server::handleRequest方法    
    DialogTool(Manager * gparam);
    
    virtual ~DialogTool();
    
    // 执行交互动作
    virtual void  dialog();

    // 将交互启动到新的线程运行()
    // join_thread 标志是否要等待线程结束。
	int start(const bool & join_thread ); 


  private:
  	static void  thread_proc acceptThread(void*); 
	int    stop();
	Thread m_thread;
	
	void connect(const char *buf);
	void handleRequest(const char *buf);

	virtual char *dump(char* buf);            
    virtual void  trace(char const* fmt, ...);
    virtual void  help();
    void input(char const* prompt, char* buf, size_t buf_size);
    
    void dataDump(char* buf);   

    // 上下文环境 root, session, server, data
    // server 执行的命令server的操作
    // data 执行的命令数据管理的操作 
    string m_contex; 

    Manager * m_gparam; 
    bool m_needDestory;
};



#endif




