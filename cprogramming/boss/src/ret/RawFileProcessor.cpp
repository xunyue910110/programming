// ##########################################
// Source file : RawFileProcessor.cpp
// System      : Mobile Billing System
// Version     : 1.04
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20010827
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "RawFileProcessor.h"

//##ModelId=3B978CA7000C
RawFileProcessor::~RawFileProcessor()
{
	;
}

//##ModelId=3B60C5A603E4
int RawFileProcessor::postValid()
{
	return 0;
}

//##ModelId=3B71083B00EA
bool RawFileProcessor::attach(char *filename)
{
	return true;
}

//##ModelId=3B710D0B0073
int RawFileProcessor::preValid()
{
	return 0;
}

//##ModelId=3B8217ED00A4
bool RawFileProcessor::setRawFile(const string SrcFileName,const unsigned char *FileBuffer,
								const int FileLength)
{
    m_FileName 	    = SrcFileName;
    m_FileBuffer	= FileBuffer;
    m_FileLength	= FileLength;

    return true;
}

