// ##########################################
// Source file : SendLog.cpp
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

#include "SendLog.h"

SendLog * SendLog::m_theSendLog = 0;

//##ModelId=3B62167F038E
SendLog::SendLog()
{
   m_fileNo = -1;
}

//##ModelId=3B621680017D
SendLog::~SendLog()
{
	clear();
}

//##ModelId=3BD8CCED02AC
void SendLog::setEndDateTime(string &left)
{
    m_EndDateTime = left;
}

//##ModelId=3BD8CCF0022F
void SendLog::setStartDateTime()
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
bool SendLog::write()
{
	return true;
}

//##ModelId=3B57CCAB01BB
void SendLog::clear()
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
    
    m_errFeeSum=0;
    m_ErrorSum=0;
    m_feeSum=0;
    
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
	m_MinStartDateTime= "";     
  m_MaxStartDateTime= "";     

}

//##ModelId=3B7CB8B002EB
void  SendLog::ShowMessage(string theMessage,int ShowFlag)
{
	if ( isShowFlag() )
		cout<<theMessage<<flush;
}

//##ModelId=3B70EFB50350
void SendLog::setFileName(string filename)
{
   m_SrcFileName=filename;
}

void SendLog::setDesFileName(string filename)
{
   m_DesFileName=filename;
}

//##ModelId=3BD8CCED039D
void SendLog::addErrorSum()
{
    m_ErrorSum++;
}


//##ModelId=3BD8CCEE0087
void SendLog::addForwSum()
{
    m_ForwSum++;
}

//##ModelId=3BD8CCEE022C
void SendLog::addMocSum()
{
    m_MocSum++;
}

//##ModelId=3BD8CCEE0308
void SendLog::addMtcSum()
{
    m_MtcSum++;
}

//##ModelId=3BD8CCEE03DA
void SendLog::addNoUseSum()
{
    m_NoUseSum++;
}

//##ModelId=3BD8CCEF01AB
void SendLog::addPocSum()
{
    m_PocSum++;
}

//##ModelId=3C9F3E130309
void SendLog::addPtcSum()
{
    m_PtcSum++;
}

//##ModelId=3BD8CCEF0287
void SendLog::addRoamSum()
{
    m_RoamSum++;
}

//##ModelId=3BD8CCEF036E
void SendLog::addSmocSum()
{
    m_SmocSum++;
}

//##ModelId=3BD8CCF00062
void SendLog::addSmtcSum()
{
    m_SmtcSum++;
}

//##ModelId=3BD8CCF0031F
void SendLog::addTotalCdrs()
{
    m_TotalCdrs++;
}

void SendLog::addTotalFee(int CdrFee)
{
    m_feeSum += CdrFee;
}

void SendLog::addErrorFee(int ErrFee)
{
    m_errFeeSum += ErrFee;
}

//##ModelId=3BD8E9AC0196
void SendLog::addOtherSum()
{
    m_OtherSum++;
}


//##ModelId=3BD8EB6400EE
string SendLog::getEndDateTime() const
{
    return m_EndDateTime;
}


//##ModelId=3BD8EB640152
int SendLog::getErrorSum() const
{
    return m_ErrorSum;
}


//##ModelId=3BD8EB6401B7
int SendLog::getForwSum() const
{
    return m_ForwSum;
}


//##ModelId=3BD8EB640225
bool SendLog::getisShow() const
{
    return m_isShow;
}


//##ModelId=3BD8EB6402A7
int SendLog::getMocSum() const
{
    return m_MocSum;
}


//##ModelId=3BD8EB640315
int SendLog::getMtcSum() const
{
    return m_MtcSum;
}

//##ModelId=3BD8EB64038D
int SendLog::getNoUseSum() const
{
    return m_NoUseSum;
}


//##ModelId=3BD8EB650013
int SendLog::getOtherSum() const
{
    return m_OtherSum;
}


//##ModelId=3BD8EB65008C
int SendLog::getPocSum() const
{
    return m_PocSum;
}

//##ModelId=3C9F3E130331
int SendLog::getPtcSum() const
{
    return m_PtcSum;
}

//##ModelId=3BD8EB650104
int SendLog::getRoamSum() const
{
    return m_RoamSum;
}


//##ModelId=3BD8EB650186
int SendLog::getSmocSum() const
{
    return m_SmocSum;
}


//##ModelId=3BD8EB6501FE
int SendLog::getSmtcSum() const
{
    return m_SmtcSum;
}


//##ModelId=3BD8EB650280
string SendLog::getSrcFileName() const
{
    return m_SrcFileName;
}

string SendLog::getDesFileName() const
{
    return m_DesFileName;
}

//##ModelId=3BD8EB650302
string SendLog::getStartDateTime() const
{
    return m_StartDateTime;
}

time_t SendLog::getStartDateTime_tm() const
{
	return m_tNowTime;	
}

//##ModelId=3BD8EB650385
int SendLog::getTotalCdrs() const
{
    return m_TotalCdrs;
}

int SendLog::getTotalFee() const
{
    return m_feeSum;
}

int SendLog::getErrorFee() const
{
    return m_errFeeSum;
}

//##ModelId=3BFDB1D4026A
int SendLog::getBeginCdrNo() const
{
    return m_beginCdrNo;
}


//##ModelId=3BFDB1D40294
void SendLog::setBeginCdrNo(int left)
{
    m_beginCdrNo = left;
}


//##ModelId=3BFDB1D40329
int SendLog::getEndCdrNo() const
{
    return m_endCdrNo;
}


//##ModelId=3BFDB1D40352
void SendLog::setEndCdrNo(int left)
{
    m_endCdrNo = left;
}

//以下方法用于时效统计
void SendLog::addDelaySum1()
{
	m_delaySum1++;	
}
void SendLog::addDelaySum2()
{
	m_delaySum2++;	
}
void SendLog::addDelaySum3()
{
	m_delaySum3++;	
}
void SendLog::addDelaySum4()
{
	m_delaySum4++;	
}
void SendLog::addDelaySum5()
{
	m_delaySum5++;	
}
void SendLog::addDelaySum6()
{
	m_delaySum6++;	
}
void SendLog::addDelaySum7()
{
	m_delaySum7++;	
}
void SendLog::addDelaySum8()
{
	m_delaySum8++;	
}
void SendLog::addDelaySum9()
{
	m_delaySum9++;	
}
//以下方法用于得到统计数据
int SendLog::getDelaySum1()
{
	return m_delaySum1;	
}
int SendLog::getDelaySum2()
{
	return m_delaySum2;	
}
int SendLog::getDelaySum3()
{
	return m_delaySum3;	
}
int SendLog::getDelaySum4()
{
	return m_delaySum4;	
}
int SendLog::getDelaySum5()
{
	return m_delaySum5;	
}
int SendLog::getDelaySum6()
{
	return m_delaySum6;	
}
int SendLog::getDelaySum7()
{
	return m_delaySum7;	
}
int SendLog::getDelaySum8()
{
	return m_delaySum8;	
}
int SendLog::getDelaySum9()
{
	return m_delaySum9;	
}
void SendLog::setFileLenth(const int fileLength)
{
	m_fileLenth = fileLength;
	return;	
}
int SendLog::getFileLenth()
{
	return m_fileLenth;	
}

void SendLog::setSourceType(const string &strSourceType)
{
	m_sourceType = strSourceType;
	return;	
}

const string & SendLog::getSourceType()
{
	return m_sourceType;	
}
// add by chenm 2006-1-4 记录当前原始文件中cdr的最早开始时间和最晚开始时间
void SendLog::setMinMaxStartDate(const string & left)
{
	//if( left < m_MinStartDateTime || m_MinStartDateTime=="0" )
	if( left < m_MinStartDateTime || m_MinStartDateTime=="" )
	{
		m_MinStartDateTime = left;	
	}
	//if(left > m_MaxStartDateTime || m_MaxStartDateTime=="0")
	if(left > m_MaxStartDateTime || m_MaxStartDateTime=="")    
    {
    	m_MaxStartDateTime = left;
    }
	return;
}
string SendLog::getMaxStartDateTime() const
{
	return m_MaxStartDateTime;
}

string SendLog::getMinStartDateTime() const
{
	return m_MinStartDateTime;
}

int SendLog::getDesFileLenth()
{
	return m_desFileLenth;	
}
void SendLog::setDesFileLenth(const int fileLength)
{
	m_desFileLenth = fileLength;
	return;	
}


