//////////////////////////////////////////////////////////////////////
// 
// 类名: 主应用程序类
// 描述: 使用该类生成命令行程序的主要框架
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-23
// 版本: V1.0.0 
// 版权: 南京联创科技股份有限公司
//////////////////////////////////////////////////////////////////////

#ifndef ARCHY_APP_HEADER
#define ARCHY_APP_HEADER
#include "archy_config.hpp"
#include "noncopyable.hpp"
USING_STD_NAMESPACE;

BEGIN_NAMESPACE_ARCHY

class CAppMain:private noncopyable
{

public:
	CAppMain();
	virtual ~CAppMain(){}

public:
	virtual int Init() // initialization
	{ return 0;}

	virtual int Run() = 0; // main loop

	virtual int Exit() // cleanup (on the application exit)
	{ return 0;} 
	
public:
	static CAppMain* GetMain();
	static void SetMain(int argc, char **argv, char **envp);

public:
	static int GetArgc();
	static char** GetArgv();
	static char** GetEnvp();

	static void SetAppInfo(const string& strAppDesc, 
			const string& strVersion,
			const string& strOtherInfo
			);
	static void PrintAppInfo();

protected:
	// 只有在unix上才有效
	static void Daemon();
	static int KillOld(const string& sysid = "", int signo = 9);

	// 只有在unix上才有效
	// 创建一个子进程,返回子进程，父进程循环等待，一旦发现子进程死亡，立即重新启动子进程
	static void  ChildCall();
/*
 * Name: Singleton
 * purpose: 防止程序重起
 * parameter:
 *       sysid   -- 程序标识字符串
 *       iFlag   -- 0, 直接启动程序（如果已经重起，则不启动） 1, 测试程序是否是重起
 * return: 如果大于零，说明程序已经重起，而且iFlag 是1，返回的是正在运行的同一程序的pid
 *        
 */

	// 只有在unix上才有效
	static int  Singleton(const string& sysid = "", int iFlag = 0);

	
private:
	static CAppMain* m_pAppMain;
	static int m_argc;
	static char** m_argv;
	static char** m_envp;
	static string m_strAppDesc, m_strVersion, m_strstrDetail;
};

END_NAMESPACE_ARCHY

#endif


