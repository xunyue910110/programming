#include "evs.hpp"
#include <string.h>

namespace archy {
	
void evs(const char* in, char* out, int times)
{
	archy::evs(in, out);
	while( --times) {
		archy::evs(out, out);
	}
}

void dvs(const char* in, char* out, int times)
{
	archy::dvs(in, out);
	while( --times) {
		archy::dvs(out, out);
	}
}

// 简单加密
// Encrypt visible string
void evs(const char* in, char* out)
{
	int iLen = strlen(in);

	int off = iLen%94;
	for(int i = 0; i < iLen; ++i) {
		*out++ = (*in++ - 33 + off + i%94)%94 + 33;
	}
	*out = 0;
}
// 解密
// Dncrypt visible string
void dvs(const char* in, char* out)
{
	int iLen = strlen(in);
	int off = iLen%94;
	int iOff, iDiff;
	for( int i = 0; i < iLen; ++i ) {
		iOff = i%94;
		iDiff = off + iOff - *in + 33;
		if( iDiff > 0 ) {
			*out = *in + (iDiff/95 + 1)*94  - off - iOff;
		}
		else {
			*out = *in  - off - iOff;
		}
		out++; in++;
	}
	*out = 0;
}

string Evs5Num(const string& str) {

	char * ret = new char[str.length() + 1];

	archy::evs(str.c_str(), (char*)ret, 5);

	string strRet = archy::Str2HexNum(ret);
	delete ret;
	return strRet;
}

string Dvs5Num(const string& str) {
	string strRet = archy::HexNum2Str(str.c_str());
	archy::dvs(strRet.c_str(), (char*)strRet.c_str(), 5);
	return strRet;

}

string Encode(const string& str) {

	char * ret = new char[str.length() + 1];

	archy::evs(str.c_str(), (char*)ret, 5);

	string strRet = ret;
	delete ret;
	return strRet;
}

string Decode(const string& str) {
	char * ret = new char[str.length() + 1];

	archy::dvs(str.c_str(), (char*)ret, 5);
		
	string strRet = ret;
	delete ret;
	return strRet;

}

string Str2HexNum(const char* in)
{
	char buf[8];
        string strRet;
        unsigned int len = strlen(in);
        for(unsigned int i = 0; i < len; ++i ) {
        	sprintf(buf, "%02x", in[i]);
                strRet += buf;
        }
        return strRet;
}

string HexNum2Str(const char* in)
{
	char buf[8];
        char* endstr;
        string strRet;
        unsigned int len = strlen(in);
        for(unsigned int i = 0; i < len; i+=2 ) {
              buf[0] = in[i];
              buf[1] = in[i+1];
              buf[2] = 0;
              char bByte = strtol(buf, &endstr, 16);
              strRet += bByte;
        }
        return strRet;

}


} //namespace archy
