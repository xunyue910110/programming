// ##########################################
// Source file : DateTime.cpp
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#include "DateTime.h"
#include "base/supdebug.h"

#define ISLEAPYEAR(x) (((x)%4==0&&(x)%100!=0)||(x)%400==0)


//##ModelId=485712420222
DateTime::DateTime()
{
    m_tm.tm_isdst = -1;
}

//##ModelId=4857125502EE
DateTime *DateTime::addYear(int yr)
{
    m_tm.tm_year += yr;
    while (isValid())
    {
        m_tm.tm_mday--;
    }

    return this;
}

//##ModelId=48571275029F
DateTime *DateTime::addMonth(int mo)
{
    int i;

    if (mo == 0)
    {
        return this;
    }
    mo += m_tm.tm_mon;
    if (mo > 0)
    {
        m_tm.tm_year += mo / 12;
        m_tm.tm_mon = mo % 12;
    }
    else
    {
        for ( i = 0; i * 12 + mo < 0; i++)
        { ; }
        m_tm.tm_year -= i;
        m_tm.tm_mon = mo + i * 12;
    }
    while (isValid())
    {
        m_tm.tm_mday--;
    }
    return this;
}

//##ModelId=4857128A009C
DateTime *DateTime::addDay(int da)
{
    time_t tSecond = mktime(&m_tm) + da * 24 * 3600;
    m_tm = *(localtime(&tSecond));
    return this;
}

//##ModelId=485712AE03B9
DateTime *DateTime::addHour(int hr)
{
    time_t tSecond = mktime(&m_tm) + hr * 3600;
    m_tm = *(localtime(&tSecond));
    return this;
}

//##ModelId=485712C702CE
DateTime *DateTime::addMinute(int mi)
{
    time_t tSecond = mktime(&m_tm) + mi * 60;
    m_tm = *(localtime(&tSecond));
    return this;
}

//##ModelId=485712E2029F
DateTime *DateTime::addSecond(int se)
{
	int iMinuteAdd,iHourAdd,iDayAdd;

	iMinuteAdd  = (m_tm.tm_sec + se)/60;
	m_tm.tm_sec = (m_tm.tm_sec + se)%60;
	
	iHourAdd    = (m_tm.tm_min + iMinuteAdd)/60;
	m_tm.tm_min = (m_tm.tm_min + iMinuteAdd)%60;
	
	iDayAdd      = (m_tm.tm_hour + iHourAdd)/24;
	m_tm.tm_hour = (m_tm.tm_hour + iHourAdd)%24;
	
	if(iDayAdd!=0)
	{
		addupDay2Tm(iDayAdd);
	}

    return this;
}

void DateTime::addupDay2Tm(int &iDayAdd)
{
	int iDaysInFeb;
	switch(m_tm.tm_mon+1)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
	    if( (m_tm.tm_mday + iDayAdd -31 ) >0 ) // 跨月
	    {
	    	m_tm.tm_mon += 1;
	    	if(m_tm.tm_mon>11) // 跨年
	    	{
	    		m_tm.tm_mon = 0;
	    		m_tm.tm_year += 1;	
	    	}
	    	iDayAdd -= 31;
	    	addupDay2Tm(iDayAdd);
	    }
	    else
	    {
	    	m_tm.tm_mday += iDayAdd;
		}
	    break;
	case 2:
	    if( ISLEAPYEAR(m_tm.tm_year+1900))
	    {
	    	iDaysInFeb = 29;	
	    }
	    else
	    {
	    	iDaysInFeb = 28;	
	    }
	    if( (m_tm.tm_mday + iDayAdd -iDaysInFeb) >0 ) // 跨月
	    {
	    	m_tm.tm_mon += 1;
	    	iDayAdd -= iDaysInFeb;
	    	addupDay2Tm(iDayAdd);
	    }
	    else
	    {
	    	m_tm.tm_mday += iDayAdd;
		}
	    break;
	case 4:
	case 6:
	case 9:
	case 11:
	    if( (m_tm.tm_mday + iDayAdd -30 ) >0 ) // 跨月
	    {
	    	m_tm.tm_mon += 1;
	    	iDayAdd -= 30;
	    	addupDay2Tm(iDayAdd);
	    }
	    else
	    {
	    	m_tm.tm_mday += iDayAdd;
		}
	    break;		
	}
}

//##ModelId=485712FE0128
int DateTime::yearDiff(DateTime *theDateTime)
{
    return m_tm.tm_year - theDateTime->m_tm.tm_year;
}

//##ModelId=4857131B02CE
int DateTime::monthDiff(DateTime *theDateTime)
{
    int m1, m2;
    m1 = theDateTime->m_tm.tm_year * 12 + theDateTime->m_tm.tm_mon;
    m2 = m_tm.tm_year * 12 + m_tm.tm_mon;
    return m2 - m1;
}

//##ModelId=4857133E03A9
int DateTime::dayDiff(DateTime *theDateTime)
{
    time_t t1, t2;
    t1 = mktime(&theDateTime->m_tm);
    t2 = mktime(&m_tm);
    return (t2 - t1) / (24 * 3600);
}

//##ModelId=4857137602CE
int DateTime::hourDiff(DateTime *theDateTime)
{
    time_t t1, t2;
    t1 = mktime(&theDateTime->m_tm);
    t2 = mktime(&m_tm);
    return (t2 - t1) / 3600;
}

//##ModelId=4857138901E4
int DateTime::minuteDiff(DateTime *theDateTime)
{
    time_t t1, t2;
    t1 = mktime(&theDateTime->m_tm);
    t2 = mktime(&m_tm);
    return (t2 - t1) / 60;
}

//##ModelId=4857139D034B
int DateTime::secondDiff(DateTime *theDateTime)
{
    time_t t1, t2;
    t1 = mktime(&(theDateTime->m_tm));
    t2 = mktime(&m_tm);
    return t2 - t1;
}

//##ModelId=4857145F035B
bool DateTime::setDateTime(int yr, int mo, int da, int hr, int mi, int se)
{
    if (!get4Year(yr))
    {
        return false;
    }

    if(!isValid(yr, mo, da, hr, mi, se))
    {
        return false;
    }

    m_tm.tm_year  = yr - 1900;
    m_tm.tm_mon   = mo - 1;
    m_tm.tm_mday  = da;
    m_tm.tm_hour  = hr;
    m_tm.tm_min   = mi;
    m_tm.tm_sec   = se;
    m_tm.tm_isdst = -1;

    return true;
}

//##ModelId=485714DE00BB
bool DateTime::setDateTime(int mo, int da, int hr, int mi, int se)
{
    int iYr, iMo;
    struct tm tm;
    time_t theDate;

    time(&theDate);
    tm  = *localtime(&theDate);
    iYr = tm.tm_year + 1900;
    iMo = tm.tm_mon  + 1;

    if (iMo < mo)
    {
        iYr--;
    }

    return setDateTime(iYr, mo, da, hr, mi, se);
}

//##ModelId=48571509004E
bool DateTime::setDateTime(const string &yr, const string &mo, const string &da, const string &hr, const string &mi, const string &se)
{
    int iMo, iDa, iYr, iHr, iMi, iSe;

    iYr = atoi(yr.c_str());
    iMo = atoi(mo.c_str());
    iDa = atoi(da.c_str());
    iHr = atoi(hr.c_str());
    iMi = atoi(mi.c_str());
    iSe = atoi(se.c_str());

    return setDateTime(iYr, iMo, iDa, iHr, iMi, iSe);
}

//##ModelId=4857154A0242
bool DateTime::setDateTime(const string &mo, const string &da, const string &hr, const string &mi, const string &se)
{
    struct tm tm;
    time_t theDate;
    int iYr, iMo, iDa, iHr, iMi, iSe, iMo1;

    iMo = atoi(mo.c_str());
    iDa = atoi(da.c_str());
    iHr = atoi(hr.c_str());
    iMi = atoi(mi.c_str());
    iSe = atoi(se.c_str());

    time(&theDate);
    tm  = *localtime(&theDate);
    iYr = tm.tm_year + 1900;
    iMo1= tm.tm_mon  + 1;

    if (iMo1 < iMo)
    {
        iYr--;
    }

    return setDateTime(iYr, iMo, iDa, iHr, iMi, iSe);
}

//##ModelId=4857158002FD
bool DateTime::isValid(int yr, int mo, int da, int hr, int mi, int se)
{
    int febDay = 28;

    if (!get4Year(yr))
    {
        return false;
    }

    if ((yr > 2200) || (yr < 1950))
    {
        return false;
    }

    if ((mo > 12) || (mo < 1) )
    {
        return false;
    }
    if ((da > 31) || (da < 1) )
    {
        return false;
    }
    if ((hr > 23) || (hr < 0) )
    {
        return false;
    }
    if ((mi > 59) || (mi < 0) )
    {
        return false;
    }
    if ((se > 59) || (se < 0) )
    {
        return false;
    }

    switch (mo)
    {
    case 2:
        if (isLeapYear(yr))
        {
            febDay = 29;
        }
        if (da > febDay)
        {
            return false;
        }
        break;
    case 4:
        if (da > 30)
        {return false;
        }
        break;
    case 6:
        if (da > 30)
        {
            return false;
        }
        break;
    case 9:
        if (da > 30)
        {
            return false;
        }
        break;
    case 11:
        if (da > 30)
        {
            return false;
        }
        break;
    }
    return true;
}

//##ModelId=485715AB00DA
bool DateTime::isValid()
{
    return isValid(m_tm.tm_year + 1900, m_tm.tm_mon + 1, m_tm.tm_mday,
                    m_tm.tm_hour, m_tm.tm_min, m_tm.tm_sec);
}

//##ModelId=485716000261
int DateTime::GetYear()
{
    return m_tm.tm_year + 1900;
}

//##ModelId=4857161400EA
int DateTime::GetMonth()
{
    return m_tm.tm_mon + 1;
}

//##ModelId=4857161F0157
int DateTime::GetDay()
{
    return m_tm.tm_mday;
}

//##ModelId=485716540148
int DateTime::GetHour()
{
    return m_tm.tm_hour;
}

//##ModelId=4857165D0290
int DateTime::GetMinute()
{
    return m_tm.tm_min;
}

//##ModelId=485716680167
int DateTime::GetSecond()
{
    return m_tm.tm_sec;
}

//##ModelId=485716C0004E
bool DateTime::get4Year(int &yr)
{
    if( yr > 1900 )
    {
        return true;
    }

    if( yr <= 50 && yr > 0 )
    {
        yr += 2000;
        return true;
    }

    if( yr > 50 && yr < 100 )
    {
        yr += 1900;
        return true;
    }

    if( yr == 0 )
    {
        yr = 2000;
        return true;
    }
    return false;
}

//##ModelId=485717350186
struct tm DateTime::operator = (const struct tm &aTm)
{
    struct tm theTm;

    theTm.tm_year = aTm.tm_year;
    theTm.tm_mon  = aTm.tm_mon;
    theTm.tm_mday = aTm.tm_mday;
    theTm.tm_hour = aTm.tm_hour;
    theTm.tm_min  = aTm.tm_min;
    theTm.tm_sec  = aTm.tm_sec;

    return theTm;
}

