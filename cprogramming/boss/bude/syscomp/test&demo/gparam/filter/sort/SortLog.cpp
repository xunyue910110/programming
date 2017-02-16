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

#include "SortLog.h"

//##ModelId=3BE9DF2F0357
void SortLog::clear()
{
    int size = m_channelSum.size();

    m_isShow          = false;
    m_TotalCdrs       = 0;
    m_nrSum           = 0;
    m_irSum           = 0;
    m_noinfoSum       = 0;
    m_pocSum          = 0;
    m_specialSum      = 0;
    m_pocNoinfoSum    = 0;
    m_trashSum        = 0;
    m_errorSum        = 0;

    for(int i=0;i<size;i++)
    {
        m_channelSum[i]=0;
    }
}

//##ModelId=3BE9DF360018
SortLog::SortLog()
{
   clear();
}

//##ModelId=3BE9DF450272
SortLog::~SortLog()
{
}

//##ModelId=3BE9E05F0250
void SortLog::showLog()
{
    int size;
    char chann[3];

    size = m_channelSum.size();

    cout<<"src file is "<<getSrcFileName()<<endl;

    for(int i=0;i<size;i++)
    {
        sprintf(chann,"% 2d",i);
        cout<<"channel        "<<chann<<"  sum  is "<<getChannelSum(i) <<endl;
    }
    cout<<"channel         nr  sum  is "<<getNrSum()    <<endl;
    cout<<"channel         ir  sum  is "<<getIrSum()    <<endl;
    cout<<"channel      trash  sum  is "<<getTrashSum() <<endl;
    cout<<"channel      error  sum  is "<<getErrorSum() <<endl;
    cout<<"channel     noinfo  sum  is "<<getNoinfoSum()<<endl;
    cout<<"channel        poc  sum  is "<<getPocSum()   <<endl;
    cout<<"channel   Speciala  sum  is "<<getSpecialSum() <<endl;
    cout<<"channel  pocNoinfo  sum  is "<<getPocNoinfoSum()<<endl;

    cout<<"-----------------------------------"<<endl;
    cout<<"channel  total  Sum  is "<<getTotalCdrs()<<endl<<endl;
}

//##ModelId=3BE9E0DC02D2
bool SortLog::isShowFlag()
{
   return m_isShow;
}

//##ModelId=3BE9E14201AC
int SortLog::getErrorSum() const
{
    return m_errorSum;
}


//##ModelId=3BE9E142021A
void SortLog::addErrorSum()
{
    m_errorSum++;
}


//##ModelId=3BE9E142030A
int SortLog::getIrSum() const
{
    return m_irSum;
}


//##ModelId=3BE9E1420383
void SortLog::addIrSum()
{
    m_irSum++;
}


//##ModelId=3BE9E1430095
int SortLog::getNoinfoSum() const
{
    return m_noinfoSum;
}


//##ModelId=3BE9E143010D
void SortLog::addNoinfoSum()
{
    m_noinfoSum++;
}


//##ModelId=3BE9E1430207
int SortLog::getNrSum() const
{
    return m_nrSum;
}

//##ModelId=3BE9E1430280
void SortLog::addNrSum()
{
    m_nrSum++;
}

//##ModelId=3C85D0C5016A
string SortLog::getSrcFileName() const
{
    return m_SrcFileName;
}


//##ModelId=3BE9E144001E
void SortLog::setSrcFileName(string left)
{
    m_SrcFileName = left;
}

//##ModelId=3BE9E1440137
int SortLog::getTotalCdrs() const
{
    return m_TotalCdrs;
}

//##ModelId=3BE9E14401B9
void SortLog::addTotalCdrs()
{
    m_TotalCdrs++;
}


//##ModelId=3BE9E14402D1
int SortLog::getTrashSum() const
{
    return m_trashSum;
}

//##ModelId=3BE9E1440353
void SortLog::addTrashSum()
{
    m_trashSum++;
}

//##ModelId=3C1EF6FF02E7
int SortLog::getSpecialSum() const
{
    return m_specialSum;
}


//##ModelId=3C1EF7000053
void SortLog::addSpecialSum()
{
    m_specialSum++;
}


//##ModelId=3C1EF7000162
int SortLog::getPocSum() const
{
    return m_pocSum;
}


//##ModelId=3C1EF70001A8
void SortLog::addPocSum()
{
    m_pocSum++;
}


//##ModelId=3C1EF7000270
int SortLog::getPocNoinfoSum() const
{
    return m_pocNoinfoSum;
}


//##ModelId=3C1EF7000307
void SortLog::addPocNoinfoSum()
{
    m_pocNoinfoSum++;
}

//##ModelId=3C84DCA202F6
void SortLog::addChannelSum(const int channel)
{
    int size;

    size = m_channelSum.size();
    if(channel<=size) 
    {
        m_channelSum[channel]++;
    }
}

//##ModelId=3C84DCAE02F3
int SortLog::getChannelSum(const int channel) const
{
    int size;

    size = m_channelSum.size();
    if(channel<=size) {
        return m_channelSum[channel];
    }

    return 0;
}
//##ModelId=3C84E761019D
void SortLog::initialize(const int channel)
{
    for(int i=0;i<channel;i++)
    {
        m_channelSum.push_back(0);
    }
}

//##ModelId=3C8579DD019F
int SortLog::getChannelCount()
{
    return m_channelSum.size();
}

//##ModelId=3C887254015B
int SortLog::getUpdataTimeLimen() const
{
    return m_updataTimeLimen;
}

//##ModelId=3C8872910181
void SortLog::setUpdataCDRsLimen(const int cdrsLimen)
{
    m_updataCDRsLimen=cdrsLimen;
}

//##ModelId=3C8872D502AB
void SortLog::setUpdataTimeLimen(const int timeLimen)
{
    m_updataTimeLimen=timeLimen;
}

//##ModelId=3C8872F0006F
int SortLog::getUpdataCDRsLimen() const
{
    return m_updataCDRsLimen;
}

