//////////////////////////////////////////////////////////////////////
// 
// 类名: ftp_client
// 描述: ftp客户端封装类
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-15 21:11 
// 版本: V2.0.0 
// 版权: 南京联创科技股份有限公司
// 在PUTFILE中加入了断点续传

//////////////////////////////////////////////////////////////////////
#include "ftp_client.hpp"

#include <time.h>
#include <stdio.h>

namespace archy {

int ftp_client::get_result(int timeout)
{
	bool multiline;
	int ret = get_single_result(timeout, &multiline);
printf("get_single_result:%d\n", ret);
	if( ret <= 0 ) return ret;
printf("111::%s", m_recvbuf);
	while(multiline) {
		ret = get_single_result(timeout, &multiline);
printf("get_single_result:%d\n", ret);
		if( ret <= 0 )break;
printf("222::%s", m_recvbuf);
	}
	return ret;
		
}

ftp_client::ftp_client()
{
	m_ctrltimeout = 30;
	m_datatimeout = 900;
	m_iResume = true;
}

int ftp_client::option(ftp_option opt, int value)
{
	switch(opt) {
	case CTRLTIMEOUT:
		m_ctrltimeout = value;
		break;
	case DATATIMEOUT:
		m_datatimeout = value;
		break;
	case RESUME:
		m_iResume = (value == 1);
		break;
	default:
		break;
	}
	return 0;
}


// ret: 0 timeout
// ret: -1 error
// ret: > 0 ok
int ftp_client::get_single_result(int timeout, bool* multiline)
{
	int ret, tolallen(0);
	int retcode(0), timer(::time(0));
	memset(m_recvbuf, 0, sizeof(m_recvbuf));
printf("\nresult::");
	while(1) {
		double wastetime = ::difftime( time(0), timer);
		if( wastetime > m_ctrltimeout ) {
			return 0;
		}
		ret = m_tcpCtrl.read(&m_recvbuf[tolallen], 1, timeout);
		if( ret != 1 ) return 0;
		if( ret < 0 ) return -1;
		
printf("%c", m_recvbuf[tolallen]);
		tolallen += 1;
		
		// 这里返回多行仔细考虑，多行时，除最后一行外其余各行第四个字符是减号
		if( tolallen >= 3 && m_recvbuf[tolallen-2] == '\r' 
			&& m_recvbuf[tolallen-1] == '\n' ) {
			retcode = atoi(m_recvbuf);
			
			if( multiline ) {
				if( tolallen < 4 || m_recvbuf[4] != '-' ) {
					*multiline = false;
				}
				else if( m_recvbuf[4] == '-' ) {
					*multiline = true;
                }
			}
			break;
		}
	}
	return retcode;
}
		
// connect to and close from the server
int ftp_client::connect(const string& server, const string& user, const string& pass, int port)
{
	// 设置本地ftp 标记
	if( server == "localhost"  || server == "127.0.0.1" ) {
		b_local = true;
		return 0;
	}
	b_local = false;

	int ret = m_tcpCtrl.connect(server.c_str(), port, m_ctrltimeout);
	if( ret < 0 ) {
		return -1;
	}
	
	ret= get_result(m_ctrltimeout);
	if(ret != 220){
		return -1;
	}
	
	// 发送user命令 
	ret = ftp_command(string("USER ") + user);
	if(ret != 331){
		return -2;
	}
	
	// 发送pass命令 
	ret = ftp_command(string("PASS ") + pass);
	if(ret != 230){
		return -3;
	}

	return 0;
}

int ftp_client::ftp_command(const string& cmd, bool waitresult)
{
	string strCmd = cmd + "\r\n";
	int len = strCmd.length();
printf("Send:<%s>\n", strCmd.c_str());
	int ret = m_tcpCtrl.write(strCmd.c_str(), len);

	if( ret < 0 || ret != len ){
		return -1;
	}
	return waitresult ? get_result(m_ctrltimeout) : ret;	
}

int ftp_client::close()
{
        if( !b_local )   {
//	        ftp_command("ABOR");
	        ftp_command("QUIT");
	        m_tcpCtrl.close();
        }
	return 0;
}


int ftp_client::getfile(const string& from, const string& to)
{
        if( b_local)  {
#if defined(WIN32)
                return ::CopyFileA(from.c_str(), to.c_str(), true);
#else
                string cmd = "cp ";
                cmd += from + " " + to;
                return system(cmd.c_str());
#endif
        }
	tcp_listener server;
	int ret = server.initial(0);
	if( ret < 0 ) {
		return -1;
	}
	ret = send_port(server);
	if( ret < 0 )
		return -1;
	if( m_iResume ) {
		FILE* fp = fopen(to.c_str(), "r");
		if( fp ) {
			fseek(fp, 0, SEEK_END);
			long size = ftell(fp);
			char buf[128];
			sprintf(buf, (char*)"REST %d", size);
			// 如果服务器支持断点续传，那么返回应答应是：
			// 350 Restarting at 0. Send STORE or RETRIEVE to initiate transfer.
			ret = ftp_command(buf);
			if( ret/100 != 3 ) {
				m_iResume = false;
			}
			fclose(fp);
		}
	}
	
	ret = ftp_command(string("RETR ") + from, false);
	if( ret < 0 ) return -1;

//	ret = get_result(0);

	tcp_socket cli;
	ret = server.accept(cli, m_ctrltimeout);	
	if( ret <= 0 ) {
		get_result(m_ctrltimeout);
		return -1;
	}
	
	ret = get_result(m_ctrltimeout);
	if( ret != 150 ) return -1;

	
	ret = exchange_file(cli, to, false);
	cli.close();
	if( ret <= 0 )
		return ret;
	
	ret = get_result(m_ctrltimeout);
	if( ret != 226 ) return -1;
	
	return 0;
}

int ftp_client::putfile(const string& from, const string& to)
{
         if( b_local)  {
#if defined(WIN32)
                return ::CopyFileA(from.c_str(), to.c_str(), true);
#else
                string cmd = "cp ";
                cmd += from + " " + to;
                return system(cmd.c_str());
#endif
        }
	tcp_listener server;
	int ret = server.initial(0);
	if( ret < 0 ) {
		return -1;
	}
	ret = send_port(server);
	if( ret < 0 )
		return -1;

	int iFileSize(0);
	if( m_iResume ) {
		char retSize[32];
		ret = ftp_client::command(ftp_client::SIZE, to.c_str(), retSize, sizeof(retSize));
		if( ret == 0 ) {
			iFileSize = atoi(retSize);
			char buf[128];
			sprintf(buf, (char*)"REST %d", iFileSize);
			// 如果服务器支持断点续传，那么返回应答应是：
			// 350 Restarting at 0. Send STORE or RETRIEVE to initiate transfer.
			ret = ftp_command(buf);
			if( ret/100 != 3 ) {
				m_iResume = false;
			}
		}
		else {
			m_iResume = false;
		}
	}
	

	ret = ftp_command(string("STOR ") + to, false);
	if( ret < 0 ) return -1;

	tcp_socket cli;
	ret = server.accept(cli, m_ctrltimeout);	
	if( ret <= 0 ) return ret;

	ret = get_result(m_ctrltimeout);
	if( ret != 150 ) return -1;
	
	
	ret = exchange_file(cli, from, true, iFileSize);
	if( ret <= 0 )
		return ret;
	
	cli.close();
	
	ret = get_result(m_ctrltimeout);
	if( ret != 226 ) return -1;
	
	return 0;
}

int ftp_client::command(ftp_cmdtype cmd, const char* cmdvalue /*= 0*/, 
	char *returnvalue /*= 0 */, int retmaxsize /*= 0*/)
{
	int ret(0);
if( b_local)  {
	switch(cmd) {
	case BIN:
        case TXT:
        case NOOP:
                break;
        default:
        	ret = -1;
                break;
        }
        return ret;
}

	switch(cmd) {
	case ABORT:
		ret = ftp_command("ABOR");
		break;
	case BIN:
		ret = ftp_command("TYPE I");
		break;
	case TXT:
		ret = ftp_command("TYPE A");
		break;
	case NOOP:
		ret = ftp_command("NOOP");
		break;
	case CHDIR:
		ret = ftp_command(string("CWD ") + cmdvalue);
		break;
	case CDUP:
		ret = ftp_command(string("CDUP") + cmdvalue);
		break;
	case DEL:
		ret = ftp_command(string("DELE ") + cmdvalue);
		break;
	case MKDIR:
		ret = ftp_command(string("MKD ") + cmdvalue);
		break;
	case RMDIR:
		ret = ftp_command(string("RMD ") + cmdvalue);
		break;
	case RENAME:
		ret = ftp_command(string("RNFR ") + cmdvalue);
		if( ret != 350 ) break;
		
		ret = ftp_command(string("RNTO ") + returnvalue);
		break;
	case STAT:
	case SIZE:
	case PWD:
	case SYSTYPE:
		switch(cmd) {
		case STAT:
			ret = ftp_command(string("STAT ") + cmdvalue);
			break;
		case SIZE:
			ret = ftp_command(string("SIZE ") + cmdvalue);
			break;
		case PWD:
			ret = ftp_command(string("PWD") + cmdvalue);
			break;
		case SYSTYPE:
			ret = ftp_command(string("SYST") + cmdvalue);
			break;
		}
				
		if(ret/100 !=  2)break;

		if( retmaxsize == 0 ) {
			strcpy(returnvalue, m_recvbuf + 4);
		}
		else {
			strncpy(returnvalue, m_recvbuf + 4, retmaxsize);
			returnvalue[strlen(m_recvbuf + 4) -1] = 0;
		}
		break;
	case LS:
	case NLS:
	{ 
		tcp_listener server;
		ret = server.initial(0);
		if( ret < 0 ) {
			return -1;
		}
		ret = send_port(server);
		if( ret < 0 )
			return -1;
			
		string strCmd;
		if( cmdvalue == 0 )
			strCmd = (cmd == LS ? "LIST" : "NLST");
		else 
			strCmd = string(cmd == LS ? "LIST " : "NLST ") + cmdvalue;
		
		// hp 中NLIST如果不不存在文件列表，则直接返回550 No files found.
		ret = ftp_command(strCmd, false);
		if( ret < 0 )
			return -1;
			
		tcp_socket cli;
		ret = server.accept(cli, m_ctrltimeout);		
		if( ret <= 0 ) return -1;
		
		ret = get_result(m_ctrltimeout);
		if( ret != 150 ) return -1;
		
		
		ret = exchange_file(cli, returnvalue, false);
		if( ret <= 0 )
			return ret;
		
		ret = get_result(m_ctrltimeout);
		if( ret != 226 ) return -1;
	}	
		break;
	default:
		ret = -1;
		break;
	}
	return ret/100 == 2 ? 0 : ret;
}

int ftp_client::send_port(tcp_listener& fd)
{
	/* Ask the server to connect to the port monitored by the listener socket*/
	struct sockaddr_in addr;
#if defined(_AIX) || defined(__linux__) || defined(__sun__)
	socklen_t iLength = sizeof(addr);
#else
	int iLength = sizeof(addr);
#endif
	addr = fd.get_addr();
	
	/* Get port number */
	int ret = ::getsockname(fd.get_socket(), reinterpret_cast<struct sockaddr*>(&addr),  &iLength);
	if( ret < 0){
		return -1;
	}
	int iPort = addr.sin_port;
	/* Get local ip address  */
	ret = ::getsockname(m_tcpCtrl.get_socket(), reinterpret_cast<struct sockaddr *>(&addr), &iLength);
	if( ret < 0){
		return -1;
	}
	//PORT h1,h2,h3,h4,p1,p2
	//where h1 is the high order 8 bits of the internet host  address.
	char strCmd[128];
#if defined(ARCHY_BIG_ENDIAN)
	sprintf(strCmd, "PORT %d,%d,%d,%d,%d,%d",
		(addr.sin_addr.s_addr>>24) & 0x000000ff,
		(addr.sin_addr.s_addr>>16) & 0x000000ff,
		(addr.sin_addr.s_addr>>8) & 0x000000ff,
		(addr.sin_addr.s_addr) & 0x000000ff,
		iPort >> 8,
		iPort & 0xFF);
#elif defined(ARCHY_LITTLE_ENDIAN)
	sprintf(strCmd, "PORT %d,%d,%d,%d,%d,%d",
		(addr.sin_addr.s_addr) & 0x000000ff,
		(addr.sin_addr.s_addr>>8) & 0x000000ff,
		(addr.sin_addr.s_addr>>16) & 0x000000ff,
		(addr.sin_addr.s_addr>>24) & 0x000000ff,
		iPort & 0xFF,
		iPort >>8 );
#endif
	ret = ftp_command(strCmd);
	if( ret/100 != 2 ) return -1;
	return 0;
}

int ftp_client::exchange_file(tcp_socket& fd, const string& filename, 
							  bool bSendorRecv, int iFilePos)
{
	FILE* fp;
	fp = fopen(filename.c_str(), ( bSendorRecv ? "r" : "w+" ) );
	if( !fp ) {
		return -1;
	}
	int len, ret;
	if( bSendorRecv && iFilePos > 0) {
		ret = ::fseek(fp, iFilePos, SEEK_CUR);
		if( ret != 0 ) {
			perror("fseek");
			return -1;
		}
	}
	int timer = time(0);
	int totallen=0;
	char buf[128];
	while(!feof(fp)){
		if( ::difftime(time(0), timer) > m_datatimeout) {
			fclose(fp);
			return 0;
		}
				
		len = 127;
		ret = bSendorRecv ? 
			fread(buf, 1, len, fp):
			fd.read(buf, len, m_datatimeout);
			
		if( ret < 0 ) {fclose(fp); return -2;}
		if( ret == 0 )
			break;
		
		len = ret;
		ret = bSendorRecv ? 
			fd.write(buf, len):
			fwrite(buf, 1, len, fp);
		
		if( ret < len ) {
			fclose(fp);
			return -3;
		}
		totallen += len;
	}
	fclose(fp);
	return 1;
}


} // namespace archy

