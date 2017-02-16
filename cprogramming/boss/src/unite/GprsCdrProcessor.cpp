// ##########################################
// Source file : GprsCdrProcessor.cpp
// System      : Mobile Billing System
// Version     : 0.1.8
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20011029
// Update      : 20020423
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "GprsCdrProcessor.h"
#include <iostream>

void GprsCdrProcessor::isOverStored()
{
	  vector<string> * topPfieldVect;
	  long d_timeDiff;
	  time_t tDealTime;
	  cout<<"--------------test22------------------"<<endl;	
	  while( m_cdrPool->begin() != m_cdrPool->end() )
	  {
	  	  topPfieldVect = (m_cdrPool->begin())->second;
	  	
	      tDealTime = m_pLog->getStartDealTime_tm();
	      string dotime = (*topPfieldVect)[m_cdrFieldCount];
	      d_timeDiff = tDealTime - atol( dotime.c_str() );
	  	
	  	if( m_cdrPool->size() > m_maxStoredCdr || d_timeDiff > m_maxDelaySecond )
	  	{	
	  		  topPfieldVect = (m_cdrPool->begin())->second;
	  		  
	  		  //如果删除的是中间记录结果，则删除m_midUnite中对应的记录值 add by lizhao 2008-10-25
	  		   string key = (m_cdrPool->begin())->first;
           multimap<string, vector<string> >::iterator first = m_needUnite.lower_bound(key);
           multimap<string, vector<string> >::iterator last = m_needUnite.upper_bound(key);
           multimap<string, vector<string> >::iterator begin = first;
           while ( first != last )
           {
               m_alreadyUnite.insert(*first);
               ++first;
           }
           
            m_needUnite.erase(begin, last);
            
           // m_pInOutPut->writeLogFile(getFileType(),m_alreadyUnite);
           // m_alreadyUnite.clear();
           
           m_cdrPool->erase( m_cdrPool->begin() );	
                
	  		  // 超过阀值的话单,就直接输出
	  		  // gprs字段vector中的最后一个字段存放的是处理时间,不是标准话单中的输出字段 
	  		  while(topPfieldVect->size()>m_cdrFieldCount)
	  		  {
	  		  	 topPfieldVect->pop_back();
	  		  }
	  		  
	  		  string cdrTmp = StringUtil::join(*topPfieldVect,m_cdrToken);
	  		  
	  		  m_pLog->addPool2file();
	  		  m_pInOutPut->writeCdr(cdrTmp.c_str());
	  		  
	  		  delete topPfieldVect;
	  	}		
	  	else
	  	{
	  		  break;	
	  	}
	  }
	  cout<<"--------------test23------------------"<<endl;	
	  return ;	
}

void GprsCdrProcessor::doMergeCdr( vector<string> *midFieldVect,vector<string> *desFieldVect)
{
	int iSum;
	vector<string> * pTmpVect;
	
	iSum = atoi( (*midFieldVect)[m_call_duration].c_str() )
					 + atoi( (*desFieldVect)[m_call_duration].c_str() );
	(*desFieldVect)[m_call_duration] = StringUtil::toString(iSum);
	
	iSum = atoi( (*midFieldVect)[m_dataUp1].c_str() )
					 + atoi( (*desFieldVect)[m_dataUp1].c_str() );
	(*desFieldVect)[m_dataUp1] = StringUtil::toString(iSum);
	
	iSum = atoi( (*midFieldVect)[m_dataDn1].c_str() )
					 + atoi( (*desFieldVect)[m_dataDn1].c_str() );
	(*desFieldVect)[m_dataDn1] = StringUtil::toString(iSum);

	iSum = atoi( (*midFieldVect)[m_dataUp2].c_str() )
					 + atoi( (*desFieldVect)[m_dataUp2].c_str() );
	(*desFieldVect)[m_dataUp2] = StringUtil::toString(iSum);
	
	iSum = atoi( (*midFieldVect)[m_dataDn2].c_str() )
					 + atoi( (*desFieldVect)[m_dataDn2].c_str() );
	(*desFieldVect)[m_dataDn2] = StringUtil::toString(iSum);
	
	(*desFieldVect)[m_startDatePosi] = (*midFieldVect)[m_startDatePosi];
	(*desFieldVect)[m_startTimePosi] = (*midFieldVect)[m_startTimePosi];

  (*desFieldVect).push_back("UNITED");
	
	return;
}

void GprsCdrProcessor::getMergedCdr()
{
	  string cdrTmp;
	
	  // gprs字段vector中的最后一个字段存放的是处理时间,不是标准话单中的输出字段
	  m_pFieldVect->pop_back();
	  m_pFieldVect->pop_back();
	  cdrTmp = StringUtil::join(*m_pFieldVect,m_cdrToken);
	  strcpy( m_cdrBuffer,cdrTmp.c_str() );
	
	return ;
}

void GprsCdrProcessor::convertCdr2Fields()
{
	char cTmp[15];
	
	m_pFieldVect = new vector<string>();
	
	if( StringUtil::split(m_cdrBuffer,m_cdrToken,(*m_pFieldVect),true) < m_cdrFieldCount)
	{
		delete m_pFieldVect;
		throw string("记录字段数错!");		
	}
	
	return;
}

bool GprsCdrProcessor::setIndexFieldPosi(const string &posi)
{
  int iCount;
	vector<string> tmpStrs;
	
	iCount = StringUtil::split(posi,",",tmpStrs,true);
	
	if( iCount != 2)
		return false;
	
	m_beginDate = atoi( tmpStrs[0].c_str() );          //开始日期
	m_beginTime = atoi( tmpStrs[1].c_str() );          //开始时间
	
	return true;		
}

bool GprsCdrProcessor::setMergeFieldPos(const string &str)
{
	int iCount;
	vector<string> tmpStrs;
	
	iCount = StringUtil::split(str,",",tmpStrs,true);
	
	if( iCount != 7)
		return false;
	
	m_startDatePosi = atoi( tmpStrs[0].c_str() );
	m_startTimePosi = atoi( tmpStrs[1].c_str() );
	m_call_duration = atoi( tmpStrs[2].c_str() );
	m_dataUp1       = atoi( tmpStrs[3].c_str() );
	m_dataDn1       = atoi( tmpStrs[4].c_str() );
	m_dataUp2       = atoi( tmpStrs[5].c_str() );
	m_dataDn2       = atoi( tmpStrs[6].c_str() );
	
	return true;	
}

