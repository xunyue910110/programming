#include "UserBillFacOutputFile.h"
#include "appfrm/ErrorHandle.h"
#include "log4cpp/Logger.h"

extern log4cpp::Logger *theLogger;

UserBillFacOutputFile::UserBillFacOutputFile()
{
}


UserBillFacOutputFile::~UserBillFacOutputFile()
{
}


bool UserBillFacOutputFile::init(const string& performId,
                                 const BillFacConfig*  pBillFacConfig,
                                 const string& strBcycFromBFileName,
                                 const string& strOperateTime)
{
	int i;
	string fileNameHead = 'D' + strBcycFromBFileName.substr(0,6);
	string fileNameTail = strBcycFromBFileName.substr(2,6)
	                                  + strOperateTime.substr(2);
	string strChannelNo;
	char   tmp[4];
	memset (tmp, 0, sizeof(tmp));
	m_performId = performId;
	m_RecordNum = pBillFacConfig->m_RecordNum;
	//初始化输出结构信息
	for (i=0; i<MAX_USERACCOUNT_CHANNELNUM; i++)
	{
		resetFileTag(i);
		
		//strChannelNo不足3位，前面补零
		sprintf(tmp, "%03d", i%100);
		strChannelNo = tmp;
		memset (tmp, 0, sizeof(tmp));

		//add by zhouq begin
	  m_seqNo[i] = 0;
	  char  seqNo[4];
	  memset (seqNo, 0, sizeof(seqNo));
	  sprintf(seqNo, "%03d", m_seqNo[i]);
	  //add by zhouq end

		//modify by zhouq begin
		/*
		m_outputFileInfo[i].m_workFileName =
		                    pBillFacConfig->m_outputFileWorkPath +
		                    fileNameHead + strChannelNo + fileNameTail+seqNo;

		m_outputFileInfo[i].m_outputFileName =
		                    pBillFacConfig->m_outputFilePath[i] +
		                    fileNameHead + strChannelNo + fileNameTail+seqNo;
		*/
		m_workFilehead[i] = pBillFacConfig->m_outputFileWorkPath +
		                   fileNameHead + strChannelNo + fileNameTail;
		m_workFile[i] =  m_workFilehead[i] + seqNo;
    m_outputFileInfo[i].m_workFileName.push_back(m_workFile[i]);

    m_outputFilehead[i] = pBillFacConfig->m_outputFilePath[i] +
		               fileNameHead + strChannelNo + fileNameTail;
		m_outputFile[i] =  m_outputFilehead[i] + seqNo;
		m_outputFileInfo[i].m_outputFileName.push_back(m_outputFile[i]);
		//add by zhouq end


	/*	if (openFile(i))
		{
		    m_outputFileInfo[i].m_fileStat = '1';
		}
		else
		{
			return false;
		}*/
	}

	return true;
}


bool UserBillFacOutputFile::writeFile(const USERBILLMAP& userBillMap)
{
	USERBILLMAPCITER itr,itrEnd;

	vector<UserBill>::const_iterator vecitr;

	int iChannel;

  for (itr=userBillMap.begin(); itr!=userBillMap.end(); itr++)
  {
    iChannel = computeChannelNo((*itr).first.m_id);
		if (iChannel == -1)
		{
			cout << "获取通道号码失败" << endl;
			/*theErrorMessages->insert(ERROR_GET_USERCHANNELNO,
                  (string)(char*)(*itr).m_userId + "--获取通道号码失败");*/
		}
		//通道内客户记录数目+1
		m_outputFileInfo[iChannel].m_idTotalNum ++;
		//如果本通道是第一次数出客户记录
        if (m_outputFileInfo[iChannel].m_idTotalNum == 1)
        {
            if (openFile(iChannel))
		    {
		        m_outputFileInfo[iChannel].m_fileStat = '1';
		        //针对输出文件日志处理开始
                theLogger->pBeginOutput(m_performId,
                      m_outputFile[iChannel],
                      "TOTAL_ID_NUM=0,ERR_ID_NUM=0,ERR_REC_NUM=0,"
                      "TOTAL_FEE_SUM=0,FILTER_FEE_SUM=0");
		    }
		    else
		    {
		    	return false;
		    }
        }
		
		if (m_outputFileInfo[iChannel].m_fileStat == '1')
		{			
		  //输出账单
		  //add by zhouq begin
		  if(m_outputFileInfo[iChannel].m_recordTotalNum >= m_RecordNum)
		  {
		  	m_outputFileInfo[iChannel].m_idNum.push_back(
		  	   m_outputFileInfo[iChannel].m_idTotalNum);
		  	m_outputFileInfo[iChannel].m_recordNum.push_back(
		  	   m_outputFileInfo[iChannel].m_recordTotalNum);
		  	 
       //modify by yih @ 20100303  此处修改赋值,初始值为1,可以改正该BUG.
		  	//m_outputFileInfo[iChannel].m_idTotalNum = 0;
		  	m_outputFileInfo[iChannel].m_idTotalNum = 1;
		  	m_outputFileInfo[iChannel].m_recordTotalNum = 0;
		  	swichOutputFile(iChannel);
		  	theLogger->pBeginOutput(m_performId,
		               m_outputFile[iChannel],
		                  "TOTAL_RECORD_NUM=0,TOTAL_ID_NUM=0,ERR_ID_NUM=0,"
		                  "ERR_REC_NUM=0");
		  }
		  //add by zhouq end
	    for (vecitr=(*itr).second.begin();
			              vecitr!=(*itr).second.end(); vecitr++)
		  {
				m_outputFileInfo[iChannel].m_workOfStream <<
		        (*itr).first               <<
		        vecitr->m_userId           << ',' <<
		        vecitr->m_detailItemCode   << ',' <<
		        vecitr->m_discountFee      << ',' <<
		        vecitr->m_discountId       << ',' <<
		        vecitr->m_billTypeCode     << ',' <<
		        vecitr->m_priority         << ',' <<
		        vecitr->m_adjustInsId      << ','<<
		        vecitr->m_effectId         << endl;//add by xuf 2010-1-23 10:21:29
		        m_outputFileInfo[iChannel].m_recordTotalNum++;
		        //m_outputFileInfo[iChannel].m_feeSum += (*vecitr).m_discountFee;
		  }
		}
		else
		{
			//该通道输出文件流尚未打开
			return false;
		}
	}
	//add by zhouq begin
	

	for (int i=0 ; i< MAX_USERACCOUNT_CHANNELNUM;i++)
  {
  	  m_outputFileInfo[i].m_idNum.push_back(
	    	   m_outputFileInfo[i].m_idTotalNum);
	  m_outputFileInfo[i].m_recordNum.push_back(
	    	   m_outputFileInfo[i].m_recordTotalNum);
	}
	//add by zhouq end
	return true;
}


void UserBillFacOutputFile::closeFile()
{
	for (int i=0; i<MAX_USERACCOUNT_CHANNELNUM; i++)
	{
		if (m_outputFileInfo[i].m_fileStat == '1')
		{
			//关闭流
			m_outputFileInfo[i].m_workOfStream.close();
			m_outputFileInfo[i].m_workOfStream.clear();
		}
	}
}


void UserBillFacOutputFile::resetFileTag(const int i)
{
	  /*
  	m_outputFileInfo[i].m_workFileName   = "";
  	m_outputFileInfo[i].m_outputFileName = "";
    m_outputFileInfo[i].m_idNum          =  0;
    m_outputFileInfo[i].m_recordNum      =  0;
    */
    m_outputFile[i]                      =  "";
    m_outputFilehead[i]                  =  "";
    m_workFile[i]                        =  "";
    m_workFilehead[i]                    =  "";
    m_seqNo[i]                           =  0;
    m_outputFileInfo[i].m_feeSum         =  0;
    m_outputFileInfo[i].m_recordTotalNum =  0;
    m_outputFileInfo[i].m_idTotalNum     =  0;
    m_outputFileInfo[i].m_fileStat       = '0';
    m_outputFileInfo[i].m_workFileName.clear();
	m_outputFileInfo[i].m_outputFileName.clear();
	m_outputFileInfo[i].m_idNum.clear();
	m_outputFileInfo[i].m_recordNum.clear();
}


bool UserBillFacOutputFile::linkUnlink()
{
	  //把各种类型工作文件搬迁到输出目录下，生成输出文件
	  for (int i=0; i<MAX_USERACCOUNT_CHANNELNUM; i++)
  	{
        //如果该通道无文件输出，继续
        if (m_outputFileInfo[i].m_fileStat == '0')
        {
            continue;
		}
		    //add by zhouq begin
		    int size = m_outputFileInfo[i].m_workFileName.size();
		    for (int j=0; j<size;j++)
		    {
		    	if (m_outputFileInfo[i].m_recordNum[j] > 0)
          {
          	if (rename(m_outputFileInfo[i].m_workFileName[j].c_str(),
                       m_outputFileInfo[i].m_outputFileName[j].c_str()) < 0)
            {
            	theErrorMessages->insert(ERROR_LINK_UNLINKE,
                        m_outputFileInfo[i].m_workFileName[j] + " ->" +
                        m_outputFileInfo[i].m_outputFileName[j] +
                        "--file rename error");
                return false;
            }
            theLogger->pEndOutput(m_performId,
                                  m_outputFileInfo[i].m_outputFileName[j].c_str(),
                                  m_outputFileInfo[i].m_recordNum[j],
                                  "TOTAL_ID_NUM=%d,ERR_ID_NUM=0,ERR_REC_NUM=0",
                                  m_outputFileInfo[i].m_idNum[j]);
          }
          else
          {
         	  if (unlink(m_outputFileInfo[i].m_workFileName[j].c_str()) != 0)
            {
            	theErrorMessages->insert(ERROR_LINK_UNLINKE,
                        m_outputFileInfo[i].m_workFileName[j] +
                        "--file unlink error");
                return false;
            }
          }
        }
        //add by zhouq end

/*
		    //如果该类型工作文件中数据记录数大于0，改名为输出文件
        if (m_outputFileInfo[i].m_recordNum > 0)
        {
            if (rename(m_outputFileInfo[i].m_workFileName.c_str(),
                       m_outputFileInfo[i].m_outputFileName.c_str()) < 0)
            {
            	theErrorMessages->insert(ERROR_LINK_UNLINKE,
                        m_outputFileInfo[i].m_workFileName + " ->" +
                        m_outputFileInfo[i].m_outputFileName +
                        "--file rename error");
                return false;
            }
            theLogger->pEndOutput(m_performId,
                                  m_outputFileInfo[i].m_outputFileName.c_str(),
                                  m_outputFileInfo[i].m_recordNum,
                                  "TOTAL_ID_NUM=%d,ERR_ID_NUM=0,ERR_REC_NUM=0,"
                                  "TOTAL_FEE_SUM=%lld,FILTER_FEE_SUM=0",
                                  m_outputFileInfo[i].m_idNum,
                                  m_outputFileInfo[i].m_feeSum);
        }
        //如果该通道文件中数据记录数等于0，直接删除
        else
        {
            if (unlink(m_outputFileInfo[i].m_workFileName.c_str()) != 0)
            {
            	theErrorMessages->insert(ERROR_LINK_UNLINKE,
                        m_outputFileInfo[i].m_workFileName +
                        "--file unlink error");
                return false;
            }
        }
*/
        //初始化输出文件标记
        resetFileTag(i);
    }

    m_performId = "";
    return true;
}


int  UserBillFacOutputFile::computeChannelNo(const ACCTID &acctId)
{
    if (acctId == INVALID_ACCTID)
    {
        return -1;
    }
    string sTmp = (char*)acctId;

	if (sTmp.length()<4)  //湖南小于4位的acctId归为0通道
	{
		return 0;
	}

	//return ((sTmp.substr(sTmp.length()-4,1))[0]-'0');
	return atoi(sTmp.substr(sTmp.length()-4, 4).c_str())/100;
}


bool UserBillFacOutputFile::openFile(const int &channelNo)
{
	//关闭清除输出流
	m_outputFileInfo[channelNo].m_workOfStream.close();
	m_outputFileInfo[channelNo].m_workOfStream.clear();

	//modify by zhouq
	/*m_outputFileInfo[channelNo].m_workOfStream.open(
	            m_outputFileInfo[channelNo].m_workFileName.c_str(),ios::out);*/
	m_outputFileInfo[channelNo].m_workOfStream.open(
	            m_workFile[channelNo].c_str(),ios::out);

	if (m_outputFileInfo[channelNo].m_workOfStream.rdstate() != ios::goodbit)
	{
		theErrorMessages->insert(ERROR_OPEN_FILE,
                             m_workFile[channelNo]+ "--open file error");
		m_outputFileInfo[channelNo].m_workOfStream.close();
		return false;
	}

	return true;
}

//add by zhouq
void UserBillFacOutputFile::swichOutputFile(const int &channelNo)
{
	m_seqNo[channelNo]++;
	char  seqNo[4];
	memset (seqNo, 0, sizeof(seqNo));
	sprintf(seqNo, "%03d", m_seqNo[channelNo]);

	m_workFile[channelNo] =  m_workFilehead[channelNo] + seqNo;
  m_outputFileInfo[channelNo].m_workFileName.push_back(m_workFile[channelNo]);
  m_outputFile[channelNo] =  m_outputFilehead[channelNo] + seqNo;
	m_outputFileInfo[channelNo].m_outputFileName.push_back(m_outputFile[channelNo]);

	openFile(channelNo);
}