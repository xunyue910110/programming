/**
 *
 * <p>Title: CDateTime 日期时间处理类</p>
 * <p>Description:</p>
 * <p>Copyright: Copyright (c) 2003</p>
 * <p>Company: Linkage</p>
 * @author wangxr
 * @version 1.0
 */

#ifndef CDATETIME_HEADER_INCLUDED
#define CDATETIME_HEADER_INCLUDED
#include <iostream>
#include "system.h"
#include <time.h>
#include <ctype.h>
class CDateTime
{
protected:

    char m_Date[8+1];
    char m_Time[6+1];
    unsigned char  m_WeekDay;
    
    void Standard();
    
    long GetTimetVal() const;

public:
    
    //! 默认构造函数
    CDateTime(){
        memset(m_Date,0,sizeof(m_Date));
        memset(m_Time,0,sizeof(m_Time));
        m_WeekDay='0'; 
    }
    
    //! 带整型日期 、 时间的构造函数
    CDateTime(const int &date, const int &time){
        sprintf(m_Date,"%08d",date);
        sprintf(m_Time,"%06d",time);
        m_WeekDay='0';
    }
    
    //! 带字符串日期 、 时间的构造函数
    CDateTime(const char * date, const char * time){
        strcpy(m_Date, date);
        strcpy(m_Time, time);
        m_WeekDay='0';
    }
    
    //! 带字符串日期时间的构造函数
     CDateTime(const char * dt){
        memcpy(m_Date, dt, 8); m_Date[8] = 0;
        strcpy(m_Time, dt + 8);
        m_WeekDay = '0';
    }
    
    //! 虚拟析构函数
    virtual ~CDateTime(){}
    
    //! 获得本对象的日期
    const char* getThisDate() const{
        return m_Date;        
    }
    
    //! 获得本对象的时间
    const char* getThisTime() const{
        return m_Time;   
    }
    
    //! 获得日期的整型值
    int getDateInteger() const{
        return atoi(m_Date);
    }
    
    //! 获得时间的整型值
    int getTimeInteger() const{
        return atoi(m_Time);   
    }
    
    //! 清楚时间内容
    void clear(){
        m_Date[0] = 0;
        m_Time[0] = 0;
        m_WeekDay = '0';   
    }
    
    //! 设置时间日期
    void set(const char *date,const char *time){
        setDate(date);
        setTime(time);
        m_WeekDay = '0';
    }
    
    //! 设置时间日期
    void setDateTime(const char * dt){
        memcpy(m_Date, dt, 8); m_Date[8] = 0;
        strcpy(m_Time, dt + 8);
        m_WeekDay = '0';
    }
    
    //! 设置时间
    void setTime(const char *time){
        sprintf(m_Time, "%06d", atoi(time));
        m_Time[6] = 0;
    }
    
    //! 设置日期
    void setDate(const char *date){
        sprintf(m_Date, "%08d", atoi(date));
        m_Date[8] = 0;
    }
    
    //! 设置时间日期
    void set(const int date,const int time){
        sprintf(m_Date,"%08d",(int)date);
        sprintf(m_Time,"%06d",(int)time);
    }
    
    //! 设置时间
    void setTime(const int time){
        sprintf(m_Time,"%06d",(int)time);
        m_Time[6] = 0;
    }
    
    //! 设置日期
    void setDate(const int date){
        sprintf(m_Date,"%08d",(int)date);
        m_Date[8] = 0;
    }
    
    //! 获得星期几的判断
    int getWeekDay();
    
    //! 将当前时间设置为下一天的时间
    CDateTime&  nextday();
    
    //! 
    CDateTime&  overnight();
    
    friend CDateTime operator + (const CDateTime & t1,const long duration);
    
    void operator +=(long duration);
    
    long operator-(const CDateTime &s2) const;
    
    bool operator==(const CDateTime &tp) const;
    
    bool operator < (const CDateTime&  tp) const;
    
    bool operator <= (const CDateTime&  tp) const;
    
    bool operator > (const CDateTime&  tp) const;
    
    bool operator >= (const CDateTime&  tp) const;
    
    CDateTime& operator = (const CDateTime& tp);
    
    //friend ostream & operator << (ostream & os, const CDateTime & tp;
  
    
    //! 返回完整的字符串
    const char * c_str(){
        strcpy(str_datetime, m_Date);
        strcat(str_datetime, m_Time);
        return str_datetime;
    }
    
    //! 上面的是单个对象所拥有的功能， 下面的实现是static，CDateTime共有的功能
public:

    static  char    str_date[8 + 1];                        //YYYYMMDD
    
    static  char    str_time[9 + 1];                        //HHMISSTTT，精确到毫秒级
    
    static  char    str_datetime[18];
    
    static char     str_fmt_datetime[36];               //经过格式化后的时间
    
public:
    
    /**
     * 函数名称：static  int     timeadd(char *dt,char *st,long secs)
     * 函数功能：时间HHMISS加secs秒数得到新的时间HHMISS
     * 使用注意：时间的格式
     * @param   dt              目标字符串
     * @param   st              原始字符串
     * @secs    secs            秒数
     * @return  sprintf的结果
     * @throws
     */
    static  int     timeadd(char *dt,char *st,long secs);   //在
    
    /**
     * 函数名称：static  int     dayadd(char *d,char *s,long n)
     * 函数功能：在日期上YYYYMMDD上加上天数，得到新的日期YYYYMMDD
     * 使用注意：日期的格式
     * @param   d               目标字符串
     * @param   s               原始字符串
     * @secs    n               天数
     * @return  sprintf的结果
     * @throws
     */    
    static  int     dayadd(char *d,char *s,long n);
    
    /**
     * 函数名称：static  long    diffday(char *dt1,char *dt2)
     * 函数功能：两个日期之间的天数
     * 使用注意：日期的格式
     * @param   d               目标字符串
     * @param   s               原始字符串
     * @secs    n               天数
     * @return  sprintf的结果
     * @throws
     */    
    static  long    diffday(char *dt1,char *dt2);
    
    static  long    datetimediff(const char *dt1,const char *dt2);
    
    static  long    timediff(char *dt1,char *dt2);
    
    static  int     getwday(char *timestr);
    
    static  int     datetimeadd(const char *s_date, const char *s_time,long secs);
    
    //! src_datetime格式如下：YYYYMMDDHHMISS
    static  int     datetimeadd(const char * src_datetime, long secs);
    
    static  double  changeprecision(double v,int n);
    
    static  double  setprecision(int v,int n);
    
    static  int     timevalidation(char *t);                //时间合法性
    
    static  long    time_to_second(char *s);                //将时间HHMMSS转换为秒
    
	static bool check_date(const char *pchString);          //校验日期正确性
	
	static bool check_time(const char *pchString);          //校验时间正确性
	
    static  void    reset();                                //将日期时间重置当前日期时间
    
    static  const char * date_time();                       //获取日期+时间，要取得当前时间先reset
    
    static  const char * get_date(){                        //获取日期
        return CDateTime::str_date;
    }        
    
    static  const char * get_time(){                        //获取存储在时间
        return CDateTime::str_time;
    }        
    
public:
    static const char * getTime(){                          //获取存储了的时间
        return CDateTime::str_time;
    }
    
    static const char * getDate(){                          //获取存储来的日期
        return CDateTime::str_date;
    }
    
    static const char * getDateTime();                      //获取存储的日期时间
    
    static const char * now(){                              //获取当前的日期加时间
        reset();
        return getDateTime();
    }

    static const char * now(const char * format){ //返回格式化后的当前时间
       return to_char(str_fmt_datetime,time(NULL),format);
        //return str_fmt_datetime;
    }
    
    static const char * to_char(char *Time,time_t tm,const char * fmt);
    
    static const char * addTime(const char * inTime, long nSec);
    
                                                            //time1, time2支持到毫秒级，毫秒级的则四舍五入
    static long diffTime(const char * time1, const char * time2);
    
    static long convertTimeToSecond(const char * inTime);   //HHMMSSTTT时间串转换为秒，串里支持毫秒
};

typedef CDateTime IDT;

typedef CDateTime idt;

inline CDateTime now()
{
    return CDateTime::now();
}

inline CDateTime now(char * fmt)
{
    return CDateTime::now(fmt);
}

#endif  /* CDATETIME_HEADER_INCLUDED */

