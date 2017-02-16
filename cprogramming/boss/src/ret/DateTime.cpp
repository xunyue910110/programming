// ##########################################
// Source file : DataTime.cpp
// System      : Move Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010915
// Update      : 20011024
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "DateTime.h"

//##ModelId=3BA5EDF500B8
bool DateTime::setDateTime(int mo, int da,int hr,int mi,int se)
{
	int iYr,iMo;
	struct tm tm;
	time_t theDate;

	time(&theDate);
	tm	= *localtime(&theDate);
	iYr	= tm.tm_year + 1900;
	iMo	= tm.tm_mon	 + 1;

	if ( iMo < mo )
		iYr--;

	return setDateTime(iYr,mo,da,hr,mi,se);
}


//##ModelId=3BA5EDF30188
bool DateTime::setDateTime(int yr,int mo, int da,int hr,int mi,int se)
{
	if ( !get4Year(yr) )
		return false;

	if( !isValid(yr,mo,da,hr,mi,se) )
		return false;

	m_tm.tm_year  = yr - 1900;
	m_tm.tm_mon   = mo - 1;
	m_tm.tm_mday  = da;
	m_tm.tm_hour  = hr;
	m_tm.tm_min   = mi;
	m_tm.tm_sec   = se;
	m_tm.tm_isdst = -1;

	return true;
}


//##ModelId=3BA72E9103C0
bool DateTime::setDateTime(string mo,string da,string hr,string mi,string se)
{
	struct tm tm;
	time_t theDate;
	int iYr,iMo,iDa,iHr,iMi,iSe,iMo1;

	iMo=atoi( mo.c_str() );
	iDa=atoi( da.c_str() );
	iHr=atoi( hr.c_str() );
	iMi=atoi( mi.c_str() );
	iSe=atoi( se.c_str() );

	time(&theDate);
	tm	= *localtime(&theDate);
	iYr	= tm.tm_year + 1900;
	iMo1= tm.tm_mon	 + 1;

	if ( iMo1 < iMo )
		iYr--;

	return setDateTime(iYr,iMo,iDa,iHr,iMi,iSe);
}

//##ModelId=3BA72E8F0056
bool DateTime::setDateTime(string yr,string mo, string da,
							string hr,string mi,string se)
{
	int iMo,iDa,iYr,iHr,iMi,iSe;

	iYr=atoi( yr.c_str() );
	iMo=atoi( mo.c_str() );
	iDa=atoi( da.c_str() );
	iHr=atoi( hr.c_str() );
	iMi=atoi( mi.c_str() );
	iSe=atoi( se.c_str() );

	return setDateTime(iYr,iMo,iDa,iHr,iMi,iSe);
}


//##ModelId=3BA60BC2020D
DateTime *DateTime::addYear(int yr)
{
	m_tm.tm_year+=yr;
	while ( isValid() ) {
		m_tm.tm_mday--;
	}

	return this;
}

//##ModelId=3BA60BC203DA
DateTime *DateTime::addMonth(int mo)
{
	int i;

	if (mo==0) return this;
	mo+=m_tm.tm_mon;
	if (mo>0) {
		m_tm.tm_year+=mo/12;
		m_tm.tm_mon=mo%12;
	}
	else {
		for ( i=0;i*12+mo<0;i++) { ; }
	  	m_tm.tm_year-=i;
		m_tm.tm_mon=mo+i*12;
	}
	while ( isValid() ) {
		m_tm.tm_mday--;
	}

	return this;
}

//##ModelId=3BA60BC301AA
DateTime *DateTime::addDay(int da)
{
	time_t tSecond=mktime(&m_tm)+da*24*3600;
	m_tm=*( localtime(&tSecond) );
	return this;
}

//##ModelId=3BA60BC30377
DateTime *DateTime::addHour(int hr)
{
	time_t tSecond=mktime(&m_tm)+hr*3600;
	m_tm=*( localtime(&tSecond) );

	return this;
}

//##ModelId=3BA60BC4015C
DateTime *DateTime::addMinute(int mi)
{
	time_t tSecond=mktime(&m_tm)+mi*60;
	m_tm=*( localtime(&tSecond) );

	return this;
}

//##ModelId=3BA60BC4033C
DateTime *DateTime::addSecond(int se)
{
	time_t tSecond=mktime(&m_tm);
	tSecond+=se;
	m_tm=*( localtime(&tSecond) );


	return this;
}

//##ModelId=3BA60BC5013F
int DateTime::yearDiff(DateTime *theDateTime)
{
	return m_tm.tm_year - theDateTime->m_tm.tm_year;
}

//##ModelId=3BA60BC5032A
int DateTime::monthDiff(DateTime *theDateTime)
{
	int m1,m2;

	m1 = theDateTime->m_tm.tm_year*12 + theDateTime->m_tm.tm_mon;
	m2 = m_tm.tm_year*12 + m_tm.tm_mon;

	return m2-m1;
}

//##ModelId=3BA60BC60140
int DateTime::dayDiff(DateTime *theDateTime)
{
	time_t t1,t2;

	t1=mktime(&theDateTime->m_tm);
	t2=mktime(&m_tm);

	return (t2-t1)/(24*3600);
}

//##ModelId=3BA60BC6035D
int DateTime::hourDiff(DateTime *theDateTime)
{
	time_t t1,t2;

	t1=mktime(&theDateTime->m_tm);
	t2=mktime(&m_tm);

	return (t2-t1)/3600;
}

//##ModelId=3BA60BC70192
int DateTime::minuteDiff(DateTime *theDateTime)
{
	time_t t1,t2;

	t1=mktime(&theDateTime->m_tm);
	t2=mktime(&m_tm);

	return (t2-t1)/60;
}

//##ModelId=3BA60BC703D7
int DateTime::secondDiff(DateTime *theDateTime)
{
	time_t t1,t2;

	t1=mktime(&(theDateTime->m_tm));
	t2=mktime(&m_tm);

	return t2-t1;
}

//##ModelId=3BA60DA500A9
bool DateTime::isValid(int yr,int mo, int da,int hr,int mi,int se)
{
	int febDay=28;

	if (!get4Year(yr))
		return false;

	if ( (yr>2200) || (yr<1950) ) return false;

	if ( (mo>12) || (mo<1) ) return false;
	if ( (da>31) || (da<1) ) return false;
	if ( (hr>23) || (hr<0) ) return false;
	if ( (mi>59) || (mi<0) ) return false;
	if ( (se>59) || (se<0) ) return false;

	switch (mo)
	{
	case 2:
		if (isLeapYear(yr))  {
		   febDay=29;
      }
		if (da>febDay) return false;
		break;
	case 4:
		if (da>30) return false;
		break;
	case 6:
		if (da>30) return false;
		break;
	case 9:
		if (da>30) return false;
		break;
	case 11:
		if (da>30) return false;
		break;
	}

	return true;
}

//##ModelId=3BA60DA500A9
bool DateTime::isValid()
{
	return isValid(m_tm.tm_year+1900,m_tm.tm_mon+1,
					m_tm.tm_mday,m_tm.tm_hour,m_tm.tm_min,m_tm.tm_sec);
}


//##ModelId=3BA6AF9A00D4
struct tm DateTime::operator =(const struct tm &aTm)
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

//##ModelId=3BA5EDF700ED
bool DateTime::get4Year(int &yr)
{
	if( yr > 1900 )
		return true;

	if( yr <= 50 && yr > 0 ) {
		yr += 2000;
		return true;
	}

	if( yr > 50 && yr < 100 ) {
		yr += 1900;
		return true;
	}

	if( yr == 0 ) {
		yr=2000;
		return true;
	}

	return false;
}


