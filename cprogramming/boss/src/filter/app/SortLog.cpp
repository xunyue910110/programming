// ##########################################
// Source file : SortLog.cpp
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011108
// Update      : 20020308
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include <stdio.h>
#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "log4cpp/Logger.h"

#include "SortLog.h"
#include "OutChanInfo.h"

SortLog::SortLog()
{
}

SortLog::~SortLog()
{
}


void SortLog::initialize(FilterGlobalInfo *r_gFilterInfo)
{
	  m_ferrCdrs = 0;
    m_gFilterInfo = r_gFilterInfo;

    LOGINFO r_logInfo;
    vector<OutChanInfo>::iterator itr;

    m_logInfo.clear();

    //输入
    r_logInfo.m_fullPathName=m_gFilterInfo->m_fullPathName;
    r_logInfo.m_channelNo=CHANNEL_FILEINPUT;
    r_logInfo.m_total=0;
    m_logInfo[r_logInfo.m_channelNo]=r_logInfo;

//cout<<"r_logInfo.m_fullPathName\t"<<r_logInfo.m_fullPathName<<"  "<<__FILE__<<__LINE__<<endl;
//cout<<"r_logInfo.m_fullPathName\t"<<r_logInfo.m_channelNo<<"  "<<__FILE__<<__LINE__<<endl;


    //输出
    for(vector<OutChanInfo>::iterator r_Itr=m_gFilterInfo->m_outChanInfo->begin();
                                      r_Itr!=m_gFilterInfo->m_outChanInfo->end();++r_Itr)
    {
        //r_logInfo.m_pathName=r_Itr->m_desPath;
        //r_logInfo.m_fileName="";
        r_logInfo.m_fullPathName="";
        r_logInfo.m_channelNo=r_Itr->m_channelNo;
        r_logInfo.m_total=0;
        m_logInfo[r_logInfo.m_channelNo]=r_logInfo;

//cout<<"r_logInfo.m_fullPathName\t"<<r_logInfo.m_channelNo<<"  "<<__FILE__<<__LINE__<<endl;

    }
/**/
}



bool SortLog::begin()
{
   m_logInfo.clear();
   theLoggerProcessID = theLogger->pBegin();
   return true;
}

void SortLog::addLogCdrs(const PString &r_channelNo,const PString &r_fileName,const PString &r_outfiletype)
{
    if( m_logInfo.count(r_channelNo)>0 )
    {
        m_logInfo[r_channelNo].m_total++;
        if( m_logInfo[r_channelNo].m_fullPathName=="")
        {
            m_logInfo[r_channelNo].m_fullPathName=r_fileName;
            m_logInfo[r_channelNo].m_outfiletype=r_outfiletype;
        }
    }
}

bool SortLog::write()
{
    //map<PString,LOGINFO>   &r_log=theSortLog->getLogs();
    for(map<PString,LOGINFO>::iterator r_Itr=m_logInfo.begin(); r_Itr!=m_logInfo.end(); ++r_Itr)
    {
        if(r_Itr->first==CHANNEL_FILEINPUT)
        {
           theLogger->pInput(theLoggerProcessID,r_Itr->second.m_fullPathName.c_str(),r_Itr->second.m_total);
        }
        else
        {
           if(r_Itr->second.m_total>0)
           {
              theLogger->pOutput(theLoggerProcessID,r_Itr->second.m_fullPathName.c_str(),r_Itr->second.m_total,
              "channel_type=%s,out_type=%s",r_Itr->second.m_channelNo.c_str(),r_Itr->second.m_outfiletype.c_str());
           }
        }
    }
    if(m_ferrCdrs>0)
   {
      theLogger->pOutput(theLoggerProcessID,m_gFilterInfo->m_ferrfullPathName.c_str(),m_ferrCdrs,"channel_type=ferr,out_type=0");
	 }
    return true;
}

map<PString,LOGINFO> &SortLog::getLogs()
{
    return m_logInfo;
}

bool SortLog::commit()
{
   write();
   theLogger->pEnd(theLoggerProcessID);
   return true;
}

void SortLog::addFerrCdr()
{
  m_ferrCdrs++;
}

