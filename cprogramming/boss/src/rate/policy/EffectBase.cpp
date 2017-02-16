#include "EffectBase.h"
#include "RateGlobalInfo.h"

#include <time.h>

extern RateGlobalInfo g_globalInfo;
RateGlobalInfo* EffectBase::m_globalInfo = &g_globalInfo;
  
bool EffectBase::rateExecute(const vector<CdrSeg> &r_pCdrSegList)
{
  return true;
}

  
/*
void EffectBase::getCurrentDate(string& currDate)
{
	time_t now;
    char date[14+1];
    struct tm *w;

    time(&now);
    w = localtime(&now);
    sprintf(date,"%04d%02d%02d%02d%02d%02d",
          w->tm_year+1900,w->tm_mon+1,w->tm_mday,
          w->tm_hour,w->tm_min,w->tm_sec);
          
    currDate=date;
    
    //if (currDate > m_acycPara.m_acycEndTime)
    //{
    //    currDate=m_acycPara.m_acycEndTime;
    //}
}

void EffectBase::getDuration(const string& endDate, 
                             const string& startDate,
                             int& dayNum)
{
    time_t endsecond,startsecond;  
    int    endyear,endmonth,endday,endhour,endminute,endseconds;
    int    startyear,startmonth,startday;
    struct tm p_end;
    struct tm p_start;
    
    endyear = atoi(endDate.substr(0,4).c_str());
    endmonth = atoi(endDate.substr(4,2).c_str());
    endday = atoi(endDate.substr(6,2).c_str());
    endhour = atoi(endDate.substr(8,2).c_str());
    endminute = atoi(endDate.substr(10,2).c_str());
    endseconds = atoi(endDate.substr(12,2).c_str());
    
    startyear = atoi(startDate.substr(0,4).c_str());
    startmonth = atoi(startDate.substr(4,2).c_str());
    startday = atoi(startDate.substr(6,2).c_str());  
    
    p_end.tm_sec = 0;
    p_end.tm_min = 0;
    p_end.tm_hour = 0;
    p_end.tm_mday = endday;
    p_end.tm_mon = endmonth - 1;
    p_end.tm_year = endyear - 1900;
    
    endsecond = mktime(&p_end);
    
    p_start.tm_sec = 0;
    p_start.tm_min = 0;
    p_start.tm_hour = 0;
    p_start.tm_mday = startday;
    p_start.tm_mon = startmonth - 1;
    p_start.tm_year = startyear - 1900;  
    
    startsecond = mktime(&p_start);
    
    if (endhour==0 && endminute==0 && endseconds==0)
    {
        dayNum = difftime(endsecond,startsecond)/24/3600;
    }
    else
    {
        dayNum = difftime(endsecond,startsecond)/24/3600 + 1;
    }
}
*/
