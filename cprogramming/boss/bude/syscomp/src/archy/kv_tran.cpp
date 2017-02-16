//////////////////////////////////////////////////////////////////////
// 
// 类名: 键值序列传输类
// 描述: 在tcp/ip网路上传输键值序列,使用简单的请求－应答方式实现
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-25 12:12
// 版本: V1.0.0 
// 版权: 南京联创科技股份有限公司
//////////////////////////////////////////////////////////////////////

#include "kv_tran.hpp"
#include "archy_type.hpp"
#include "utils.hpp"
#include "checksum.hpp"
#include "stringutils.hpp"

BEGIN_NAMESPACE_ARCHY

kv_tran::kv_tran()
:m_so(0)
,m_type(-1)
,m_bSendChecksum(true)
{
	m_recvBuffer[0] = 0;
}

kv_tran::kv_tran(tcp_socket &so)
:m_so(&so)
,m_type(-1)
,m_bSendChecksum(true)
{
}

void kv_tran::set_socket(tcp_socket& so)
{
	m_so = &so;
}

int kv_tran::put_start(int type /*= TYPE_AUTO*/)
{
	m_type = type;
	if( m_type == TYPE_MANU)
		m_buf.assign("KV_MANU:");
	else if(m_type == TYPE_AUTO  )
		m_buf.assign("KV_AUTO:");
	else
		return -1;
	return 0;
}

int kv_tran::put(const string &strKey, const string &strVal)
{
	if( m_type == TYPE_AUTO ) {
		string::size_type keyLen = strKey.length();
		string::size_type valLen = strVal.length();
		if( keyLen >= ARCHY_Uint2(-1) || valLen >= ARCHY_Uint2(-1) ) {
			return -1;
		}
		ARCHY_Uint2 len1 = (ARCHY_Uint2)keyLen;
		ARCHY_Uint2 len2 = (ARCHY_Uint2)valLen;
		// 转化为网络字节序，即大端字节序
		byte_order_convert(&len1, 2);
		byte_order_convert(&len2, 2);
		m_buf.append(&len1, 2);
		m_buf.append(strKey.c_str(), strKey.length());
		m_buf.append(&len2, 2);
		m_buf.append(strVal.c_str(), strVal.length());
		
		printf("m_buf size:%d\n", m_buf.size());

		return 0;


	}
	else if( m_type == TYPE_MANU) {
		if( strKey.find('=') != string::npos 
				|| strVal.find('"') != string::npos ) {
			return -1;
		}
		m_buf.append(strKey.c_str(), strKey.length());
		m_buf.append("=\"", 2);
		m_buf.append(strVal.c_str(), strVal.length());
		m_buf.append("\",", 2);
		return 0;
	}
	return -1;
}

// 发送返回
// 0 成功
// -1 失败

int kv_tran::put_end(int iTimeout)
{
	char buf[16];
	if( m_type == TYPE_AUTO ) {
		ARCHY_Uint2 len = 0;
		m_buf.append(&len, 2);
		// 添加校验和
		checksum check;
		check.add_chars(m_buf.getcbuf(), m_buf.size());
		ARCHY_Uint4 sum = check.get_checksum();
		sprintf(buf, "%08x", sum);
	}
	else if( m_type == TYPE_MANU ) {
		strcpy(buf, "00000000");
	}
	else
		return -1;
	
//fprintf(stderr, "size:%d, checksum:%s\n", m_buf.size(), buf);
	m_buf.append(buf, 8);

	// 发送
	int ret = m_so->write(m_buf.getcbuf(), m_buf.size());
	if( ret < m_buf.size() )
		return -1;

	if( m_bSendChecksum ) {

		char recvbuf[16];
		ret = m_so->readn(recvbuf, 8, iTimeout);
		if( ret < 8 || strncmp(buf, recvbuf, 8) != 0 )
			return -1;
	}
	return 0;
}


int kv_tran::get_manu_start(int iTimeout)
{
	int iRet;

	m_recv.clear();
	while (1) {
		unsigned int iLen = strlen(m_recvBuffer);
		if( iLen > 0 ) {
			iRet = ReadFixedHeadTailMsg(m_recvBuffer, "KV_MANU:", ",00000000");
			if( iRet > 0 ) {
				m_recv.append(m_recvBuffer + 8, iRet - 8);
				memmove(m_recvBuffer, m_recvBuffer + iRet, iLen - iRet);
				m_recvBuffer[iLen - iRet] = 0;
				break;
			}
			iLen = strlen(m_recvBuffer);
		}
		
		iRet = m_so->read(m_recvBuffer + iLen,  
				1000 - iLen, iTimeout);
		if( iRet <= 0 )
			return iRet;
		cerr << "iRet:" << iRet << ",iLen:" << iLen << endl;
		m_recvBuffer[iRet + iLen] = 0;

	}
	if( m_bSendChecksum ) {
		iRet = m_so->write("00000000", 8);
		if( iRet < 8 )
			return -1;
	}
	m_iOff = 0;
	m_recvtype = TYPE_MANU;
	return 1;
}
/*
 *	key_val 序列在网络上传输的类
 *  一条消息支持两种格式:
 *
 *  KV_MANU:key1="val1", key2="val2",...,0000000(8位校验和, 8个0表示不使用校验和)
 *  KV_AUTO:(两个字节的key1长度)key1(两个字节的val1长度)val1...(两个字节key长度的全零表示结束)00000000(8位校验和, 8个0表示不使用校验和)
 *  
 *  返回 -1 失败 0 超时 1 ok
 */
int kv_tran::get_start(int iTimeout)
{
	char buf[128];
	int ret = m_so->readn(buf, 8, iTimeout);
	if( ret < 8 )
		return ret;
	
	if( strncmp(buf, "KV_MANU:", 8) == 0 ) {
		m_recvtype = TYPE_MANU;
	}
	else if( strncmp(buf, "KV_AUTO:", 8) == 0 ) {
		m_recvtype = TYPE_AUTO;
	}
	else return -1;

	m_recv.clear();
	char sumbuf[16];
	if( m_recvtype == TYPE_MANU ) {
		while (1) {
			ret = m_so->read(buf, sizeof(buf) - 1, iTimeout);
			if( ret <= 0 )
				return ret;
			buf[ret] = 0;
			m_recv.append(buf, ret);
			if( m_recv.size() > 1024 )
				return -1;
			if(strstr(buf, "00000000") != 0 ) {
				strcpy(sumbuf, "00000000");
				break;
			}
		}
	}
	else if( m_recvtype == TYPE_AUTO ) {
		bufwrap wrap;
		while(1) {
			// 先读键名的长度
			ret = m_so->read(buf, 2, iTimeout);
			if( ret <= 0 )
				return ret;
			else if( ret < 2 )
				return -1;
			ARCHY_Uint2 len;
			memcpy(&len, buf, 2);
			byte_order_convert(&len, 2);
//	printf("keylen:%d", len);
			m_recv.append(buf, ret);
			if( len == 0 ) {
				ret = m_so->read(buf, 8, iTimeout);
				if( ret <=0 )
					return ret;
				else if( ret < 8 )
					return -1;
				checksum check;
				check.add_chars("KV_AUTO:", 8);
				check.add_chars(m_recv.getcbuf(), m_recv.size());
				ARCHY_Uint4 sum = check.get_checksum();
				sprintf(sumbuf, "%08x", sum);
				if( strncmp(sumbuf, buf, 8) != 0 )
					return -1;
				break;
			}
			wrap.resize(len);

			// 读键名
			ret = m_so->read(wrap.getcbuf(), len, iTimeout);
			if( ret <= 0 )
				return ret;
			else if( ret < len )
				return -1;
//	printf(",key:%s\n", wrap.getcbuf());
			
			m_recv += wrap;
			
			// 再读键值的长度
			ret = m_so->read(buf, 2, iTimeout);
			if( ret <=0 )
				return ret;
			else if( ret < 2 )
				return -1;
			memcpy(&len, buf, 2);
			byte_order_convert(&len, 2);
			m_recv.append(buf, ret);
//	printf("vallen:%d,", len);
			if( len == 0 ) continue;
			wrap.resize(len);
			// 读键值
			ret = m_so->read(wrap.getcbuf(), len, iTimeout);
			if( ret <= 0 )
				return ret;
			else if( ret < len )
				return -1;
			
//	printf("val:%s,", wrap.getcbuf());

			m_recv += wrap;
			if( m_recv.size() > 1024 )
				return -1;
		}
	}

	if( m_bSendChecksum ) {
		ret = m_so->write(sumbuf, 8);
		if( ret < 8 )
			return -1;
	}
	m_iOff = 0;
	return 1;
}

int kv_tran::get(string &strKey, string &strVal)
{
	if( m_recvtype == TYPE_AUTO ) {
		ARCHY_Uint2 len;
		memcpy(&len, &m_recv.getcbuf()[m_iOff], 2);
		byte_order_convert(&len, 2);
		if( len == 0 ) 			
			return 0;
	
		m_iOff += 2;
		bufwrap wrap;
		wrap = m_recv.subbuf(m_iOff, len);
		strKey = wrap.getcbuf();
		m_iOff += len;
		
		memcpy(&len, &m_recv.getcbuf()[m_iOff], 2);
		byte_order_convert(&len, 2);
		if( len == 0 ) {
			strVal.erase();
		}
		else {
			m_iOff += 2;
			wrap = m_recv.subbuf(m_iOff, len);
			strVal = wrap.getcbuf();
			m_iOff += len;
		}
		return 1;

	}
	else if( m_recvtype == TYPE_MANU) {
		char* strOrigin = m_recv.getcbuf() + m_iOff;
		char* equalpos = strchr(strOrigin, '=');
		if( equalpos == 0 )
			return 0;
		
		*equalpos++ = 0;
		strKey = strOrigin;
		if(*equalpos++ != '"' )
			return -1;
		char* doublequot = strchr(equalpos, '"');
		if(doublequot == 0 )
			return -1;
		*doublequot++ = 0;
		strVal = equalpos;

		if( *doublequot++ != ',')
			return -1;

		m_iOff = doublequot - m_recv.getcbuf();
		return 1;
	}
	return -1;
}

END_NAMESPACE_ARCHY

