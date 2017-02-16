#ifndef CDATETIME_CPP_INCLUDED
#define CDATETIME_CPP_INCLUDED

#include "CDateTime.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

char CDateTime::str_date[] = {'\0'};
char CDateTime::str_time[] = {'\0'};
char CDateTime::str_datetime[] = {'\0'};
char CDateTime::str_fmt_datetime[] ={'\0'};

void CDateTime::Standard()
{
    // m_Date must like YYYYMMDD
    // m_Time must like HHMMSS
    int nYear,nMonth,nDay,nHour,nMinute,nSecend;
    sscanf(this->m_Date, "%04d%02d%02d", &nYear, &nMonth , &nDay   );
    sscanf(this->m_Time, "%02d%02d%02d", &nHour, &nMinute, &nSecend);
    struct tm tmThis;
    tmThis.tm_year  = nYear   - 1900;
    tmThis.tm_mon   = nMonth  -    1;
    tmThis.tm_mday  = nDay      ;
    tmThis.tm_hour  = nHour     ;
    tmThis.tm_min   = nMinute     ;
    tmThis.tm_sec   = nSecend     ;
    tmThis.tm_isdst = -1;
    // 标准化
    mktime(&tmThis);
    nYear   = tmThis.tm_year  + 1900;
    nMonth  = tmThis.tm_mon   +    1;
    nDay    = tmThis.tm_mday      ;
    nHour   = tmThis.tm_hour      ;
    nMinute = tmThis.tm_min     ;
    nSecend = tmThis.tm_sec     ;
    
    // 重新设置
    sprintf(this->m_Date, "%04d%02d%02d", nYear, nMonth , nDay   );
    sprintf(this->m_Time, "%02d%02d%02d", nHour, nMinute, nSecend);
}

long CDateTime::GetTimetVal() const
{
    int nYear,nMonth,nDay,nHour,nMinute,nSecend;
    sscanf(this->m_Date, "%04d%02d%02d", &nYear, &nMonth , &nDay   );
    sscanf(this->m_Time, "%02d%02d%02d", &nHour, &nMinute, &nSecend);
    struct tm tmThis;
    tmThis.tm_year  = nYear   - 1900;
    tmThis.tm_mon   = nMonth  -    1;
    tmThis.tm_mday  = nDay      ;
    tmThis.tm_hour  = nHour     ;
    tmThis.tm_min   = nMinute     ;
    tmThis.tm_sec   = nSecend     ;
    tmThis.tm_isdst = -1;
    // 标准化
    return mktime(&tmThis);
}

int CDateTime::getWeekDay()
{
    int nYear,nMonth,nDay,nHour,nMinute,nSecend;
    sscanf(this->m_Date, "%04d%02d%02d", &nYear, &nMonth , &nDay   );
    sscanf(this->m_Time, "%02d%02d%02d", &nHour, &nMinute, &nSecend);
    
    struct tm tmValue;
    tmValue.tm_year = nYear -1900;
    tmValue.tm_mon  = nMonth -1;
    tmValue.tm_mday = nDay;
    tmValue.tm_hour = 0;
    tmValue.tm_min  = 0;
    tmValue.tm_sec  = 0;
    tmValue.tm_isdst = -1;
    
    if ( mktime(&tmValue) != (time_t)-1 )
    {
        if ( tmValue.tm_wday == 0 )
        {
            tmValue.tm_wday += 7;
            return 7;
        }
        m_WeekDay =  tmValue.tm_wday + '0';
        return tmValue.tm_wday;
    }
    
    return 0;
}
// 下一天00:00:00
CDateTime&  CDateTime::nextday()
{
    Standard();
    setTime("240000");
    Standard();
    return *this;
}

// 前一天24:00:00(今天00:00:00)
CDateTime&  CDateTime::overnight()
{
    int nYear,nMonth,nDay,nHour,nMinute,nSecend;
    sscanf(this->m_Date, "%04d%02d%02d", &nYear, &nMonth , &nDay   );
    //前一天
    nDay -= 1;
    sprintf(this->m_Date, "%04d%02d%02d", nYear, nMonth , nDay   );
    Standard();
    setTime("240000");
    
    return *this;
}

CDateTime operator + (const CDateTime & t1,const long duration)
{
    CDateTime t2;
    struct tm tmThis;
    sscanf(t1.m_Date, "%04d%02d%02d", &tmThis.tm_year, &tmThis.tm_mon , &tmThis.tm_mday);
    sscanf(t1.m_Time, "%02d%02d%02d", &tmThis.tm_hour, &tmThis.tm_min, &tmThis.tm_sec);
    
    tmThis.tm_year -= 1900;
    tmThis.tm_mon -= 1;
    tmThis.tm_sec += duration;
    tmThis.tm_isdst = -1;
    
    // 标准化
    if ( mktime(&tmThis) != (time_t)-1 )
    {
      //set m_Date
      sprintf(t2.m_Date,"%04d%02d%02d",
      tmThis.tm_year+1900,
      tmThis.tm_mon+1,
      tmThis.tm_mday);
    
      //set m_Time
      sprintf(t2.m_Time,"%02d%02d%02d",
      tmThis.tm_hour,
      tmThis.tm_min,
      tmThis.tm_sec);
    
      t2.m_WeekDay='0';
      return t2;
    }
    
    throw "timer convert error";
}

void CDateTime::operator +=(long duration)
{
    struct tm tmThis;
    sscanf(m_Date, "%04d%02d%02d", &tmThis.tm_year, &tmThis.tm_mon , &tmThis.tm_mday);
    sscanf(m_Time, "%02d%02d%02d", &tmThis.tm_hour, &tmThis.tm_min, &tmThis.tm_sec);
    
    tmThis.tm_year -= 1900;
    tmThis.tm_mon -= 1;
    tmThis.tm_sec += duration;
    tmThis.tm_isdst = -1;
    
    // 标准化
    if ( mktime(&tmThis) != (time_t)-1 )
    {
      //set m_Date
      sprintf(m_Date,"%04d%02d%02d",
      tmThis.tm_year+1900,
      tmThis.tm_mon+1,
      tmThis.tm_mday);
    
      //set m_Time
      sprintf(m_Time,"%02d%02d%02d",
      tmThis.tm_hour,
      tmThis.tm_min,
      tmThis.tm_sec);
    
      m_WeekDay='0';
      return;
    }
    
    throw "timer convert error";
}

long CDateTime::operator-(const CDateTime &s2) const
{
    time_t nTime1,nTime2;
    struct tm tmVal;
    int nYear,nMonth,nDay,nHour,nMinute,nSecend;
    sscanf(m_Date, "%04d%02d%02d", &nYear, &nMonth , &nDay   );
    sscanf(m_Time, "%02d%02d%02d", &nHour, &nMinute, &nSecend);

    tmVal.tm_year  = nYear   - 1900;
    tmVal.tm_mon   = nMonth  -    1;
    tmVal.tm_mday  = nDay      ;
    tmVal.tm_hour  = nHour     ;
    tmVal.tm_min   = nMinute     ;
    tmVal.tm_sec   = nSecend     ;
    tmVal.tm_isdst = -1;
    nTime1 = mktime(&tmVal);
    
    sscanf(s2.m_Date, "%04d%02d%02d", &nYear, &nMonth , &nDay   );
    sscanf(s2.m_Time, "%02d%02d%02d", &nHour, &nMinute, &nSecend);

    tmVal.tm_year  = nYear   - 1900;
    tmVal.tm_mon   = nMonth  -    1;
    tmVal.tm_mday  = nDay      ;
    tmVal.tm_hour  = nHour     ;
    tmVal.tm_min   = nMinute     ;
    tmVal.tm_sec   = nSecend     ;
    tmVal.tm_isdst = -1;
    nTime2 = mktime(&tmVal);

    return (long)float(difftime(nTime1, nTime2));
}


bool CDateTime::operator==(const CDateTime &tp) const
{
    return (strcmp(m_Date, tp.m_Date) == 0 && strcmp(m_Time, tp.m_Time) == 0);
}


bool CDateTime::operator < (const CDateTime&  tp) const
{
    int rtn = strcmp(m_Date, tp.m_Date);
    if( rtn < 0 ){
        return true;
    }
    else if( rtn > 0 ){
        return false;
    }
    else {
        return strcmp(m_Time, tp.m_Time) < 0;
    }
}

bool CDateTime::operator <= (const CDateTime&  tp) const
{
    int rtn = strcmp(m_Date, tp.m_Date);
    if( rtn < 0 ){
        return true;
    }
    else if( rtn > 0 ){
        return false;
    }
    else {
        return strcmp(m_Time, tp.m_Time) <= 0;
    }
}

bool CDateTime::operator > (const CDateTime&  tp) const
{
    int rtn = strcmp(m_Date, tp.m_Date);
    if( rtn > 0 ){
        return true;
    }
    else if( rtn < 0 ){
        return false;
    }
    else {
        return strcmp(m_Time, tp.m_Time) > 0;
    }
}

bool CDateTime::operator >= (const CDateTime&  tp) const
{
    int rtn = strcmp(m_Date, tp.m_Date);
    if( rtn > 0 ){
        return true;
    }
    else if( rtn < 0 ){
        return false;
    }
    else {
        return strcmp(m_Time, tp.m_Time) >= 0;
    }
}


CDateTime& CDateTime::operator = (const CDateTime& tp)
{
    strcpy(this->m_Date,tp.m_Date);
    strcpy(this->m_Time,tp.m_Time);
    this->m_WeekDay = tp.m_WeekDay;
    return(*this);
}

//ostream& operator << (ostream& os,const CDateTime& tp)
//{
//    os << tp.m_Date << tp.m_Time;
//    return os;
//}

/*------------------------------------------------------ */
/*
** Purpose:
**      add secs from st to dt formed as 'HHMMSS'
**
** Parameter:
**      dt  ---destination string
**      st  ---source string
**      secs---added seconds
** Return:
**       >=0:Done correctly
**      -1  :something error
*/          
int CDateTime::timeadd(char *dt,char *st,long secs)
{
    int h,m,s,ss;
    char tmp[10];

    memcpy(tmp,st,2);
    tmp[2]=0;
    h=atoi(tmp);

    memcpy(tmp,st+2,2);
    tmp[2]=0;
    m=atoi(tmp);

    memcpy(tmp,st+4,2);
    tmp[2]=0;
    s=atoi(tmp);

    h+=secs/3600;
    m+=(secs%3600)/60;
    s+=secs%60;

    ss=s/60;
    s%=60;
    m+=ss;
    ss=m/60;
    m%=60;
    h+=ss;
    h%=24;

    return sprintf(dt,"%02d%02d%02d",h,m,s);
}

/*------------------------------------------------------ */
/*
** Name:
**      int dayadd(char *d,char *s,long n)
** Purpose:
**      add n days from s to d formed as 'YYYYMMDD'
**
** Parameter:
**      d  ---destination string
**      s  ---source string
**      n  ---added seconds
** Return:
**       >=0:Done correctly
**      -1:something error
*/          
 
int CDateTime::dayadd(char *d,char *s,long n)
{
    char tmp[10];
    time_t now,ret;
    struct tm when;

    time(&now);
    when=*localtime(&now);
    
    memcpy(tmp,s,4);
    tmp[4]=0;
    when.tm_year=atoi(tmp)-1900;
    memcpy(tmp,s+4,2);
    tmp[2]=0;
    when.tm_mon=atoi(tmp)-1;
    memcpy(tmp,s+6,2);
    tmp[2]=0;
    when.tm_mday=atoi(tmp)+n;
    when.tm_isdst=0;

    if((ret=mktime(&when))!=(time_t)-1)
    {
        sprintf(d,"%04d%02d%02d",when.tm_year+1900,when.tm_mon+1,when.tm_mday);
    }
    return ret;
}


/*------------------------------------------------------ */
/*
** Purpose:
**      add secs seconds from s_date,s_time to d_date,d_time
**      the return value formed as 'YYYYMMDD","HHMMSS' 
**		the input value formed as 'YYYYMMDD","HHMMSS' ,or 'YYMMDD","HHMMSS' 
** Parameter:
**      d_date  ---destination date string
**      d_time  ---destination time string
**      s_date  ---source date string
**      s_time  ---source time string
**      secs    ---added seconds
** Return:
**       >=0:Done correctly
**      -1  :something error
*/          

int CDateTime::datetimeadd(const char *s_date, const char *s_time,long secs)
{
    char * d_date = str_date;
    char * d_time = str_time;
    char	tmp[10];
    time_t	now,ret;
    struct	tm when;
    char	s[8+6+1];
    
    memset(s,0,sizeof(s));
    strcpy(s,s_date);
    strcat(s,s_time);

    time(&now);
    memcpy(tmp,s,4);
    tmp[4]=0;
    when.tm_year=atoi(tmp)-1900;
    memcpy(tmp,s+4,2);
    tmp[2]=0;
    when.tm_mon=atoi(tmp)-1;
    memcpy(tmp,s+6,2);
    tmp[2]=0;
    when.tm_mday=atoi(tmp);
    memcpy(tmp,s+8,2);
    tmp[2]=0;
    when.tm_hour=atoi(tmp);
    memcpy(tmp,s+10,2);
    tmp[2]=0;
    when.tm_min=atoi(tmp);
    memcpy(tmp,s+12,2);
    tmp[2]=0;
    when.tm_sec=atoi(tmp)+secs;
    /*先得到tm_isdst值*/	
    if((ret=mktime(&when))==(time_t)-1)
    {
	//printf("mktimeerror\n");
    }

    memcpy(tmp,s,4);
    tmp[4]=0;
    when.tm_year=atoi(tmp)-1900;
    memcpy(tmp,s+4,2);
    tmp[2]=0;
    when.tm_mon=atoi(tmp)-1;
    memcpy(tmp,s+6,2);
    tmp[2]=0;
    when.tm_mday=atoi(tmp);
    memcpy(tmp,s+8,2);
    tmp[2]=0;
    when.tm_hour=atoi(tmp);
    memcpy(tmp,s+10,2);
    tmp[2]=0;
    when.tm_min=atoi(tmp);
    memcpy(tmp,s+12,2);
    tmp[2]=0;
    when.tm_sec=atoi(tmp)+secs;
    //when.tm_isdst = 1;


    if((ret=mktime(&when))!=(time_t)-1)
    {
    
        //printf("when.tm_isdst is %d\n",when.tm_isdst);
        sprintf(d_date,"%04d%02d%02d",
                when.tm_year+1900,when.tm_mon+1,when.tm_mday);
        sprintf(d_time,"%02d%02d%02d",
                when.tm_hour,when.tm_min,when.tm_sec);
    }

    return ret;
}

int CDateTime::datetimeadd(const char * src_datetime, long secs)
{
    char * d_date = str_date;
    char * d_time = str_time;
    char	tmp[10];
    time_t	now,ret;
    struct	tm when;
    char	s[8+6+1];
    
    strcpy(s,src_datetime);

    time(&now);
    memcpy(tmp,s,4);
    tmp[4]=0;
    when.tm_year=atoi(tmp)-1900;
    memcpy(tmp,s+4,2);
    tmp[2]=0;
    when.tm_mon=atoi(tmp)-1;
    memcpy(tmp,s+6,2);
    tmp[2]=0;
    when.tm_mday=atoi(tmp);
    memcpy(tmp,s+8,2);
    tmp[2]=0;
    when.tm_hour=atoi(tmp);
    memcpy(tmp,s+10,2);
    tmp[2]=0;
    when.tm_min=atoi(tmp);
    memcpy(tmp,s+12,2);
    tmp[2]=0;
    when.tm_sec=atoi(tmp)+secs;
    /*先得到tm_isdst值*/	
    if((ret=mktime(&when))==(time_t)-1)
    {
	//printf("mktimeerror\n");
    }

    memcpy(tmp,s,4);
    tmp[4]=0;
    when.tm_year=atoi(tmp)-1900;
    memcpy(tmp,s+4,2);
    tmp[2]=0;
    when.tm_mon=atoi(tmp)-1;
    memcpy(tmp,s+6,2);
    tmp[2]=0;
    when.tm_mday=atoi(tmp);
    memcpy(tmp,s+8,2);
    tmp[2]=0;
    when.tm_hour=atoi(tmp);
    memcpy(tmp,s+10,2);
    tmp[2]=0;
    when.tm_min=atoi(tmp);
    memcpy(tmp,s+12,2);
    tmp[2]=0;
    when.tm_sec=atoi(tmp)+secs;
    //when.tm_isdst = 1;


    if((ret=mktime(&when))!=(time_t)-1)
    {
    
        //printf("when.tm_isdst is %d\n",when.tm_isdst);
        sprintf(d_date,"%04d%02d%02d",
                when.tm_year+1900,when.tm_mon+1,when.tm_mday);
        sprintf(d_time,"%02d%02d%02d",
                when.tm_hour,when.tm_min,when.tm_sec);
    }

    return ret;
}


/*-------------------------------------------------------*/
/*
** Name:
**      long diffday(char *dt1,char *dt2)
** Purpose:
**      caculate the minus of date d and date s
**      date format:YYYYMMDD
** Parameter:
**      dt2  ---subtrahend
**      dt1  ---minuend
** Return:
**       dt2-dt1 days
*/          

long CDateTime::diffday(char *dt1,char *dt2)
{
    struct tm t;
    time_t t1,t2;
    char tmp[10];

    memcpy(tmp,dt1,4);      tmp[4]=0;
    t.tm_year=atoi(tmp)-1900;
    memcpy(tmp,dt1+4,2);    tmp[2]=0;
    t.tm_mon=atoi(tmp)-1;
    memcpy(tmp,dt1+6,2);    tmp[2]=0;
    t.tm_mday=atoi(tmp);
    t.tm_hour=0;
    t.tm_min=0;
    t.tm_sec=0;
    t.tm_isdst = 0;
    t1=mktime(&t);

    memcpy(tmp,dt2,4);      tmp[4]=0;
    t.tm_year=atoi(tmp)-1900;
    memcpy(tmp,dt2+4,2);    tmp[2]=0;
    t.tm_mon=atoi(tmp)-1;
    memcpy(tmp,dt2+6,2);    tmp[2]=0;
    t.tm_mday=atoi(tmp);
    t.tm_hour=0;
    t.tm_min=0;
    t.tm_sec=0;
    t.tm_isdst = 0;
    t2=mktime(&t);

    return (long)difftime(t2,t1)/(3600*24);
}




/*-------------------------------------------------------- */
/*
** Name:
**      long datetimediff(char *dt1,char *dt2)
** Purpose:
**      Compute the duration of 
**      two time strings formed as 'yyyymmddhhmmss'
** Parameter:
**      dt2  ---subtrahend
**      dt1  ---minuend
** Return:
**       dt2-dt1 seconds
*/          
long CDateTime::datetimediff(const char *dt1, const char *dt2)
{
    struct tm t;
    time_t t1,t2;
    char tmp[10];

    memcpy(tmp,dt1,4);      tmp[4]=0;
    t.tm_year=atoi(tmp)-1900;
    memcpy(tmp,dt1+4,2);    tmp[2]=0;
    t.tm_mon=atoi(tmp)-1;
    memcpy(tmp,dt1+6,2);    tmp[2]=0;
    t.tm_mday=atoi(tmp);
    memcpy(tmp,dt1+8,2); tmp[2]=0;
    t.tm_hour=atoi(tmp);
    memcpy(tmp,dt1+10,2); tmp[2]=0;
    t.tm_min=atoi(tmp);
    memcpy(tmp,dt1+12,2); tmp[2]=0;
    t.tm_sec=atoi(tmp);
    t.tm_isdst=0;  
    t1=mktime(&t);

    memcpy(tmp,dt2,4);      tmp[4]=0;
    t.tm_year=atoi(tmp)-1900;
    memcpy(tmp,dt2+4,2);    tmp[2]=0;
    t.tm_mon=atoi(tmp)-1;
    memcpy(tmp,dt2+6,2);    tmp[2]=0;
    t.tm_mday=atoi(tmp);
    memcpy(tmp,dt2+8,2); tmp[2]=0;
    t.tm_hour=atoi(tmp);
    memcpy(tmp,dt2+10,2); tmp[2]=0;
    t.tm_min=atoi(tmp);
    memcpy(tmp,dt2+12,2); tmp[2]=0;
    t.tm_sec=atoi(tmp);
    t.tm_isdst=0;  
    t2=mktime(&t);

    return (long)difftime(t2,t1);
}

/*-------------------------------------------------------- */
/*
** Name:
**      long timediff(char *dt1,char *dt2)
** Purpose:
**      Compute the duration of 
**      two time strings formed as 'hhmmss'
** Parameter:
**      dt2  ---subtrahend
**      dt1  ---minuend
** Return:
**       dt2-dt1 seconds
*/          
long CDateTime::timediff(char *dt1,char *dt2)
{
    char tmp[10];
    int hh1,mm1,ss1,hh2,mm2,ss2;
    long rtn;
    
    memcpy(tmp,dt1,2);      tmp[2]=0;
    hh1 = atoi(tmp);
    
    memcpy(tmp,dt1+2,2);    tmp[2]=0;
    mm1 = atoi(tmp);
    
    memcpy(tmp,dt1+4,2);    tmp[2]=0;
    ss1 = atoi(tmp);
    
    memcpy(tmp,dt2,2);      tmp[2]=0;
    hh2 = atoi(tmp);
    
    memcpy(tmp,dt2+2,2);    tmp[2]=0;
    mm2 = atoi(tmp);
    
    memcpy(tmp,dt2+4,2);    tmp[2]=0;
    ss2 = atoi(tmp);
    
    rtn = (long)((hh2-hh1)*3600+(mm2-mm1)*60+(ss2-ss1));
    if(rtn<0) rtn+=86400;
	return rtn;
}

/*-------------------------------------------------------- */
/*
** Name:
**      int getwday(char *timestr)
** Purpose:
**      get the week day of time string 
**      timestr form like 'yyyymmdd%' 
** Parameter:
**      timestr ---time string
** Return:
**      week day(1-7)
*/          
int CDateTime::getwday(char *timestr)
{
    struct tm t;
    char tmp[10];

    memcpy(tmp,timestr,4);  tmp[4]=0;
    t.tm_year=atoi(tmp)-1900;
    memcpy(tmp,timestr+4,2);        tmp[2]=0;
    t.tm_mon=atoi(tmp)-1;
    memcpy(tmp,timestr+6,2);        tmp[2]=0;
    t.tm_mday=atoi(tmp);
    t.tm_hour=0;
    t.tm_min=0;
    t.tm_sec=0;
    t.tm_isdst=0;

    if(mktime(&t)==(time_t)-1) return -1;
    else if(t.tm_wday==0) return 7;
    else return t.tm_wday;
}

/*-------------------------------------------------------- */
/*
** Name:
**      double changeprecision(double v,int n)
** Purpose:
**      change the dot of v to n bits 
**      eg. v=123,n=2,then v = 123.00 
** Parameter:
**      v ---value
**      n ---the position of dot 
** Return:
**      =v
*/          
double CDateTime::changeprecision(double v,int n)
{
    int i;
    int tmp=1;
    int t;
    
    for(i=0;i<n;i++)
      tmp = tmp*10;
    
    v = v*tmp;
    t = (int)v;
    
    if((v-t)>0.500) v = v+1;
   
    t = (int)v;
    
    v = (double)t/tmp;
    
    return v;
}       

/*-------------------------------------------------------- */
/*
** Name:
**      double setprecision(int v,int n)
** Purpose:
**      set the dot of v to n bits 
**      eg. v=123,n=2,then v = 1.23 
** Parameter:
**      v ---value
**      n ---the position of dot 
** Return:
**      =v
*/          

double CDateTime::setprecision(int v,int n)
{
    int i;
    int tmp=1;
    
    for(i=0;i<n;i++)
      tmp = tmp*10;
    
    return (double)v/tmp;
    
    
}       


/*-------------------------------------------------------- */
/*
** Name:
**      int timevalidation(char *t)
** Purpose:
**      validate the time string,format:HHMMSS
** Parameter:
**      t ---time string
** Return:
**      =0:error
**       1:correct time   
*/
int CDateTime::timevalidation(char *t)
{
    if(strlen(t)<6||strlen(t)>6)
       return 0;

    if(strcmp(t,"240000")>0)
            return 0;
    if(strncmp(t+2,"6000",4)>0)
            return 0;

    if(strncmp(t+4,"60",2)>0)
            return 0;
   
    return 1;
}

long CDateTime::time_to_second(char *s)
{
	char tmp[10];
	long ret;

	ret=0;
	memcpy(tmp,s,2);
	tmp[2]=0;
	ret=3600*atoi(tmp);
	memcpy(tmp,s+2,2);
	tmp[2]=0;
	ret+=60*atoi(tmp);
	memcpy(tmp,s+4,2);
	tmp[2]=0;
	ret+=atoi(tmp);

	return ret;
}

void CDateTime::reset()
{
	time_t timer;
	struct tm *today;
	timer = time(NULL);
	today = localtime(&timer);
	sprintf(str_date, "%04d%02d%02d",today->tm_year + 1900,today->tm_mon+1,today->tm_mday);
	sprintf(str_time, "%02d%02d%02d",today->tm_hour,today->tm_min,today->tm_sec);
}


const char * CDateTime::date_time()
{
    strcpy(str_datetime, str_date);
    strcat(str_datetime, str_time);
    return str_datetime;
}

const char * CDateTime::getDateTime()
{
    strcpy(str_datetime, CDateTime::str_date);
    strcat(str_datetime, CDateTime::str_time);
    return str_datetime;
}

bool CDateTime::check_date(const char *pchString)
{
	int num;
	char tmp[10];
	int iYear,iMonth,i;

	if(strlen(pchString) != 8)
		return false;
	int iLen = strlen(pchString);
	for (i = 0; i < iLen; i++)
	{
		if ((pchString[i] < '0') || (pchString[i] > '9'))
			return false;
	}

	strncpy(tmp , pchString , 4);
	tmp[4] = 0;
	num = atoi(tmp);
	iYear = num;
	if((num < 1900) || (num > 2099))
		return false;
	strncpy(tmp , pchString + 4 , 2);
	tmp[2] = 0;
	num = atoi(tmp);
	iMonth = num;
	if((num < 1) || (num > 12))
		return false;
	strncpy(tmp , pchString + 6 , 2);
	tmp[2] = 0;
	num = atoi(tmp);
	if((num < 1) || (num > 31))
		return false;
	switch(iMonth)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		if((num < 1) || (num > 31))
			return false;
		break;
	case 2:
		if((iYear%4==0&&iYear%100!=0)||iYear%400==0) //judge leap year
		{
			if((num < 1) || (num > 29))
				return false;
		}
		else
		{
			if((num < 1) || (num > 28))
				return false;
		}
		break;
	default :
		if((num < 1) || (num > 30))
			return false;
	}
	return true;
}

bool CDateTime::check_time(const char *pchString)
{
	int num,i;
	char tmp[10];

	if(strlen(pchString) != 6)
		return false;
	int iLen = strlen(pchString);
	for (i=0;i<iLen;i++)
	{
		if ((pchString[i]<'0')||(pchString[i]>'9'))
			return false;
	}
	strncpy(tmp , pchString , 2);
	tmp[2] = 0;
	num = atoi(tmp);
	if((num < 0) || (num > 23))
		return false;
	strncpy(tmp , pchString + 2 , 2);
	tmp[2] = 0;
	num = atoi(tmp);
	if((num < 0) || (num > 59))
		return false;
	strncpy(tmp , pchString + 4 , 2);
	tmp[2] = 0;
	num = atoi(tmp);
	if((num < 0) || (num > 59))
		return false;

	return true;
}


const char * CDateTime::addTime(const char * inTime, long nSec)
{
    int h,m,s,ss;                   //ss标识进位
    char tmp[8];                    //临时缓冲

    memcpy(tmp, inTime, 2);
    tmp[2] = 0;
    h=atoi(tmp);                    //当前的小时

    memcpy(tmp,inTime+2,2);
    tmp[2]=0;
    m=atoi(tmp);                    //当前的分钟

    memcpy(tmp,inTime+4,2);         
    tmp[2]=0;
    s=atoi(tmp);                    //当前的秒
    
    h += nSec / 3600;
    m += (nSec % 3600) / 60;
    s +=  nSec % 60;

    ss = s/60;                      //秒进位
    s%=60;
    m+=ss;
    ss=m/60;
    m%=60;
    h+=ss;
    h%=24;

    sprintf(str_time, "%02d%02d%02d", h, m, s);
    
    strcpy(str_time + 6, inTime + 6);
    return str_time;
}

long CDateTime::diffTime(const char * time1, const char * time2)
{
    char tmp[8];
    int hh1,mm1,ss1,hh2,mm2,ss2;    //时分秒
    int ttt1,ttt2;                  //微秒
    long rtn;
    
    memcpy(tmp,time1,2);      tmp[2]=0;
    hh1 = atoi(tmp);
    
    memcpy(tmp,time1+2,2);    tmp[2]=0;
    mm1 = atoi(tmp);
    
    memcpy(tmp,time1+4,2);    tmp[2]=0;
    ss1 = atoi(tmp);
    
    strcpy(tmp, time1 + 6);
    ttt1 = static_cast<int>(atoi(tmp) * pow(10.0, (int)(3 - strlen(tmp))));
    
    memcpy(tmp,time2,2);      tmp[2]=0;
    hh2 = atoi(tmp);
    
    memcpy(tmp,time2+2,2);    tmp[2]=0;
    mm2 = atoi(tmp);
    
    memcpy(tmp,time2+4,2);    tmp[2]=0;
    ss2 = atoi(tmp);
    
    strcpy(tmp, time2 + 6);
    ttt2 = static_cast<int>(atoi(tmp) * pow(10.0, (int)(3 - strlen(tmp))));
    
    rtn = (long)((hh2-hh1)*3600+(mm2-mm1)*60+(ss2-ss1));
    
    
    int delta_t = ttt2 - ttt1;              //计算毫秒差
    if( delta_t >= 0 )
        delta_t = (delta_t + 500) / 1000;
    else
        delta_t = (delta_t - 500) / 1000;
    
    rtn += delta_t;
    
    if(rtn<0) rtn+=86400;
    
	return rtn;
}

long CDateTime::convertTimeToSecond(const char * inTime)
{
    char tmp[8];
	long ret;

	ret=0;
	memcpy(tmp,inTime,2);
	tmp[2]=0;
	ret=3600*atoi(tmp);
	memcpy(tmp,inTime+2,2);
	tmp[2]=0;
	ret+=60*atoi(tmp);
	memcpy(tmp,inTime+4,2);
	tmp[2]=0;
	ret+=atoi(tmp);
	
    strcpy(tmp, inTime + 6);
    ret += static_cast<long>((atoi(tmp) * pow(10.0, (int)(3 - strlen(tmp))) + 500) / 1000);
	return ret;
}

/*把日期转换成格式化形式 类似ORACLE的to_char()
失败则返回NULL
*/
const char * CDateTime::to_char(char *Time,time_t tm,const char *fmt)
{
#ifndef EscChar
#define EscChar '\\'
#endif
#define LEN_TIME_STR	64
static char MonTabAbr[12][4]={
	"Jan","Feb","Mar","Apr","May","Jun",
	"Jul","Aug","Sep","Oct","Nov","Dec"
};
static char MonTab[12][10]={
	"January","February","March","April","May","June",
	"July","August","September","October","November","December"
};
static char DayTabAbr[12][4]={
	"Sun","Mon","Tue","Wed","Thu","Fri","Sat"
};
static char DayTab[12][10]={
	"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"
};
time_t now;
struct tm *stpTime;
char str[16];
int i,j;

	if(tm==NULL)	/* SYSDATE */
		now=time(NULL);
	else
		now=tm;
	stpTime=localtime(&now);
	for(i=0,j=0;fmt[i]!='\0'&&j<LEN_TIME_STR-10;){
		if(fmt[i]==EscChar){
			Time[j]=fmt[i+1];
			i+=2;	j++;
		}
		else if(toupper(fmt[i])=='D'){
			if(toupper(fmt[i+1])=='D'){
				if(toupper(fmt[i+2])=='D'){	/* 'DDD' day of year(1-366).*/
					sprintf(str,"%03d",stpTime->tm_yday+1);
					strncpy(&Time[j],str,3);
					i+=3;	j+=3;
				}
				else{	/* 'DD' day of month(1-31).*/
					sprintf(str,"%02d",stpTime->tm_mday);
					strncpy(&Time[j],str,2);
					i+=2;	j+=2;
				}
			}
			else if(toupper(fmt[i+1])=='A'&&toupper(fmt[i+2])=='Y'){/* 'DAY' name of day.*/
					sprintf(str,"%s",DayTab[stpTime->tm_wday]);
					strncpy(&Time[j],str,strlen(str));
					i+=3;	j+=strlen(str);
			}
			else if(toupper(fmt[i+1])=='Y'){	/* 'DY' abbreviated name of day.*/
					sprintf(str,"%s",DayTabAbr[stpTime->tm_wday]);
					strncpy(&Time[j],str,strlen(str));
					i+=2;	j+=strlen(str);
			}
			else{	/* 'D' day of week(1-7).*/
				Time[j]=stpTime->tm_wday+1+'0';
				i++;	j++;
			}
		}
		else if(toupper(fmt[i])=='H'&&toupper(fmt[i+1])=='H'){
			if(toupper(fmt[i+2])=='1'&&toupper(fmt[i+3])=='2'){	/* 'HH12' hour of day(1-12).*/
				sprintf(str,"%02d",(stpTime->tm_hour%12==0 ? 12 : stpTime->tm_hour%12));
				strncpy(&Time[j],str,2);
				i+=4;	j+=2;
			}
			else if(toupper(fmt[i+2])=='2'&&toupper(fmt[i+3])=='4'){/* 'HH24' hour of day(0-23).*/
				sprintf(str,"%02d",stpTime->tm_hour);
				strncpy(&Time[j],str,2);
				i+=4;	j+=2;
			}
			else{	/* 'HH' hour of day(1-12)*/
				sprintf(str,"%02d",(stpTime->tm_hour%12==0 ? 12 : stpTime->tm_hour%12));
				strncpy(&Time[j],str,2);
				i+=2;	j+=2;
			}
		}
		else if(toupper(fmt[i])=='M'){
			if(toupper(fmt[i+1])=='I'){	/* 'MI' minute(0-59).*/
				sprintf(str,"%02d",stpTime->tm_min);
				strncpy(&Time[j],str,2);
				i+=2;	j+=2;
			}
			else if(toupper(fmt[i+1])=='M'){	/* 'MM' month(01-12).*/
				sprintf(str,"%02d",stpTime->tm_mon+1);
				strncpy(&Time[j],str,2);
				i+=2;	j+=2;
			}
			else if(toupper(fmt[i+1])=='O'&&toupper(fmt[i+2])=='N'&&toupper(fmt[i+3])=='T'&&
				toupper(fmt[i+4])=='H'){
				sprintf(str,"%s",MonTab[stpTime->tm_mon]);
				strncpy(&Time[j],str,strlen(str));
				i+=5;	j+=strlen(str);
			}
			else if(toupper(fmt[i+1])=='O'&&toupper(fmt[i+2])=='N'){
				sprintf(str,"%s",MonTabAbr[stpTime->tm_mon]);
				strncpy(&Time[j],str,strlen(str));
				i+=3;	j+=strlen(str);
			}
			else{
				Time[j]=fmt[i];
				i++;	j++;
			}
		}
		else if(toupper(fmt[i])=='W'){
			if(toupper(fmt[i+1])=='W'){	/* 'WW' week of year(1-53) where week 1
					starts on the first day of the year and continues to the
					seventh day of the year.*/
				sprintf(str,"%02d",(stpTime->tm_yday+1)/7+1);
				strncpy(&Time[j],str,2);
				i+=2;	j+=2;
			}
			else{		/* 'W' week of month(1-5) where week 1 starts on the
					first day of the month and ends on the seventh.*/
				sprintf(str,"%d",stpTime->tm_mday/7+1);
				Time[j]=str[0];
				i++;	j++;
			}
		}
		else if(toupper(fmt[i])=='S'){
			if(toupper(fmt[i+1])=='S'){	/* 'SS' second(0-59).*/
				sprintf(str,"%02d",stpTime->tm_sec);
				strncpy(&Time[j],str,2);
				i+=2;	j+=2;
			}
			else{
				Time[j]=fmt[i];
				i++;	j++;
			}
		}
		else if(toupper(fmt[i])=='Y'){
			sprintf(str,"%04d",stpTime->tm_year+1900);
			if(toupper(fmt[i+1])=='Y'){
				if(toupper(fmt[i+2])=='Y'){
					if(toupper(fmt[i+3])=='Y'){	/* 'YYYY' */
						strncpy(&Time[j],str,4);
						i+=4;	j+=4;
					}
					else{	/* 'YYY' */
						strncpy(&Time[j],&str[1],3);
						i+=3;	j+=3;
					}
				}
				else{	/* 'YY' */
					strncpy(&Time[j],&str[2],2);
					i+=2;	j+=2;
				}
			}
			else{	/* 'Y' */
				Time[j]=str[3];
				i++;	j++;
			}
		}
		else{
			Time[j]=fmt[i];
			i++;	j++;
		}
	}
	if(j==(LEN_TIME_STR-10)) return(NULL);	/*Internal buffer overflowed.*/
	Time[j]='\0';
	return Time;
}

#endif  /* CDATETIME_CPP_INCLUDED */





