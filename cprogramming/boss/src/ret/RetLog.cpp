// ##########################################
// Source file : RetLog.cpp
// System      : Mobile Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 2000325
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "RetLog.h"

RetLog * RetLog::m_theRetLog = 0;

//##ModelId=3B62167F038E
RetLog::RetLog()
{
   m_fileNo = -1;
}

//##ModelId=3B621680017D
RetLog::~RetLog()
{
	clear();
}

//##ModelId=3BD8CCED02AC
void RetLog::setEndDateTime(string &left)
{
    m_EndDateTime = left;
}

//##ModelId=3BD8CCF0022F
void RetLog::setStartDateTime()
{
    //time_t lNowTime;
    struct tm* stNowTime;
	char tmp[15];
	
	time(&m_tNowTime);
	stNowTime = localtime(&m_tNowTime);
    memset(tmp,0,sizeof(tmp));
	sprintf(tmp,"%04d%02d%02d%02d%02d%02d",stNowTime->tm_year + 1900,stNowTime->tm_mon+1,stNowTime->tm_mday,
									stNowTime->tm_hour,stNowTime->tm_min,stNowTime->tm_sec);
	
	m_StartDateTime = tmp;
}

//##ModelId=3B70F1FB00F4
bool RetLog::write()
{
	return true;
}

//##ModelId=3B57CCAB01BB
void RetLog::clear()
{
    m_TotalCdrs = 0;
    m_MocSum    = 0;
    m_MtcSum    = 0;
    m_ForwSum   = 0;
    m_RoamSum   = 0;
    m_PocSum    = 0;
    m_PtcSum    = 0;
    m_SmocSum   = 0;
    m_SmtcSum   = 0;
    m_OtherSum  = 0;
    m_NoUseSum  = 0;
    m_ErrorSum  = 0;
    m_beginCdrNo= 0;
    m_endCdrNo  = 0;
    
    //时效统计数据
    m_delaySum1 = 0;
	m_delaySum2 = 0;
	m_delaySum3 = 0;
	m_delaySum4 = 0;
	m_delaySum5 = 0;
	m_delaySum6 = 0;
	m_delaySum7 = 0;
	m_delaySum8 = 0;
	m_delaySum9 = 0;
	
	m_fileLenth = 0;
	m_desFileLenth = 0;
	
	//获得处理时间dealtime
	setStartDateTime();
	m_MinStartDateTime= "0";     
    m_MaxStartDateTime= "0";     

}

//##ModelId=3B7CB8B002EB
void  RetLog::ShowMessage(string theMessage,int ShowFlag)
{
	if ( isShowFlag() )
		cout<<theMessage<<flush;
}

//##ModelId=3B70EFB50350
void RetLog::setFileName(string filename)
{
   m_SrcFileName=filename;
}

//##ModelId=3BD8CCED039D
void RetLog::addErrorSum()
{
    m_ErrorSum++;
}


//##ModelId=3BD8CCEE0087
void RetLog::addForwSum()
{
    m_ForwSum++;
}

//##ModelId=3BD8CCEE022C
void RetLog::addMocSum()
{
    m_MocSum++;
}

//##ModelId=3BD8CCEE0308
void RetLog::addMtcSum()
{
    m_MtcSum++;
}

//##ModelId=3BD8CCEE03DA
void RetLog::addNoUseSum()
{
    m_NoUseSum++;
}

//##ModelId=3BD8CCEF01AB
void RetLog::addPocSum()
{
    m_PocSum++;
}

//##ModelId=3C9F3E130309
void RetLog::addPtcSum()
{
    m_PtcSum++;
}

//##ModelId=3BD8CCEF0287
void RetLog::addRoamSum()
{
    m_RoamSum++;
}

//##ModelId=3BD8CCEF036E
void RetLog::addSmocSum()
{
    m_SmocSum++;
}

//##ModelId=3BD8CCF00062
void RetLog::addSmtcSum()
{
    m_SmtcSum++;
}

//##ModelId=3BD8CCF0031F
void RetLog::addTotalCdrs()
{
    m_TotalCdrs++;
}


//##ModelId=3BD8E9AC0196
void RetLog::addOtherSum()
{
    m_OtherSum++;
}


//##ModelId=3BD8EB6400EE
string RetLog::getEndDateTime() const
{
    return m_EndDateTime;
}


//##ModelId=3BD8EB640152
int RetLog::getErrorSum() const
{
    return m_ErrorSum;
}


//##ModelId=3BD8EB6401B7
int RetLog::getForwSum() const
{
    return m_ForwSum;
}


//##ModelId=3BD8EB640225
bool RetLog::getisShow() const
{
    return m_isShow;
}


//##ModelId=3BD8EB6402A7
int RetLog::getMocSum() const
{
    return m_MocSum;
}


//##ModelId=3BD8EB640315
int RetLog::getMtcSum() const
{
    return m_MtcSum;
}

//##ModelId=3BD8EB64038D
int RetLog::getNoUseSum() const
{
    return m_NoUseSum;
}


//##ModelId=3BD8EB650013
int RetLog::getOtherSum() const
{
    return m_OtherSum;
}


//##ModelId=3BD8EB65008C
int RetLog::getPocSum() const
{
    return m_PocSum;
}

//##ModelId=3C9F3E130331
int RetLog::getPtcSum() const
{
    return m_PtcSum;
}

//##ModelId=3BD8EB650104
int RetLog::getRoamSum() const
{
    return m_RoamSum;
}


//##ModelId=3BD8EB650186
int RetLog::getSmocSum() const
{
    return m_SmocSum;
}


//##ModelId=3BD8EB6501FE
int RetLog::getSmtcSum() const
{
    return m_SmtcSum;
}


//##ModelId=3BD8EB650280
string RetLog::getSrcFileName() const
{
    return m_SrcFileName;
}


//##ModelId=3BD8EB650302
string RetLog::getStartDateTime() const
{
    return m_StartDateTime;
}

time_t RetLog::getStartDateTime_tm() const
{
	return m_tNowTime;	
}

//##ModelId=3BD8EB650385
int RetLog::getTotalCdrs() const
{
    return m_TotalCdrs;
}


//##ModelId=3BFDB1D4026A
int RetLog::getBeginCdrNo() const
{
    return m_beginCdrNo;
}


//##ModelId=3BFDB1D40294
void RetLog::setBeginCdrNo(int left)
{
    m_beginCdrNo = left;
}


//##ModelId=3BFDB1D40329
int RetLog::getEndCdrNo() const
{
    return m_endCdrNo;
}


//##ModelId=3BFDB1D40352
void RetLog::setEndCdrNo(int left)
{
    m_endCdrNo = left;
}

//以下方法用于时效统计
void RetLog::addDelaySum1()
{
	m_delaySum1++;	
}
void RetLog::addDelaySum2()
{
	m_delaySum2++;	
}
void RetLog::addDelaySum3()
{
	m_delaySum3++;	
}
void RetLog::addDelaySum4()
{
	m_delaySum4++;	
}
void RetLog::addDelaySum5()
{
	m_delaySum5++;	
}
void RetLog::addDelaySum6()
{
	m_delaySum6++;	
}
void RetLog::addDelaySum7()
{
	m_delaySum7++;	
}
void RetLog::addDelaySum8()
{
	m_delaySum8++;	
}
void RetLog::addDelaySum9()
{
	m_delaySum9++;	
}
//以下方法用于得到统计数据
int RetLog::getDelaySum1()
{
	return m_delaySum1;	
}
int RetLog::getDelaySum2()
{
	return m_delaySum2;	
}
int RetLog::getDelaySum3()
{
	return m_delaySum3;	
}
int RetLog::getDelaySum4()
{
	return m_delaySum4;	
}
int RetLog::getDelaySum5()
{
	return m_delaySum5;	
}
int RetLog::getDelaySum6()
{
	return m_delaySum6;	
}
int RetLog::getDelaySum7()
{
	return m_delaySum7;	
}
int RetLog::getDelaySum8()
{
	return m_delaySum8;	
}
int RetLog::getDelaySum9()

{
	return m_delaySum9;	
}
void RetLog::setFileLenth(const int fileLength)
{
	m_fileLenth = fileLength;
	return;	
}
int RetLog::getFileLenth()
{
	return m_fileLenth;	
}

void RetLog::setSourceType(const string &strSourceType)
{
	m_sourceType = strSourceType;
	return;	
}

const string & RetLog::getSourceType()
{
	return m_sourceType;	
}
// add by chenm 2006-1-4 记录当前原始文件中cdr的最早开始时间和最晚开始时间
void RetLog::setMinMaxStartDate(const string & left)
{
	if( left < m_MinStartDateTime || m_MinStartDateTime=="0" )
	{
		m_MinStartDateTime = left;	
	}
	
	if(left > m_MaxStartDateTime || m_MaxStartDateTime=="0")    
    {
    	m_MaxStartDateTime = left;
    }
	return;
}
string RetLog::getMaxStartDateTime() const
{
	return m_MaxStartDateTime;
}

string RetLog::getMinStartDateTime() const
{
	return m_MinStartDateTime;
}

int RetLog::getDesFileLenth()
{
	return m_desFileLenth;	
}
void RetLog::setDesFileLenth(const int fileLength)
{
	m_desFileLenth = fileLength;
	return;	
}


