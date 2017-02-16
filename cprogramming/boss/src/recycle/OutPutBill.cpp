
#include "OutPutBill.h"
#include "base/StringUtil.h"

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "log4cpp/Logger.h"

#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>


bool OutPutBill::initialize(/*vector<OutputRule>* outputRuleList,const int maxCdrNum,*/
     const char *dealTime,RecConfig * config)
{ 
    //m_pOutputRule = outputRuleList;
    m_dealTime    = dealTime;
    m_config 	  = config;
    m_maxCdrNum   = m_config->m_maxCdrNum;
    m_fileNo      = -1;
    m_tmpFileList.clear();
//    m_channelList.clear();
    if (m_maxCdrNum <= 0)   m_maxCdrNum   = MAXCDRNUM;
    m_billNum = 0;
/*    Channe_Def tmpChannel;
    for (vector<OutputRule>::iterator itr = m_pOutputRule->begin();itr != m_pOutputRule->end();itr++)
    {
        tmpChannel.m_fileNo = -1;
        tmpChannel.m_cdrNum = 0;
        tmpChannel.m_stdFilePath = itr->m_OutPutBillPath;
        tmpChannel.m_bakFilePath = itr->m_bakFilePath;
	 //tmpChannel.m_billFilePath = itr->m_billFilePath;
        tmpChannel.m_tmpFileList.clear();
        m_channelList.push_back(tmpChannel);
    }*/
    
    return true;
}

/*bool OutPutBill::setDealTime(const char *dealTime)
{    
    m_dealTime    = dealTime;
    Channe_Def tmpChannel;
    for (vector<OutputRule>::iterator itr = m_pOutputRule->begin();
         itr != m_pOutputRule->end();itr++)
    {
        tmpChannel.m_fileNo = -1;
        tmpChannel.m_cdrNum = 0;
        tmpChannel.m_stdFilePath = itr->m_OutPutBillPath;
        tmpChannel.m_bakFilePath = itr->m_bakFilePath;
        tmpChannel.m_tmpFileList.clear();
        m_channelList.push_back(tmpChannel);
    }
    return true;
}*/

bool  OutPutBill::outputUserBill(const int &r_billCycle,vector<RollBackBill> &r_userBill)
{
	char t_tmpBillName[100],t_billChar[500],t_stdBillName[100];
	string t_billString,t_fullName;
  if(m_billFileOfstream.rdbuf()->is_open()==false)
  {
  	m_fileNo++;
    //准备输出文件名 XXXNN_标准文件名MXXX200812REDOMMDDHHMISS.0000,
    sprintf(t_stdBillName,"M%03dREDO%d%s.%04d",
            m_config->m_billingChannalNo,m_config->m_cycleDate,m_dealTime.c_str(),m_fileNo);
//    sprintf(m_stdFileName,"%s.%c.%04d.%06d%06ld",
//            m_gRateInfo->m_fileName.c_str(),
//            FILE_TYPE_USERBILL,
//            r_billCycle%10000,
//            m_dealDate,m_fileNo);
	m_minusBillName = t_stdBillName;
	t_fullName = m_config->m_bakBillPath + PREFIX + t_stdBillName;

    m_billFileOfstream.open(t_fullName.c_str(),ios::out);
    if(!m_billFileOfstream)
    {
      #ifdef _DEBUG_
        cout<<"open cdr file:"<<t_fullName<<" failed!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
    m_tmpFileList.push_back(t_stdBillName);
    //m_openFlag = true;
  }
  
    m_billStringVec.clear();
	//准备输出记录字符串m_billStringVec
	for(vector<RollBackBill>::iterator iter=r_userBill.begin();
		  iter!=r_userBill.end();++iter)
	{
  		sprintf(t_billChar,"%s,%s,%d,%d,%ld,%ld,%ld,%d,%ld,%ld,%ld,%ld,%d,%s\n",
  		        iter->m_userId.c_str(),
  		        iter->m_accountId.c_str(),
  		        iter->m_billType,
  		        iter->m_billCycle,
  		        iter->m_tpObjId,
  		        iter->m_beginTime,
  		        iter->m_endTime,
  		        iter->m_billId,
  		        -iter->m_value,
  		        iter->m_discntValue,
  		        iter->m_a_discnt,
  		        iter->m_b_discnt,
  		        iter->m_type,
  		        iter->m_groupUserId.c_str());
        
        t_billString = t_billChar;  
  		m_billStringVec.push_back(t_billString);

  		m_billFileOfstream<<t_billString;
  	}
//  if(m_userBillFile[m_cycleIdex].write(m_rcdStr)==false)
//  {
//    #ifdef _DEBUG_
//      cout<<"write cdr error!"<<__FILE__<<__LINE__<<endl;
//    #endif
//    return false;
//  }
//  m_gRateInfo->m_rateLog->addUserBills(m_cycleIdex);
	m_billNum+=m_billStringVec.size();//cout<<"m_billNum:"<<m_billNum<<endl;
    if (m_billNum >= m_maxCdrNum)
    {
        //达到m_maxCdrNum话单数后，提交
        m_billNum = 0;
        m_billFileOfstream.close();
		m_billFileOfstream.clear();
    }
	return true;
}

bool OutPutBill::commit()
{
	bool isNeedRawFile;
	string bakBillName,minusBillName,tmpBillName,rawFileName,tmpFileName_raw,bakFileName_raw,tmpminusBillName;
	vector<string>::iterator iterS;
	
	for(iterS=m_tmpFileList.begin();iterS!=m_tmpFileList.end();++iterS)
	{
	
//		bakBillName = m_config->m_bakBillPath + m_minusBillName;
//		minusBillName = m_config->m_minusBillPath + m_minusBillName;
		bakBillName = m_config->m_bakBillPath + *iterS;
		minusBillName = m_config->m_minusBillPath + *iterS;
	
#ifdef _DEBUG_
cout <<"bakBillName:" <<bakBillName <<endl;
cout <<"minusBillName:" <<minusBillName <<endl;
#endif			
		if (!access(bakBillName.c_str(),F_OK))
		{
			m_errMsg = "文件" + bakBillName + "已存在!";
			LOG_TRACE((*theLogger),m_errMsg.c_str());
			theErrorMessages->insert(ERROR_OUTPUTCDR,m_errMsg);
			return false;
		}
		
		if (!access(minusBillName.c_str(),F_OK))
		{
			m_errMsg = "文件" + minusBillName + "已存在!";
			LOG_TRACE((*theLogger),m_errMsg.c_str());
			theErrorMessages->insert(ERROR_OUTPUTCDR,m_errMsg);
			return false;
		}
	}

	if (m_billFileOfstream.rdbuf()->is_open())
	{
		m_billFileOfstream.close();
		m_billFileOfstream.clear();
	}	
	
	for(iterS=m_tmpFileList.begin();iterS!=m_tmpFileList.end();++iterS)
	{
		//tmpBillName = m_config->m_bakBillPath+PREFIX+m_minusBillName;
		tmpBillName = m_config->m_bakBillPath + PREFIX + *iterS;
		bakBillName = m_config->m_bakBillPath + *iterS;
		tmpminusBillName = m_config->m_tmpminusBillPath + *iterS;
		minusBillName = m_config->m_minusBillPath + *iterS;
				
#ifdef _DEBUG_
cout <<"tmpBillName:" <<tmpBillName <<endl;
#endif
				
		if (rename(tmpBillName.c_str(),bakBillName.c_str()) != 0)
		{
			unlink(tmpBillName.c_str());
			return false;
		}
		
	    
		if (bakBillName != minusBillName)
		{
			if (link(bakBillName.c_str(),minusBillName.c_str()) != 0)
			{
				if(m_config->copy(bakBillName.c_str(),tmpminusBillName.c_str())!=0)
                {
                    #ifdef _DEBUG_
                        cout<<"link "<<bakBillName<<" "<<tmpminusBillName<<" false!"<<endl;
                        cout<<"copy "<<bakBillName<<" "<<tmpminusBillName<<" false!"<<endl;
                    #endif
                    unlink(bakBillName.c_str());
                    return false;
                }
                  
                if (rename(tmpminusBillName.c_str(),minusBillName.c_str()))
                {
                    #ifdef _DEBUG_
                        cout<<"rename "<<tmpminusBillName<<" "<<minusBillName<<" false!"<<endl;
                    #endif
                    return false;
                }
			}
		}
		LOG_TRACE_P1((*theLogger),"负账单:%s",minusBillName.c_str());
	}
	
	//为下一种业务的处理做准备
	m_tmpFileList.clear();
	return true;
}

bool OutPutBill::rollBack()
{
	string tmpBillName;

	tmpBillName = m_config->m_bakBillPath+PREFIX+m_minusBillName;
	if(unlink(tmpBillName.c_str()))
	{
		return false;
	}

	return true;
}

bool OutPutBill::end()
{
/*    m_pOutputRule = NULL;
    for (vector<Channe_Def>::iterator itr = m_channelList.begin();
         itr != m_channelList.end();itr++)
    {
        itr->m_tmpFileList.clear();
    }*/
    m_tmpFileList.clear();
    return true;    
}

