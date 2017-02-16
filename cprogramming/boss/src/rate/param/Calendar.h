#ifndef _CALENDAR_H_INCLUDE_20090812_
#define _CALENDAR_H_INCLUDE_20090812_

/*首先是要保存公农历之间的转换信息：
 以任意一年作为起点，把从这一年起若干年（若干是多少？就看你的需要了）的农历信息保存起来
 （以1921年作为起点）。要保存一年的信息其实只要两个信息就可以了：
 1、农历每个月的大小；2、今年是否有闰月，闰几月以及闰月的大小。
 用一个整数来保存这些信息就足够了。
 具体的方法是：用一位来表示一个月的大小，大月记为1，小月记为0，这样就用掉12位（无闰月）或13位（有闰月），
 再用高4位来表示闰月的月份，没有闰月记为0。
 比如说，2000年的信息数据是是0xC96，化成二进制就是110010010110B，表示的含义是指1、2、5、8、10、11月大，
 其余月小；2001年的农历信息数据是0x41A95，其中4表示今年闰四月，月份大小信息就是0x1A95（因为闰月，
 所以有13位），具体的就是1、2、4、5、8、10、12月大，其余月份小（0x1A95=1101010010101B），
 要注意在四月的后面那一个0表示的是闰四月小，接着的那个1表示5月大。这样就可以用一个数组来保存这些信息。
一、公历转换成农历 
1、计算出所求时间到起始年正月初一的天数。 
2、从起始年份开始，减去每一月的天数，一直到剩余天数没有下一个月多为止 
   此时，ChineseCalendarData[]的下标到了多少，就是减去了多少年，用起始年份加上这个下标就可以得到农历年份；
   然后看减去了几个月，如果本年不闰月或者闰月还在后面，就可以直接得到农历月份，如果在闰月月份数后面一个月，
   则这个月就是闰月，如果在闰月的后面，则要减去1才能得到月份数；剩余的天数就是农历日；农历时用
   （公历时+1）/2就可以简单地得到了。
二、农历到公历的转换 
   这个算法比较简单，只要计算所求时候到起始年正月初一的总天数就可以了，要计算总天数，只要统计出本月以前
   的大月小月数就可以了，然后把这个值赋予TdateTime类型的TheDate就可以用TdateTime的成员函数DecodeDate得
   到公历的年月日了。具体代码如下。
*/

/*
	输入:date公历日期
	返回:农历日期
*/
//暂时不需要
//int DateConvert::i2c(const int &date)
//{ 
//	int total,m,n,k; 
//	bool isEnd=false;            //用以判断是否不够减了
//	//total=(int)TheDate-7709;   //到1921-2-8（正月初一）的天数 
//365 - 31 - 8 = 326
//	total = difftime(t1,t2);
//	for(m=0;;m++) 
//	{ 
//		/*判断本年是否闰月，用以确定月份信息的起点有闰月有13位（0~12），无12位（0~11）*/ 
//		k=(LunarCalendarTable[m]<0xfff)?11:12;
//		for(n=k;n>=0;n--) 
//		{ 
//			//如果不够减 
//			if(total<=29+GetBit(LunarCalendarTable[m],n)) 
//			{ 
//				isEnd=true; //设置标志 
//				break; //退出内层循环 
//			} 
//			/*够减，减去一个月的天数先减去29天如果月大，则对应的信息位为1，又减去一天*/ 
//			total=total-29-GetBit(LunarCalendarTable[m],n); 
//		} 
//		if(isEnd)break; //如果不够减，退出外层循环 
//	} 
//	FcYear=1921 + m; //农历年=起始年份+下标 
//	FcMonth=k-n+1;   //农历月=本年的月份数（k+1）减去已经减去的月份数（n） 
//	FcDay=total;     //农历日=剩余天数 
//	if(k==12)              //如果本年有闰月 
//	{ 
//		if(FcMonth==LunarCalendarTable[m]/0x10000+1)//就是闰月
//			FcMonth=1-FcMonth; 
//		if(FcMonth>LunarCalendarTable[m]/0x10000+1) //闰月后面 
//			FcMonth--; 
//	} 
//}

const int LunarCalendarTable[100] ={ 
                            2635,333387,1701,1748,267701,694,2391,133423,1175,396438
                            ,3402,3749,331177,1453,694,201326,2350,465197,3221,3402
                            ,400202,2901,1386,267611,605,2349,137515,2709,464533,1738
                            ,2901,330421,1242,2651,199255,1323,529706,3733,1706,398762
                            ,2741,1206,267438,2647,1318,204070,3477,461653,1386,2413
                            ,330077,1197,2637,268877,3365,531109,2900,2922,398042,2395
                            ,1179,267415,2635,661067,1701,1748,398772,2742,2391,330031
                            ,1175,1611,200010,3749,527717,1452,2742,332397,2350,3222
                            ,268949,3402,3493,133973,1386,464219,605,2349,334123,2709
                            ,2890,267946,2773,592565,1210,2651,395863,1323,2707,265877};
class Calendar
{
  public :
	static int c2i(const int &date)
	{
		int i,k,m,p,y[]={0,0}; //y[0]：小月、y[1]：大月 
	    //本年以前的大月小月数 
	    int FcYear  = date / 10000;
	    int FcMonth = (date % 10000) / 100;
	    int FcDay   = date % 100;
	    int total;
	    int result;
  		int year,month,day,days=0;
	    for(i=0;i<FcYear-1921;i++) 
		{ 
			k=(LunarCalendarTable[i]<0xfff)?11:12; 
			for(m=0;m<=k;m++)
			{
				y[GetBit(LunarCalendarTable[i],m)]++;
			}
		} 
		//统计本年本月以前的大月小月数 
		//本年不是闰年 
		if(LunarCalendarTable[i]<0xfff)
		{
			for(m=13-FcMonth;m<=11;m++)
			{
				y[GetBit(LunarCalendarTable[i],m)]++;
			}
		}
		else // 是闰年 
		{ 
			k=LunarCalendarTable[i]/0x10000; 
			//根据在闰月前后决定统计的起始位置 
			p=(FcMonth>k)?13-FcMonth:14-FcMonth; 
	        if(k+FcMonth==0)
	        {
	        	p=13+FcMonth;     //本月就是闰月 
	        }
	        for(m=p;m<=12;m++)
	        {
				y[GetBit(LunarCalendarTable[i],m)]++;
			}
		} 
        //total即为需要转换的日期至1921年正月初一的天数 
		total = y[0]*29+y[1]*30+FcDay;
		//1921年正月初一对应1921年2月8日，1921年2月8日后共有327天 
		total -= 327;
		//此时total即为1922年1月1日到date的总天数 
		year = 1922;
		while (true)
		{
		    if ((year % 4 == 0) && (year % 100 != 0))
			{
				days = 366;
			}
			else if (year % 400 == 0)
			{
				days = 366;
			}
			else
			{
				days = 365;
			}
			//剩下的天数是否大于year年的天数，是则继续 
			if (total - days > 0)
			{
				total -= days;
				++year;
			}
			//否则结束 
			else
			{
			    break;
			}    
		}    
		for (month = 1; month <= 12; ++month)
		{
			if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
			{
				days = 31;
			}
			else if (month == 2)
			{
			    if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
			    {
                     days = 29;
                }        
                else
                {
                    days = 28;
                }    
			}
			else
			{
				days = 30;
			}
			//剩下的天数是否大于month月的天数，是则继续 
			if (total - days > 0)
			{
				total -= days;
			}
			//否则结束 
			else
			{
				break;
			}
		}
		//此时year,month,total分别为Date对应的公历年月日 
		return year*10000+month*100+total;
	}
  private :
 	//判断num的position位是1还是0，第1位的position=0
	static int GetBit(const int &num, const int &position)
	{
		int t_num = num;
		t_num >>= position;
		return (1&t_num);
	}
};

#endif
