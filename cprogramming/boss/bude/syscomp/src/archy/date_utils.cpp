#include "date_utils.hpp"


BEGIN_NAMESPACE_ARCHY

// strFmt %Y代表年份, %M代表月份, %D代表天数, %H代表24小时计数的小时数
// %N代表分钟, %S代表秒数, %%代表%号

bool date_utils::to_date(date_time& date, const string& str, const string& strFmt)
{
	const char *p = strFmt.c_str();
	const char *q = str.c_str();

	struct tm *gmt;
	time_t now(::time(0));
	gmt = ::gmtime(&now);
	gmt->tm_hour = gmt->tm_min = gmt->tm_sec = 0;
	
	int num;
	char strSub[8];
	for(; *p != '\0' && *q != '\0'; ++p)  {
        if(*p != '%') {
			if( *p == *q++)
				continue;
			else
				return false;
        }
        switch(*++p) {
        case 'Y':
				strncpy(strSub, q, 4);
				strSub[4] = 0;
				num = atoi(strSub);
				q += count_num(strSub);
				gmt->tm_year = num - 1900;
                break;
        case 'M':
        case 'D':
        case 'H':
        case 'N':
        case 'S':
				strncpy(strSub, q, 2);
				strSub[2] = 0;
				num = atoi(strSub);
				q += count_num(strSub);
				switch(*p) {
				case 'M':
					gmt->tm_mon = num -1;
					break;
				case 'D':
					gmt->tm_mday = num;
					break;
				case 'H':
					gmt->tm_hour = num;
					break;
				case 'N':
					gmt->tm_min = num;
					break;
				case 'S':
					gmt->tm_sec = num;
					break;
				}
                break;

		case '%':
			if( '%' == *++q)
				break;
		default:
				return false;
		}
	}

	time_t t = ::mktime(gmt);
	if( t == (time_t)(-1) )
		return false;
	date = date_time(t);
	string strDate = date.format("%Y-%M-%D %H:%N:%S");
	return true;
}


int date_utils::count_num(const char* str)
{
	int count(0);
	for(;*str >= '0' && *str <= '9'; ++str)
		++count;
	return count;

}
END_NAMESPACE_ARCHY
