//////////////////////////////////////////////////////////////////////
// 
// 类名: 主应用程序类
// 描述: 使用该类生成命令行程序的主要框架
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-23
// 版本: V1.0.0 
// 版权: 南京联创科技股份有限公司
//////////////////////////////////////////////////////////////////////
#include "CAppMain.hpp"
#ifndef WIN32
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include "cmd_line.hpp"
#endif
#include "cmd_line.hpp"

BEGIN_NAMESPACE_ARCHY


CAppMain* CAppMain::m_pAppMain = 0;
int CAppMain::m_argc = 0;
char** CAppMain::m_argv = 0;
char** CAppMain::m_envp = 0;
string CAppMain::m_strAppDesc; 
string CAppMain::m_strVersion;
string CAppMain::m_strstrDetail;

CAppMain::CAppMain()
{
	if( m_pAppMain ) {
		throw runtime_error("Two or more CAppMain objects is not allowed");
	}
	m_pAppMain = this;
}

CAppMain* CAppMain::GetMain()
{
	return m_pAppMain;
}

void CAppMain::SetMain(int argc, char **argv, char **envp)
{
	m_argc = argc;
	m_argv = argv;
	m_envp = envp;
}


void CAppMain::SetAppInfo(const string& strAppDesc, 
		const string& strVersion,
		const string& strDetail)
{
	m_strAppDesc = strAppDesc;
	m_strVersion = strVersion;
	m_strstrDetail = strDetail;
}

void CAppMain::PrintAppInfo()
{
	if( !m_strAppDesc.empty()) {
		cout <<endl<<"    Description:"<<m_strAppDesc<<endl;
		cout <<endl<<"    Version    :"<<m_strVersion<<endl;
		cout <<endl<<"    Detail     :"<<m_strstrDetail<<endl;
		cout <<endl<<"    南京联创科技股份有限公司版权所有"<<endl<<endl;
	}

}

int CAppMain::GetArgc() { return m_argc; }
char** CAppMain::GetArgv() { return m_argv; }
char** CAppMain::GetEnvp() { return m_envp; }

void CAppMain::Daemon()
{
#ifndef WIN32
	pid_t	pid;

	signal(SIGHUP, SIG_IGN);
	if ( (pid = fork()) != 0)
		exit(0);			

	// become session leader
	setsid();

	signal(SIGHUP, SIG_IGN);
	if ( (pid = fork()) != 0)
		exit(0);

#endif
}

/*
 * Name: Singleton
 * purpose: 防止程序重起
 * parameter:
 *       sysid   -- 程序标识字符串
 *       iFlag   -- 0, 直接启动程序（如果已经重起，则不启动） 1, 测试程序是否是重起
 * return: 如果大于零，说明程序已经重起，而且iFlag 是1，返回的是正在运行的同一程序的pid
 *        
 */

int CAppMain::Singleton(const string& sysid, int iFlag)
{
#ifndef WIN32
	int iLockFile;
	struct flock sLock;
	
	string strFileName(sysid);
	if( strFileName.empty()) {
		strFileName = CAppMain::GetArgv()[0];
	}

	strFileName += ".avd";
	
	if ((iLockFile = open(strFileName.c_str(), O_RDWR|O_CREAT, 0666)) == -1) {
		perror("CAppMain::Singleton:open file");
	}
	fchmod(iLockFile, 0666);
	sLock.l_type = F_WRLCK;
	sLock.l_start = 0;
	sLock.l_whence = SEEK_SET;
	sLock.l_len = 1;
	if(fcntl(iLockFile, F_GETLK, &sLock) == -1){
		perror("fcntl");
		close(iLockFile);
	}
	if(sLock.l_type != F_UNLCK) {
		pid_t pid = sLock.l_pid;		
		if( iFlag == 0 ) {
			fprintf(stderr, "CAppMain::Singleton:pid:%d is in running, you can not restart it\n", pid );
			exit(0);
		}
		else {
			close(iLockFile);
			return pid;
		}
	}
	if( iFlag != 0 ) {
		close(iLockFile);
		return 0;
	}
	// 加纪录锁
	sLock.l_type = F_WRLCK;
	if( fcntl(iLockFile, F_SETLK, &sLock) == -1 ) {
		close(iLockFile);
		perror("CAppMain::Singleton:fcntl");
	}
#endif
	return 0;
}


void	CAppMain::ChildCall()
{
#ifndef WIN32
	pid_t pid;
	while(1) {
		pid = fork();
		if( pid < 0 ) {
			printf("fork call failed");
			exit(-1);
		}
		else if( pid == 0 ) {
			return;
		}
		string strSysid = string(GetArgv()[0]) + string(".farther");
		Singleton(strSysid);
		wait(NULL);
	}
#endif

}

int CAppMain::KillOld(const string& sysid /*= ""*/, int signo/* = 9*/)
{
#ifndef WIN32
	archy::cmd_line cmdline(GetArgc(), GetArgv());
	if( cmdline.exists("-k")) {
		string strSysid = string(sysid == "" ? GetArgv()[0] : sysid) + string(".farther");
		int pid = Singleton(strSysid, 1);
		if( pid > 0 ) {
			kill(pid, signo);
		}
		pid = Singleton(sysid, 1);
		if( pid > 0 ) {
			kill(pid, signo);
		}		
		exit(0);
	}
#endif
	return 0;
}

END_NAMESPACE_ARCHY


#ifndef ARCHY_NO_MAIN_DEFINE

using namespace archy;

int main( int argc, char **argv, char **envp )
{
	CAppMain* pApp = CAppMain::GetMain();

	if( pApp == 0) {
		cerr << "You must define a CAppMain object firstly" << endl;
		return -1;
	}
	pApp->SetMain(argc, argv, envp);
	int iRet;
	try {
		iRet = pApp->Init();
		if( iRet != 0 )
			return iRet;

		pApp->PrintAppInfo();

		iRet = pApp->Run();

		if( iRet != 0 )
			return iRet;

		iRet = pApp->Exit();
	}
	catch(const exception& e) {
		cerr << "Catch Standard Exception:" << e.what() << endl;
		return -1;
	}
	catch(...) {
		cerr << "Catch Unkown Exception:" << endl;
		return -1;
	}

	return iRet;
}

#endif

