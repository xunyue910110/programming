// ##########################################
// Source file : PpLog.cpp
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

#include "PpLog.h"
#include "base/StringUtil.h"
#include "OutputFile.h"

PpLog * PpLog::m_thePpLog = 0;

//##ModelId=4855C6C60167
PpLog::~PpLog()
{
    clear();
}

//##ModelId=4855C7D90186
void PpLog::clear()
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
    m_UpTotalFee = 0;
    m_TotalFee = 0;
}

//##ModelId=4855C80F0242
bool PpLog::write()
{
    return true;
}

//##ModelId=4855C81F03D8
void PpLog::ShowMessage(string theMessage, int ShowFlag)
{
    if (isShowFlag())
    {
        cout << theMessage << flush;
    }
}

//##ModelId=4855C84B0196
void PpLog::setFileName(const string &filename)
{
    m_SrcFileName = filename;
}

//##ModelId=4855C8940167
void PpLog::setEndDateTime(string &left)
{
    m_EndDateTime = left;
}

//##ModelId=4855C8A80186
void PpLog::addErrorSum()
{
    m_ErrorSum++;
}

//##ModelId=4855C8B90280
void PpLog::addForwSum()
{
    m_ForwSum++;
}

//##ModelId=4855CA360232
void PpLog::addMocSum()
{
    m_MocSum++;
}

//##ModelId=4855CA3F0167
void PpLog::addMtcSum()
{
    m_MtcSum++;
}

//##ModelId=4855CA460290
void PpLog::addNoUseSum()
{
    m_NoUseSum++;
}

//##ModelId=4855CA4F0213
void PpLog::addPocSum()
{
    m_PocSum++;
}

//##ModelId=4855CAAF000F
void PpLog::addPtcSum()
{
    m_PtcSum++;
}

//##ModelId=4855CAB8004E
void PpLog::addRoamSum()
{
    m_RoamSum++;
}

//##ModelId=4855CAC20138
void PpLog::addSmocSum()
{
    m_SmocSum++;
}

//##ModelId=4855CACA0167
void PpLog::addSmtcSum()
{
    m_SmtcSum++;
}

//##ModelId=4855CAD2004E
void PpLog::addOtherSum()
{
    m_OtherSum++;
}

//##ModelId=4855CADC02FD
void PpLog::addTotalCdrs()
{
    m_TotalCdrs++;
}

void PpLog::addTotalFee(int CdrFee)
{
    m_TotalFee += CdrFee;
}

//##ModelId=4855CAE602FD
void PpLog::setSrcFileName(string &left)
{
    return;
}

//##ModelId=4855CB22001F
string PpLog::getEndDateTime() const
{
    return m_EndDateTime;
}

//##ModelId=4855CBC803A9
int PpLog::getErrorSum() const
{
    return m_ErrorSum;
}

//##ModelId=4855CC02003E
int PpLog::getForwSum() const
{
    return m_ForwSum;
}

//##ModelId=4855CC9103A9
bool PpLog::getisShow() const
{
    return m_isShow;
}

//##ModelId=4855CCA20399
int PpLog::getMocSum() const
{
    return m_MocSum;
}

//##ModelId=4855CCB000FA
int PpLog::getMtcSum() const
{
    return m_MtcSum;
}

//##ModelId=4855CCBF0157
int PpLog::getNoUseSum() const
{
    return m_NoUseSum;
}

//##ModelId=4855CCDE0261
int PpLog::getOtherSum() const
{
    return m_OtherSum;
}

//##ModelId=4855CCEC0196
int PpLog::getPocSum() const
{
    return m_PocSum;
}

//##ModelId=4855CCF80399
int PpLog::getPtcSum() const
{
    return m_PtcSum;
}

//##ModelId=4855CD0C0280
int PpLog::getRoamSum() const
{
    return m_RoamSum;
}

//##ModelId=4855CD19031C
int PpLog::getSmocSum() const
{
    return m_SmocSum;
}

//##ModelId=4855CD28008C
int PpLog::getSmtcSum() const
{
    return m_SmtcSum;
}

//##ModelId=4855CD370251
string PpLog::getSrcFileName() const
{
    return m_SrcFileName;
}

//##ModelId=4855CD53007D
string PpLog::getStartDateTime() const
{
    return m_StartDateTime;
}

//##ModelId=4855CD6903A9
time_t PpLog::getStartDateTime_tm() const
{
    return m_tNowTime;
}

//##ModelId=4855CD7E001F
int PpLog::getTotalCdrs() const
{
    return m_TotalCdrs;
}

int PpLog::getTotalFee() const
{
    return m_TotalFee;
}

//##ModelId=4855CD9503C8
void PpLog::addDelaySum1()
{
    m_delaySum1++;
}

//##ModelId=4855CDB4034B
void PpLog::addDelaySum2()
{
    m_delaySum2++;
}

//##ModelId=4855CDB7009C
void PpLog::addDelaySum3()
{
    m_delaySum3++;
}

//##ModelId=4855CDB8009C
void PpLog::addDelaySum4()
{
    m_delaySum4++;
}

//##ModelId=4855CDB9031C
void PpLog::addDelaySum5()
{
    m_delaySum5++;
}

//##ModelId=4855CDBA0290
void PpLog::addDelaySum6()
{
    m_delaySum6++;
}

//##ModelId=4855CDBB0280
void PpLog::addDelaySum7()
{
    m_delaySum7++;
}

//##ModelId=4855CDBC01F4
void PpLog::addDelaySum8()
{
    m_delaySum8++;
}

//##ModelId=4855CDBD00EA
void PpLog::addDelaySum9()
{
    m_delaySum9++;
}

//##ModelId=4855CDBD038A
int PpLog::getDelaySum1()
{
    return m_delaySum1;
}

//##ModelId=4855CDFC01C5
int PpLog::getDelaySum2()
{
    return m_delaySum2;
}

//##ModelId=4855CDFD032C
int PpLog::getDelaySum3()
{
    return m_delaySum3;
}

//##ModelId=4855CDFE037A
int PpLog::getDelaySum4()
{
    return m_delaySum4;
}

//##ModelId=4855CE000119
int PpLog::getDelaySum5()
{
    return m_delaySum5;
}

//##ModelId=4855CE060251
int PpLog::getDelaySum6()
{
    return m_delaySum6;
}

//##ModelId=4855CE0701C5
int PpLog::getDelaySum7()
{
    return m_delaySum7;
}

//##ModelId=4855CE08006D
int PpLog::getDelaySum8()
{
    return m_delaySum8;
}

//##ModelId=4855CE0802EE
int PpLog::getDelaySum9()
{
    return m_delaySum9;
}

//##ModelId=4855CE6600CB
void PpLog::setFileLenth(const int fileLength)
{
    m_fileLenth = fileLength;
    return;
}

//##ModelId=4855CE7E0196
void PpLog::setDesFileLenth(const int fileLength)
{
    m_desFileLenth = fileLength;
    return;
}

//##ModelId=4855CE90008C
int PpLog::getFileLenth()
{
    return m_fileLenth;
}

//##ModelId=4855CE9A02DE
int PpLog::getDesFileLenth()
{
    return m_desFileLenth;
}

//##ModelId=4855CEA503B9
void PpLog::setSourceType(const string &strSourceType)
{
    m_sourceType = strSourceType;
    return;
}

//##ModelId=4855CECC029F
const string & PpLog::getSourceType()
{
    return m_sourceType;
}

//##ModelId=4855CF1A030D
void PpLog::setMinMaxStartDate(const char* pStartDate, const char * pStartTime)
{
    char cTmpDateTime[20];
    sprintf(cTmpDateTime,"%s%s",pStartDate,pStartTime);
    if(strcmp(cTmpDateTime,m_MinStartDateTime.c_str())<0 || m_MinStartDateTime.compare("0")==0)
    {
        m_MinStartDateTime = cTmpDateTime;
    }

    if( strcmp(cTmpDateTime,m_MaxStartDateTime.c_str())>0 || m_MaxStartDateTime.compare("0")==0)
    {
        m_MaxStartDateTime = cTmpDateTime;
    }
    return;
}

//##ModelId=4855CF4D0196
const string & PpLog::getMaxStartDateTime() const
{
    return m_MaxStartDateTime;
}

//##ModelId=4855CF5F030D
const string & PpLog::getMinStartDateTime() const
{
    return m_MinStartDateTime;
}

//##ModelId=4855D021000F
int PpLog::getBeginCdrNo() const
{
    return m_beginCdrNo;
}

//##ModelId=4855D05800EA
void PpLog::setBeginCdrNo(int left)
{
    m_beginCdrNo = left;
    return;
}

//##ModelId=4855D06D009C
int PpLog::getEndCdrNo() const
{
    return m_endCdrNo;
}

//##ModelId=4855D080038A
void PpLog::setEndCdrNo(int left)
{
    m_endCdrNo = left;
    return;
}

//##ModelId=4855E27E038A
int PpLog::getLogField()
{
    int i;
    if(m_qualityLog == "UNLOG")
    {
        m_isDoLog = 0;
        return m_isDoLog;
    }
    else if(string::npos == m_qualityLog.find_first_of('+'))
    {
        m_LogFields.push_back(m_qualityLog);
        m_isDoLog = 1;
        return m_isDoLog;
    }
    else
    {
        StringUtil::split(m_qualityLog, "+", m_LogFields);
        m_isDoLog = 1;
        return m_isDoLog;
    }
}



//##ModelId=4855E2A50280
void PpLog::getBeginDealTime()
{
    setStartDateTime();
    m_beginDealTime = m_StartDateTime;
}

//##ModelId=4855E2B00157
void PpLog::getEndDealTime()
{
    setStartDateTime();
    m_endDealTime = m_StartDateTime;
}

//##ModelId=4855D137031C
PpLog::PpLog()
{
    m_fileNo = -1;
}

//##ModelId=4855D1D90177
void PpLog::setStartDateTime()
{
    struct tm* stNowTime;
    char tmp[15];

    time(&m_tNowTime);
    stNowTime = localtime(&m_tNowTime);
    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "%04d%02d%02d%02d%02d%02d", stNowTime->tm_year + 1900,
                stNowTime->tm_mon + 1, stNowTime->tm_mday,
                stNowTime->tm_hour, stNowTime->tm_min, stNowTime->tm_sec);
    
    m_StartDateTime = tmp;
}

void PpLog::setOutPutFile(OutputFile *pOutPutFile)
{
	m_OutPutFile = pOutPutFile;
}

OutputFile * PpLog::getOutPutFile()
{
    return m_OutPutFile;
}

void PpLog::WriteCdrtofile()
{
	m_OutPutFile->write();
}