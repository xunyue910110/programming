//////////////////////////////////////////////////////////////////////
// 
// 类名: ftp_client
// 描述: ftp客户端封装类
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-15 21:11 
// 版本: V2.0.0 
// 版权: 南京联创科技股份有限公司

//////////////////////////////////////////////////////////////////////
#ifndef FTP_CLIENT_ARCHY_LIBRARY_HUANGJB_HERADER89983414
#define FTP_CLIENT_ARCHY_LIBRARY_HUANGJB_HERADER89983414

#include "archy_config.hpp" 
#include "tcp.hpp"
#include "noncopyable.hpp"

USING_STD_NAMESPACE;

namespace archy {

class ftp_client : private noncopyable
{
public:
enum ftp_cmdtype {NOOP, CHDIR, CDUP, DEL, LS, NLS, PWD, MKDIR, 
	RMDIR, SYSTYPE, SIZE, STAT, RENAME, BIN, TXT,
	ABORT};
enum ftp_option { CTRLTIMEOUT, DATATIMEOUT, RESUME};

public:
	ftp_client();
	
	// connect to and close from the server
	int connect(const string& server, const string& user, const string& pass, int port = 21);
	int close();
	
	int getfile(const string& from, const string& to);
	int putfile(const string& from, const string& to);
	
	int getfile(const string& from) { return getfile(from, from); }
	int putfile(const string& from) { return putfile(from, from); }
	
	int command(ftp_cmdtype cmd, const char* cmdvalue= 0, 
		char *returnvalue = 0 , int retmaxsize = 0);
		
	int option(ftp_option opt, int value);
	
	const char* result()const { return m_recvbuf; }
private:
	int ftp_command(const string& cmd, bool waitresult = true);
	int exchange_file(tcp_socket& fd, const string& filename, bool bSendorRecv, int iFilePos = 0);
	int get_result(int timeout);
	int get_single_result(int timeout, bool* multiline = 0);
	int send_port(tcp_listener& fd);
private:
	bool m_iResume;
	int m_datatimeout;
	int m_ctrltimeout;
	char m_recvbuf[512];
	tcp_socket m_tcpCtrl;
	bool b_local; // 是否是本地ftp, 在地址信息为127.0.0.1或者localhost时, 认为是本地ftp
};



} // namespace archy

#endif // FTP_CLIENT_ARCHY_LIBRARY_HUANGJB_HERADER89983414


