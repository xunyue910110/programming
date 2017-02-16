/*数据字节（interpret as command "作为命令来解释")*/
const	char IAC       = (char)255;
/*选项协商数据DO*/
const	char DO        = (char)253;
/*选项协商数据DONT*/
const	char DONT      = (char)254;
/*选项协商数据WILL*/
const	char WILL      = (char)251;
/*选项协商数据WONT*/
const	char WONT      = (char)252;
/*子选项开始标志*/
const	char SB        = (char)250;
/*子选项结束标志*/
const	char SE        = (char)240;
/*抑制继续进行*/
const 	char GA	= 3;
/*IS*/
const	char IS        = 0;
/*子选项发送标志*/
const	char SEND      = 1;
/*终端类型*/
const	char TT	= 24;
/*回显*/
const	char TELNET_ECHO= 1;
/*环境变量*/
const 	char NEW_ENVIRON=39;
/*窗口大小*/
const	char NAWS	=31;
/*远程流量控制*/
const	char LFLOW	=33;
/*状态*/
//const	unsigned char STATUS	=5;

#include "telnet_client.hpp"
#include "bufwrap.hpp"
#include <string.h>

BEGIN_NAMESPACE_ARCHY

telnet_client::telnet_client()
:m_bEcho(true)
,m_strTermType("ANSI")
,m_loginPrompt("ogin:")
,m_passPrompt("assword:")
,m_prmopt("$")
{

}
// socket连接
// 返回: 0成功, -1失败
int telnet_client::connect(const string& ip, int port /*= 23*/)
{
	return m_tcp.connect(ip.c_str(), port, 10);
}

// 登录
// 返回: 1 成功 0超时, -1失败
int telnet_client::login(const string& user, const string& pass, int iTimeout/* = 3*/)
{
	// 循环读取数据，知道遇到输入用户提示
	bufwrap buf;
	int ret;
	ret = recvutil(buf, m_loginPrompt, iTimeout);
	if( ret <= 0 )
		return ret;
	ret = command(user);
	if( ret < 0 )
		return -1;
	ret = recvutil(buf, m_passPrompt, iTimeout);
	if( ret <= 0 )
		return ret;
	ret = command(pass);
	if( ret < 0 )
		return -1;
	ret = recvutil(buf, m_prmopt, iTimeout);

	if( ret < 0 )
		return -1;
	return 1;
}

// 发送命令，发送成功
// 返回 0 成功, -1 失败
int telnet_client::command(const string& cmd)
{
	m_strLastCmd = cmd;
	if( cmd[cmd.length()-1] != '\r' ) 
		m_strLastCmd += '\r';

	int ret = m_tcp.write(m_strLastCmd.c_str(), m_strLastCmd.length());
	if( ret < m_strLastCmd.length() ) {
		return -1;
	}

	return 0;
}
// 接受命令返回, 
// 返回 1 成功, 0 超时, -1失败
int telnet_client::result(string& retMsg, int iTimeout /*= 3*/)
{
	bufwrap buf;
	int ret = recvutil(buf, m_prmopt, iTimeout);
	if( ret <= 0 )
		return ret;
	buf.null_deal();
	retMsg = buf.getcbuf();
	if( m_bEcho == false 
		&& strncmp(retMsg.c_str(), m_strLastCmd.c_str(), m_strLastCmd.length()) == 0 )
		retMsg.erase(0, m_strLastCmd.length());
	return 1;
}

// 退出登录
int telnet_client::logout()
{
	return m_tcp.write("\x04", 1);
}

int telnet_client::close()
{
	return m_tcp.close();
}

// 接受直到出现endflag为止
// 返回 1 ok, 0 timeout -1 fail
int telnet_client::recvutil(bufwrap& recvbuf, const string& endflag, int iTimeout)
{
	recvbuf.clear();

	bufwrap buf,left;
	buf.resize(128);
	int ret;
	while(1) {

		ret = m_tcp.read(buf.getcbuf(), buf.size(), iTimeout);
		if( ret <= 0 ) return ret;

		ret = respond(buf, ret, left);
		if( ret < 0 ) return -1;

		left.null_deal();
		recvbuf += left;
		
		if( strstr(left.getcbuf(), endflag.c_str()) != 0 ) {
			return 1;
		}
	} // while(1)

}

int telnet_client::respond(const bufwrap& in, int iLen, bufwrap & left)
{
	int iBufIndex = 0;
	int iSendIndex = 0;
	int iLeftLen = 0;
	left.clear();
	left.resize(128);
	bufwrap out;
	out.resize(128);
	
	while(iBufIndex < iLen ) {	
		if(in[iBufIndex] != IAC) {
			left[iLeftLen++] = in[iBufIndex++];
			continue;
		}
			
		// 协议数据以IAC开头
		switch(in[++iBufIndex] ) {	// IAC后的一个字节为操作选项			
			case WONT: // 收到WONT，不理
			case DONT: // 收到DONT，不理					
				++iBufIndex; 
				break;
			// 两个IAC连一块儿，表示单个字符0xFF
			case IAC:
				left[iLeftLen++] = in[++iBufIndex];
				break;
			case WILL:
				out[iSendIndex++] = IAC;
				out[iSendIndex++] = DONT;
				out[iSendIndex++] = in[++iBufIndex];

				switch(in[iBufIndex]) {// 收到WILL，返回DONT,以下情况返回DO
					case TT:
					case GA:
					case LFLOW:
						out[iSendIndex -2] =  DO;
						break;
 					case TELNET_ECHO:
						out[iSendIndex -2] =  m_bEcho ? DO : DONT;
						break;
				}					
				break;
			case DO:
				out[iSendIndex++] = IAC;
				out[iSendIndex++] = WONT;
				out[iSendIndex++] = in[++iBufIndex];
				
				// 收到DO，返回WONT,以下情况返回WILL
				switch(in[iBufIndex]) {
					case NAWS:
					case TT:
					case GA:
					case LFLOW:
					case NEW_ENVIRON:
						out[iSendIndex -2] = WILL;
						if(in[iBufIndex] == NAWS) {
							// NAWS则发送子选项，窗口大小为80，26
							out[iSendIndex++] = IAC;
							out[iSendIndex++] = SB;
							out[iSendIndex++] = NAWS;
							out[iSendIndex++] = 0;
							out[iSendIndex++] = 80;
							out[iSendIndex++] = 0;
							out[iSendIndex++] = 26;
							out[iSendIndex++] = IAC;
							out[iSendIndex++] = SE;
						}
						break;
					default:
						break;
				}
									
				break;
			case SB:// 子选项开始标志
				if(in[iBufIndex + 2] != SEND) {
					iBufIndex += 4;
				}
				else	{
					out[iSendIndex++] = IAC;
					out[iSendIndex++] = SB;
					out[iSendIndex++] = in[++iBufIndex];
					out[iSendIndex++] = IS;
											
					if(in[iBufIndex] == TT) { // 返回终端类型
						for( int i = 0; i < m_strTermType.length(); ++i ) {
							out[iSendIndex++] = m_strTermType[i];
						}
					}					
					out[iSendIndex++] = IAC;
					out[iSendIndex++] = SE;
					
					iBufIndex += 3;
				}
			default:
				break;
		} // switch
		++iBufIndex;
	} // while
	
	// 发送响应的协议数据
	int iRet = m_tcp.write(out.getcbuf(), iSendIndex);
	if( iRet < iSendIndex )
		return -1;
	
	left.resize(iLeftLen);
	return 0;
}

END_NAMESPACE_ARCHY
