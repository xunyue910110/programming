#include "AccountCommon.h"

int operator==(const UserAccountBill &left,const UserAccountBill& right)
{
  return (left.m_acctId == right.m_acctId &&
          left.m_userPriority == right.m_userPriority );
}

void UserAccountBill::operator=(const UserAccountBill& right)
{
  m_acctId                 = right.m_acctId;
  m_userPriority           = right.m_userPriority;
  m_defaultTag             = right.m_defaultTag;
  m_detailItemFeeVector    = right.m_detailItemFeeVector;
}
void DEBUG_PRINT(const char *fmt,...)
{
#ifdef _DEBUG_
	va_list ap;
	va_start(ap,fmt);
	vprintf(fmt,ap);
	va_end(ap);
#endif
}


/*功能：帐期相减 
 *param left_cycle  帐期1 YYYYMM
 *param right_cycle 帐期2 YYYYMM
 *返回值：相差帐期数
 */
int MinusCycle(const int &left_cycle ,const int &right_cycle)
{
	if (left_cycle<190001||left_cycle>300001||
		right_cycle<190001||right_cycle>300001)
    {
   #ifdef _DEBUG_ 	
	  cout<<"cycle is error!"<<endl;
	 #endif 
	  return 0;
		}
	
	if (left_cycle%100>12||right_cycle%100>12)
    {
	  //cout<<"cycle is error!"<<endl;
	  return 0;
		}

	return((left_cycle/100 - right_cycle/100)*12 + (left_cycle%100 - right_cycle%100));	
}

 //把秒转换成HHMMSS， sec 秒 ，sTime 小时分钟秒HHMMSS
 void FDateTime::secToTimeString(int sec, string& sTime)
 {
    int hour=0, minute=0, second=0;
    if (sec<=0)
    {
        sTime = "000000";
        return ;
    }
    else if (sec>24*3600)
    {
        sec %= (24*3600);
    }
    hour = sec/3600;
    sec %=  3600;
    minute = sec/60;
    sec %= 60;
    second = sec;
    char time[100];
    memset(time, 0, 100);
    sprintf(time, "%02d%02d%02d", hour, minute, second);
    sTime = time;
    
    return;
 }