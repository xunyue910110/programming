//////////////////////////////////////////////////////////////////////
// 
// 类名: pulse: 心跳类，负责定期发出心跳，pulse_monitor: 心跳监测类，负责监测心跳
// 描述: 心博处理类
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-7-19 10:41
// 版本: V1.0.0 
// 版权: 南京联创科技股份有限公司
///////////////////////////////////////////////////////////////////////

#include "pulse.hpp"

namespace archy {
	
pulse::pulse()
:m_span(0, 0, 5, 0)
,m_monitor(0)
,m_pulseno(0)
{
}

pulse::pulse(const time_span& span)
:m_span(span)
,m_monitor(0)
,m_pulseno(0)
{}

pulse::~pulse()
{
	if( m_monitor ) this->detach();
}

		

// 注册心跳监视,返回本对象在心跳监视中的No号
// 返回0, 说明注册不成功，心跳监测对象没有空床位
int pulse::attach(pulse_monitor& monitor, const string& attachName)
{
	if( m_pulseno != 0 ) return m_pulseno;
	m_time = date_time::current();
	m_monitor = &monitor;
	m_attchname = attachName;
	monitor.add_pulse(this);
	return m_pulseno;
}


// 注销心跳监视
void pulse::detach()
{
	if( m_monitor == 0 ) return;
	m_monitor->del_pulse(this);
	m_monitor = 0;
}


pulse_monitor::pulse_monitor()
:m_min_span(0, 0, 15, 0)
,m_iPulseNo(0)
{
}

void pulse_monitor::add_pulse(pulse* pul)
{
	scoped_lock lock(m_mutex);

	m_lstPulse.push_back(pul);
	pul->m_pulseno = ++m_iPulseNo;
	if( m_min_span > pul->m_span )
		m_min_span = pul->m_span;	
}

void pulse_monitor::del_pulse(pulse* pul)
{
	list<pulse*>::iterator it, it1;
	scoped_lock lock(m_mutex);
	
	// 最小时间间隔需要重新计算
	m_min_span = time_span(0,0,15,0);
	for( it = m_lstPulse.begin(); it != m_lstPulse.end(); ++it) {
 		if( *it == pul ) {
 			m_lstPulse.erase(it);
			pul->m_pulseno = 0;
                        break;
		}
        }
        
 	for( it = m_lstPulse.begin(); it != m_lstPulse.end(); ++it) {
		if( m_min_span > (*it)->m_span ) {
			m_min_span = (*it)->m_span;
                }
	}
				
}



// 获取停止心跳的心跳对象的注册No号, 可循环调用
// 返回0说明没有注册对象停止心跳
int pulse_monitor::get_dead(string* strAttchName, date_time* deadtime)
{
	list<pulse*>::iterator it;
	scoped_lock lock(m_mutex);
	
	for( it = m_lstPulse.begin(); it != m_lstPulse.end(); ++it ) {
		if( date_time::current() - (*it)->m_time > (*it)->m_span ) {
                	if( deadtime ) *deadtime = (*it)->m_time;
			(*it)->m_time = date_time::current();
			if( strAttchName ) *strAttchName = (*it)->m_attchname;
			return (*it)->m_pulseno;
		}
	}	
	return 0;
}



} // namespace archy 

