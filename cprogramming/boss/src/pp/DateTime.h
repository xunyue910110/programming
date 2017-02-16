// ##########################################
// Source file : DateTime.h
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

#ifndef DATETIME_H_HEADER_INCLUDED_B7A64EB3
#define DATETIME_H_HEADER_INCLUDED_B7A64EB3

#include "config-all.h"

#include <time.h>
#include <string>

USING_NAMESPACE(std);

//##ModelId=485712390138
//##Documentation
//## 通用通话时间处理类
class DateTime
{
  public:
    //##ModelId=485712420222
    DateTime();

    //##ModelId=4857125502EE
    DateTime *addYear(int yr);

    //##ModelId=48571275029F
    DateTime *addMonth(int mo);

    //##ModelId=4857128A009C
    DateTime *addDay(int da);

    //##ModelId=485712AE03B9
    DateTime *addHour(int hr);

    //##ModelId=485712C702CE
    DateTime *addMinute(int mi);

    //##ModelId=485712E2029F
    DateTime *addSecond(int se);

    //##ModelId=485712FE0128
    int yearDiff(DateTime *theDateTime);

    //##ModelId=4857131B02CE
    int monthDiff(DateTime *theDateTime);

    //##ModelId=4857133E03A9
    int dayDiff(DateTime *theDateTime);

    //##ModelId=4857137602CE
    int hourDiff(DateTime *theDateTime);

    //##ModelId=4857138901E4
    int minuteDiff(DateTime *theDateTime);

    //##ModelId=4857139D034B
    int secondDiff(DateTime *theDateTime);

    //##ModelId=4857142D00CB
    string getDateTime()
    {
        char cTmp[20];
        strftime(cTmp, 20, "%Y%m%d%H%M%S", &m_tm);
        return cTmp;
    }

    //##ModelId=4857143901F4
    const char *getDate()
    {
        strftime(m_cDate, 9, "%Y%m%d", &m_tm);
        return m_cDate;
    }

    //##ModelId=4857144403D8
    const char * getTime()
    {
        strftime(m_cTime, 7, "%H%M%S", &m_tm);
        return m_cTime;
    }

    //##ModelId=485714510000
    bool setDateTime()
    {
        time_t theDate;
        time(&theDate);
        m_tm = *localtime(&theDate);
        return true;
    }

    //##ModelId=4857145F035B
    bool setDateTime(int yr, int mo, int da, int hr, int mi, int se);

    //##ModelId=485714DE00BB
    bool setDateTime(int mo, int da, int hr, int mi, int se);

    //##ModelId=48571509004E
    bool setDateTime(const string &yr, const string &mo, const string &da, const string &hr, const string &mi, const string &se);

    //##ModelId=4857154A0242
    bool setDateTime(const string &mo, const string &da, const string &hr, const string &mi, const string &se);

    //##ModelId=4857158002FD
    bool isValid(int yr, int mo, int da, int hr, int mi, int se);

    //##ModelId=485715AB00DA
    bool isValid();

  public:
    //针对北电的特殊处理
    //##ModelId=485716000261
    int GetYear();

    //##ModelId=4857161400EA
    int GetMonth();

    //##ModelId=4857161F0157
    int GetDay();

    //##ModelId=485716540148
    int GetHour();

    //##ModelId=4857165D0290
    int GetMinute();

    //##ModelId=485716680167
    int GetSecond();
    
    

  private:
    //##ModelId=485716770203
    //##Documentation
    //## Test for leap year.
    bool isLeapYear(int yr) const
    {
        bool bLeap;
        if (yr % 4 == 0)
        {
            if (yr % 100 == 0)
            {
                if (yr % 400 == 0)
                {
                    bLeap=true;
                }
                else
                {
                    bLeap=false;
                }
            }
            else
            {
                bLeap=true;
            }
        }
        else
        {
            bLeap=false;
        }
        return bLeap;
    }

    //##ModelId=485716C0004E
    bool get4Year(int &yr);
	void addupDay2Tm(int &iDayAdd);
	
  private:
    //##ModelId=485717190222
    string m_DateTime_;

    //##ModelId=485717350186
    struct tm operator =(const struct tm &aTm);

    //##ModelId=48571777035B
    struct tm m_tm;
    
    char m_cDate[9];
    char m_cTime[7];

};



#endif /* DATETIME_H_HEADER_INCLUDED_B7A64EB3 */
