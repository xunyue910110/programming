#ifndef ARCHY_STRING_TO_DATE_UTILS_HEADER
#define ARCHY_STRING_TO_DATE_UTILS_HEADER

#include "date_time.hpp"
#include "archy_config.hpp"
USING_STD_NAMESPACE;

BEGIN_NAMESPACE_ARCHY

class date_utils
{
public:
	// strFmt %Y代表年份, %M代表月份, %D代表天数, %H代表24小时计数的小时数
	// %N代表分钟, %S代表秒数, %%代表%号

	static bool to_date(date_time& date, const string& str, const string& strFmt);

	static int count_num(const char* str);
};

END_NAMESPACE_ARCHY


#endif // ARCHY_STRING_TO_DATE_UTILS_HEADER

