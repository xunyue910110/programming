// AMC.hpp
// eAMC-API 定义文件
// 联机接口监控c++ api v5.1
// 作者：黄进兵
// 时间：2003-3-26 15:35
// 版本：5.0.0
//
#ifndef ARCHY_EXTEND_AMC_API_HEADER
#define ARCHY_EXTEND_AMC_API_HEADER

#include "thread.hpp"
#include "mutex.hpp"
#include "udp.hpp"
#include <vector>
#include <stdarg.h>
class CAmc;
class CAmcReport;
class CAmcThread;


USING_STD_NAMESPACE;
using namespace archy;

#define AMC_SPLIT	"^%^"	// 消息各字段之间的分隔符

// Application Monitor & Control class definition


class CAmcThread:public archy::thread
{
friend class CAmc;
friend class CAmcReport;
private:
	
	CAmc* m_amc;
public:
	// free_onterminate设为true，那么该类的对象必须在堆上创建,而且自动成功脱离状态
	CAmcThread()
	{}

protected:
	void run();
private:
	int SendTitle(const string& strTitle);
};

// 监控消息格式:
// Title Msg:(时间格式YYYYMMDDHH24MISS)
// V5.1~采集组IP~Title标识~程序路径~日志路径~PID~当前时间~NT(或者WT)~具体title
// 一般消息格式:
// V5.1~IP~Title标识~采集点唯一标识~端口号~PID~当前时间~(ND\WD)~具体内容
// 具体内容内必须首先包含:告警级别、告警类型
// 心跳格式:
// V5.1~IP~系统标识(可空)~程序路径~日志路径~PID~当前时间~SH~NOOP
// 退出格式:
// V5.1~IP~系统标识(可空)~PID~当前时间~SE~EXIT

// 磁盘信息: 文件名disk.info
// IP~mounted_on~total~used~free

// 告警返销
// V5.1~192.168.10.118~COREDAS~YANGZHOU~8080~12345~20030424103012~WC~告警类型

// 控制依然可以采用命令的方式，但是控制命令发往的路径必须重新指定

class CAmc:private archy::noncopyable
{
public:
	CAmc();
	~CAmc();

	// 设置一些属性,需要在设置标题之前进行设置
	
	void SetHdcAddr(const string& strIp, int iPort)
	{
		m_hdcIp = strIp; m_hdcPort = iPort;
	}
	void SetVersion(const string& strVer)
	{	m_strVersion = strVer;	}
	void SetIp(const string& strIp)
	{   m_strIp = strIp; }
	void SetRunPath(const string& runPath)
	{	m_runPath = runPath; }
	void SetLogPath(const string& logPath)
	{   m_logPath = logPath; }
	void SetPid(const string& pid)
	{   m_strPid = pid; }
	
	// 注册正常监视信息的标题
	void NormalTitle(const string&  strNormalTitle);
	// 注册告警监视信息的标题
	void WarningTitle(const string&   strWarningTitle);
	// 初始化amc对象，设定一个amc api的种类表示，例如所有的采集系统都可以使用
	// "coredas"进行初始化
	bool Start(const string&  strAmcType);
		
	// 心跳消息
	void Noop();
	
	enum{MODE_AMC = 0x01, MODE_STDOUT = 0x02};
	void SetMode(int mode){
		m_iMode = mode;
	}
	// 通知进程退出
	void Exit();
private:
	void AddReport(CAmcReport* report);
	void DelReport(CAmcReport* report);

private:

	int m_iMode;// 0 发往监控 1 发往stdout

	string m_strAmcType;
	string m_strVersion;
	string m_strIp;
	string m_runPath, m_logPath;
	string m_strPid;
	string m_hdcIp;
	int m_hdcPort;
	udp_socket m_so;
	archy::mutex m_mutex;
	archy::mutex m_mutexNormal;
	archy::mutex m_mutexAlarm;
	
	list<CAmcReport*> m_lstAmcReport;
	list<string> m_lstTitle;
	list<string> m_lstTitleWarn;
	list<string> m_lstAmcNormalMsg;
	list<pair<string, int> > m_lstAmcAlarmMsg;
//	list<string> m_lstTitles;
	friend class CAmcReport;
	friend class CAmcThread;
	CAmcThread m_thread;
};

// Amc Report class definition

class CAmcReport:private archy::noncopyable
{
	friend class CAmc;
	friend class CAmcThread;

private:
	CAmc* m_pAmc;
	string m_strId;
	int m_iLivingTime;
public:
	// ctor & dtor
	CAmcReport(CAmc& amc);
	~CAmcReport();
	
	// initialize methods
	void InitId(const string& strId) {
		m_strId = strId;
	}
	
	// reporting methods	
	// 汇报正常监视信息
	void NormalMsg(const string& strNormalMsg, int iTitleIndex = 0);

	// 要求最后一个实参是NULL(0)
	void Normal0(const char* msg1, ...);
	void Normal(int iTitleIndex, const char* msg1, ...);

	// 汇报告警监视信息，告警生存时间为iLivingSecs	
	void Warning(const string& strWarnGrade, const string& strWarnType, 
		const string&  strWarningMsg, int iTitleIndex = 0, int iLivingSecs = 1);
	
	// 要求最后一个实参是NULL(0)
	
	void Warning0(const char* msg1, ...);
	void Warning1(const string& strWarnGrade, const string& strWarnType, 
		const char* msg1, ...);
	void Warning2(int iTitleIndex, const string& strWarnGrade, 
		const string& strWarnType, const char* msg1, ...);
	void Warning3(int iTitleIndex, int iLivingSecs, 
		const string& strWarnGrade, const string& strWarnType, const char* msg1, ...);
	
	// 告警取消
	void WarningCancel(const string& strWarningType, int iTitleIndex = 0);
	
	void WarningV(const string& strWarnGrade, const string& strWarnType,const char* msg1, 
		va_list args, int iTitleIndex = 0, int iLivingSecs = 1 );
	void NormalV(const char* msg1, va_list args, int iTitleIndex = 0);

private:
	void AddMsg(int  iType, const string& strMsg, int iLivingSecs=0);
};

#endif // ARCHY_EXTEND_AMC_API_HEADER


