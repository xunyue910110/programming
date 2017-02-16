#ifndef ARCHY_TIME_HEADER_802850841048
#define ARCHY_TIME_HEADER_802850841048
#include "time_span.hpp"

namespace archy {
	
class time_span;
/*
A time object represents an absolute time and date, 
based on coordinated universal time (UTC), 
which is equivalent to Greenwich mean time (GMT). 
The local time zone is controlled by the TZ environment variable.
*/

class date_time
{
private:
	time_t m_time;

public:
	// Constructors 
	date_time(){}	
	date_time( const date_time& timeSrc );	
	date_time( time_t timefrom );	
	date_time( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST = -1 );

	// Creates a date_time object that represents the current time.  
	static date_time current();
	
	// Returns a time_t value for the given time object. 
	time_t get_time() const { return m_time; }
	
	// Returns the year that this time object represents.  
	int get_year() const
	{ return (get_localtm()->tm_year) + 1900; }
	
	// Returns the month that this time object represents (1 through 12). 
	int get_month() const
	{ return get_localtm()->tm_mon + 1; }
	
	// Returns the day that this time object represents (1 through 31). 
	int get_day() const
	{ return get_localtm()->tm_mday; }
	
	// Returns the hour that this time object represents (0 through 23). 
	int get_hour() const
	{ return get_localtm()->tm_hour; }
	
	// Returns the minute that this time object represents (0 through 59). 
	int get_minute() const
	{ return get_localtm()->tm_min; }

	// Returns the second that this time object represents (0 through 59). 
	int get_second() const
	{ return get_localtm()->tm_sec; }

	// Returns the day of the week (1 for Sunday, 2 for Monday, and so forth). 
	int get_dayofweek() const
	{ return get_localtm()->tm_wday + 1; }
	
	// Breaks down a time object into components — based on UTC. 
	struct tm* get_gmttm( struct tm* ptm = 0 ) const;
	// Breaks down a time object into components — based on the local time zone. 
	struct tm* get_localtm( struct tm* ptm = 0 ) const;
	
	// %Y 四位年份; %M 两位月份; %D 两位天数; %H 两位小时数;%N 两位分钟数;%S两位秒数
	// %y 两位年份; %m 月份; %d 天数 %h小时数 %n分钟数 %s秒数
	// %% 表示一个%号
	std::string format( const char* fmt ) const;
	
	// Assigns new time values. 
	date_time& operator =( const date_time& timeSrc )
	{ m_time = timeSrc.m_time; return *this; }
	
	date_time& operator =( time_t timeSrc )
	{ m_time = timeSrc; return *this; }

	/*  Add and subtract QTimeSpan and time objects.  */
	date_time operator +( time_span timeSpan ) const
	{ return date_time( m_time + timeSpan.m_timeSpan); }

	date_time operator -( time_span timeSpan ) const
	{ return date_time( m_time - timeSpan.m_timeSpan); }

	time_span operator -( date_time timeSrc ) const
	{ return time_span(m_time - timeSrc.m_time); }

	/* Add and subtract a QTimeSpan object to and from this time object. */
	date_time& operator +=( time_span timeSpan )
	{ m_time += timeSpan.m_timeSpan; return *this; }

	date_time& operator -=( time_span timeSpan )
	{ m_time -= timeSpan.m_timeSpan; return *this; }

	/* Compare two absolute times. */
	bool operator ==(date_time s) { return m_time == s.m_time; }
	bool operator !=(date_time s) { return m_time != s.m_time; }
	bool operator < (date_time s) { return m_time < s.m_time; }
	bool operator > (date_time s) { return m_time > s.m_time; }
	bool operator <=(date_time s) { return m_time <= s.m_time; }
	bool operator >=(date_time s) { return m_time >= s.m_time; }

};

} // namespace archy

#endif // ARCHY_TIME_HEADER_802850841048

