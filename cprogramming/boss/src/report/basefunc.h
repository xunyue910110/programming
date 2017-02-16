#ifndef __BASEFUNC_H__
#define __BASEFUNC_H__

int getcdate(char * date);
int getcmonth(char * month);
int getclastmonth(char * month);
long getltime();
int dayadd(char *d,char *s,long n);// add n days from s to d formed as 'yyyymmdd'
int CheckDate(long);//检查日期参数是否错误,返回:"1",正确，"0"错误
int CheckTime(long);//检查时间参数是否错误,返回:"1",正确，"0"错误
long TimeDecrease(long,long);//求两个时间之间的差(HHMI形式):60minute = 100;
long getTimeDifference(long,long);//求两个时间相隔的分钟数
long timediff(char *,char *);//Compute the duration of two time strings formed as 'yyyymmdd'
bool CheckLeapYear(long);//校验是否闰年

#endif /* __BASEFUNC_H__*/
