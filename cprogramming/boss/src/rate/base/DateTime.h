#ifndef	DATETIME_H
#define	DATETIME_H

#include <time.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include "FieldsXX.h"
//时间点类
class DateTime
{
  private:
     char m_Date[8+1];
     char m_Time[6+1];
     char m_WeekDay;    /* '1'--'7' */

  public:
     virtual ~DateTime();
     DateTime();

     char *getDate();         //得到日期
     char *getTime();         //得到时间

     Integer getDateInteger() const;
     Integer getTimeInteger() const;
     //Added By Sunam 2009/12/29 11:14:33 得到日期时间
     T_DATE_TIME getDateTime();

     void set(const char *date,const char *time);
     void setTime(const char *time);
     void setDate(const char *date);

     void set(const Integer date,const Integer time);
     void setTime(const Integer time);
     void setDate(const Integer date);

     int getWeekDay();

     DateTime&  nextday();

     DateTime&  overnight();

     friend DateTime operator+(const DateTime &t1,const long &duration);

     long operator-(const DateTime &s2) const;

     bool operator==(const DateTime &tp) const;

     bool operator < (const DateTime&  tp) const;

     bool operator <= (const DateTime&  tp) const;

     bool operator > (const DateTime&  tp) const;

     bool operator >= (const DateTime&  tp) const;

     DateTime& operator = (const DateTime& tp);

     void print();
     friend ostream& operator<<(ostream &r_os,const DateTime& r_obj);
};
#endif  //DateTime
