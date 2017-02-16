
#include "CryptUtil.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>

class CryptHelper {
public:	
	static char INT2HCHAR(int n) {
		if ( 0<=n && n<=9 )
			return '0'+n;
		if ( 10<=n && n<=15 )
			return 'A'+n-10;
		throw(n);
		return '0';
	}
	
	
	static char HCHAR2INT(char chPart1, char chPart2) {
		if( ( ('0'<=chPart1 && chPart1<='9') ||
			  ('A'<=chPart1 && chPart1<='F') )  &&
			( ('0'<=chPart2 && chPart2<='9') ||
			  ('A'<=chPart2 && chPart2<='F') ) )
		{
			int nPart[2]={0,0};
			if ('0'<=chPart1 && chPart1<='9')
				nPart[0] = chPart1 - '0';
			else
				nPart[0] = chPart1 - 'A';
	
			if ('0'<=chPart2 && chPart2<='9')
				nPart[1] = chPart2 - '0';
			else
				nPart[1] = chPart2 - 'A' + 10;
			return char(nPart[0]*16+nPart[1]);
		}
		return '0';
	}
	
	
	static bool Str2Hstr(const char * src, string & dst) {
		int len = strlen(src);
		dst.clear();
		dst.resize(len*2+1);
		dst[len*2+1] = 0;
		for( int n=0; n<len; n++ )	{
			dst[2*n+0] = INT2HCHAR(src[n]/16);
			dst[2*n+1] = INT2HCHAR(src[n]%16);
		}
		return true;
	}
	
	static bool Hstr2Str(const char * src, string & dst)
	{
		int len = strlen(src);
		if (len%2 != 0)
			return false;
		dst.clear();
		dst.resize(len/2);

		dst[len/2] = 0;
		for ( int n=0; n<len; n+=2 ) {
			dst[n/2] = HCHAR2INT(src[n],src[n+1]);
		}
		return true;
	}
	
	// 从1开始数把bit搬到某个位置
	static unsigned char moveBit(unsigned char chr, int from, int to)
	{
		unsigned char buffer = 0;
		unsigned char maskchr= 0x01<<(8-from);
		buffer = maskchr&chr;
		buffer = (buffer>>(8-from));
		buffer = buffer<<(8-to);
		return buffer;
	}
	
	
	static void mask(char* char3)
	{
		unsigned char& A = ((unsigned char*)char3)[0];
		unsigned char& B = ((unsigned char*)char3)[1];
		unsigned char& C = ((unsigned char*)char3)[2];
		unsigned char buffer[3];
	// A1A2A3A4 A5A6A7A8   B1B2B3B4 B5B6B7B8  C1C2C3C4 C5C6C7C8 
	// C1A4B1C7 A7C4B2B7   C2A2C8B4 A1B5B8B3  C3A8B6A5 C5A6C6A3 
		buffer[0] = moveBit(C,1,1)|moveBit(A,4,2)|
		            moveBit(B,1,3)|moveBit(C,7,4)|
		            moveBit(A,7,5)|moveBit(C,4,6)|
		            moveBit(B,2,7)|moveBit(B,7,8);
		            
		buffer[1] = moveBit(C,2,1)|moveBit(A,2,2)|
					moveBit(C,8,3)|moveBit(B,4,4)|
					moveBit(A,1,5)|moveBit(B,5,6)|
					moveBit(B,8,7)|moveBit(B,3,8);
					
		buffer[2] = moveBit(C,3,1)|moveBit(A,8,2)|
					moveBit(B,6,3)|moveBit(A,5,4)|
					moveBit(C,5,5)|moveBit(A,6,6)|
					moveBit(C,6,7)|moveBit(A,3,8);
		A = buffer[0]; 
		B = buffer[1]; 
		C = buffer[2];
	}
	
	static void mask2(char* char3)
	{
		unsigned char& A = ((unsigned char*)char3)[0];
		unsigned char& B = ((unsigned char*)char3)[1];
		unsigned char& C = ((unsigned char*)char3)[2];
		unsigned char buffer[3];
	// A1A2A3A4 A5A6A7A8   B1B2B3B4 B5B6B7B8  C1C2C3C4 C5C6C7C8 
	// C1A4B1C7 A7C4B2B7   C2A2C8B4 A1B5B8B3  C3A8B6A5 C5A6C6A3
	// B5B2C8A2 C4C6A5C2   A3A7B8B4 B6C3A8B7  A1B1C1A6 C5C7A4B3*
		buffer[0] = moveBit(B,5,1)|moveBit(B,2,2)|
		            moveBit(C,8,3)|moveBit(A,2,4)|
		            moveBit(C,4,5)|moveBit(C,6,6)|
		            moveBit(A,5,7)|moveBit(C,2,8);
		buffer[1] = moveBit(A,3,1)|moveBit(A,7,2)|
				    moveBit(B,8,3)|moveBit(B,4,4)|
				    moveBit(B,6,5)|moveBit(C,3,6)|
				    moveBit(A,8,7)|moveBit(B,7,8);
		buffer[2] = moveBit(A,1,1)|moveBit(B,1,2)|
				    moveBit(C,1,3)|moveBit(A,6,4)|
				    moveBit(C,5,5)|moveBit(C,7,6)|
				    moveBit(A,4,7)|moveBit(B,3,8);
		A = buffer[0]; 
		B = buffer[1]; 
		C = buffer[2];
	}
};



bool CryptUtil::encrypt(const char * src,string & encrypted) {

    string data;
    CryptHelper::Str2Hstr(src,data);

	int len = strlen(data.c_str());
	if ( len%2 != 0 )
		return false;

    encrypted.clear();
	string buffer ;
	buffer.resize(len/2*3);
	
	char char3[3];
	for ( int n=0; n<len/2; n++ ) {
		char3[0] = data[2*n+0];
		char3[1] = data[2*n+1];
		char3[2] = 0xE0|((n+1)&0x1F);
		CryptHelper::mask(char3);
		buffer[3*n+0] = char3[0];
		buffer[3*n+1] = char3[1];
		buffer[3*n+2] = char3[2];
	}
	buffer[len/2*3] = 0;
    encrypted = buffer;
	return true;
}


bool  CryptUtil::decrypt(const char * src,string & decrypted) {
	string data = src;
	int len = strlen(data.c_str());
	if ( len%3 != 0 )
		return false;

	decrypted.clear();
	string buffer;  
	buffer.resize(len/3*2+1);
	
	char char3[3];
	for ( int n=0; n<len/3; n++ ) {
		
		char3[0] = data[3*n+0];
		char3[1] = data[3*n+1];
		char3[2] = data[3*n+2];
		
		CryptHelper::mask2(char3);
		
		buffer[2*n+0] = char3[0];
		buffer[2*n+1] = char3[1];
		
		if ((char3[2]&0x1F) != ((n+1)&0x1F))
			return false;
	}
	buffer[len/3*2+1] = 0;
	CryptHelper::Hstr2Str(buffer.c_str(),decrypted);
	return true;
}
	


