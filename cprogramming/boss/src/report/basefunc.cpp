#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
//#include <sys/vfs.h>
#include "basefunc.h"

int getcdate(char * date)/* get date string formed as 'YYYYMMDD' */
{
  time_t now;
  struct tm  *w;
  time(&now);
  w=localtime(&now);
  return sprintf(date,"%04d%02d%02d",w->tm_year+1900,w->tm_mon+1,w->tm_mday);
}

int getcmonth(char * month)/* get date string formed as 'YYYYMM' */
{
  time_t now;
  struct tm  *w;
  time(&now);
  w=localtime(&now);
  return sprintf(month,"%04d%02d",w->tm_year+1900,w->tm_mon+1);
}

int getclastmonth(char * month)/* get date string formed as 'YYYYMM' */
{
  time_t now;
  struct tm  *w;
  time(&now);
  w=localtime(&now);
  if (w->tm_mon == 0)
    return sprintf(month,"%04d%02d",w->tm_year+1899,12);
  else
    return sprintf(month,"%04d%02d",w->tm_year+1900,w->tm_mon);
}

long getltime()/* get long time formed as n minutes */
{
  time_t now;
  struct tm  *w;
  char cTemp[5];
  long lTemp;
  time(&now);
  w=localtime(&now);
  sprintf(cTemp,"%02d%02d",w->tm_hour,w->tm_min);
  lTemp = atol(cTemp);
  return (long)((lTemp/100)*60+lTemp%100);
}

int dayadd(char *d,char *s,long n)/* add n from s to d formed as 'YYYYMMDD' */
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
    when.tm_isdst = 0;  // please attention 
  if((ret=mktime(&when))!=(time_t)-1)
  {
    sprintf(d,"%04d%02d%02d",when.tm_year+1900,when.tm_mon+1,when.tm_mday);
  }
  return ret;
}

int CheckDate(long date)
{
  long iYear = (long)(date/10000);
  long iMonth = (long)((date-long(date/10000)*10000)/100);
  long iDay = (long)(date%100);

  if ( iYear>99 )
    if ( (iYear>2200) || (iYear<1950) ) return -1;
  else
    if ( iYear<50 ) return -1;

  if ( (iMonth >12) || (iMonth <1) ) return -1;
  if ( (   iDay>31) || (iDay   <1) ) return -1;

  switch (iMonth)
  {
  case 2:
    if (CheckLeapYear(iYear))
      if (iDay>29) return -1;
    else
      if (iDay>28) return -1;
    break;
  case 4:
    if (iDay>30) return -1;
    break;
  case 6:
    if (iDay>30) return -1;
    break;
  case 9:
    if (iDay>30) return -1;
    break;
  case 11:
    if (iDay>30) return -1;
    break;
  }
  return 1;
}

bool CheckLeapYear(long iYear)
{
  bool bLeap;
  if (iYear%4==0) {
    if (iYear%100==0) {
      if (iYear%400==0)
        bLeap=true;
      else
        bLeap=false;
    }
    else
      bLeap=true;
  }
  else
    bLeap=false;
  return bLeap;
}

int CheckTime(long time)//检查时间参数是否错误,返回:"1",正确，"0"错误
{  
  long temp;
  if ((int(time/100)<0)||(int(time/100)>23))
     return -1;
  temp=time-int(time/100)*100;
  if ((temp<0)||(temp>=60))
    return -1;
  
  return 1;
}

long TimeDecrease(long btime,long etime)
{
   long b,e,m;
   b=long(btime/100);
   e=long(etime/100);
   m=(b*60+btime-b*100)-(e*60+etime-e*100);
   return long(m/60)*100+(m-long(m/60)*60);
}

long getTimeDifference(long btime,long etime)
{
  long timedifference;
  if(btime<=etime)
    timedifference=(int(etime/100)*60+(etime-int(etime/100)*100))-
             (int(btime/100)*60+(btime-int(btime/100)*100));  
  else
    timedifference=(int(etime/100)*60+(etime-int(etime/100)*100))+1440-
             (int(btime/100)*60+(btime-int(btime/100)*100));  
  return timedifference;
}

long timediff(char *dt1,char *dt2)
{
    struct tm t;
    time_t t1,t2;
    char tmp[10],tmp1[3];
    sprintf(tmp1,"00");

    memcpy(tmp,dt1,4 );    tmp[4]=0;
    t.tm_year=atoi(tmp)-1900;
    memcpy(tmp,dt1+4,2);  tmp[2]=0;
    t.tm_mon=atoi(tmp)-1;
    memcpy(tmp,dt1+6,2);  tmp[2]=0;
    t.tm_mday=atoi(tmp);
    memcpy(tmp,dt1+8,2); tmp[2]=0;
    t.tm_hour=atoi(tmp1);
    memcpy(tmp,dt1+10,2); tmp[2]=0;
    t.tm_min=atoi(tmp1);
    memcpy(tmp,dt1+12,2); tmp[2]=0;
    t.tm_sec=atoi(tmp1);
    t1=mktime(&t);

    memcpy(tmp,dt2,4);    tmp[4]=0;
    t.tm_year=atoi(tmp)-1900;
    memcpy(tmp,dt2+4,2);  tmp[2]=0;
    t.tm_mon=atoi(tmp)-1;
    memcpy(tmp,dt2+6,2);  tmp[2]=0;
    t.tm_mday=atoi(tmp);
    memcpy(tmp,dt2+8,2); tmp[2]=0;
    t.tm_hour=atoi(tmp1);
    memcpy(tmp,dt2+10,2); tmp[2]=0;
    t.tm_min=atoi(tmp1);
    memcpy(tmp,dt2+12,2); tmp[2]=0;
    t.tm_sec=atoi(tmp1);
    t2=mktime(&t);

    return (long)(difftime(t1,t2)/(3600*24));
}
