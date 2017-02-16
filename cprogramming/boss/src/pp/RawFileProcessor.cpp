// ##########################################
// Source file : RawFileProcessor.cpp
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#include "RawFileProcessor.h"

//##ModelId=4850ECD8035B
RawFileProcessor::~RawFileProcessor()
{
    ;
}

//##ModelId=4850ED7E037A
int RawFileProcessor::postValid()
{
    return 0;
}

//##ModelId=4850EDF10186
bool RawFileProcessor::attach(char *filename)
{
    return true;
}

//##ModelId=4850EE0B00EA
int RawFileProcessor::preValid()
{
    return 0;
}

//##ModelId=4850FA8902DE
bool RawFileProcessor::setRawFile(const string &SrcFileName, const unsigned char *FileBuffer, const int FileLength)
{
    m_FileName      = SrcFileName;
    m_FileBuffer	= FileBuffer;
    m_FileLength	= FileLength;

    return true;
}

