#ifndef ARCHY_TIME_SPAN_HEADER_840890380184014810481048104
#define ARCHY_TIME_SPAN_HEADER_840890380184014810481048104
#include <time.h>
#include "archy_config.hpp"

namespace archy {

class time_span
{
public:
// Constructors
	time_span(){}
	
	time_span(time_t time_src)
	{ m_timeSpan = time_src; }
	
	time_span(long lDays, int nHours, int nMins, int nSecs)
	{ m_timeSpan = nSecs + 60* (nMins + 60* (nHours + 24* lDays)); }
	
	time_span(const time_span& timeSpanSrc)
	{ m_timeSpan = timeSpanSrc.m_timeSpan; }
	
	time_span& operator=(const time_span& timeSpanSrc)
	{ m_timeSpan = timeSpanSrc.m_timeSpan;  return *this;}

// Attributes
	// extract parts
	long get_days() const   // total # of days
	{ return m_timeSpan / (24*3600L); }
	
	long get_totalhours() const
	{ return m_timeSpan/3600; }
	
	int get_hours() const
	{ return (int)(get_totalhours() - get_days()*24); }
	
	long get_totalminutes() const
	{ return m_timeSpan/60; }
	
	int get_minutes() const
	{ return (int)(get_totalminutes() - get_totalhours()*60); }
	
	long get_totalseconds() const
	{ return m_timeSpan; }
	
	int get_seconds() const
	{ return (int)(get_totalseconds() - get_totalminutes()*60); }

// Operations
	// time math
	time_span operator-(time_span timeSpan) const
	{ return time_span(m_timeSpan - timeSpan.m_timeSpan); }
	
	time_span operator+(time_span timeSpan) const
	{ return time_span(m_timeSpan + timeSpan.m_timeSpan); }

	time_span& operator+=(time_span timeSpan)
	{ m_timeSpan += timeSpan.m_timeSpan; return *this; }
	
	time_span& operator-=(time_span timeSpan)
	{ m_timeSpan -= timeSpan.m_timeSpan; return *this; }
	
	bool operator==(time_span timeSpan) const { return m_timeSpan == timeSpan.m_timeSpan; }
	bool operator!=(time_span timeSpan) const { return m_timeSpan != timeSpan.m_timeSpan; }
	bool operator< (time_span timeSpan) const { return m_timeSpan <  timeSpan.m_timeSpan; }
	bool operator> (time_span timeSpan) const { return m_timeSpan >  timeSpan.m_timeSpan; }
	bool operator<=(time_span timeSpan) const { return m_timeSpan <= timeSpan.m_timeSpan; }
	bool operator>=(time_span timeSpan) const { return m_timeSpan >= timeSpan.m_timeSpan; }

	// %D 总天数; %H 两位小时数;%N 两位分钟数;%S两位秒数
	// %h小时数 %n分钟数 %s秒数
	// %% 表示一个%号
	std::string format( const char* fmt ) const;
	
private:
	time_t m_timeSpan;
	friend class date_time;
};

} // namespace archy

#endif //ARCHY_TIME_SPAN_HEADER_840890380184014810481048104


