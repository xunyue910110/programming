
#include "StringUtil.h"
#include "PatternMatcher.h"

    /// 获得重复的字符串
    string StringUtil::repeat(const int count,const string & chr){
      string buffer;
      for(int i=0;i<count;++i)
      buffer+=chr;
    
      return buffer;
    };

  ///去掉字符串左边空白字符
    string & StringUtil::ltrim(string & str) {
        string::iterator pos = str.begin();
        for (; pos != str.end() && (* pos == ' ' || * pos == '\t'); pos++);
        if (pos != str.begin()) str.erase(str.begin(), pos);
        return str;
    };

    ///去掉字符串右边空白字符
     string & StringUtil::rtrim(string & str) {
        string::reverse_iterator pos = str.rbegin();
        for (; pos != str.rend() && (* pos == ' ' || * pos == '\t'); pos++);
        if (pos != str.rbegin()) str.erase(pos.base(), str.end());
        return str;
    }; 
    
    ///去掉字符串两边边空白字符
     string & StringUtil::trim(string & str) {        
        return ltrim(rtrim(str));
    }; 


    ///去掉字符串右边空白字符
    /// @return 返回字符串长度
     int StringUtil::rtrim(char *str) {
      int len=strlen(str);
      int i;
      for(i=len-1;i>=0;i--) {
         if(str[i]==' ')
           str[i]='\0';
         else
           break;
      }
      return strlen(str);
    };
    
  ///去掉字符串左边空白字符    
  /// @return 返回字符串长度
     int StringUtil::ltrim(char *str) {
      int i,j;
      int length=strlen(str);
      for(i=0;i<length;i++) {
        if((str[i]!=' ')&&(str[i]!='\t'))
          break;
      }
      
      if(i>0) {
        for(j=0;j<length-i;j++) 
          str[j]=str[j+i];
        str[j]=0;
      }
    
      return strlen(str);
    };
    
    ///去掉字符串两边边空白字符
    /// @return 返回字符串长度
     int StringUtil::trim(char *str) {
      ltrim(str);
      rtrim(str);
      return strlen(str);
    };



    /// 左对齐字符串
    /// @return 返回字符串长度
     int StringUtil::lalign(char *str, /// 待处理字符串
                      const int strlength, /// 处理的长度 
                      const char stuff  /// 填充的字符
                      ){
      int i,length;
      ltrim(str);
    
      length=strlen(str);
      if (length == strlength)
        return 1;
    
      if (length < strlength) {
        for(i=1;i <= strlength-length;i++)
          str[i+length-1] = stuff;
        
        str[i+length-1] = '\0';
      } else {
        str[strlength] = '\0';
      }
      return strlen(str);
    };



    /// 右对齐字符串
    /// @return 返回字符串长度
     int StringUtil::ralign(char *str, /// 待处理字符串
                      const int strlength, /// 处理的长度 
                      const char stuff   /// 填充的字符
                      ){
      int i,j,length;
      trim(str);
      length=strlen(str);
      if (length == strlength)
        return 1;
    
      if (length < strlength) {
        str[strlength] = '\0';
        for(j=length-1,i=strlength-1;j>=0;i--,j--) {
          str[i] = str[j];
        }
        
        for(i=strlength-length-1;i>=0;i--) {
          str[i] = stuff;
        }
      } else {
        str[strlength] = '\0';
      }
    
      return 1;
    };


    /// 删除特定字符
    /// @return 返回字符串长度
     int StringUtil::eraseChar(char *str,
                         const char& ch /// 去除的字符
                         ){
    int i,j,length;

    length=strlen(str);
    for(i=0,j=0;i<length;i++) {
      if ((str[i]!=ch)) {
        if (i!=j) 
          str[j]=str[i];
        j++;
      }
    }
    str[j]='\0';
      return (j+1);
    };


    ///字符串转换成大写
     string & StringUtil::toUpperCase(string & str) {
        for (string::iterator pos=str.begin(); pos != str.end(); pos++)
            *pos = toupper(*pos);
        return str;
    };
    
    ///字符串转换成小写
     string & StringUtil::toLowerCase(string & str) {
        for (string::iterator pos=str.begin(); pos != str.end(); pos++)
            *pos = tolower(*pos);
        return str;
    };
    
    ///(忽略大小写)比较字符串
     bool StringUtil::equalsIgnoreCase(const string& s1, const string& s2) {
        if(s1.length() != s2.length())
            return false;
        string::const_iterator it1=s1.begin();
        string::const_iterator it2=s2.begin();    
        for(; it1 != s1.end(); it1++, it2++) {
            if(tolower(*it1) != tolower(*it2)) {
                return false;
            }
        }        
        return true;
    };


    /// 测试是否是空字符串
     bool StringUtil::isBlank(const string& str) {
      size_t i;
      for ( i=0; i<str.length(); ++i) {
            if (!isspace(str[i])) 
                return false;
        }
        return true;
    };


    /// 简单模式匹配 Shell-Style
     bool StringUtil::matches(const char* str, const char* mask) 
    {
        return PatternMatcher::matches(mask,str);
    };
    
    ///分割字符串
     size_t StringUtil::split(const string& str, ///源字符串 
                        const string& delim, ///分割字符 
                        vector<string>& result, ///结果
                        bool needTrim ///是否去掉结果字串两边空格
                        ) {
        if(str.length() == 0) return 0;

        if (delim.length() <=0) { 
           result.push_back(str);
           return 1;
        }
             
        size_t c = 0;
        for (size_t i, pos=0; ; pos = i+delim.length()) {
            i = str.find(delim, pos);

            if (i == string::npos) 
                result.push_back(str.substr(pos));
            else
                result.push_back(str.substr(pos,i - pos));

            if(needTrim) 
              trim(result[result.size()-1]);

            c++;                        
            if (i == string::npos) break;
        }
        #ifdef _DEBUG_
           if(c!=35){
		cout<<"str="<<str.c_str()<<endl;	
	        cout<<"str count="<<c<<endl;
            }
	#endif
        return c;
    };


    /// 字符串分割 (效率低的一个实现)
     vector<string> StringUtil::split(const string & str,
                    const string& delim,
                    bool needTrim 
                    ) {
      vector<string> result;
      split(str,delim,result,needTrim );
      return result;
    };
   
   /// 分割关键字
    size_t StringUtil::tokenize(const string& str,vector<string>&result,
                          const string& delim,
                          bool needTrim ///是否去掉结果字串两边空格
                          ) {
        if(str.length() == 0) return 0;

        if (delim.length() <=0) { 
           result.push_back(str);
           return 1;
        }
             
        size_t c = 0;
        for (size_t i, pos=0; ; pos = i+1) {
            i = str.find_first_of(delim, pos);

            if (i == string::npos) 
                result.push_back(str.substr(pos));
            else
                result.push_back(str.substr(pos,i - pos));

            if(needTrim) 
              trim(result[result.size()-1]);

            c++;                        
            if (i == string::npos) break;
        }
        return c;
        return result.size();
    };
  



    /// 替换字符串
     bool StringUtil::replace(string& in, 
              const string& search, 
                        const string& replacement){
      bool bRet = false;
      string::size_type pos = 0;
      while( pos < in.length() && (pos=in.find(search, pos))!=string::npos )
      {
        bRet = true;
        in.replace(pos, search.length(), replacement);
        pos += replacement.length();
      }
      return bRet;
    };


    /// 替换字符串
     string StringUtil::replace(const string& in, 
                          const string& search, 
                          const string& replacement) {
      string i =in;
      replace(i, search, replacement);
      return i;
    };


    /// 填充字符串
     string StringUtil::pad(const char *str,/// 源字符串
                      const int  length, /// 预期长度
                      const char *stuff , /// 要填充的字符
                      const bool left  /// 是否在左边填充到(反之在右边)
                      ) {
      size_t len = strlen(str);
      if (len >= length )
        return str;

    if ( strlen(stuff)<=0 ) 
      return str;

    size_t len_stuff = strlen(stuff);
        string buffer;
    for (int i = 0; i < ((length-len) / len_stuff) ; ++i) {
      buffer += stuff;
    }
    buffer += stuff;
       
    buffer = string(buffer.c_str(),buffer.c_str()+ (length-len));
        
        return (left ? (buffer+str) : (str+buffer));
    };
    


    /// Converts the long type value @c value into a string
     string StringUtil::toString(long value, bool sign ) {
        return format(sign ? "%+ld" : "%ld", value);
    };
    
    /// Converts the double-precision floating-point value @c x into a String.
     string StringUtil::toString(double value, bool sign ) {
      return format(sign ? "%+lf" : "%lf", value);
    };

    /// Converts the int type value @c x into a String.
     string StringUtil::toString(int value, bool sign ) {
      return format(sign ? "%+ld" : "%ld", value);
    };
    
    /// A wrapper around the c-library sprintf routine.
    ///   Note: We attempt to perform the string formatting using a stack-based
    ///   buffer of a 1024 byte size.  If that fails then we return a null string.
     string StringUtil::format(const char* fmt, /// format specifers string 
                         ...  /// strings        
                         ) {
    #ifdef _MSC_VER
    #define VSNPRINTF _vsnprintf
    #else
    #define VSNPRINTF vsnprintf
    #endif
    
      string ret;
      const int stackBufferSize = 1024;
      char stackBuffer[stackBufferSize];
    
      va_list ap;
      va_start(ap, fmt);
      int n = VSNPRINTF(stackBuffer, stackBufferSize, fmt, ap);
      va_end(ap);
    
      /* If that worked, return the string. */
      if (n > -1 && n < stackBufferSize)
      {
        return string((const char*)stackBuffer, n);
      }
        
        return "";
    };



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
     string StringUtil::format(time_t timet , /// 要格式化的时间字符串
               const char* specifers  /// conversion specifers 
               ){
      time_t t = timet;
      if (timet==0)  // get now time
        t = time((time_t*)0);

      struct tm * tmptr = localtime(&t);
        
      if (specifers == 0)
        return asctime(tmptr);
      
      char buffer[1024];
      size_t ret = strftime(buffer,sizeof(buffer),specifers,tmptr);
        return buffer;
    };


    /// Tests if the passed string contains only hexadecimal characters
    /// [0-9], [a-f], [A-F]
    /// 
    /// @param in the string to test
    /// @returns true if the passed String contains hexadecimal characters only;
    ///         false otherwise
     bool StringUtil::isHexString(const string& in)
    {
      for(string::const_iterator i=in.begin(); i!=in.end(); ++i)
      {
        const char x = (*i);
        if(!isxdigit(x))
        {
          return false;
        }
      }
      return !in.empty();
    };



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
     string StringUtil::toHexString(const string& str)
    {
      string::const_iterator i(str.data());
      string::const_iterator end(str.data()+str.size());
      string ret;
    
      while(i != end)
      {
        ret += ("\\x");
        ret += (format("%04X", (unsigned int)(*i++)));
      }
      return ret;
    };

    
    /// 得到一个命令执行的输出行
    /// @return  命令输出行行数；返回－1表示错误。
     int StringUtil::command(const string & cmd,  ///命令
                vector< string > & lines ///命令输出行
                 ) {
      FILE *pipe;
      char buffer[2048];
    
        lines.clear();
      if((pipe=popen(cmd.c_str(),"r"))==NULL) {
        return -1;
      }
    
      while( fgets( buffer,sizeof(buffer),pipe ) != NULL ){
        buffer[2047]=0;
        lines.push_back(buffer);
      }
      pclose(pipe);
    
      return lines.size();
    }; 

    

