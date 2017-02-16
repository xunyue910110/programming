//////////////////////////////////////////////////////////////////////
// 
// 类名: pulse: 心跳类，负责定期发出心跳，pulse_monitor: 心跳监测类，负责监测心跳
// 描述: 心博处理类
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-19 10:41
// 版本: V1.0.0 
// 版权: 南京联创科技股份有限公司
///////////////////////////////////////////////////////////////////////

#if !defined(ARCHY_PULSE_MONITOR_PULSE_HEADER_)
#define ARCHY_PULSE_MONITOR_PULSE_HEADER_
#include "date_time.hpp"
#include "mutex.hpp"
#include "archy_config.hpp"
#include <list>
USING_STD_NAMESPACE;

namespace archy {
	
class pulse_monitor;

// 一个心博对象只能在同一个线程内进行心跳，
// 否则可能发生心跳紊乱
class pulse
{
friend class pulse_monitor;
public:
	pulse();
	pulse(const time_span& span);
	~pulse();
			
	// 设置心跳间隔的时间,不设置默认是300秒
	void set_pulse(const time_span& span)
	{ m_span = span; }

	// 注册心跳监视,返回本对象在心跳监视中的No号
	int attach(pulse_monitor& monitor, const string& attachName = "");

	// 获取注册后对象在心跳监视中的No号
	// 如果没有注册或者注册失败，则返回0
	int get_pulseno()const { return m_pulseno; }
	
	// 注销心跳监视
	void detach();
	
	// 心脏跳动一次
	void beat() { m_time = date_time::current(); }
	
	const string& get_attchname()const { return m_attchname; }
private:
	// 心跳时间，如果该时间内仍然没有心跳的话，
	// 心跳监视器将认为该心跳对象已经停止心跳
	time_span m_span;
	// 上一次心跳的时间
	date_time m_time;
	// 心跳监视器的指针
	pulse_monitor* m_monitor;
	// 心跳号码，在注册心跳监视后获得
	int m_pulseno;	
	// 自身标示号
	string m_attchname;
};

// 心跳监控对象，一般在单独的线程内使用
// 一般建议使用全局对象
class pulse_monitor
{
friend class pulse;
public:
	pulse_monitor();

	// 获取停止心跳的心跳对象的注册No号, 可循环调用
	// 返回0说明没有注册对象停止心跳
	int get_dead(string* strAttchName = 0, date_time* deadtime = 0);
	
	// 获取已注册对象的最小心跳间隔
	time_span get_min_span()const { return m_min_span; }
	
private:
	void add_pulse(pulse* pul);
	void del_pulse(pulse* pul);

	archy::mutex m_mutex;
	// 最小的心跳秒数
	time_span m_min_span;
	
	list<pulse*> m_lstPulse;
	
	// 不断递增的注册号码
	int m_iPulseNo;
};

} // namespace archy 

#endif //ARCHY_PULSE_MONITOR_PULSE_HEADER_
