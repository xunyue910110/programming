#if !defined(ARCHY_TELNET_CLIENT_HEADER_FILE__)
#define ARCHY_TELNET_CLIENT_HEADER_FILE__
#include "archy_config.hpp"
#include "tcp.hpp"


USING_STD_NAMESPACE;

BEGIN_NAMESPACE_ARCHY

class bufwrap;

class telnet_client
{
public:
	telnet_client();

	// 设置命令是否允许回显,默认是回显
	void set_echo(bool bEcho) { m_bEcho = bEcho; }

	// 设置终端类型,默认是ANSI方式
	void set_terminal(const string& type) { m_strTermType = type; }

	// 设置命令行提示符号，默认是"$"
	void set_prompt(const string& prompt) {
		m_prmopt = prompt;
	}
	// 设置登录提示符号，默认输入用户名的提示是"ogin:"，默认输入密码的提示是"assword:"
	void set_login(const string& loginPrompt, const string& passPrompt="assword:"){
		m_loginPrompt = loginPrompt; m_passPrompt = passPrompt;
	}
	
	// socket连接
	// 返回: 0成功, -1失败
	int connect(const string& ip, int port = 23);
	
	// 登录
	// 返回: 1 成功 0超时, -1失败
	int login(const string& user, const string& pass, int iTimeout = 3);
	
	// 发送命令, 在发送的内容后面会自动添加'\r'字符
	// 返回 0 成功, -1 失败
	int command(const string& cmd);

	// 接受命令返回
	// 返回 1 成功, 0 超时, -1失败
	int result(string& retMsg, int iTimeout = 3);
	
	// 退出登录
	int logout();
	
	// 关闭socket连接
	int close();

private:

	// -1 失败 0 成功
	int respond(const bufwrap& in, int iLen, bufwrap& left);

	// 接受直到出现endflag为止
	// 返回 1 ok, 0 timeout -1 fail
	int recvutil(bufwrap& recvbuf, const string& endflag, int iTimeout);

private:
	archy::tcp_socket m_tcp;

	bool m_bEcho;
	string m_strTermType;
	string m_prmopt;
	string m_loginPrompt, m_passPrompt;
	string m_strLastCmd;
};

END_NAMESPACE_ARCHY

#endif  // ARCHY_TELNET_CLIENT_HEADER_FILE__

