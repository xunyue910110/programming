//-------------------------------------------------------------------
// Name			: dbibase.h
// Version		: 0.2.8
// Create		: 2001.4.10
// Update		: 2001.8.21
// Auther		: qianyx@lianchuang.com
// Copyright	: All Rights Reserved.
// Description	: Should be included in dbi kernel programs.
//-------------------------------------------------------------------
#ifndef DBIBASE_H
#define DBIBASE_H

#include <stdio.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <ctype.h>

inline void DEBUG_PRINTF(const char *fmt,...)
{
#ifdef _DEBUG_
	va_list ap;
	va_start(ap,fmt);
	vprintf(fmt,ap);
	va_end(ap);
#endif
}

char *strcasestr(const char *str1,const char *str2)
{
	char *str;
	int i;

	str=(char *)str1;

	while(str1+strlen(str1)>=str+strlen(str2))
	{
		for(i=0;i<strlen(str2);i++)
		{
			if(toupper(str[i])!=toupper(str2[i]))break;
		}
		if(i==strlen(str2))return str;
		str++;
	}
	return 0;
}

#endif //DBIBASE_H

//------------------------------ THE END ----------------------------

