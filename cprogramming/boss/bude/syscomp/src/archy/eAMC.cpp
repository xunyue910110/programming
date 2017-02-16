#ifdef _MBCS
#pragma warning( disable : 4786)
#endif

#include "eAMC.hpp"
#include "debug.hpp"
#include "date_time.hpp"
#include "udp.hpp"
#include "stringutils.hpp"
#include "archy_type.hpp"
#include "small_dns.hpp"
#include <stdarg.h>
#ifdef WIN32
#include <direct.h>
#include <process.h>
#endif

#define DEF_NFIFO_SIZE	100		// 默认正常信息先进先出列表大小
#define DEF_WFIFO_SIZE	1000	// 默认告警信息先进先出列表大小
#define DEF_NMIN_INT	1		// 默认正常信息最小时间间隔大小（秒）
#define DEF_WMIN_INT	100		// 默认告警信息最小时间间隔大小（毫秒）
#define DEF_WLIVE_TIME	60		// 默认告警信息生存时间大小(秒)
#define DEF_HDC_ADDR	"127.0.0.1"	// 默认AMC-HDC地址
#define DEF_HDC_PORT	5500	// 默认AMC-HDC 端口
#define AMC_CUR_VER		"V5.1"	// amc目前的版本号
#define NORMAL_GRADE	"3" // 一般告警级别
CAmc::CAmc()
:m_hdcIp(DEF_HDC_ADDR)
,m_hdcPort(DEF_HDC_PORT)
,m_strVersion(AMC_CUR_VER)
,m_logPath("NULL")
,m_iMode(MODE_AMC)
{
	char tmpbuf[512];
	m_runPath = getcwd(tmpbuf, 512);
	m_strIp = CSmallDNS::GetLocalIP();
	m_strPid = to_string(getpid());
}
CAmc::~CAmc()
{
	if( m_thread.running() ) {
		m_thread.terminate();
		m_thread.join();
	}
}

void CAmc::Exit()
{
	if( m_thread.running() ) {
		m_thread.terminate();
		m_thread.join();
	}
}
int CAmcThread::SendTitle(const string& strTitle)
{
	// 发送
	if( m_amc->m_iMode & CAmc::MODE_STDOUT ) {
		printf("%s\n", strTitle.c_str());
		fflush(stdout);
	}
	if( m_amc->m_iMode & CAmc::MODE_AMC ) {
		int iPos = strTitle.find("~NT~");
		string strTmp(strTitle);
		if (iPos >= 0)
			strTmp = strTitle.substr(0, iPos+4)+ string("YES");
		else
		{
			iPos = strTitle.find("~WT~");
			if (iPos >= 0)
				strTmp = strTitle.substr(0, iPos+4) + string("YES");
		}
		int n = m_amc->m_so.write(strTitle.c_str(), strTitle.length());
		char recvline[128];

		// 读取确认
		n = m_amc->m_so.read(recvline, 128, 3);

		if( n > 0 ) {
			recvline[n] = 0;

			if ((int)strTmp.find(recvline) >= 0)
			return 1;
		}
	}
	if( m_amc->m_iMode & CAmc::MODE_STDOUT ) 
		return 1;
	return 0;
}

// 要求最后一个实参是NULL(0)
void CAmcReport::Normal0(const char* msg1, ...)
{
	va_list ap;
	va_start(ap, msg1);
	NormalV(msg1, ap);
	va_end(ap);
}
void CAmcReport::Normal(int iTitleIndex, const char* msg1, ...)
{
	va_list ap;
	va_start(ap, msg1);
	NormalV(msg1, ap, iTitleIndex);
	va_end(ap);
}

void CAmcReport::NormalV(const char* msg1, va_list args, int iTitleIndex)
{
	string strMsg;
	strMsg = msg1;
	const char* msgSec = 0;
LOOP_ADD:
	msgSec = va_arg(args, const char*);
	if( msgSec != 0 ) {
		strMsg += AMC_SPLIT;
		strMsg += msgSec;
		goto LOOP_ADD;
	}

	NormalMsg(strMsg, iTitleIndex);
}
void CAmcReport::Warning0(const char* msg1, ...)
{
	va_list ap;
	va_start(ap, msg1);
	WarningV(NORMAL_GRADE, "COMMON", msg1, ap);
	va_end(ap);
}

void CAmcReport::Warning1(const string& strWarnGrade, const string& strWarnType, 
						 const char* msg1, ...)
{
	va_list ap;
	va_start(ap, msg1);
	WarningV(strWarnGrade, strWarnType, msg1, ap);
	va_end(ap);
}
void CAmcReport::Warning2(int iTitleIndex, const string& strWarnGrade, 
						 const string& strWarnType, 
						 const char* msg1, ...)
{
	va_list ap;
	va_start(ap, msg1);
	WarningV(strWarnGrade, strWarnType, msg1, ap, iTitleIndex);
	va_end(ap);
}
void CAmcReport::Warning3(int iTitleIndex, int iLivingSecs, const string& strWarnGrade, 
						 const string& strWarnType, const char* msg1, ...)
{
	va_list ap;
	va_start(ap, msg1);
	WarningV(strWarnGrade, strWarnType, msg1, ap, iTitleIndex, iLivingSecs);
	va_end(ap);
}

void CAmcReport::WarningV(const string& strWarnGrade, const string& strWarnType,
	const char* msg1, va_list args, int iTitleIndex, int iLivingSecs )
{
	string strMsg;
	strMsg = msg1;
	const char* msgSec = 0;
LOOP_ADD:
	msgSec = va_arg(args, const char*);
	if( msgSec != 0 ) {
		strMsg += AMC_SPLIT;
		strMsg += msgSec;
		goto LOOP_ADD;
	}
	Warning(strWarnGrade, strWarnType, strMsg, iTitleIndex, iLivingSecs);	
}

bool CAmc::Start(const string& strAmcType)
{
	
	// 保存标示
	m_strAmcType = strAmcType;
	int iRet(0);
		
	if( m_hdcIp.empty() )
		m_hdcIp = DEF_HDC_ADDR;
	if( m_hdcPort == 0)
		m_hdcPort = DEF_HDC_PORT;

	// 申请UDP套接口描述字
	if( m_iMode & MODE_AMC ) {
		iRet = m_so.connect(m_hdcIp.c_str(), m_hdcPort);
	
		if( iRet < 0 ) {
			printf("socket call failed\n");
			return false;
		}
	}
	m_thread.m_amc = this;
	try  {
		if( !m_thread.running())
			m_thread.start();
	}
	catch(...) {
		return false;
	}
	return true;
}

// Title Msg:(时间格式YYYYMMDDHH24MISS)
// V5.1~采集组IP~Title标识~程序路径~日志路径~PID~当前时间~NT(或者WT)~具体title

// 注册正常监视信息的标题
void CAmc::NormalTitle(const string&   strNormalTitle)
{
	string strTitle;
	strTitle = m_strVersion + string("~") + m_strIp + string("~") 
			+ "NT" + to_string(m_lstTitle.size()) + string("~") 
			+ add_slash(m_runPath) + string("~") + m_logPath +  string("~") + m_strPid
			+ date_time::current().format("~%Y%M%D%H%N%S") + string("~NT~") + strNormalTitle;

	m_lstTitle.push_back(strTitle);
}

// 注册告警监视信息的标题
void CAmc::WarningTitle(const string&   strWarningTitle)
{
	string strTitle;
	strTitle = m_strVersion + string("~") + m_strIp + string("~") 
			+ "WT" + to_string(m_lstTitleWarn.size()) + string("~") 
			+ add_slash(m_runPath) + string("~") + m_logPath +  string("~") + m_strPid
			+ date_time::current().format("~%Y%M%D%H%N%S") + string("~WT~") 
			+ string("WARM_GRADE^%^WARM_TYPE^%^") + strWarningTitle;

	m_lstTitleWarn.push_back(strTitle);
}

void CAmc::AddReport(CAmcReport* report)
{
	archy::scoped_lock lock(m_mutex);
	m_lstAmcReport.push_back(report);
}

void CAmc::DelReport(CAmcReport* report)
{
	archy::scoped_lock lock(m_mutex);

	list<CAmcReport*>::iterator it;
	list<CAmcReport*>& lst = m_lstAmcReport;

	for( it = lst.begin(); it != lst.end(); ++it) {
		if( *it == report ) { 
			lst.erase(it); 
			return;	
		}
	}
}


CAmcReport::CAmcReport(CAmc& amc)
{
	m_pAmc = &amc;
	amc.AddReport(this);
	m_iLivingTime = 1;
}

CAmcReport::~CAmcReport()
{
	if( m_pAmc != NULL )
		m_pAmc->DelReport(this);
	
}


void CAmcReport::AddMsg(int  iType, const string& strMsg, int iLivingSecs)
{
	
	if( iType == 0 ) {
		archy::scoped_lock lock(m_pAmc->m_mutexNormal);
		
		list<string>& lst = m_pAmc->m_lstAmcNormalMsg;
		lst.push_back(strMsg);
		if( lst.size() > DEF_NFIFO_SIZE ) {
			lst.pop_front();
		}
	}
	else {
		archy::scoped_lock lock(m_pAmc->m_mutexAlarm);

		list<pair<string, int> >& lst = m_pAmc->m_lstAmcAlarmMsg;
		lst.push_back(make_pair(strMsg, iLivingSecs));
		if( lst.size() > DEF_WFIFO_SIZE ) {
			lst.pop_front();
		}
	}
}

// 一般消息格式:
// V5.1~IP~Title标识~采集点唯一标识~端口号~PID~当前时间~(ND\WD)~具体内容
// 具体内容内必须首先包含:告警级别、告警类型
// 心跳格式:

void CAmcReport::NormalMsg(const string&  strNormalMsg, int iTitleIndex)
{
	string strMsg;
	strMsg = m_pAmc->m_strVersion + string("~") + m_pAmc->m_strIp + string("~NT") 
			+ to_string(iTitleIndex) + string("~")
			+ m_strId + string("~NULL~") + m_pAmc->m_strPid
			+ date_time::current().format("~%Y%M%D%H%N%S~ND~")
			+ strNormalMsg;
	AddMsg(0, strMsg);
	
}

// 一般消息格式:
// V5.1~IP~Title标识~采集点唯一标识~端口号~PID~当前时间~(ND\WD)~具体内容
// 具体内容内必须首先包含:告警级别、告警类型
// 心跳格式:


void CAmcReport::Warning(const string& strWarnGrade, const string& strWarnType, 
						 const string&  strWarningMsg, int iTitleIndex, int iLiveSecs)
{
	string strMsg;
	strMsg = m_pAmc->m_strVersion + string("~") + m_pAmc->m_strIp + string("~WT") 
			+ to_string(iTitleIndex) + string("~")
			+ m_strId + string("~NULL~") + m_pAmc->m_strPid
			+ date_time::current().format("~%Y%M%D%H%N%S~WD~")
			+ strWarnGrade + string("^%^")
			+ strWarnType + string("^%^")
			+ strWarningMsg;
	AddMsg(1, strMsg, iLiveSecs);
}

void CAmc::Noop()
{
// V5.1~IP~系统标识(可空)~程序路径~日志路径~PID~当前时间~SH~NOOP
	// 发送心跳消息
	string strMsg;
	strMsg = m_strVersion + string("~") + m_strIp + string("~NULL~") 
			+ m_runPath + string("~") + m_logPath + string("~")
			+ m_strPid + date_time::current().format("~%Y%M%D%H%N%S")+ string("~SH~NOOP");
	
	archy::scoped_lock lock(m_mutexNormal);
	
	list<string>& lst = m_lstAmcNormalMsg;
	lst.push_back(strMsg);
	if( lst.size() > DEF_NFIFO_SIZE ) {
		lst.pop_front();
	}

}

// 告警取消
void CAmcReport::WarningCancel(const string&  strWarningType, int iTitleIndex)
{
	// V5.1~192.168.10.118~COREDAS~YANGZHOU~8080~12345~20030424103012~WC~告警类型

	// 头部信息改变一下而已
	string strMsg;
	strMsg = m_pAmc->m_strVersion + string("~") + m_pAmc->m_strIp + string("~") 
			+ to_string(iTitleIndex) + string("~")
			+ m_strId + string("~NULL~") + m_pAmc->m_strPid 
			+ date_time::current().format("~%Y%M%D%H%N%S~WC~")
			+ strWarningType;
	AddMsg(1, strMsg, 1);
}

void CAmcThread::run()
{
	// 首先发送Title信息
	list<string>::iterator it = m_amc->m_lstTitle.begin();
	list<pair<string, int> >::iterator itAlarm, itTemp;
	string strSendMsg;
	int ret;

	for( ; it != m_amc->m_lstTitle.end(); ++it ) {
		strSendMsg = *it;
		while (1) {
			ret = SendTitle(strSendMsg);
			if( this->terminated == true )
				break;
			if( ret == 1)
				break;
			archy::thread::sleep(1);
		}
	}

	it = m_amc->m_lstTitleWarn.begin();
	for( ; it != m_amc->m_lstTitleWarn.end(); ++it ) {
		strSendMsg = *it;
		while (1) {
			ret = SendTitle(strSendMsg);
			if( this->terminated == true )
				break;
			if( ret == 1)
				break;
			archy::thread::sleep(1);
		}
	}
	
	list<string> lstSendMsg;
	short iNoopCount(0);
	while(true) {

		if( ++iNoopCount > 30 ) {
			m_amc->Noop();
			iNoopCount = 0;
		}
		archy::thread::sleep(1);		

		// 获取告警信息列表
		archy::mutex& mutex1 = m_amc->m_mutexAlarm;
		mutex1.do_lock();

		for(itAlarm = m_amc->m_lstAmcAlarmMsg.begin();
				itAlarm != m_amc->m_lstAmcAlarmMsg.end(); ) {
			lstSendMsg.push_back(itAlarm->first);
			itTemp = itAlarm;
			++itTemp;
			if(--(itAlarm->second) <= 0)
				m_amc->m_lstAmcAlarmMsg.erase(itAlarm);
			itAlarm = itTemp;
		}
		mutex1.do_unlock();


		// 获取正常信息列表
		archy::mutex& mutex2 = m_amc->m_mutexNormal;
		mutex2.do_lock();

		lstSendMsg.insert(lstSendMsg.end(), 
				m_amc->m_lstAmcNormalMsg.begin(),
				m_amc->m_lstAmcNormalMsg.end());
		m_amc->m_lstAmcNormalMsg.clear();
		mutex2.do_unlock();
		

		if( !lstSendMsg.empty())  {
			// 逐条发送监视信息
			for( it = lstSendMsg.begin(); it != lstSendMsg.end(); ++it) {
				strSendMsg = *it;
				if( m_amc->m_iMode & CAmc::MODE_STDOUT ) {
					printf("%s\n", strSendMsg.c_str());
					fflush(stdout);
				}
				if( m_amc->m_iMode & CAmc::MODE_AMC ) {
					m_amc->m_so.write(strSendMsg.c_str(), strSendMsg.length());
				}
			} // for( it = ......

			// 清空列表
			lstSendMsg.clear();
		}

		if( this->terminated == true ) {
			// 发送退出消息
			// V5.1~IP~系统标识(可空)~PID~当前时间~SE~EXIT
			strSendMsg = m_amc->m_strVersion + string("~")
				+ m_amc->m_strIp + string("~NULL~") + m_amc->m_strPid + "~"
				+ date_time::current().format("%Y%M%D%H%N%S")+ string("~SE~EXIT");

			if( m_amc->m_iMode & CAmc::MODE_STDOUT ) {
				printf("%s\n", strSendMsg.c_str());
				fflush(stdout);
			}
			if( m_amc->m_iMode & CAmc::MODE_AMC ) {
				m_amc->m_so.write(strSendMsg.c_str(), strSendMsg.length());
			}
			break;
		}
	} // while(true)
}
