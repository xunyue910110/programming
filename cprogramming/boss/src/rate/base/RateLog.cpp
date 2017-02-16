#include "RateLog.h"
#include "hessian/exceptions.h"
#include "log4cpp/Logger.h"
#include "base/StringUtil.h"

extern log4cpp::Logger *theLogger;
extern string theLoggerProcessID;
void RateLog::reset()
{
  m_fileName.clear();
  m_inputCdrs =0;
  m_normalCdrs=0;
  m_errorCdrs =0; 
  m_delayedCdrs  =0; 
  m_balRcds = 0;
  //Added By Sunam
  m_remindCdrs = 0;
  //Added By Sunam 2009/9/29 14:21:03
  m_userSumBills =0;
  for(int i=0;i<2;++i)
  {
    m_normalCdr[i]=m_userBills[i]=m_acctBills[i]=0;
    m_fixRcds[i]=m_discntRcds[i]=m_otherRcds[i]=0;
    m_monfeeBBill[i]=m_monfeeIBill[i]=m_monfeePBill[i]=m_monfeeMBill[i]=0;
    //add by cuiyw 20090915
    m_wirelessBills[i]=0;
    //end by cuiyw 20090915
    m_baseFee      [i] =0;
    m_totalFee     [i] =0;
    m_totalDuration[i] =0;
    m_totalData    [i] =0;
	m_adjustBfeoreBill[i]=0;
  }
}
void RateLog::setFileName(const char * r_fileName)
{
  reset();
  m_fileName = r_fileName;
  time(&m_beginDealTime);
}
void RateLog::setDelayFileName(const char *r_delayFullFileName)
{
  m_delayFileName = r_delayFullFileName;
}
void RateLog::addTotalCdr()
{
  ++m_inputCdrs;
}

void RateLog::addErrCdr()
{
  ++m_errorCdrs;
}
void RateLog::addUserBills(const int &r_cycleIdx)
{
  ++(m_userBills[r_cycleIdx]);
}
void RateLog::addAcctBills(const int &r_cycleIdx)
{
  ++(m_acctBills[r_cycleIdx]);
}
void RateLog::addBalRcds(const int &r_cycleIdx)
{
  ++m_balRcds;
}
//Added By Sunam 2009/8/8 19:28:49
void RateLog::addRemindRcds(const int &r_cycleIdx)
{
  ++m_remindCdrs;
}
//Added By Sunam 2009/9/29 10:00:49
void RateLog::adduserSumBills()
{
  ++m_userSumBills;
}
void RateLog::addDiscntRcds(const int &r_cycleIdx)
{
  ++(m_discntRcds[r_cycleIdx]);
}
void RateLog::addFixRcds(const int &r_cycleIdx)
{
  ++(m_fixRcds[r_cycleIdx]);
}
void RateLog::addOtherRcds(const int &r_cycleIdx)
{
  ++(m_otherRcds[r_cycleIdx]);
}
//add by cuiyw 20090915
void RateLog::addWirelessBills(const int &r_cycleIdx)
{
  ++(m_wirelessBills[r_cycleIdx]);
}
//add by cuiyw 20090915

//add by dingr 2009/09/17
void RateLog::addAdjustBeforeBills(const int &r_cycleIdx)
{
  ++(m_adjustBfeoreBill[r_cycleIdx]);
}
void RateLog::addMonfeeBBills(const int &r_cycleIdx)
{
	  ++(m_monfeeBBill[r_cycleIdx]);
}
void RateLog::addMonfeeIBills(const int &r_cycleIdx)
{
	  ++(m_monfeeIBill[r_cycleIdx]);
}
void RateLog::addMonfeePBills(const int &r_cycleIdx)
{
	  ++(m_monfeePBill[r_cycleIdx]);
}   
void RateLog::addMonfeeMBills(const int &r_cycleIdx)
{
	  ++(m_monfeeMBill[r_cycleIdx]);
}  
void RateLog::addNormalCdr(const int  &r_cycleIdx,
                           const long &r_baseFee,
                           const long &r_totalFee,
                           const long &r_totalDuration,
                           const long &r_totalData)
{
  ++(m_normalCdr[r_cycleIdx]);
  ++m_normalCdrs;
  m_baseFee[r_cycleIdx]      += r_baseFee;
  m_totalFee[r_cycleIdx]     += r_totalFee;
  m_totalDuration[r_cycleIdx]+= r_totalDuration;
  m_totalData[r_cycleIdx]    += r_totalData;
}

void RateLog::addDelayCdr()
{
  ++m_delayedCdrs;
}
void RateLog::setOCdrFileName(const int &r_cycleIdx,
                              const int &r_errCode,
                              const char *r_oFileName)
{
  if(r_errCode == 0)
  {
    m_outCdrFileName[r_cycleIdx] = r_oFileName;
  }else
    m_outCdrFileName[2] = r_oFileName;
}
void RateLog::setOCdrFileName(const int &r_cycleIdx,const int &r_errCode,
                              const char* r_path,const char * r_fileName)
{
  int t_index = 0;
  if(r_errCode == 0)
  {
    t_index = r_cycleIdx;
  }else
  {
    t_index = 2;
  }
  m_outCdrFileName[t_index] = r_path;
  if(strlen(r_path)>0)
  {
    if(r_path[strlen(r_path)-1] != '/')
    {
      m_outCdrFileName[t_index]+= "/";
    }
  }
  m_outCdrFileName[t_index]+= r_fileName;
}

void RateLog::writeLog()
{
  time(&m_endDealTime);
  if(m_inputCdrs <= 0) return;

  //输出输入文件的日志
  theLogger->pInput(theLoggerProcessID,m_fullFileName.c_str(),m_inputCdrs,
        "type=in,call_duration=0,data=0,baseFee=0,FEE=0");
  if(m_delayedCdrs>0)
  { //删除延迟话单的日志
    theLogger->pOutput(theLoggerProcessID,m_delayFileName.c_str(),
              m_delayedCdrs,
  						"type=delay,call_duration=0,data=0,baseFee=0,FEE=0");
  }
  if(m_errorCdrs>0)
  { //输出错单日志
    theLogger->pOutput(theLoggerProcessID,m_outCdrFileName[2].c_str(),
              m_errorCdrs,
  						"type=error,call_duration=0,data=0,baseFee=0,FEE=0");
  }
  for(int i=0;i<2;++i)
  { //输出正常话单日志
    if(m_normalCdr[i]>0)
    {
      theLogger->pOutput(theLoggerProcessID,m_outCdrFileName[i].c_str(),
                m_normalCdr[i],
    						"type=normal,call_duration=%ld,data=%ld,baseFee=%ld,FEE=%ld",
    						m_totalDuration[i],m_totalData[i],m_baseFee[i],m_totalFee[i]);
    }
  }  
  #ifdef _DEBUG_  
    cout<<"------------------------------文件输入-----------------------------"<<endl;
    cout<<"\t"<<m_fullFileName<<" 总话单数:"<<m_inputCdrs<<" 延迟话单数:"<<m_delayedCdrs<<endl;
    cout<<"==============================输出信息==========================="<<endl;
    for(int j=0;j<2;++j)
    {
      if(m_normalCdr[j]>0)
      {
        cout<<"\t"<<m_outCdrFileName[j]<<" 话单数:"<<m_normalCdr[j]
            <<" 时长:"<<m_totalDuration[j]<<" 流量:"<<m_totalData[j]
            <<" 一批费用:"<<m_baseFee[j]<<" 二批费用:"<<m_totalFee[j]
            <<endl;
      }
    }
    if(m_errorCdrs>0)
    {
      cout<<"\t"<<m_outCdrFileName[2]<<" 错单数:"<<m_errorCdrs<<endl;
    }
    cout<<"================================================================"<<endl;  
  #endif
}
void RateLog::logBegin()
{
  theLoggerProcessID = theLogger->pBegin();
}
void RateLog::logEnd()
{
  try
  {
    theLogger->pEnd(theLoggerProcessID);
  }catch(hessian::exceptions::io_exception &e)
  {
    #ifdef _DEBUG_
      cout<<e.what()<<" "<<__FILE__<<__LINE__<<endl;
    #endif
  }
}

