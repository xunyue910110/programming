#ifdef _MBCS
#pragma warning( disable : 4786)
#endif


#include "stringutils.hpp"
#ifndef WIN32
#include <sys/time.h>
#else
#include <winsock.h>
#endif

#ifdef _MBCS
#pragma warning( disable : 4786)
#endif

namespace archy {

char *itoa(long i, char *buf, int base)
{
        char *pos = buf + sizeof(buf) - 1;
        unsigned long u;
        int negative = 0;
        int j;
        if (i < 0) {
                negative = 1;
                u = ((unsigned long)(-(1+i))) + 1;
        } else {
                u = i;
        }

        *pos = 0;

        do {
                if( --pos == buf )
                        return 0;
                *pos = '0' + (u % base) ;
                if( *pos > '9' )
                        *pos +=  'a' - '9' - 1;
                u /= base;
        } while (u );

        if (negative) {
                *--pos = '-';
        }
        for(  j = 0; ; j++ ) {
                *(buf + j ) = *pos;
                if( *pos++ == 0 ) 
                        break;
        }
        return buf;
}

string& trim_left(string& str, const string& strSet/* = " \r\n\t"*/)
{
        string::size_type iPos = str.find_first_not_of(strSet);
        // Not found
        if( iPos == string::npos ) {
		str.erase();
                return str;
        }
        else {
                return str.erase(0, iPos);
        }
}

string& trim_right(string& str, const string& strSet /*= " \r\t"*/)
{
        string::size_type iPos = str.find_last_not_of(strSet);
        // Not found
        if( iPos == string::npos ) {
				str.erase();
                return str;
        }
        else{
                return str.erase(iPos + 1);
        }
}

string& trim(string& str, const string& strSet /*= " \r\n\t"*/)
{
	trim_left(str, strSet);
	return trim_right(str, strSet);
}

string& to_upper(string& str)
{
        for( unsigned i = 0; i < str.length(); ++i ) {
                if( str[i] >= 'a' && str[i] <= 'z' )
                	str[i] -= 32;
        }
        return str;
}

string& to_lower(string& str)
{
        for( unsigned i = 0; i < str.length(); ++i ) {
                if( str[i] >= 'A' && str[i] <= 'Z' )
                	str[i] += 32;
        }
        return str;
}

string  to_string(long iNum, int iSize /*= 0*/)
{
	char fmt[8];
        if(iSize > 0 )
        	sprintf(fmt, "%%0%dld", iSize);
        else
                strcpy(fmt, "%ld");
        
	char buf[16];
	sprintf(buf, fmt, iNum);
	return string(buf);
}

bool is_number(const string& strSrc)
{
	string::const_iterator iter;
	for( iter = strSrc.begin(); iter != strSrc.end(); ++iter)  {
		if( *iter > '9' || *iter < '0' )
			return false;
	}
	return true;
}



string& add_slash(string& strDir)
{
#ifndef WIN32
	if( strDir[strDir.length() - 1] != '/' )
		strDir += '/';
#else
	if( strDir[strDir.length() - 1] != '\\' )
		strDir += '\\';
#endif
	return strDir;
}

vector<string> split_string_to_vector(const string& strSrc, const string& strSplitSet, 
									  bool bSkipEmpty)
{
	int iStart(0);
	string::size_type pos;
	vector<string> lstRet;
	string strTmp;
	while(1) {
		pos = strSrc.substr(iStart, strSrc.length() - iStart).find_first_of(strSplitSet);
		if(pos == string::npos ) {
			strTmp = strSrc.substr(iStart, strSrc.length() - iStart);
			// Trim(strTmp, strSplitSet);
			if( !strTmp.empty() || !bSkipEmpty ) {
				lstRet.push_back(strTmp);
			}
			
			break;
		}
		strTmp = strSrc.substr(iStart, pos);
		//Trim(strTmp, strSplitSet);
		if( !strTmp.empty() || !bSkipEmpty) {
			lstRet.push_back(strTmp);
		}
		iStart += pos + 1;
	}
	return lstRet;

}
list<string> split_string(const string& strSrc, const string& strSplitSet, bool bSkipEmpty)
{
	int iStart(0);
	string::size_type pos;
	list<string> lstRet;
	string strTmp;
	while(1) {
		pos = strSrc.substr(iStart, strSrc.length() - iStart).find_first_of(strSplitSet);
		if(pos == string::npos ) {
			strTmp = strSrc.substr(iStart, strSrc.length() - iStart);
			// Trim(strTmp, strSplitSet);
			if( !strTmp.empty() || !bSkipEmpty ) {
				lstRet.push_back(strTmp);
			}
			
			break;
		}
		strTmp = strSrc.substr(iStart, pos);
		//Trim(strTmp, strSplitSet);
		if( !strTmp.empty() || !bSkipEmpty) {
			lstRet.push_back(strTmp);
		}
		iStart += pos + 1;
	}
	return lstRet;
}

// return 0: 没有找到完整消息
// return 大于0:找到一条返回大小长度的消息
int ReadFixedHeadTailMsg(char* buf, 
						 const char* fixedHead, const char* fixedTail)
{
	int iOff;
	unsigned int headLen = strlen(fixedHead);
	unsigned int tailLen = strlen(fixedTail);
	// 首先寻找固定头部
	char* pos = strstr(buf, fixedHead);
	unsigned int bufLen = strlen(buf);
	if( pos == 0 ) {
		// 如果找不到，左移数据，只剩最多headLen - 1个字符
		if( strlen(buf) >= headLen ) {
			iOff = headLen - 1;
			memmove(buf, buf + bufLen - iOff, iOff);
			buf[iOff] = 0;
		}
		return 0;
	}
	iOff = strlen(pos);
	memmove(buf, pos, iOff);
	buf[iOff] = 0;
	// 找到了头部，寻找尾部
	char* posEnd = strstr(buf + headLen, fixedTail);
	if( posEnd == 0 ) {
		// 没有找到尾部
		return 0;
	}
	// 找到尾部，重新设定*iOff

	return posEnd - buf + tailLen;
}


}


