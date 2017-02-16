// ##########################################
// Source file : DataTime.h
// System      : Move Billing System
// Version     : 0.1.04
// Language	   : ANSI C++
// OS Platform : HPUX,AIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010915
// Update      : 2001127
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef DATETIME_H_INCLUDED_C45CF144
#define DATETIME_H_INCLUDED_C45CF144

#include "config-all.h"

#include <time.h>
#include <string>

USING_NAMESPACE(std);

//##ModelId=3B5390F40118
//##Documentation
//## 通用通话时间处理类
class DateTime {
  public:

	//##ModelId=3BD62A590346
	DateTime() {m_tm.tm_isdst = -1;}

	//##ModelId=3BA60BC2020D
	DateTime *addYear	(int yr);
	//##ModelId=3BA60BC203DA
	DateTime *addMonth	(int mo);
	//##ModelId=3BA60BC301AA
	DateTime *addDay	(int da);
	//##ModelId=3BA60BC30377
	DateTime *addHour	(int hr);
	//##ModelId=3BA60BC4015C
	DateTime *addMinute	(int mi);
	//##ModelId=3BA60BC4033C
	DateTime *addSecond	(int se);

	//##ModelId=3BA60BC5013F
	int yearDiff	(DateTime *theDateTime);
	//##ModelId=3BA60BC5032A
	int monthDiff	(DateTime *theDateTime);
	//##ModelId=3BA60BC60140
	int dayDiff		(DateTime *theDateTime);
	//##ModelId=3BA60BC6035D
	int hourDiff	(DateTime *theDateTime);
	//##ModelId=3BA60BC70192
	int minuteDiff	(DateTime *theDateTime);
	//##ModelId=3BA60BC703D7
	int secondDiff	(DateTime *theDateTime);

	//##ModelId=3B539E790382
	string getDateTime()
	{
		char cTmp[20];

		strftime(cTmp,20,"%Y%m%d%H%M%S",&m_tm);

		return cTmp;
	}

	//##ModelId=3BA6C836023B
	string getDate()
	{
		char cTmp[9];

		strftime(cTmp,9,"%Y%m%d",&m_tm);

		return cTmp;
	}

	//##ModelId=3BA6C8360336
	string getTime()
	{
		char cTmp[7];

		strftime(cTmp,7,"%H%M%S",&m_tm);

		return cTmp;
	}

	// get now time
	//##ModelId=3BA5EDF3012E
	bool setDateTime()
	{
		time_t theDate;

		time(&theDate);
		m_tm=*localtime(&theDate);

		return true;
	}

	//##ModelId=3BA5EDF30188
	bool setDateTime(int yr,int mo, int da,int hr,int mi,int se);

	//##ModelId=3BA5EDF500B8
	bool setDateTime(int mo, int da,int hr,int mi,int se);

	//##ModelId=3BA72E8F0056
	bool setDateTime(string yr,string mo,string da,string hr,string mi,string se);

	//##ModelId=3BA72E9103C0
	bool setDateTime(string mo,string da,string hr,string mi,string se);

	//##ModelId=3BA5EDF70075
	bool isValid(int yr,int mo, int da,int hr,int mi,int se);

	//##ModelId=3BA60DA500A9
	bool isValid();

  private:

	//##ModelId=3BA3071D017B
	//##Documentation
	//## Test for leap year.
   bool isLeapYear(int yr) const
   {
	   bool bLeap;

	   if (yr%4==0) {
	   	if (yr%100==0) {
	   		if (yr%400==0)
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

	//##ModelId=3BA5EDF700ED
	bool get4Year(int &yr);

	//##ModelId=3BA6AF9A00D4
	struct tm operator =(const struct tm &aTm);

  private:
	//##ModelId=3B95A3A30099
	string m_DateTime_;

	//##ModelId=3BA5EDEE0145
	struct tm m_tm;

};



#endif /* DATETIME_H_INCLUDED_C45CF144 */
