 //////////////////////////////////////////////////////////////////////
// 
// 类名: 键值序列传输类
// 描述: 在tcp/ip网路上传输键值序列,使用简单的请求－应答方式实现
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-25 12:12
// 版本: V1.0.0 
// 版权: 南京联创科技股份有限公司
//////////////////////////////////////////////////////////////////////

#ifndef ARCHY_KEY_VAL_FORMAT_TRANSFER_HEADER
#define ARCHY_KEY_VAL_FORMAT_TRANSFER_HEADER
#include "tcp.hpp"
#include "noncopyable.hpp"
#include "bufwrap.hpp"
#include "archy_config.hpp"

USING_STD_NAMESPACE;

BEGIN_NAMESPACE_ARCHY
/*
 *	key_val 序列在网络上传输的类
 *  一条消息支持两种格式:
 *
 *  KV_MANU:key1="val1", key2="val2",...,00000000(8位校验和, 8个0表示不使用校验和)
 *  KV_AUTO:(两个字节的key1长度)key1(两个字节的val1长度)val1...(两个字节key长度的全零表示结束)00000000(8位校验和, 8个0表示不使用校验和)
 *  第一种格式要求key字符串中不能包含等于号，要求val字符串中不包含双引号
 *  第二种格式对key字符串和val字符串只有不超过65536个字节长度的限制，没有任何其他限制
 *  
 *  第一种格式适合于手工发送命令，不使用校验和，即检验和全为0
 *  第二种方式适合于程序进行处理，可以使用校验和
 *  对方收到后必须把校验和返回当作确认
 *  每条消息的最大长度不允许超过5K
 */

class kv_tran:private noncopyable
{
public:
	kv_tran();
	kv_tran(tcp_socket& so);

	void set_socket(tcp_socket& so);
	
	enum { TYPE_AUTO = 0, TYPE_MANU = 1};

	// 开始发送, 本地操作
	// 返回0 ok -1 fail
	int put_start(int type = TYPE_AUTO);
	// 发送key,val, 本地操作
	// 返回0 ok -1 fail
	int put(const string& strKey, const string& strVal);
	// 发送, 使用socket发送并且等待对方确认
	// 返回0 ok -1 fail
	int put_end(int iTimeout);

	// 开始接受, 在socket上等待数据到达
	// 返回 -1 失败 0 超时 1 ok
	int get_start(int iTimeout);
	int get_manu_start(int iTimeout);
	int get_type()const { return m_recvtype; }

	// 接受键值对
	// 返回 1 成功, 0 解析完毕, -1发生格式错误
	int get(string& strKey, string& strVal);

	const char* get_recv()const { return m_recv.getcbuf(); }
	const char* get_put()const { return m_buf.getcbuf(); }

	void SetEchoCheckSum(bool bSendChecksum){
		m_bSendChecksum = bSendChecksum;
	}
private:

private:
	tcp_socket* m_so;
	int m_type;
	bufwrap m_buf;
	bufwrap m_recv;
	int m_recvtype;
	int m_iOff;
	bool m_bSendChecksum;
	char m_recvBuffer[1024];
};

END_NAMESPACE_ARCHY

#endif //ARCHY_KEY_VAL_FORMAT_TRANSFER_HEADER



