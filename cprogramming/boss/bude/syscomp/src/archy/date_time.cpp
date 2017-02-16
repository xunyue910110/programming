#include "date_time.hpp"
#include "debug.hpp"
#include <stdio.h>

namespace archy {
	
// Constructors 

date_time date_time::current()
{
	return date_time(::time(0));
}

date_time::date_time( const date_time& timeSrc )
{
	m_time = timeSrc.m_time;
}
	
date_time::date_time( time_t timefrom )
{
	m_time = timefrom;
}

date_time::date_time( int nYear, int nMonth, int nDay, int nHour, 
	int nMin, int nSec, int nDST /*= -1 */ )
{
	struct tm atm;
	atm.tm_sec = nSec;
	atm.tm_min = nMin;
	atm.tm_hour = nHour;
	ARCHY_ASSERT(nDay >= 1 && nDay <= 31);
	atm.tm_mday = nDay;
	ARCHY_ASSERT(nMonth >= 1 && nMonth <= 12);
	atm.tm_mon = nMonth - 1;        // tm_mon is 0 based
	ARCHY_ASSERT(nYear >= 1900);
	atm.tm_year = nYear - 1900;     // tm_year is 1900 based
	atm.tm_isdst = nDST;
	m_time = mktime(&atm);
	ARCHY_ASSERT(m_time != -1);       // indicates an illegal input time
}


struct tm* date_time::get_gmttm(struct tm* ptm) const
{
	if (ptm != NULL)
	{
		*ptm = *gmtime(&m_time);
		return ptm;
	}
	else
		return gmtime(&m_time);
}

struct tm* date_time::get_localtm( struct tm* ptm ) const
{
	if (ptm != NULL)
	{
		struct tm* ptmTemp = localtime(&m_time);
		if (ptmTemp == NULL)
			return NULL;    // indicates the m_time was not initialized!

		*ptm = *ptmTemp;
		return ptm;
	}
	else
		return localtime(&m_time);
}

// %Y 四位年份; %M 两位月份; %D 两位天数; %H 两位小时数;%N 两位分钟数;%S两位秒数
// %y 两位年份; %m 月份; %d 天数 %h小时数 %n分钟数 %s秒数

std::string date_time::format( const char* fmt ) const
{
	ARCHY_ASSERT( fmt != 0 );	
	const char *p;
	std::string strFmt;
	char buf[8];

	for(p = fmt; *p != '\0'; p++)  {
            if(*p != '%') {
            	sprintf(buf, "%c", *p );
                strFmt += buf;
                continue;
            }
            switch(*++p) {
            case 'Y':
            		sprintf(buf, "%04d", get_year() );
                    break;
            case 'y':
            		sprintf(buf, "%02d", get_year() % 100 );
                    break;
            case 'M':
            		sprintf(buf, "%02d", get_month() );
                    break;
            case 'm':
            		sprintf(buf, "%d", get_month() );
                    break;
            case 'D':
            		sprintf(buf, "%02d", get_day() );
                    break;
            case 'd':
            		sprintf(buf, "%d", get_day() );
                    break;
            case 'H':
            		sprintf(buf, "%02d", get_hour() );
                    break;
            case 'h':
            		sprintf(buf, "%d", get_hour() );
                    break;
            case 'N':
            		sprintf(buf, "%02d", get_minute() );
                    break;
            case 'n':
            		sprintf(buf, "%d", get_minute() );
                    break;
            case 'S':
            		sprintf(buf, "%02d", get_second() );
                    break;
            case 's':
            		sprintf(buf, "%d", get_second() );
                    break;
            case '%':
            		sprintf(buf, "%c", '%' );
                    break;
            }
	    strFmt += buf;
	}
	return strFmt;
}

} // namespace archy

