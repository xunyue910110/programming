#include "AcctBillFacOutputFile.h"
#include "appfrm/ErrorHandle.h"
#include "log4cpp/Logger.h"

extern log4cpp::Logger *theLogger;

AcctBillFacOutputFile::AcctBillFacOutputFile()
{
//序列号初始化
 m_seqNo = 0;
}


AcctBillFacOutputFile::~AcctBillFacOutputFile()
{
}


bool AcctBillFacOutputFile::init(const string& performId,
                                 const BillFacConfig*  pBillFacConfig,
                                 const string& strBcycFromBFileName, 
                                 const string& strOperateTime)
{
	int i;
	string fileNameHead = 'A' + strBcycFromBFileName.substr(0,6);
	string fileNameTail = strBcycFromBFileName.substr(2,6) 
	                                  + strOperateTime.substr(2);
	string strChannelNo;
	char   tmp[4];
	memset (tmp, 0, sizeof(tmp));	
	
	++m_seqNo;
	if(m_seqNo>999)
	{
     m_seqNo = 0;
	}
  
  char  seqNo[4];
  memset (seqNo, 0, sizeof(seqNo));
  sprintf(seqNo, "%03d", m_seqNo);
	
	m_performId = performId;
	
	//初始化输出结构信息	
	for (i=0; i<MAX_USERACCOUNT_CHANNELNUM; i++)
	{
		resetFileTag(i);
		
		//strChannelNo不足3位，前面补零
		sprintf(tmp, "%03d", i);
		strChannelNo = tmp;
		memset (tmp, 0, sizeof(tmp));
		
		m_outputFileInfo[i].m_workFileName = 
		                    pBillFacConfig->m_outputFileWorkPath + 
		                    fileNameHead + strChannelNo + fileNameTail+seqNo;
		                    
		m_outputFileInfo[i].m_outputFileName = 
		                    pBillFacConfig->m_outputFilePath[i] +  
		                    fileNameHead + strChannelNo + fileNameTail+seqNo;
		
		if (openFile(i))
		{
		    m_outputFileInfo[i].m_fileStat = '1';
		}
		else
		{
			return false;
		}
	}
	
	return true;
}


bool AcctBillFacOutputFile::writeFile(const ACCTBILLMAP& acctBillMap)
{
    ACCTBILLMAPCITER itr,itrend;
	
	vector<AcctBill>::const_iterator vecitr;
	
	int iChannel;
	
	itrend=acctBillMap.end();	
	
    for (itr=acctBillMap.begin(); itr!=itrend; itr++)
    {
    	iChannel = computeChannelNo((*itr).first.m_acctId);
		if (iChannel == -1)
		{
			cout << "获取通道号码失败" << endl;
			/*theErrorMessages->insert(ERROR_GET_USERCHANNELNO,
                               (string)(char*)((*itr).first.m_acctId) + "--获取通道号码失败");*/
		}
		//通道内客户记录数目+1
		m_outputFileInfo[iChannel].m_idNum ++;
		if (m_outputFileInfo[iChannel].m_fileStat == '1')
		{	
			//如果本通道是第一次数出客户记录
			if (m_outputFileInfo[iChannel].m_idNum == 1)
			{
				//针对输出文件日志处理开始
			    theLogger->pBeginOutput(m_performId,
		                  m_outputFileInfo[iChannel].m_outputFileName.c_str(),
		                  "TOTAL_ID_NUM=0,ERR_ID_NUM=0,ERR_REC_NUM=0,"
		                  "TOTAL_FEE_SUM=0,FILTER_FEE_SUM=0");
		    }
			for (vecitr=(*itr).second.begin(); 
			              vecitr!=(*itr).second.end(); vecitr++)
			{
				m_outputFileInfo[iChannel].m_workOfStream <<
		        (*itr).first               <<
		        vecitr ->m_eparchyCode << '\t'  <<
		        vecitr->m_detailItemCode   << '\t' << 
		        vecitr->m_initfee          << '\t' <<
		        vecitr->m_fee              << '\t' << 
		        vecitr->m_adiscount        << '\t' << 
		        vecitr->m_adjustBefore     << '\t' <<
		        vecitr->m_acctDay          <<endl;
		        m_outputFileInfo[iChannel].m_recordNum++;
		        m_outputFileInfo[iChannel].m_feeSum += vecitr->m_fee;
			}
		}
		else
		{
			//该通道输出文件流尚未打开
			return false;
		}
	}
	return true;
}


void AcctBillFacOutputFile::closeFile()
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


void AcctBillFacOutputFile::resetFileTag(const int i)
{
	m_outputFileInfo[i].m_workFileName   = "";
	m_outputFileInfo[i].m_outputFileName = "";	
    m_outputFileInfo[i].m_idNum          =  0;
    m_outputFileInfo[i].m_recordNum      =  0;
    m_outputFileInfo[i].m_feeSum         =  0;
    m_outputFileInfo[i].m_fileStat       = '0';
}


bool AcctBillFacOutputFile::linkUnlink()
{
	//把各种类型工作文件搬迁到输出目录下，生成输出文件
	for (int i=0; i<MAX_USERACCOUNT_CHANNELNUM; i++)
	{
        //如果该通道无文件输出，继续
        if (m_outputFileInfo[i].m_fileStat == '0')
        {
            continue;
		}    
		
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
        
        //初始化输出文件标记
        resetFileTag(i);
    }
    
    m_performId = "";
    return true;	
}


int  AcctBillFacOutputFile::computeChannelNo(const ACCTID &acctId)
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
	
	return ((sTmp.substr(sTmp.length()-4,1))[0]-'0');
}


bool AcctBillFacOutputFile::openFile(const int &channelNo)
{	
	//关闭清除输出流
	m_outputFileInfo[channelNo].m_workOfStream.close();
	m_outputFileInfo[channelNo].m_workOfStream.clear();
	
	m_outputFileInfo[channelNo].m_workOfStream.open(
	            m_outputFileInfo[channelNo].m_workFileName.c_str(),ios::out);
	if (m_outputFileInfo[channelNo].m_workOfStream.rdstate() != ios::goodbit)
	{
		theErrorMessages->insert(ERROR_OPEN_FILE, 
                                  m_outputFileInfo[channelNo].m_workFileName 
                                                        + "--open file error");
		m_outputFileInfo[channelNo].m_workOfStream.close();
		return false;
	}   
	
	return true;
}

