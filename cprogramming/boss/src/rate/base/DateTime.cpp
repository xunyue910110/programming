#include "DateTime.h"

//时间点类

DateTime::~DateTime()
{
}

DateTime::DateTime()
{
   memset(m_Date,0,sizeof(m_Date));
   memset(m_Time,0,sizeof(m_Time));
   m_WeekDay='0';
}

char *DateTime::getDate() 
{
   return(m_Date);
}

char *DateTime::getTime()
{
   return(m_Time);
}

Integer DateTime::getDateInteger()  const
{
   return atoi(m_Date);
}

Integer DateTime::getTimeInteger() const
{
   return atoi(m_Time);
}

//Added By Sunam 2009/12/29 11:14:59 得到日期时间
T_DATE_TIME DateTime::getDateTime()
{
   T_DATE_TIME dateTime;
   dateTime = m_Date;
   dateTime.append(8,m_Time);
   return(dateTime);
}

void DateTime::set(const char *date,const char *time)
{
   char tmp[10];
   memset(tmp,0,sizeof(tmp));
   strncpy(tmp,date,sizeof(m_Date)-1);
   sprintf(m_Date,"%08s",tmp);
   memset(tmp,0,sizeof(tmp));
   strncpy(tmp,time,sizeof(m_Time)-1);
   sprintf(m_Time,"%06s",tmp);

   m_WeekDay = '0';
}

void DateTime::setTime(const char *time)
{
    char tmp[10];
   	memset(tmp,0,sizeof(tmp));
   	strncpy(tmp,time,sizeof(m_Time)-1);
   	sprintf(m_Time,"%06s",tmp);

}

void DateTime::setDate(const char *date)
{
   char tmp[10];

   memset(tmp,0,sizeof(tmp));
   strncpy(tmp,date,sizeof(m_Date)-1);
   sprintf(m_Date,"%08s",tmp);
   m_WeekDay = 0;
}


void DateTime::set(const Integer date,const Integer time)
{
    sprintf(m_Date,"%08d",(long)date);
    sprintf(m_Time,"%06d",(long)time);
}

void DateTime::setDate(const Integer date)
{
    sprintf(m_Date,"%08d",(long)date);
}

void DateTime::setTime(const Integer time)
{
    sprintf(m_Time,"%06d",(long)time);
}

int DateTime::getWeekDay()
{
   struct tm t;
   char tmp[10];

   //if(m_WeekDay!='0')
   //   return m_WeekDay;

   memcpy(tmp,m_Date,4);
   tmp[4]=0;
   t.tm_year=atoi(tmp)-1900;

   memcpy(tmp,m_Date+4,2);
   tmp[2]=0;
   t.tm_mon=atoi(tmp)-1;

   memcpy(tmp,m_Date+6,2);
   tmp[2]=0;
   t.tm_mday=atoi(tmp);

   t.tm_hour=0;
   t.tm_min=0;
   t.tm_sec=0;
   t.tm_isdst = -1;

   if(mktime(&t)==(time_t)-1)
      return 0;
   else
   {
      if(t.tm_wday==0)
      {
      	m_WeekDay = 7+'0';
      	return 7;
      }
      else
      {
      	  m_WeekDay =  t.tm_wday+'0';
          return t.tm_wday;
      }
         //return (m_WeekDay-'0');
   }
}

DateTime&  DateTime::nextday()
{
   char tmp[10];
   time_t now,rd;
   struct tm when;

   time(&now);
   when=*localtime(&now);

   //get YEAR
   memcpy(tmp,m_Date,4);
   tmp[4]=0;
   when.tm_year=atoi(tmp)-1900;
   //get Month
   memcpy(tmp,m_Date+4,2);
   tmp[2]=0;
   when.tm_mon=atoi(tmp)-1;
   //get Day
   memcpy(tmp,m_Date+6,2);
   tmp[2]=0;
   when.tm_mday=atoi(tmp)+1;

   when.tm_isdst=-1;

   if((rd=mktime(&when))!=(time_t)-1)
   {
     //set m_Date
     memset(m_Date,0,sizeof(m_Date));

     sprintf(m_Date,"%04d%02d%02d",
             when.tm_year+1900,when.tm_mon+1,when.tm_mday);
     //set m_Time
     strcpy(m_Time,"000000");
     m_WeekDay='0';
     return *this;
   }
   return *this;
}


DateTime&  DateTime::overnight()
{
   char tmp[10];
   time_t now,rd;
   struct tm when;

   time(&now);
   when=*localtime(&now);

   //get YEAR
   memcpy(tmp,m_Date,4);
   tmp[4]=0;
   when.tm_year=atoi(tmp)-1900;
   //get Month
   memcpy(tmp,m_Date+4,2);
   tmp[2]=0;
   when.tm_mon=atoi(tmp)-1;
   //get Day
   memcpy(tmp,m_Date+6,2);
   tmp[2]=0;
   when.tm_mday=atoi(tmp)-1;

   when.tm_isdst=-1;

   if((rd=mktime(&when))!=(time_t)-1)
   {
     //set m_Date
     memset(m_Date,0,sizeof(m_Date));

     sprintf(m_Date,"%04d%02d%02d",
             when.tm_year+1900,when.tm_mon+1,when.tm_mday);
     //set m_Time
     strcpy(m_Time,"240000");
     m_WeekDay='0';
     return *this;
   }
   return *this;
}

DateTime operator+(const DateTime &t1,const long &duration) 
{
   char tmp[10];
   time_t now,rd;
   struct tm when;
   DateTime t2;

   time(&now);
   when=*localtime(&now);

   //get YEAR
   memcpy(tmp,t1.m_Date,4);
   tmp[4]=0;
   when.tm_year=atoi(tmp)-1900;
   //get Month
   memcpy(tmp,t1.m_Date+4,2);
   tmp[2]=0;
   when.tm_mon=atoi(tmp)-1;
   //get Day
   memcpy(tmp,t1.m_Date+6,2);
   tmp[2]=0;
   when.tm_mday=atoi(tmp);

   //get Hour
   memcpy(tmp,t1.m_Time,2);
   tmp[2]=0;
   when.tm_hour=atoi(tmp);
   //get Minute
   memcpy(tmp,t1.m_Time+2,2);
   tmp[2]=0;
   //get Second
   when.tm_min=atoi(tmp);
   memcpy(tmp,t1.m_Time+4,2);
   tmp[2]=0;
   //Add Duration
   when.tm_sec=atoi(tmp)+duration;

   when.tm_isdst=-1;

   if((rd=mktime(&when))!=(time_t)-1)
   {
       //set m_Date
       memset(t2.m_Date,0,sizeof(t2.m_Date));

       sprintf(t2.m_Date,"%04d%02d%02d",
               when.tm_year+1900,when.tm_mon+1,when.tm_mday);
       //set m_Time
       memset(t2.m_Time,0,sizeof(t2.m_Time));
       sprintf(t2.m_Time,"%02d%02d%02d",
               when.tm_hour,when.tm_min,when.tm_sec);
       t2.m_WeekDay='0';
       return t2;
   }
   return t1;

}

long DateTime::operator-(const DateTime &s2) const
{
     struct tm t;
     time_t t1,t2;
     char tmp[10];

     memcpy(tmp,m_Date,4);
     tmp[4]=0;
     t.tm_year=atoi(tmp)-1900;

     memcpy(tmp,m_Date+4,2);
     tmp[2]=0;
     t.tm_mon=atoi(tmp)-1;

     memcpy(tmp,m_Date+6,2);
     tmp[2]=0;
     t.tm_mday=atoi(tmp);

     memcpy(tmp,m_Time,2);
     tmp[2]=0;
     t.tm_hour=atoi(tmp);

     memcpy(tmp,m_Time+2,2);
     tmp[2]=0;
     t.tm_min=atoi(tmp);
     memcpy(tmp,m_Time+4,2);
     tmp[2]=0;
     t.tm_sec=atoi(tmp);
     t.tm_isdst = -1;
     t1=mktime(&t);

     memcpy(tmp,s2.m_Date,4);
     tmp[4]=0;
     t.tm_year=atoi(tmp)-1900;

     memcpy(tmp,s2.m_Date+4,2);
     tmp[2]=0;
     t.tm_mon=atoi(tmp)-1;

     memcpy(tmp,s2.m_Date+6,2);
     tmp[2]=0;
     t.tm_mday=atoi(tmp);

     memcpy(tmp,s2.m_Time,2);
     tmp[2]=0;
     t.tm_hour=atoi(tmp);

     memcpy(tmp,s2.m_Time+2,2);
     tmp[2]=0;
     t.tm_min=atoi(tmp);
     memcpy(tmp,s2.m_Time+4,2);
     tmp[2]=0;
     t.tm_sec=atoi(tmp);
     t.tm_isdst = -1;

     t2=mktime(&t);

     return (long)difftime(t1,t2);


}


bool DateTime::operator==(const DateTime &tp) const
{
   	int nYear,nMonth,nDay,nHour,nMinute,nSecend;
	sscanf(this->m_Date, "%04d%02d%02d", &nYear, &nMonth, &nDay);
	sscanf(this->m_Time, "%02d%02d%02d", &nHour, &nMinute, &nSecend);
	struct tm tmThis;
	tmThis.tm_year  =nYear-1900;
	tmThis.tm_mon   =nMonth-1;
	tmThis.tm_mday  =nDay;
	tmThis.tm_hour  =nHour;
	tmThis.tm_min   =nMinute;
	tmThis.tm_sec   =nSecend;
	tmThis.tm_isdst = -1;

	sscanf(tp.m_Date, "%04d%02d%02d", &nYear, &nMonth, &nDay);
	sscanf(tp.m_Time, "%02d%02d%02d", &nHour, &nMinute, &nSecend);
	struct tm tmThat;
	tmThat.tm_year  =nYear-1900;
	tmThat.tm_mon   =nMonth-1;
	tmThat.tm_mday  =nDay;
	tmThat.tm_hour  =nHour;
	tmThat.tm_min   =nMinute;
	tmThat.tm_sec   =nSecend;
	tmThat.tm_isdst = -1;
	return ( mktime(&tmThis) == mktime(&tmThat) ); // 标准化后比较

//   return((strcmp(this->m_Date,tp.m_Date)==0)&&(strcmp(this->m_Time,tp.m_Time)==0));

}


bool DateTime::operator < (const DateTime&  tp) const
{

    // 2002/8/9 20020809240000 == 20020810000000
	//20020809240000 < 20020810000000 为false
	int nYear,nMonth,nDay,nHour,nMinute,nSecend;
	sscanf(this->m_Date, "%04d%02d%02d", &nYear, &nMonth, &nDay);
	sscanf(this->m_Time, "%02d%02d%02d", &nHour, &nMinute, &nSecend);
	struct tm tmThis;
	tmThis.tm_year  =nYear-1900;
	tmThis.tm_mon   =nMonth-1;
	tmThis.tm_mday  =nDay;
	tmThis.tm_hour  =nHour;
	tmThis.tm_min   =nMinute;
	tmThis.tm_sec   =nSecend;
	tmThis.tm_isdst = -1;

	sscanf(tp.m_Date, "%04d%02d%02d", &nYear, &nMonth, &nDay);
	sscanf(tp.m_Time, "%02d%02d%02d", &nHour, &nMinute, &nSecend);
	struct tm tmThat;
	tmThat.tm_year  =nYear-1900;
	tmThat.tm_mon   =nMonth-1;
	tmThat.tm_mday  =nDay;
	tmThat.tm_hour  =nHour;
	tmThat.tm_min   =nMinute;
	tmThat.tm_sec   =nSecend;
	tmThat.tm_isdst = -1;
	return ( mktime(&tmThis) < mktime(&tmThat) ); // 标准化后比较
	// Add End
   /*
   if(strcmp(this->m_Date,tp.m_Date)<0)
     return true;
   else
   {
     if((strcmp(this->m_Date,tp.m_Date)==0)&& (strcmp(this->m_Time,tp.m_Time)<0))
            return true;

   }
   return false;*/
}

bool DateTime::operator <= (const DateTime&  tp) const
{
	return (this->operator<(tp)) || (this->operator==(tp));
}

bool DateTime::operator > (const DateTime&  tp) const
{
	return ( ! ( this->operator<=(tp) ) );
}

bool DateTime::operator >= (const DateTime&  tp) const
{
	return ( ! ( this->operator<(tp) ) );
}


DateTime& DateTime::operator = (const DateTime& tp)
{
    strcpy(this->m_Date,tp.m_Date);
    strcpy(this->m_Time,tp.m_Time);
    this->m_WeekDay = tp.m_WeekDay;
    return(*this);
}

void DateTime::print()
{
    cout <<"[" <<m_Date <<"-" << m_Time << "  Week:" << m_WeekDay<<"]" << endl;
}

ostream& operator<<(ostream &r_os,const DateTime& r_obj)
{
  r_os<<"[" <<r_obj.m_Date <<"-" << r_obj.m_Time << "  Week:" << r_obj.m_WeekDay<<"]";
  return r_os;
}
