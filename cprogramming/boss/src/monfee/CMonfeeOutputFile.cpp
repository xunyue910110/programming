#include "CMonfeeOutputFile.h"
#include "CMonfeeInternalData.h"
#include "appfrm/ErrorHandle.h"
#include "log4cpp/Logger.h"
#include <unistd.h>


extern ErrorMessages *const theErrorMessages;
extern ErrorMessages g_errormessages;
extern log4cpp::Logger *theLogger;


CMonfeeOutputFile::CMonfeeOutputFile()
{
    m_batchNo = 0;    
    m_monfeeConfig =NULL;
}

	
CMonfeeOutputFile::~CMonfeeOutputFile()
{
}	


void CMonfeeOutputFile::initialize(const string& performId,
                                       MonfeeConfig *monfeeConfig)
{
    
    //初始化配置信息、日志信息
    m_monfeeConfig = monfeeConfig;
    m_performId = performId;
    
    //初始文件序号、操作流水标识和操作类型标志
    m_batchNo = 0;
    
    //初始化输出结构信息	
	for (int i=0; i<MAX_MONTHFEE_OUTFILETYPE; i++)
	{
		m_outputFileInfo[i].m_idNum = 0;
		m_outputFileInfo[i].m_recordNum = 0;
		m_outputFileInfo[i].m_feeSum = 0;
		m_outputFileInfo[i].m_fileStat = 0;
	}
	
	// modified by Malone start
	
	//初始工作文件信息
//	m_outputFileInfo[WORK_FILE_D].m_workFileName = 
//	    m_monfeeConfig->m_workFilePath+FILE_TYPE_OUT_D+
//	    m_monfeeConfig->m_fileName.substr(1);
//	
//	m_outputFileInfo[WORK_FILE_P].m_workFileName = 
//	    m_monfeeConfig->m_workFilePath+FILE_TYPE_OUT_P+
//	    m_monfeeConfig->m_fileName.substr(1);
//	
//	m_outputFileInfo[WORK_FILE_I].m_workFileName = 
//	    m_monfeeConfig->m_workFilePath+FILE_TYPE_OUT_I+
//	    m_monfeeConfig->m_fileName.substr(1);
//	
	m_outputFileInfo[WORK_FILE_B].m_workFileName = 
	    m_monfeeConfig->m_workFilePath+FILE_TYPE_OUT_B+
	    m_monfeeConfig->m_fileName.substr(1);

	m_outputFileInfo[WORK_FILE_C].m_workFileName = 
	    m_monfeeConfig->m_workFilePath+FILE_TYPE_OUT_C+
	    m_monfeeConfig->m_fileName.substr(1);	    
	
	m_outputFileInfo[WORK_FILE_N].m_workFileName = 
	    m_monfeeConfig->m_workFilePath+FILE_TYPE_OUT_N+
	    m_monfeeConfig->m_fileName.substr(1);    
	  
	// modified by Malone end  	 
	
  char    batchNo[8+1];
  string  strBacthNo;
  memset(batchNo, 0, sizeof(batchNo));
  
  sprintf(batchNo, "%08d", m_batchNo);
  strBacthNo = batchNo;
  memset(batchNo, 0, sizeof(batchNo));
  
  string  fileName = m_monfeeConfig->m_fileName.substr(1) + strBacthNo;
  string  cdrFileName = m_monfeeConfig->m_cdrFileName + strBacthNo;
  
  m_outputFileInfo[WORK_FILE_C].m_outputFileName = WORK_FILE_C + cdrFileName;

  m_outputFileInfo[WORK_FILE_C].m_outputFileFullName = m_monfeeConfig->m_outputFilePath_C + WORK_FILE_C + cdrFileName;

	//初始输出文件信息
//	m_outputFileInfo[WORK_FILE_D].m_outputFileName = FILE_TYPE_OUT_D + cdrFileName;
//	m_outputFileInfo[WORK_FILE_D].m_outputFileFullName = m_monfeeConfig->m_outputFilePath_D + FILE_TYPE_OUT_D + cdrFileName;
//	
//        m_outputFileInfo[WORK_FILE_P].m_outputFileName = 
//	    m_monfeeConfig->m_outputFilePath_P + 
//	    									FILE_TYPE_OUT_P + cdrFileName;
//		
//        m_outputFileInfo[WORK_FILE_I].m_outputFileName = 
//	    m_monfeeConfig->m_outputFilePath_I + 
//	    									FILE_TYPE_OUT_I + cdrFileName;
//
//	    // Add by Malone 2007-7-19 10:43 start
//	      m_outputFileInfo[WORK_FILE_P].m_outputBakupFileName = 
//	    m_monfeeConfig->m_outputBakupFilePath_P + 
//	    									FILE_TYPE_OUT_P + cdrFileName;
//		
//        m_outputFileInfo[WORK_FILE_I].m_outputBakupFileName = 
//	    m_monfeeConfig->m_outputBakupFilePath_I + 
//	    									FILE_TYPE_OUT_I + cdrFileName;
//      // Add by Malone 2007-7-19 10:43 end

	m_outputFileInfo[WORK_FILE_B].m_outputFileName = FILE_TYPE_OUT_B + cdrFileName;
	m_outputFileInfo[WORK_FILE_B].m_outputFileFullName = m_monfeeConfig->m_outputFilePath_B + FILE_TYPE_OUT_B + cdrFileName;
	
	m_outputFileInfo[WORK_FILE_N].m_outputFileName = FILE_TYPE_OUT_N + cdrFileName;    	
	m_outputFileInfo[WORK_FILE_N].m_outputFileFullName = m_monfeeConfig->m_outputFilePath_N + FILE_TYPE_OUT_N + cdrFileName;    	    
}	


bool CMonfeeOutputFile::dispatch(CMonfeeInternalData &monfeeInternalData)
{		
//	if (!writeFile(WORK_FILE_D,monfeeInternalData))
//	{
//		return false;
//	}
//	if (!writeFile(WORK_FILE_P,monfeeInternalData))
//	{
//		return false;
//	}
//	if (!writeFile(WORK_FILE_I,monfeeInternalData))
//	{
//		return false;
//	}	
	if (!writeFile(WORK_FILE_B,monfeeInternalData))
	{
		return false;
	}	
	if (!writeFile(WORK_FILE_N,monfeeInternalData))
	{
		return false;
	}
		
	return true;	
}

	
bool CMonfeeOutputFile::writeFile(const int &fileType,
                            vector<AccountCheckInfo>& accountCheckInfoVector)
{
	//文件如果没有打开，打开文件
	if (m_outputFileInfo[fileType].m_fileStat == 0)
	{
		if (!openFile(fileType))
		{
			return false;
		}
		m_outputFileInfo[fileType].m_fileStat = 1;	
	}
	
	//输出结果并累计记录数
	int                                 i = 0;
	vector<AccountCheckInfo>::iterator  iter;	
	for (iter=accountCheckInfoVector.begin(); 
		 iter!=accountCheckInfoVector.end(); iter++)
	{
		i++;
		
		m_outputFileInfo[fileType].m_workOfStream   <<
        		        m_performId                 << FIELD_SPLIT <<  
        		        iter->m_checkContentCode    << FIELD_SPLIT << 
        		        iter->m_itemCode            << FIELD_SPLIT << 
        		        iter->m_factValue           << FIELD_SPLIT << 
        		        iter->m_minValue            << FIELD_SPLIT <<
        		        iter->m_maxValue            << endl;
	}

	//记录户数、记录总数、费用总金额
	m_outputFileInfo[fileType].m_idNum += 0;
	m_outputFileInfo[fileType].m_recordNum += i;
	m_outputFileInfo[fileType].m_feeSum += 0;
	
	return true;
}


void CMonfeeOutputFile::closeFile()
{
	for (int i=0; i<MAX_MONTHFEE_OUTFILETYPE; i++)
	{
		if (m_outputFileInfo[i].m_fileStat == 1)
		{
			//关闭流
			m_outputFileInfo[i].m_workOfStream.close();
			m_outputFileInfo[i].m_workOfStream.clear();
        }		
	}	
}

	
bool CMonfeeOutputFile::linkUnlink(const char &errorTag)
{
  
  //系统数据异常
  if (errorTag != '0')
  {
      //初始输出文件信息
//    m_outputFileInfo[WORK_FILE_D].m_outputFileFullName = 
//    m_monfeeConfig->m_outputFilePath_D + string("error/") + m_outputFileInfo[WORK_FILE_D].m_outputFileName;
//
//        m_outputFileInfo[WORK_FILE_P].m_outputFileName = 
//	    m_monfeeConfig->m_outputFilePath_P + string("error/") +
//	    FILE_TYPE_OUT_P + cdrFileName ;
//	
//        m_outputFileInfo[WORK_FILE_I].m_outputFileName = 
//	    m_monfeeConfig->m_outputFilePath_I + string("error/") +
//	    FILE_TYPE_OUT_I + cdrFileName;

    m_outputFileInfo[WORK_FILE_B].m_outputFileFullName = 
    m_monfeeConfig->m_outputFilePath_B + string("error/") + m_outputFileInfo[WORK_FILE_B].m_outputFileName;
    
    m_outputFileInfo[WORK_FILE_N].m_outputFileFullName = 
    m_monfeeConfig->m_outputFilePath_N + string("error/") + m_outputFileInfo[WORK_FILE_N].m_outputFileName;     
  }
   
    //把各种类型工作文件搬迁到输出目录下，生成输出文件
	for (int i=0; i<MAX_MONTHFEE_OUTFILETYPE; i++)
	{
    //如果该类型文件无输出，继续
    if (m_outputFileInfo[i].m_fileStat == 0)
    {
        continue;
		}
		
		//如果该类型工作文件中数据记录数大于0，改名为输出文件	
    if (m_outputFileInfo[i].m_recordNum > 0)
    {
//        	  //Add by Malone 2007-7-19 9:03 start
//        	  //对于P文件和I文件，当配置文件中有复制输出路径时进行复制。(计费使用)
//        	  if ((i == WORK_FILE_P && m_monfeeConfig->m_outputBakupFilePath_P != "null/") ||
//        	  	  (i == WORK_FILE_I && m_monfeeConfig->m_outputBakupFilePath_I != "null/"))
//        	  {
//        			  if (link(m_outputFileInfo[i].m_workFileName.c_str(),
//                       		 m_outputFileInfo[i].m_outputBakupFileName.c_str()) < 0)
//                {
//                		theErrorMessages->insert(ERROR_LINK_UNLINKE,
//                       		 m_outputFileInfo[i].m_workFileName + " ->" + 
//                       		 m_outputFileInfo[i].m_outputBakupFileName + 
//                       		 "--file link error");	
//                		return false;
//            		}
//        	  }
//        	
//        	//Add by Malone 2007-7-19 9:03 end
        if (rename(m_outputFileInfo[i].m_workFileName.c_str(),
                   m_outputFileInfo[i].m_outputFileFullName.c_str()) < 0)
        {
            theErrorMessages->insert(ERROR_LINK_UNLINKE,
                    m_outputFileInfo[i].m_workFileName + " ->" + 
                    m_outputFileInfo[i].m_outputFileFullName + 
                    "--file rename error");	
            return false;
        }
        //针对输出文件日志处理
        theLogger->pBeginOutput(m_performId,
                            m_outputFileInfo[i].m_outputFileFullName.c_str(),
                            "TOTAL_ID_NUM=0,ERR_ID_NUM=0,ERR_REC_NUM=0,"
                            "TOTAL_FEE_SUM=0,FILTER_FEE_SUM=0");          

        theLogger->pEndOutput(m_performId,
                            m_outputFileInfo[i].m_outputFileFullName.c_str(),
                            m_outputFileInfo[i].m_recordNum,
                            "TOTAL_ID_NUM=%d,ERR_ID_NUM=0,ERR_REC_NUM=0,"
                            "TOTAL_FEE_SUM=%lld,FILTER_FEE_SUM=0",
                            m_outputFileInfo[i].m_idNum,
                            m_outputFileInfo[i].m_feeSum);
    }
    //如果该类型工作文件中数据记录数等于0，直接删除
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
    
    //初始化输出文件
    m_outputFileInfo[i].m_idNum = 0;
    m_outputFileInfo[i].m_recordNum = 0;
    m_outputFileInfo[i].m_feeSum = 0;
    m_outputFileInfo[i].m_fileStat = 0;        
  }
    
  char    batchNo[8+1];
  string  strBacthNo;
  memset(batchNo, 0, sizeof(batchNo));
  
  m_batchNo++;
  
  sprintf(batchNo, "%08d", m_batchNo);
  strBacthNo = batchNo;
  memset(batchNo, 0, sizeof(batchNo));
  
  string  fileName = m_monfeeConfig->m_fileName.substr(1) + strBacthNo;
  string  cdrFileName = m_monfeeConfig->m_cdrFileName + strBacthNo;
  
  m_outputFileInfo[WORK_FILE_C].m_outputFileName = WORK_FILE_C + cdrFileName;
	m_outputFileInfo[WORK_FILE_C].m_outputFileFullName = m_monfeeConfig->m_outputFilePath_C + WORK_FILE_C + cdrFileName;

  //初始输出文件信息
//  m_outputFileInfo[WORK_FILE_D].m_outputFileName = FILE_TYPE_OUT_D + cdrFileName;
//	m_outputFileInfo[WORK_FILE_D].m_outputFileFullName = m_monfeeConfig->m_outputFilePath_D + FILE_TYPE_OUT_D + cdrFileName;
//
//        m_outputFileInfo[WORK_FILE_P].m_outputFileName = 
//	    m_monfeeConfig->m_outputFilePath_P + 
//	    									FILE_TYPE_OUT_P + cdrFileName;
//		
//        m_outputFileInfo[WORK_FILE_I].m_outputFileName = 
//	    m_monfeeConfig->m_outputFilePath_I + 
//	    									FILE_TYPE_OUT_I + cdrFileName;
//
//	    // Add by Malone 2007-7-19 10:43 start
//	      m_outputFileInfo[WORK_FILE_P].m_outputBakupFileName = 
//	    m_monfeeConfig->m_outputBakupFilePath_P + 
//	    									FILE_TYPE_OUT_P + cdrFileName;
//		
//        m_outputFileInfo[WORK_FILE_I].m_outputBakupFileName = 
//	    m_monfeeConfig->m_outputBakupFilePath_I + 
//	    									FILE_TYPE_OUT_I + cdrFileName;
//      // Add by Malone 2007-7-19 10:43 end

  m_outputFileInfo[WORK_FILE_B].m_outputFileName = FILE_TYPE_OUT_B + cdrFileName;
	m_outputFileInfo[WORK_FILE_B].m_outputFileFullName = m_monfeeConfig->m_outputFilePath_B + FILE_TYPE_OUT_B + cdrFileName;
	
	m_outputFileInfo[WORK_FILE_N].m_outputFileName = FILE_TYPE_OUT_N + cdrFileName;
	m_outputFileInfo[WORK_FILE_N].m_outputFileFullName = m_monfeeConfig->m_outputFilePath_N + FILE_TYPE_OUT_N + cdrFileName;
	    
	return true;					
}
	
	
bool CMonfeeOutputFile::openFile(const int &fileType)
{
	//关闭清除输出流
	m_outputFileInfo[fileType].m_workOfStream.close();
	m_outputFileInfo[fileType].m_workOfStream.clear();
	
	//将输出流和输出工作文件关联
	m_outputFileInfo[fileType].m_workOfStream.open(
	            m_outputFileInfo[fileType].m_workFileName.c_str(),ios::out);
	if (m_outputFileInfo[fileType].m_workOfStream.rdstate() != ios::goodbit)
	{
		m_outputFileInfo[fileType].m_workOfStream.close();
		theErrorMessages->insert(ERROR_OPEN_FILE,
      		m_outputFileInfo[fileType].m_workFileName + "--file open error");	
	    
		return false;
	}	
	
	return true;
}

	
bool CMonfeeOutputFile::writeFile(const int& fileType,
                                      CMonfeeInternalData& monfeeInternalData)
{
	int     recordNum = 0;
	int     feeSum = 0;
	
	//如果文件没有打开，打开文件
	if (m_outputFileInfo[fileType].m_fileStat == 0)
	{
		if (!openFile(fileType))
		{
			return false;
		}	
        m_outputFileInfo[fileType].m_fileStat = 1;
	}
	
	//输出数据
	monfeeInternalData.out(fileType, m_outputFileInfo[fileType].m_workOfStream,m_outputFileInfo[fileType].m_outputFileName,
	                     recordNum, feeSum);
	
	//记录户数、记录总数、费用总金额
	m_outputFileInfo[fileType].m_idNum += 1;
	m_outputFileInfo[fileType].m_recordNum += recordNum;
	m_outputFileInfo[fileType].m_feeSum += feeSum;
	
	return true;	
}
