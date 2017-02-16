#include "Outputer.h"
#include "RateError.h"
#include "RateLog.h"

Outputer::Outputer()
{
  m_gRateInfo=NULL;
  m_outputRuleMgr=NULL;
  m_OutputRule=NULL;
  m_patternType=PATTERN_TYPE_FILE;
  m_openFlag   =false;
}
Outputer::~Outputer()
{
  m_gRateInfo=NULL;
  m_outputRuleMgr=NULL;
  m_OutputRule=NULL;
}

bool  Outputer::initialize(RateGlobalInfo *r_gRateInfo,
                           OutputRuleMgr *r_outputRule,
                           const RateRunInfo &r_runState)
{
  m_gRateInfo     =r_gRateInfo;
  m_outputRuleMgr =r_outputRule;
  m_billCycle     = r_runState.m_billCycle;
  m_dealDate      = r_runState.m_rateDate;
  m_fileNo        = r_runState.m_nextFileNo;
  //Added By Sunam 2009/10/8 10:44:30 通道号
  m_channelNo     = r_runState.m_channelNo;
  return true;
}
bool  Outputer::fileInit(const int &r_dealDate)
{
  closeTmpFile(); //先关闭已打开的文件
  if(daySwitch(r_dealDate)==false)
  {
    #ifdef _DEBUG_
      cout<<"fileInit("<<r_dealDate<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  m_sFileName = m_gRateInfo->m_fileName.c_str();
  m_workPath  = m_gRateInfo->m_pConfig->m_workPath.c_str();
  //一些文件级初始化
  m_OutputRule = m_outputRuleMgr->getOutputRule(m_sFileName.c_str(),
                                                m_patternType);
  
  if(m_OutputRule==NULL)
  {
    #ifdef _DEBUG_
      cout<<"file:"<<m_gRateInfo->m_fileName<<" output rule not defined!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  m_cdrFileNos[0] = m_cdrFileNos[1] = -1;
  m_nextCdrFileNo = m_fileNo;
  m_balAcctIdList.clear();
  return true;
}

//输出话单
bool  Outputer::output(const int &r_billCycle)
{
  bool t_bRet=true;
  RateCdr &t_cdr=m_gRateInfo->m_cdr;
  int  t_errCode;
  
  if(r_billCycle > m_billCycle)
  {
    m_cycleIdex = 1;
  }else
  {
    m_cycleIdex = 0;
  }
  if(t_cdr[ATTRI_NEED_OUTPUTCDR] == FLAG_NO)
  { //清单不输出(比如出帐事件)
  }else
  {
    #ifdef _DEBUG_
      t_cdr.dumpCdrInfo(m_OutputRule->m_pFormat);
    #endif
    //1. 拼凑话单
    if(t_cdr.outputCdr(m_OutputRule->m_pFormat)==false)
    {
      #ifdef _DEBUG_
        cout<<"t_cdr.outputCdr(m_OutputRule->m_pFormat) false!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      t_bRet = false;
    }
    t_errCode = t_cdr[ATTRI_ERROR_CODE].value();
    //输出清单
    if(outputCdr(r_billCycle,t_errCode,t_cdr.m_desBuff)==false)
    {
      #ifdef _DEBUG_
        cout<<"outputCdr error!"<<__FILE__<<__LINE__<<endl;
      #endif
      t_bRet = false;
    }
  }
  if(m_gRateInfo->m_billUserId == INVALID_SERVER_ID)
  {//userid为0 不输出
  }else if(outputBills(r_billCycle)==false)
  {
    t_bRet = false;
  }
  //Added By Sunam 2009/8/8 19:39:10
  outputRemindCdr();
  return t_bRet;
}
bool Outputer::outputCdr(const int &r_billCycle,const int &r_errCode,const char *r_cdrStr)
{
  Inoutputer * t_pCdrFile=NULL;
  int  t_fileNo;
  if(r_errCode == E_DEFAULT)
  {
    t_pCdrFile = &(m_cdrFile[m_cycleIdex]);
    if(m_cdrFileNos[m_cycleIdex]<0)
    {
      m_cdrFileNos[m_cycleIdex] = m_nextCdrFileNo;
      ++m_nextCdrFileNo;
      t_fileNo = m_cdrFileNos[m_cycleIdex]; 
    }
  }else
  {
    t_pCdrFile = &m_errorFile;
    t_fileNo   = m_fileNo;
  }
  if(t_pCdrFile->is_open()==false)
  { //打开文件
    //准备输出文件名 XXXNN_标准文件名.C.YYMM.YYMMDDXXXXXX
    sprintf(m_stdFileName,"%s.%c.%04d.%06d%06d",
            m_gRateInfo->m_fileName.c_str(),
            (r_errCode==E_DEFAULT)?FILE_TYPE_NORMALCDR:FILE_TYPE_ERRORCDR,
            r_billCycle%10000,
            m_dealDate,t_fileNo);
    t_pCdrFile->setName(m_stdFileName,m_gRateInfo->m_pConfig->m_workPath.c_str());
    if(t_pCdrFile->open(ios::out)==false)
    {
      #ifdef _DEBUG_
        cout<<"open cdr file:"<<m_stdFileName<<" failed!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    //m_gRateInfo->m_rateLog->setOCdrFileName(m_cycleIdex,r_errCode,
    //                                        t_pCdrFile->getFullFileName());
    m_openFlag = true;
  }
  if(t_pCdrFile->write(r_cdrStr)==false)
  {
    #ifdef _DEBUG_
      cout<<"write cdr error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  if(r_errCode == E_DEFAULT)
  { //正常话单
    m_gRateInfo->m_rateLog->addNormalCdr(m_cycleIdex,
                                         m_gRateInfo->m_cdr.m_baseFee,
                                         m_gRateInfo->m_cdr.m_tatolFee,
                                         m_gRateInfo->m_cdr.m_totalDuration,
                                         m_gRateInfo->m_cdr.m_totalData);
  }else
  { //错单
    m_gRateInfo->m_rateLog->addErrCdr();
  }
  return true;
}
bool  Outputer::outputUserBill(const int &r_billCycle,const RollBackBill &r_userBill)
{
  if(m_userBillFile[m_cycleIdex].is_open()==false)
  {
    //准备输出文件名 XXXNN_标准文件名.U.YYMM.YYMMDDXXXXXX
    sprintf(m_stdFileName,"%s.%c.%04d.%06d%06ld",
            m_gRateInfo->m_fileName.c_str(),
            FILE_TYPE_USERBILL,
            r_billCycle%10000,
            m_dealDate,m_fileNo);
    m_userBillFile[m_cycleIdex].setName(m_stdFileName,m_gRateInfo->m_pConfig->m_workPath.c_str());
    if(m_userBillFile[m_cycleIdex].open(ios::out)==false)
    {
      #ifdef _DEBUG_
        cout<<"open cdr file:"<<m_stdFileName<<" failed!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_openFlag = true;
  }
  //准备输出记录字符串m_rcdStr
  sprintf(m_rcdStr,"%s,%s,%d,%d,%ld,%ld,%ld,%d,%ld,%ld,%ld,%ld,%d,%s\n",
          r_userBill.m_userId.c_str(),
          r_userBill.m_accountId.c_str(),
          r_userBill.m_billType,
          r_userBill.m_billCycle,
          r_userBill.m_tpObjId,
          r_userBill.m_beginTime,
          r_userBill.m_endTime,
          r_userBill.m_billId,
          r_userBill.m_value,
          r_userBill.m_discntValue,
          r_userBill.m_a_discnt,
          r_userBill.m_b_discnt,
          r_userBill.m_type,
          r_userBill.m_groupUserId.c_str());
  
  if(m_userBillFile[m_cycleIdex].write(m_rcdStr)==false)
  {
    #ifdef _DEBUG_
      cout<<"write cdr error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  m_gRateInfo->m_rateLog->addUserBills(m_cycleIdex);
  return true;
}

//add by cuiyw 20090914 
bool  Outputer::outputWirelessBill(const int &r_billCycle,const RollBackBill &r_wirelessBill)
{  
  int retrunValue;
  IdInfo    idInfo;
  T_MSISDN          m_msisdn;
  retrunValue = 0;  
  
  if(m_wirelessBillFile[m_cycleIdex].is_open()==false)
  {
    //准备输出文件名 XXXNN_标准文件名.U.YYMM.YYMMDDXXXXXX
    sprintf(m_stdFileName,"%s.%c.%04d.%06d%06ld",
            m_gRateInfo->m_fileName.c_str(),
            FILE_TYPE_WIRELESSBILL,
            r_billCycle%10000,
            m_dealDate,m_fileNo);
    m_wirelessBillFile[m_cycleIdex].setName(m_stdFileName,m_gRateInfo->m_pConfig->m_workPath.c_str());
    if(m_wirelessBillFile[m_cycleIdex].open(ios::out)==false)
    {
      #ifdef _DEBUG_
        cout<<"open cdr file:"<<m_stdFileName<<" failed!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_openFlag = true;
  }

  //获取用户的serial_number   
  retrunValue =m_gRateInfo->m_paramMgr->getUserInfo(r_wirelessBill.m_userId,idInfo); 

  if (retrunValue <= 0)
     {
       //m_theErrorMessages->insert(ERROR_EVENT_DEAL, m_dealDate + "--Mdb deal error!");
       #ifdef _DEBUG_
       cout << "(*m_pEventParam).getUserInfo(r_wirelessBill.m_userId,idInfo)" << endl
            << __FILE__ << __LINE__ << endl;
       #endif
       return false;
      }   
  else
      {             
       m_msisdn = idInfo.m_idAttr[IdInfo::ID_MSISDN];
      }                 
                
  
  //准备输出记录字符串m_rcdStr
  sprintf(m_rcdStr,"%s,%s,%d,%ld\n",
          r_wirelessBill.m_userId.c_str(),
          m_msisdn.c_str(),
          r_wirelessBill.m_billId,
          r_wirelessBill.m_discntValue);    
  
  if(m_wirelessBillFile[m_cycleIdex].write(m_rcdStr)==false)
  {
    #ifdef _DEBUG_
      cout<<"write cdr error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  m_gRateInfo->m_rateLog->addWirelessBills(m_cycleIdex);
  return true;
}
//end by cuiyw 20090914

bool  Outputer::outputAcctBill(const int &r_billCycle,const RollBackBill &r_userBill)
{
  if(m_acctBillFile[m_cycleIdex].is_open()==false)
  {
    //准备输出文件名 XXXNN_标准文件名.A.YYMM.YYMMDDXXXXXX
    sprintf(m_stdFileName,"%s.%c.%04d.%06d%06ld",
            m_gRateInfo->m_fileName.c_str(),
            FILE_TYPE_ACCTBILL,
            r_billCycle%10000,
            m_dealDate,m_fileNo);
    m_acctBillFile[m_cycleIdex].setName(m_stdFileName,m_gRateInfo->m_pConfig->m_workPath.c_str());
    if(m_acctBillFile[m_cycleIdex].open(ios::out)==false)
    {
      #ifdef _DEBUG_
        cout<<"open cdr file:"<<m_stdFileName<<" failed!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_openFlag = true;
  }
  //准备输出记录字符串m_rcdStr
  sprintf(m_rcdStr,"%s,%s,%s,%d,%ld,%ld,%ld,%ld,%d\n",
          r_userBill.m_accountId.c_str(),
          r_userBill.m_userId.c_str(),
          r_userBill.m_areaCode.c_str(),
          r_userBill.m_billId,
          r_userBill.m_value ,
          r_userBill.m_discntValue,
          r_userBill.m_a_discnt,
          r_userBill.m_b_discnt,
          m_gRateInfo->m_acctDay);
  
  if(m_acctBillFile[m_cycleIdex].write(m_rcdStr)==false)
  {
    #ifdef _DEBUG_
      cout<<"write cdr error!"<<__FILE__<<__LINE__<<endl;
    #endif
    return false;
  }
  m_gRateInfo->m_rateLog->addAcctBills(m_cycleIdex);
  return true;
}


bool  Outputer::outputBalInfo(const int &r_billCycle,const vector<T_SERVER_ID> &r_acctIdList)
{
  if(r_acctIdList.empty()) return true;
   
  if(m_balFile.is_open()==false)
  {
    //准备输出文件名 XXXNN_标准文件名.R.YYMM.YYMMDDXXXXXX
    sprintf(m_stdFileName,"%s.%c.%04d.%06d%06ld",
            m_gRateInfo->m_fileName.c_str(),
            FILE_TYPE_BALFILE,
            r_billCycle%10000,
            m_dealDate,m_fileNo);
    m_balFile.setName(m_stdFileName,m_gRateInfo->m_pConfig->m_workPath.c_str());
    if(m_balFile.open(ios::out)==false)
    {
      #ifdef _DEBUG_
        cout<<"open bal file:"<<m_stdFileName<<" failed!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_openFlag = true;
  }
  for(vector<T_SERVER_ID>::const_iterator t_acctItr=m_gRateInfo->m_balAcctIdList.begin();
      t_acctItr!=m_gRateInfo->m_balAcctIdList.end();++t_acctItr)
  {
    if(addBalAcctId(*t_acctItr)==true)
    {
      sprintf(m_rcdStr,"%s\n",t_acctItr->c_str());
      if(m_balFile.write(m_rcdStr)==false)
      {
        #ifdef _DEBUG_
          cout<<"write cdr error!"<<__FILE__<<__LINE__<<endl;
        #endif
        return false;
      }
      m_gRateInfo->m_rateLog->addBalRcds(r_billCycle);
    }
  }
  return true;
}

bool  Outputer::outputAcctTrack(const int &r_billCycle,const vector<DiscntBill> &r_acctTrackList)
{
  if(r_acctTrackList.empty()) return true;
  if(!m_gRateInfo->m_comBill.ifNeedOutDiscount())	return true;
  if(m_acctTrackFile[m_cycleIdex].is_open()==false)
  {
    //准备输出文件名 XXXNN_标准文件名.C.YYMM.YYMMDDXXXXXX
    sprintf(m_stdFileName,"%s.%c.%04d.%06d%06ld",
            m_gRateInfo->m_fileName.c_str(),
            FILE_TYPE_ACCTTRACK,
            r_billCycle%10000,
            m_dealDate,m_fileNo);
    m_acctTrackFile[m_cycleIdex].setName(m_stdFileName,m_gRateInfo->m_pConfig->m_workPath.c_str());
    if(m_acctTrackFile[m_cycleIdex].open(ios::out)==false)
    {
      #ifdef _DEBUG_
        cout<<"open cdr file:"<<m_stdFileName<<" failed!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_openFlag = true;
  }
  for(vector<DiscntBill>::const_iterator t_itr=r_acctTrackList.begin();
      t_itr!=r_acctTrackList.end();++t_itr)
  {
  
    //准备输出记录字符串m_rcdStr
    sprintf(m_rcdStr,"%s,%c,%s,%d,%ld,%d,%c,%d,%ld,%ld\n",
            t_itr->m_id.c_str(),
            (char)(t_itr->m_idType),
            t_itr->m_subId.c_str(),
            t_itr->m_detailItem,
            t_itr->m_discntFee,
            t_itr->m_discntId,
            t_itr->m_discntType,
            t_itr->m_orderNo,
            t_itr->m_adjustInsId,
           (long)t_itr->m_effectId);
    
    
    if(m_acctTrackFile[m_cycleIdex].write(m_rcdStr)==false)
    {
      #ifdef _DEBUG_
        cout<<"write cdr error!"<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_gRateInfo->m_rateLog->addDiscntRcds(m_cycleIdex);
  }
  return true;
}

//文件级提交
bool  Outputer::fileCommit(long &r_nextFileNo)
{
  bool t_bRet = true;
  int  i;
  //清单输出
  //1.1. 输出普通清单
  for(i=0;i<2;++i)
  {
    if(m_gRateInfo->m_rateLog->m_normalCdr[i]>0)
    {
      m_cdrFile[i].close();
      for(vector<PString>::iterator t_oPathItr=m_outputRuleMgr->m_outPaths.begin();
          t_oPathItr!=m_outputRuleMgr->m_outPaths.end(); ++t_oPathItr)
      {
        if(t_oPathItr==m_outputRuleMgr->m_outPaths.begin())
        {
          m_gRateInfo->m_rateLog->setOCdrFileName(i,0,t_oPathItr->c_str(),
                                                  m_cdrFile[i].getFileName());
        }
        if(t_oPathItr==m_outputRuleMgr->m_outPaths.end()-1)
        {
          if(m_cdrFile[i].renameFile(m_cdrFile[i].getFileName(),
                                     t_oPathItr->c_str())==false)
          {
            #ifdef _DEBUG_
              cout<<"rename "<<m_cdrFile[i].getFileName()
                  <<" to "<< t_oPathItr->c_str()
                  <<" failed!"<<__FILE__<<__LINE__<<endl;
            #endif
            t_bRet = false;
            //插入恢复日志。。。。
            //coding later
          }
        }else
        {
          if(m_cdrFile[i].linkFile(m_cdrFile[i].getFileName(),t_oPathItr->c_str())==false)
          {
            #ifdef _DEBUG_
              cout<<"linkFile "<<m_cdrFile[i].getFileName()
                  <<" to "<< t_oPathItr->c_str()
                  <<" failed!"<<__FILE__<<__LINE__<<endl;
            #endif
            t_bRet = false;
            //插入恢复日志。。。。
            //coding later
          }
        }
      } 
    }
  }
  //1.2. 如果有错单，则输出错单
  if(m_gRateInfo->m_rateLog->m_errorCdrs>0)
  {
    m_errorFile.close();
    m_gRateInfo->m_rateLog->setOCdrFileName(0,1,
                 m_outputRuleMgr->m_errpath.c_str(),
                 m_errorFile.getFileName());
    if(m_errorFile.renameFile(m_errorFile.getFileName(),
            m_outputRuleMgr->m_errpath.c_str())==false)
    {
      #ifdef _DEBUG_
        cout<<"rename errorFile failed!"<<__FILE__<<__LINE__<<endl;
      #endif
      t_bRet = false;
      //插入恢复日志。。。。
      //coding later
    }
  }
  for(i=0;i<2;++i)
  {
    //2.输出用户帐单
    if(m_gRateInfo->m_rateLog->m_userBills[i]>0)
    {
      m_userBillFile[i].close();
      if(!(m_outputRuleMgr->m_userBillPathBak==""))
      {
        if(m_userBillFile[i].linkFile(m_userBillFile[i].getFileName(),
            m_outputRuleMgr->m_userBillPathBak.c_str())==false)
        {
          t_bRet = false;
        }
      }
      if(m_userBillFile[i].renameFile(m_userBillFile[i].getFileName(),
              m_outputRuleMgr->m_userBillPath.c_str())==false)
      {
        t_bRet = false;
      }
    }
    //3.输出帐户帐单
    if(m_gRateInfo->m_rateLog->m_acctBills[i]>0)
    {
      m_acctBillFile[i].close();
      if(!(m_outputRuleMgr->m_acctBillPathBak==""))
      {
        if(m_acctBillFile[i].linkFile(m_acctBillFile[i].getFileName(),
            m_outputRuleMgr->m_acctBillPathBak.c_str())==false)
        {
          t_bRet = false;
        }
      }
      if(m_acctBillFile[i].renameFile(m_acctBillFile[i].getFileName(),
              m_outputRuleMgr->m_acctBillPath.c_str())==false)
      {
        t_bRet = false;
      }
    }
    //5.输出固定费优惠明细文件
    if(m_gRateInfo->m_rateLog->m_fixRcds[i]>0)
    {
      m_fixTrackFile[i].close();
      if(m_fixTrackFile[i].renameFile(m_fixTrackFile[i].getFileName(),
              m_outputRuleMgr->m_fixTrackPath.c_str())==false)
      {
        t_bRet = false;
      }
    }
    //6.输出优惠明细文件
    if(m_gRateInfo->m_rateLog->m_discntRcds[i]>0)
    {
      m_acctTrackFile[i].close();
      if(m_acctTrackFile[i].renameFile(m_acctTrackFile[i].getFileName(),
              m_outputRuleMgr->m_acctTrackPath.c_str())==false)
      {
        t_bRet = false;
      }
    }
    //7.输出存折等其它信息文件
    if(m_gRateInfo->m_rateLog->m_otherRcds[i]>0)
    {
      m_otherFile[i].close();
      if(m_otherFile[i].renameFile(m_otherFile[i].getFileName(),
              m_outputRuleMgr->m_otherInfoPath.c_str())==false)
      {
        t_bRet = false;
      }
    }
   //输出无线上网卡的月租费用
   //add by cuiyw 20090915
    if(m_gRateInfo->m_rateLog->m_wirelessBills[i]>0)
    {
      m_wirelessBillFile[i].close();
      if(m_wirelessBillFile[i].renameFile(m_wirelessBillFile[i].getFileName(),
              m_outputRuleMgr->m_wirelessPath.c_str())==false)
      {
        t_bRet = false;
      }
    }
    //end by cuiyw 20090915
    
   //输出包月费账单文件(B)
   if(m_gRateInfo->m_rateLog->m_monfeeBBill[i]>0)
    {
      m_monfeeBBill[i].close();
      if(m_monfeeBBill[i].renameFile(m_monfeeBBill[i].getFileName(),
              m_outputRuleMgr->m_monfeeBBillPath.c_str())==false)
      {
        t_bRet = false;
      }
  }
//输出包月费ADC费用清单文件(I)
   if(m_gRateInfo->m_rateLog->m_monfeeIBill[i]>0)
    {
      m_monfeeIBill[i].close();
      if(m_monfeeIBill[i].renameFile(m_monfeeIBill[i].getFileName(),
              m_outputRuleMgr->m_monfeeIBillPath.c_str())==false)
      {
        t_bRet = false;
      }
    }   
   //输出包月费SP费用清单文件(P)   
   if(m_gRateInfo->m_rateLog->m_monfeePBill[i]>0)
    {
      m_monfeePBill[i].close();
      
      if(m_monfeePBill[i].renameFile(m_monfeePBill[i].getFileName(),
              m_outputRuleMgr->m_monfeePBillPath.c_str())==false)
      {
        t_bRet = false;
      }
    }   
   //输出包月费包月费用明细帐单文件(M)
   if(m_gRateInfo->m_rateLog->m_monfeeMBill[i]>0)
    {
      m_monfeeMBill[i].close();
      
      if(m_monfeeMBill[i].renameFile(m_monfeeMBill[i].getFileName(),
              m_outputRuleMgr->m_monfeeMBillPath.c_str())==false)
      {
        t_bRet = false;
      }
    }
    //输出帐前调账信息文件
   if(m_gRateInfo->m_rateLog->m_adjustBfeoreBill[i]>0)
    {
       m_adjustBeforeFile[i].close();
      if(m_adjustBeforeFile[i].renameFile(m_adjustBeforeFile[i].getFileName(),
              m_outputRuleMgr->m_adjustBeforePath.c_str())==false)
      {
      #ifdef _DEBUG_
			  cout<<"m_outputRuleMgr->m_adjustBeforePath = "<<m_outputRuleMgr->m_adjustBeforePath<<endl;
			#endif
        t_bRet = false;
      }
    } 
  }
  //4.输出结余文件
  if(m_gRateInfo->m_rateLog->m_balRcds>0)
  {
    m_balFile.close();
    if(m_balFile.renameFile(m_balFile.getFileName(),
            m_outputRuleMgr->m_balInfoPath.c_str())==false)
    {
      #ifdef _DEBUG_
        cout<<"m_outputRuleMgr->m_balInfoPath = "<<m_outputRuleMgr->m_balInfoPath<<endl;
      #endif
      t_bRet = false;
    }
  }
  
	//Added by Sunam 2009/8/8 19:17:49
	//输出免费资源提醒文件
	if (m_gRateInfo->m_rateLog->m_remindCdrs > 0)
	{
		m_remindFile.close();
		if (!m_remindFile.renameFile(m_remindFile.getFileName(),m_outputRuleMgr->m_remindInfoPath.c_str()))
		{
			#ifdef _DEBUG_
			  cout<<"m_outputRuleMgr->m_remindInfoPath = "<<m_outputRuleMgr->m_remindInfoPath<<endl;
			#endif
			t_bRet = false;
		}
	}
	
	//Added By Sunam 2009/9/29 10:07:26
    outputUserSumBill();
	//Added by Sunam 2009/9/29 10:11:35
	if (m_gRateInfo->m_rateLog->m_userSumBills > 0)
	{
		m_userSumBillFile.close();
		if (!m_userSumBillFile.renameFile(m_userSumBillFile.getFileName(),m_outputRuleMgr->m_userSumBillPath.c_str()))
		{
			#ifdef _DEBUG_
			  cout<<"m_outputRuleMgr->m_userSumBillPath = "<<m_outputRuleMgr->m_userSumBillPath<<endl;
			#endif
			t_bRet = false;
		}
	}
  
  m_openFlag   = false;
  m_fileNo     = m_nextCdrFileNo;
  r_nextFileNo = m_fileNo;
  return t_bRet;
}


bool  Outputer::daySwitch(const int &r_dealDate)
{
  if(r_dealDate > m_dealDate)
  { //换天
    //可以增加一些换天处理
    //比如输出清单汇总文件等
    ///////////////////////
    m_dealDate = r_dealDate;
    m_fileNo   = 0;
  }
  return true;
}


//将所有文件关闭
void  Outputer::closeTmpFile()
{
  if(m_openFlag == false) return;
  if(m_errorFile.is_open()==true)
  {
    m_errorFile.close();
    m_errorFile.unlinkFile();
  }
  for(int i=0;i<2;++i)
  {
    if(m_cdrFile[i].is_open()==true)
    {
      m_cdrFile[i].close();
      m_cdrFile[i].unlinkFile();
    }
    if(m_userBillFile[i].is_open()==true)
    {
      m_userBillFile[i].close();
      m_userBillFile[i].unlinkFile();
    }
    if(m_acctBillFile[i].is_open()==true)
    {
      m_acctBillFile[i].close();
      m_acctBillFile[i].unlinkFile();
    }
    if(m_balFile.is_open()==true)
    {
      m_balFile.close();
      m_balFile.unlinkFile();
    }
    if(m_acctTrackFile[i].is_open()==true)
    {
      m_acctTrackFile[i].close();
      m_acctTrackFile[i].unlinkFile();
    }
    if(m_otherFile[i].is_open()==true)
    {
      m_otherFile[i].close();
      m_otherFile[i].unlinkFile();
    }
     //added by dingr
    if(m_adjustBeforeFile[i].is_open()==true)
    {
      m_adjustBeforeFile[i].close();
      m_adjustBeforeFile[i].unlinkFile();
    }
    //Added By Sunam 2009/8/8 19:19:07
    if(m_remindFile.is_open()==true)
    {
      m_remindFile.close();
      m_remindFile.unlinkFile();
    }
	//add by cuiyw 20090915
    if(m_wirelessBillFile[i].is_open()==true)
    {
      m_wirelessBillFile[i].close();
      m_wirelessBillFile[i].unlinkFile();
    }
    //end by cuiyw
   
    if(m_monfeeBBill[i].is_open()==true)
    {
      m_monfeeBBill[i].close();
      m_monfeeBBill[i].unlinkFile();
    }
    
    if(m_monfeeIBill[i].is_open()==true)
    {
      m_monfeeIBill[i].close();
      m_monfeeIBill[i].unlinkFile();
    }

    if(m_monfeePBill[i].is_open()==true)
    {
      m_monfeePBill[i].close();
      m_monfeePBill[i].unlinkFile();
    }

    if(m_monfeeMBill[i].is_open()==true)
    {
      m_monfeeMBill[i].close();
      m_monfeeMBill[i].unlinkFile();
    }
  }
  m_openFlag=false;
  return ;
}

//帐期切换
void Outputer::billCycleSwitch(const int &r_newCycle)
{
  m_billCycle = r_newCycle;
}

//true 表示插入成功, false 表示重复
bool  Outputer::addBalAcctId(const T_SERVER_ID &r_acctId)
{
  vector<T_SERVER_ID>::iterator t_itr;
  t_itr=::lower_bound(m_balAcctIdList.begin(),m_balAcctIdList.end(),r_acctId);
  if(t_itr==m_balAcctIdList.end() || !(*t_itr == r_acctId))
  {
    m_balAcctIdList.insert(t_itr,r_acctId);
    return true;
  }else
  {
    return false;
  }
}
bool Outputer::ouputRentCdr(const int &r_billCycle,const vector<PString> &r_cdrList)
{
  bool t_bRet=true;
  
  if(r_billCycle > m_billCycle)
  {
    m_cycleIdex = 1;
  }else
  {
    m_cycleIdex = 0;
  }
  //输出清单
  for(vector<PString>::const_iterator t_cdrItr=r_cdrList.begin();
      t_cdrItr!=r_cdrList.end();++t_cdrItr)
  {
    if(outputCdr(r_billCycle,0,t_cdrItr->c_str())==false)
    {
      #ifdef _DEBUG_
        cout<<"outputCdr error!"<<__FILE__<<__LINE__<<endl;
      #endif
      t_bRet = false;
    }
  }
  if(outputBills(r_billCycle)==false)
  {
    t_bRet = false;
  }
  return true;
}
OutputRule* Outputer::getOutputRule()
{
  return m_OutputRule;
}

bool Outputer::outputBills(const int &r_billCycle)
{
  bool t_bRet=true;
  //2.输出用户帐单
  //3.输出帐户帐单
  for(vector<RollBackBill>::const_iterator t_billItr=
      m_gRateInfo->m_rollBackBills.begin();
      t_billItr!=m_gRateInfo->m_rollBackBills.end();++t_billItr)
  {
    if(t_billItr->m_billType==BILL_TYPE_ACCT)
    { //帐户帐单
      if(outputAcctBill(r_billCycle,*t_billItr)==false)
      {
        #ifdef _DEBUG_
          cout<<"outputAcctBill error!"<<__FILE__<<__LINE__<<endl;
        #endif
        t_bRet = false;
      }
    }
    else 
    {
      //用户帐单
      if(outputUserBill(r_billCycle,*t_billItr)==false)
      {
        #ifdef _DEBUG_
          cout<<"outputUserBill error!"<<__FILE__<<__LINE__<<endl;
        #endif
        t_bRet = false;
      }

      //add by cuiyw 20090914     
      if(t_billItr->m_billId == 11189&&t_billItr->m_discntValue >0)
      {       
        //当前账单为无线上网卡月租账单.需要传给计费以便传给总部.
          if(outputWirelessBill(r_billCycle,*t_billItr)==false)
          {
           #ifdef _DEBUG_                    
             cout<<"outputWirelessBill error!"<<__FILE__<<__LINE__<<endl;
           #endif
           t_bRet = false;
          }
      }          
      //end by cuiyw 20090914
    }
  }
  
  //4.输出结余文件
  if(!(m_gRateInfo->m_pCdr[ATTRI_USER_SERVICE_TYPE_A] == SERVICE_TYPE_SSP))
  { //智能网话单结余文件不输出
    if(outputBalInfo(r_billCycle,m_gRateInfo->m_balAcctIdList)==false)
    {
      #ifdef _DEBUG_
        cout<<"output bal Info error!"<<__FILE__<<__LINE__<<endl;
      #endif
      t_bRet = false;
    }
  }
  //5.输出固定费优惠轨迹
  if(outputFixTrack(r_billCycle,m_gRateInfo->m_rentFeeLogInfoVector)==false)
  {
    #ifdef _DEBUG_
      cout<<"outputFixTrack error!"<<__FILE__<<__LINE__<<endl;
    #endif
    t_bRet = false;
  }
  
  //6.输出优惠明细文件
  if(outputAcctTrack(r_billCycle,m_gRateInfo->m_discountBills)==false)
  {
    #ifdef _DEBUG_
      cout<<"output acctTrack error!"<<__FILE__<<__LINE__<<endl;
    #endif
    t_bRet = false;
  }
  //7.输出存折等其它信息文件
  if(outputOther(r_billCycle,m_gRateInfo->m_addBillVector)==false)
  {
    #ifdef _DEBUG_
      cout<<"outputOther error!"<<__FILE__<<__LINE__<<endl;
    #endif
    t_bRet = false;
  }

  if(outputMonfeeBBills(r_billCycle,m_gRateInfo->outputBillVector)==false)
    {
     #ifdef _DEBUG_                    
       cout<<"outputMonfeeBBills error!"<<__FILE__<<__LINE__<<endl;
     #endif
     t_bRet = false;
    }
    
	  if(outputMonfeeIBills(r_billCycle,m_gRateInfo->outputCdrVector)==false)
    {
     #ifdef _DEBUG_                    
       cout<<"outputMonfeeIBills error!"<<__FILE__<<__LINE__<<endl;
     #endif
     t_bRet = false;
    }
    
    if(outputMonfeePBills(r_billCycle,m_gRateInfo->outputCdrVector)==false)
    {
     #ifdef _DEBUG_                    
       cout<<"outputMonfeePBills error!"<<__FILE__<<__LINE__<<endl;
     #endif
     t_bRet = false;
    }
    
    if(outputMonfeeMBills(r_billCycle,m_gRateInfo->outputUserSubInfoBill)==false)
    {
     #ifdef _DEBUG_                    
       cout<<"outputMonfeeMBills error!"<<__FILE__<<__LINE__<<endl;
     #endif
     t_bRet = false;
    }
    
  //输出帐前调账日志文件
    if(outputAdjustBeforTrack(r_billCycle,m_gRateInfo->m_adjustBeforeBills)==false)
  {
    #ifdef _DEBUG_
      cout<<"outputAdjustBeforTrack error!"<<__FILE__<<__LINE__<<endl;
    #endif
    t_bRet = false;
  }
  return true;
}

bool  Outputer::outputFixTrack(const int &r_billCycle,const vector<UserRentPriceLog> &r_fixTracks)
{
  if(r_fixTracks.empty()) return true;
  if(m_fixTrackFile[m_cycleIdex].is_open()==false)
  {
    //准备输出文件名 XXXNN_标准文件名.C.YYMM.YYMMDDXXXXXX
    sprintf(m_stdFileName,"%s.%c.%04d.%06d%06ld",
            m_gRateInfo->m_fileName.c_str(),
            FILE_TYPE_FIXTRACK,
            r_billCycle%10000,
            m_dealDate,m_fileNo);
    m_fixTrackFile[m_cycleIdex].setName(m_stdFileName,m_gRateInfo->m_pConfig->m_workPath.c_str());
    if(m_fixTrackFile[m_cycleIdex].open(ios::out)==false)
    {
      #ifdef _DEBUG_
        cout<<"open cdr file:"<<m_stdFileName<<" failed!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_openFlag = true;
  }
  for(vector<UserRentPriceLog>::const_iterator t_itr=r_fixTracks.begin();
      t_itr!=r_fixTracks.end();++t_itr)
  {
    //准备输出记录字符串m_rcdStr
    sprintf(m_rcdStr,"%s,%d,%d,%d,%c,%d,%ld\n",
            t_itr->m_userid.c_str(),
            t_itr->m_item,
            t_itr->m_subCycId,
            t_itr->m_svcId,
            t_itr->m_svcStateCode,
            t_itr->m_policyCompId,
            t_itr->m_subFee);
    
    if(m_fixTrackFile[m_cycleIdex].write(m_rcdStr)==false)
    {
      #ifdef _DEBUG_
        cout<<"write cdr error!"<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_gRateInfo->m_rateLog->addFixRcds(m_cycleIdex);
  }
  return true;
}

bool  Outputer::outputOther(const int &r_billCycle,const vector<AdditionalBill> &r_otherBills)
{
  if(r_otherBills.empty()) return true;
  if(m_otherFile[m_cycleIdex].is_open()==false)
  {
    //准备输出文件名 XXXNN_标准文件名.C.YYMM.YYMMDDXXXXXX
    sprintf(m_stdFileName,"%s.%c.%04d.%06d%06ld",
            m_gRateInfo->m_fileName.c_str(),
            FILE_TYPE_OTHER,
            r_billCycle%10000,
            m_dealDate,m_fileNo);
    m_otherFile[m_cycleIdex].setName(m_stdFileName,m_gRateInfo->m_pConfig->m_workPath.c_str());
    if(m_otherFile[m_cycleIdex].open(ios::out)==false)
    {
      #ifdef _DEBUG_
        cout<<"open cdr file:"<<m_stdFileName<<" failed!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_openFlag = true;
  }
  for(vector<AdditionalBill>::const_iterator t_itr=r_otherBills.begin();
      t_itr!=r_otherBills.end();++t_itr)
  {
    //准备输出记录字符串m_rcdStr
    sprintf(m_rcdStr,"%s,%ld,%s,%c,%s,%d,%ld,%ld,%ld\n",
            t_itr->m_billType.c_str(),
            t_itr->m_InsId,
            t_itr->m_id.c_str(),
            char(t_itr->m_idType),
            t_itr->m_operateType.c_str(),
            t_itr->m_itemCode,
            t_itr->m_factValue,
            t_itr->m_fValue,
            t_itr->m_sValue);
    
    if(m_otherFile[m_cycleIdex].write(m_rcdStr)==false)
    {
      #ifdef _DEBUG_
        cout<<"write cdr error!"<<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_gRateInfo->m_rateLog->addOtherRcds(m_cycleIdex);
  }
  return true;
}

//Added By Sunam 2009/8/8 19:08:05
bool Outputer::outputRemindCdr()
{
	if (m_gRateInfo->m_remindInfo.empty())
	{
		return true;
	}
	
	if (!m_remindFile.is_open())
	{
		//准备输出文件名 XXXNN_标准文件名.C.YYMMDDXXXXXX
    	sprintf(m_stdFileName,"%s.%c.%06d%06ld",m_gRateInfo->m_fileName.c_str(),FILE_TYPE_REMIND,m_dealDate,m_fileNo);
		m_remindFile.setName(m_stdFileName,m_gRateInfo->m_pConfig->m_workPath.c_str());
		if (!m_remindFile.open(ios::out))
		{
			#ifdef _DEBUG_
			cout<<"open cdr file:"<<m_stdFileName<<" failed!"
				<<__FILE__<<__LINE__<<endl;
			#endif
			return false;
		}
		m_openFlag = true;
	}
	for (int i = 0; i < m_gRateInfo->m_remindInfo.size(); ++i)
	{
		memset(m_rcdStr,0,sizeof(m_rcdStr));
		m_gRateInfo->m_remindInfo.at(i).getOuputStr(m_rcdStr);
		if(m_remindFile.write(m_rcdStr)==false)
    	{
    	  #ifdef _DEBUG_
    	    cout<<"write cdr error!"<<__FILE__<<__LINE__<<endl;
    	  #endif
    	  return false;
    	}
    	m_gRateInfo->m_rateLog->addRemindRcds(m_cycleIdex);
	}
	return true;
}
bool Outputer::outputMonfeeBBills(const int &r_billCycle,const vector<OutBill> &r_outBills)
{
	 char userType = '1';
	 if(m_gRateInfo->m_fileName[0] == FILE_TYPE_IN_A)
	 	{
	 		userType = '0';
	 	}
   IdInfo 		r_idInfo;
   
   
   T_EPARCHY_CODE   t_eparchycode;
   T_BRAND_CODE     t_brandcode;
   T_MSISDN         t_msisdn;
   String<18>       t_spbizcode;
   if(r_outBills.empty()) return true;
   if(m_monfeeBBill[m_cycleIdex].is_open()==false)
   {
     //准备输出文件名 XXXNN_标准文件名.B.YYMM.YYMMDDXXXXXX
   
     sprintf(m_stdFileName,"%s.%c.%04d.%06d%06ld",
   
             m_gRateInfo->m_fileName.c_str(),
   
             FILE_TYPE_MONFEE_B,
   
             r_billCycle%10000,
   
             m_dealDate,m_fileNo);
   
     m_monfeeBBill[m_cycleIdex].setName(m_stdFileName,m_gRateInfo->m_pConfig->m_workPath.c_str());
   
     if(m_monfeeBBill[m_cycleIdex].open(ios::out)==false)
   
     {
   
       #ifdef _DEBUG_
   
         cout<<"open cdr file:"<<m_stdFileName<<" failed!"<<__FILE__<<__LINE__<<endl;
   
       #endif
   
       return false;
   
     }
   
     m_openFlag = true;
   
   }
   
   for(vector<OutBill>::const_iterator t_itr=r_outBills.begin();t_itr!=r_outBills.end();++t_itr)
   
   {
    //准备输出记录字符串m_rcdStr

    int retrunValue =m_gRateInfo->m_paramMgr->getUm_monfeeBBill[m_cycleIdex].setName(m_stdFileName,m_gRateInfo->m_pConfig->m_workPath.c_str());
   
     if(m_monfeeBBill[m_cycleIdex].open(ios::out)==false)
   
     {
   
       #ifdef _DEBUG_
   
         cout<<"open cdr file:"<<m_stdFileName<<" failed!"<<__FILE__<<__LINE__<<endl;
   
       #endif
   
       return false;
   
     }
   
     m_openFlag = true;
   
   }
   
   for(vector<OutBill>::const_iterator t_itr=r_outBills.begin();t_itr!=r_outBills.end();++t_itr)
   
   {
    //鍑嗗