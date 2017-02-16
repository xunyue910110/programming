#ifndef _STRINGUTIL_
#define _STRINGUTIL_


#include "config-all.h"
#include <sstream>
#include <string>
#include <cctype>
#include <vector>
#include <list>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
USING_NAMESPACE(std);


/**
 * @class StringUtil 
 *
 * @brief 字符串工具类.
 */


class StringUtil {
  public:

    /// 获得重复的字符串
    static string repeat(const int count,const string & chr);

  ///去掉字符串左边空白字符
    static string & ltrim(string & str) ;

    ///去掉字符串右边空白字符
    static string & rtrim(string & str) ; 
    
    ///去掉字符串两边边空白字符
    static string & trim(string & str) ; 


    ///去掉字符串右边空白字符
    /// @return 返回字符串长度
    static int rtrim(char *str) ;
    
  ///去掉字符串左边空白字符    
  /// @return 返回字符串长度
    static int ltrim(char *str) ;
    
    ///去掉字符串两边边空白字符
    /// @return 返回字符串长度
    static int trim(char *str) ;



    /// 左对齐字符串
    /// @return 返回字符串长度
    static int lalign(char *str, /// 待处理字符串
                      const int strlength, /// 处理的长度 
                      const char stuff = ' ' /// 填充的字符
                      );



    /// 右对齐字符串
    /// @return 返回字符串长度
    static int ralign(char *str, /// 待处理字符串
                      const int strlength, /// 处理的长度 
                      const char stuff =' '  /// 填充的字符
                      );


    /// 删除特定字符
    /// @return 返回字符串长度
    static int eraseChar(char *str,
                         const char& ch=' ' /// 去除的字符
                         );


    ///字符串转换成大写
    static string & toUpperCase(string & str) ;
    
    ///字符串转换成小写
    static string & toLowerCase(string & str) ;
    
    ///(忽略大小写)比较字符串
    static bool equalsIgnoreCase(const string& s1, const string& s2) ;


    /// 测试是否是空字符串
    static bool isBlank(const string& str) ;


    /// 简单模式匹配 Shell-Style
    static bool matches(const char* str, const char* mask) ;
    
    ///分割字符串
    static size_t split(const string& str, ///源字符串 
                        const string& delim, ///分割字符 
                        vector<string>& result, ///结果
                        bool needTrim = false ///是否去掉结果字串两边空格
                        ) ;


    /// 字符串分割 (效率低的一个实现)
    static vector<string> split(const string & str,
                    const string& delim,
                    bool needTrim = false
                    ) ;
    ///分割字符串 支持分割带汉字的字符串
    static size_t split4GB(const string& str, ///源字符串 
                        const string& delim, ///分割字符 
                        vector<string>& result, ///结果
                        bool needTrim = false ///是否去掉结果字串两边空格
                        ) ;


    /// 字符串分割 (效率低的一个实现) 支持分割带汉字的字符串
    static vector<string> split4GB(const string & str,
                    const string& delim,
                    bool needTrim = false
                    ) ;
   
   /// 分割关键字
   static size_t tokenize(const string& str,vector<string>&result,
                          const string& delim,
                          bool needTrim = false ///是否去掉结果字串两边空格
                          ) ;
  
   //模板函数
   template <typename T>
   static string string_join(const T& arr, const string& delim) {
       if (arr.empty()) {
           return "";
       }
       string                       result = arr.front();
       typename T::const_iterator   it     = arr.begin();
       size_t needed = result.size();
       while (++it != arr.end()) {
           needed += delim.size() + it->size();
       }
       result.reserve(needed);
       it = arr.begin();
       while (++it != arr.end()) {
           result += delim;
           result += *it;
       }
       return result;
   }

    /// 合并字符串
    static string join(const list<string>& arr, const string& delim) {
        return string_join(arr, delim);
    };
    
    /// 合并字符串
    static string join(const vector<string>& arr, const string& delim ) {
        return string_join(arr, delim);
    };


    /// 替换字符串
    static bool replace(string& in, 
              const string& search, 
                        const string& replacement);


    /// 替换字符串
    static string replace(const string& in, 
                          const string& search, 
                          const string& replacement) ;


    /// 填充字符串
    static string pad(const char *str,/// 源字符串
                      const int  length, /// 预期长度
                      const char *stuff = " ", /// 要填充的字符
                      const bool left = true /// 是否在左边填充到(反之在右边)
                      ) ;
    
    /**
     * 将任何可以输出到流里的对象, 转换成字符串
     * @author luzl
     */ 
    template <typename T>
    static string toString(T value, bool sign = false) {
    	ostringstream buffer; 
    	if(sign)buffer<<showpos;
    	buffer<<fixed<<value;
    	return buffer.str();
    }
    
    /**
     * 将字符串转换成对象, toValue<int>("123");  
     * @author luzl
     */ 
    template <typename T>
    static T toValue(const string& str) {
    	stringstream buffer; 
    	buffer<<str;
    	T value = T();
    	buffer >> value;
    	return value;
    }

    /// Converts the long type value @c value into a string
    static string toString(long value, bool sign = false ) ;
    
    /// Converts the double-precision floating-point value @c x into a String.
    static string toString(double value, bool sign = false ) ;

    /// Converts the int type value @c x into a String.
    static string toString(int value, bool sign = false ) ;
    
    /// A wrapper around the c-library sprintf routine.
    ///   Note: We attempt to perform the string formatting using a stack-based
    ///   buffer of a 1024 byte size.  If that fails then we return a null string.
    static string format(const char* fmt, /// format specifers string 
                         ...  /// strings        
                         ) ;



    /// A wrapper around the c-library strftime function 
        /// A wrapper around the c-library strftime function 
    /// SunOS 5.9            Last change: 5 Feb 2001                    1
    ///      %%    Same as %.
    ///      %a    Locale's abbreviated weekday name.
    ///      %A    Locale's full weekday name.
    ///      %b    Locale's abbreviated month name.
    ///      %B    Locale's full month name.
    ///      %c    Locale's appropriate date and time representation.
    /// 
    /// Standard conforming
    ///      %C    Century number (the year divided by 100 and  truncated
    ///            to  an  integer  as  a  decimal number [1,99]); single
    ///            digits are preceded by 0; see standards(5).
    /// 
    ///      %d    Day of month [1,31]; single digits are preceded by 0.
    ///      %D    Date as %m/%d/%y.
    ///      %e    Day of month [1,31]; single digits are preceded  by  a
    ///            space.
    ///      %g    Week-based year within century [00,99].
    ///      %G    Week-based year, including the century [0000,9999].
    ///      %h    Locale's abbreviated month name.
    ///      %H    Hour (24-hour clock) [0,23]; single  digits  are  pre-
    ///            ceded by 0.
    ///      %I    Hour (12-hour clock) [1,12]; single  digits  are  pre-
    ///            ceded by 0.
    ///      %j    Day number of year [1,366]; single digits are preceded
    ///            by 0.
    ///      %k    Hour (24-hour clock) [0,23]; single  digits  are  pre-
    ///            ceded by a blank.
    ///      %l    Hour (12-hour clock) [1,12]; single  digits  are  pre-
    ///            ceded by a blank.
    ///      %m    Month number [1,12]; single digits are preceded by 0.
    ///      %M    Minute  [00,59];  leading  0  is  permitted  but   not
    ///            required.
    ///      %n    Insert a NEWLINE.
    ///      %p    Locale's equivalent of either a.m. or p.m.
    ///      %r    Appropriate time representation in 12-hour clock  for-
    ///            mat with %p.
    ///      %R    Time as %H:%M.
    ///      %S    Seconds [00,61]; the range of values is [00,61] rather
    ///            than  [00,59]  to allow for the occasional leap second
    ///            and even more occasional double leap second.
    ///      %t    Insert a TAB.
    ///      %T    Time as %H:%M:%S.
    ///      %u    Weekday as a decimal number [1,7], with 1 representing
    ///            Monday. See NOTES below.
    ///      %U    Week number of year as a decimal number [00,53],  with
    ///            Sunday as the first day of week 1.
    ///      %V    The ISO 8601 week number as a decimal number  [01,53].
    ///            In  the  ISO  8601 week-based system, weeks begin on a
    ///            Monday and week  1  of  the  year  is  the  week  that
    ///            includes  both  January  4th and the first Thursday of
    ///            the year.  If the first Monday of January is the  2nd,
    ///            3rd,  or  4th, the preceding days are part of the last
    ///            week of the preceding year.  See NOTES below.
    ///      %w    Weekday as a decimal number [0,6], with 0 representing
    ///            Sunday.
    ///      %W    Week number of year as a decimal number [00,53],  with
    ///            Monday as the first day of week 1.
    ///      %x    Locale's appropriate date representation.
    ///      %X    Locale's appropriate time representation.
    ///      %y    Year within century [00,99].
    ///      %Y    Year, including the century (for example 1993).
    ///      %Z    Time zone name or abbreviation, or no bytes if no time
    ///            zone information exists.
    /// 
    /// common usage
    ///      %Y    Year, including the century (for example 1993).
    ///      %m    Month number [1,12]; single digits are preceded by 0.
    ///      %d    Day of month [1,31]; single digits are preceded by 0.     
    ///      %H    Hour (24-hour clock) [0,23]; single  digits  are  pre-
    ///            ceded by 0.
    ///      %M    Minute  [00,59];  leading  0  is  permitted  but   not
    ///            required.
    ///      %S    Seconds [00,61]; the range of values is [00,61] rather
    ///            than  [00,59]  to allow for the occasional leap second
    ///            and even more occasional double leap second.
    static string format(time_t timet = 0, /// 要格式化的时间字符串
               const char* specifers = 0 /// conversion specifers 
               );


    /// Tests if the passed string contains only hexadecimal characters
    /// [0-9], [a-f], [A-F]
    /// 
    /// @param in the string to test
    /// @returns true if the passed String contains hexadecimal characters only;
    ///         false otherwise
    static bool isHexString(const string& in);



    /// 
    /// Creates and returns a String containing each character from @c str in
    /// hexadecimal notation.
    /// 
    /// @param str a String containing Unicode characters encoded according to the
    /// internal convention.
    /// 
    /// @returns a hexadecimal representation of the Unicode characters within @c str
    /// 
    //==============================================================================
    static string toHexString(const string& str);

    
    /// 得到一个命令执行的输出行
    /// @return  命令输出行行数；返回－1表示错误。
    static int command(const string & cmd,  ///命令
                vector< string > & lines ///命令输出行
                 ) ; 

    

};

#endif //_STRINGUTIL_

