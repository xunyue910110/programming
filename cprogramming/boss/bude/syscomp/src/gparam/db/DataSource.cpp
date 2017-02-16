// ############################################
// Source file : DataSource.cpp
// Version     : 0.1.1
// Language       : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20030925
// Update      : 20030925
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################

#include "DataSource.h"

DataSource::DataSource(const DataSource &right)
{
  *this = right;
}
void DataSource::operator=(const DataSource& right)
{
  m_attribute = right.m_attribute;
  m_sourceID  = right.m_sourceID;
  m_parameterLog = ParameterLog::getInstance();
  m_dp = right.m_dp;
}


//##ModelId=3F7275260096
DataSource::DataSource()
{  
    m_parameterLog=ParameterLog::getInstance();
}

//##ModelId=3F72752D0367
DataSource::~DataSource()
{
}

//##ModelId=3F7278BA03BE
int DataSource::getRecord(vector <string> &cdr)
{   
    return 0;
}

//##ModelId=3F74E8FC03A5
string DataSource::getSourceID()
{   
    return m_sourceID;
}


//##ModelId=3F9F4F24010B
void DataSource::close()
{
}

