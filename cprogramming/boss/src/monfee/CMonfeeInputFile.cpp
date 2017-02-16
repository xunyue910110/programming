#include "CMonfeeInputFile.h"
#include "appfrm/ErrorHandle.h"
#include "log4cpp/Logger.h"
#include <unistd.h>


extern ErrorMessages *const theErrorMessages;
extern ErrorMessages g_errormessages;
extern log4cpp::Logger *theLogger;


CMonfeeInputFile::CMonfeeInputFile()
{
    memset(m_buffer,0,sizeof(m_buffer));
    
    m_accountAddupInfo = NULL;
}    


CMonfeeInputFile::~CMonfeeInputFile()
{
    memset(m_buffer,0,sizeof(m_buffer));
} 


void CMonfeeInputFile::initialize(
                                const MonfeeConfig *monfeeConfig,
                                const string& performId,
                                AccountAddupInfo *accountAddupInfo)
{
    m_fileName = monfeeConfig->m_fileName;    
    m_inputFilePath = monfeeConfig->m_inputFilePath;
    m_inputFileFullName = m_inputFilePath + m_fileName;
    m_invalidFilepath = monfeeConfig->m_invalidFilePath;
    m_invalidFileFullName = m_invalidFilepath + m_fileName;
    
    m_fileType = m_fileName[0];
    
    // Modified by Malone start
    /*m_seperate = "\t";*/
    m_seperate = ",";
    // Modified by Malone end
    
    m_lastStrUserId = "";        
    memset(m_buffer,0,sizeof(m_buffer));
    
    m_accountLogInfo.m_idNum = 0;
    m_accountLogInfo.m_errorIdNum = 0;
    m_accountLogInfo.m_recordNum = 0;
    m_accountLogInfo.m_errorRecordNum = 0;
    m_accountLogInfo.m_fee = 0;
    m_accountLogInfo.m_filterFee = 0;

    m_performId = performId;
    m_accountAddupInfo = accountAddupInfo;
}


bool CMonfeeInputFile::getUserSubInfo(USERID& userId,
                         vector<UserSubInfo>& userSubInfoVector)
{
    //初始化输出参数
    userId = INVALID_USERID;
    userSubInfoVector.clear();
    
    //定义局部变量并赋初始值    

    bool                isValidRecord = false;        

    //如果输入文件不是重出帐用户文件
    if (m_fileType != FILE_TYPE_IN_U)
    {        
        if (m_lastStrUserId != "")
        {
            userId = m_lastStrUserId;                        
            userSubInfoVector.push_back(m_userSubInfo);
            
            if (m_inputFileStream.eof())
            {
                m_lastStrUserId = "";
                memset(&m_userSubInfo,0,sizeof(m_userSubInfo));
            }    
        }
        
        string strUserId("");
        //如果读取记录未到文件末尾
        while (!m_inputFileStream.eof()) 
        {
            //获取一行记录信息 
            memset(m_buffer,0,sizeof(m_buffer));           
            m_inputFileStream.getline(m_buffer,511);            
            
            char *tempchr=m_buffer;   
            
            //获取用户标识
            isValidRecord = false;
                
            tempchr = strtok(tempchr, m_seperate.c_str());
            if (tempchr != NULL)
            {
                strUserId = tempchr;
                isValidRecord = true;
            }
            
            //获取sp代码
            if (isValidRecord)
            {
                isValidRecord = false;
                
                tempchr = strtok(NULL, m_seperate.c_str());
                if (tempchr != NULL)
                {
                    m_userSubInfo.m_spCode = tempchr;
                    isValidRecord = true;
                }
            }
            
            //获取业务代码
            if (isValidRecord)
            {
                isValidRecord = false;
                
                tempchr = strtok(NULL, m_seperate.c_str());
                if (tempchr != NULL)
                {
                    m_userSubInfo.m_spBizCode = tempchr;
                    isValidRecord = true;
                }
            }
            
            //获取服务代码
            if (isValidRecord)
            {
                isValidRecord = false;
                
                tempchr = strtok(NULL, m_seperate.c_str());
                if (tempchr != NULL)
                {
                    m_userSubInfo.m_spServCode = tempchr;
                    isValidRecord = true;
                }
            }                   
            
            //获取接入渠道
            if (isValidRecord)
            {
                isValidRecord = false;
                
                tempchr = strtok(NULL, m_seperate.c_str());
                if (tempchr != NULL)
                {
                    m_userSubInfo.m_sourceType = tempchr;
                    isValidRecord = true;
                }
            }            
            
            //获取开始时间
            if (isValidRecord)
            {
                isValidRecord = false;
                
                tempchr = strtok(NULL, m_seperate.c_str());
                if (tempchr != NULL)
                {
                    m_userSubInfo.m_startDate = tempchr;
                    isValidRecord = true;
                }
            }
            
            //获取结束时间
            if (isValidRecord)
            {
                isValidRecord = false;
                
                tempchr = strtok(NULL, m_seperate.c_str());
                if (tempchr != NULL)
                {
                    m_userSubInfo.m_endDate = tempchr;
                    isValidRecord = true;
                }
            }
            
            //获取业务状态
            if (isValidRecord)
            {
                isValidRecord = false;
                
                tempchr = strtok(NULL, m_seperate.c_str());
                if (tempchr != NULL)
                {
                    m_userSubInfo.m_bizStateCode = tempchr[0];
                    isValidRecord = true;
                }
            }
            
            //获取计费类型
            if (isValidRecord)
            {
                isValidRecord = false;
                
                tempchr = strtok(NULL, m_seperate.c_str());
                if (tempchr != NULL)
                {
                    m_userSubInfo.m_billFlag = tempchr[0];
                    isValidRecord = true;
                }
            } 
            
            //获取集团客户编码
            if (isValidRecord)
            {
                isValidRecord = false;
                
                tempchr = strtok(NULL, m_seperate.c_str());
                if (tempchr != NULL)
                {
                    m_userSubInfo.m_ecSerialNumber = tempchr;
                    isValidRecord = true;
                }
            }
            
            //获取集团用户编码
            if (isValidRecord)
            {
                isValidRecord = false;
                
                tempchr = strtok(NULL, m_seperate.c_str());
                if (tempchr != NULL)
                {
                    m_userSubInfo.m_ecUserId = tempchr;
                    isValidRecord = true;
                }
            }
            
            //M文件读首次定购时间(usersubinfo::m_priorTime)和业务类型代码,
            //最后定购时间(m_lastTime)不需要获取
            if (m_fileType == 'M') 
            {
                if (isValidRecord)
                {
                    isValidRecord = false;
                    tempchr = strtok(NULL,m_seperate.c_str());
                    if (tempchr != NULL)
                    {
                        m_userSubInfo.m_priorTime = tempchr;
                        isValidRecord = true;
                    }
                }

                if (isValidRecord)
                {
                    isValidRecord = false;
                    tempchr = strtok(NULL,m_seperate.c_str());
                    if (tempchr != NULL)
                    {
                        //m_userSubInfo.m_lastTime = tempchr;
                        m_userSubInfo.m_bizTypeCode = tempchr; 
                        isValidRecord = true;
                    }
                }
            } 
            
            if (strlen(m_buffer) != 0)
            {
                //累计记录总数
                m_accountAddupInfo->m_recordNum++;
                m_accountLogInfo.m_recordNum++;
            }
            
            //记录无效，累计错误记录数
            if (!isValidRecord)
            {
                if (strlen(m_buffer) != 0)
                {
                    m_accountAddupInfo->m_errorRecordNum++;
                    m_accountLogInfo.m_errorRecordNum++;
                    //输入到无效文件中
                    m_invalidFileStream<<m_buffer<<endl;
                }
                
                if (m_inputFileStream.eof())
                {
                    m_lastStrUserId = "";
                    memset(&m_userSubInfo,0,sizeof(m_userSubInfo));
                    break;                       
                }
                else
                {
                    continue;
                }    
            }
            //记录有效，累计金额
            else
            {
                //m_accountLogInfo.m_fee += itemValue;
            }
            
            //如果是老用户,直接插入值
            if (strUserId == m_lastStrUserId || m_lastStrUserId == "")
            {
                userId = strUserId;                
                userSubInfoVector.push_back(m_userSubInfo);  
                
                if (m_inputFileStream.eof())
                {
                    m_lastStrUserId = "";
                    memset(&m_userSubInfo,0,sizeof(m_userSubInfo));                   
                    break;                       
                }
                
                m_lastStrUserId = strUserId;
            }
            //如果是新用户，记录最后值，跳出
            else
            {
                m_lastStrUserId = strUserId;                
                break;
            }     
        }
        
        //获取到费用，返回正确
        if (userSubInfoVector.size() > 0)
        {
            //累计用户数
            m_accountLogInfo.m_idNum++;
            
            return true;
        }
    }
    
    //返回失败
    return false;        
}


bool CMonfeeInputFile::openFile()
{
    //关闭并清理输入文件和无效文件流
    m_inputFileStream.close();
    m_inputFileStream.clear();
    
    m_invalidFileStream.close();
    m_invalidFileStream.clear();
    
    //打开输入文件关联到输入文件流
    m_inputFileStream.open(m_inputFileFullName.c_str(), ios::in);
    //失败，填写告警信息
    if (m_inputFileStream.rdstate() != ios::goodbit)
    {
        theErrorMessages->insert(ERROR_OPEN_FILE, 
                                 m_inputFileFullName + "--open file error");
        m_inputFileStream.close();
        
        return false;
    }    

    //打开无效文件关联到无效文件流
    m_invalidFileStream.open(m_invalidFileFullName.c_str(), ios::out);
    //失败，填写告警信息
    if (m_invalidFileStream.rdstate() != ios::goodbit)
    {
        theErrorMessages->insert(ERROR_OPEN_FILE, 
                                 m_invalidFileFullName + "--open file error");
        m_invalidFileStream.close();
        
        return false;
    }

    //针对输入文件日志开始处理
    theLogger->pBeginInput(m_performId,m_inputFileFullName.c_str(),
                           "TOTAL_ID_NUM=0,ERR_ID_NUM=0,ERR_REC_NUM=0,"
                           "TOTAL_FEE_SUM=0,FILTER_FEE_SUM=0");  
                                   
    return true;                                        
}


void CMonfeeInputFile::closeFile()
{
    //关闭并清理文件流
    m_inputFileStream.close();
    m_inputFileStream.clear();
    
    m_invalidFileStream.close();
    m_invalidFileStream.clear();
}


bool CMonfeeInputFile::linkUnlink()
{
    //删除输入文件
    if (::unlink(m_inputFileFullName.c_str()) != 0)
    {
        theErrorMessages->insert(ERROR_LINK_UNLINKE,m_inputFileFullName +
                                                    "-- file unlink error");
        
        return false;
    }

    //针对输入文件日志结束处理
    theLogger->pEndInput(m_performId,m_inputFileFullName.c_str(),
                         m_accountLogInfo.m_recordNum,
                         "TOTAL_ID_NUM=%d,ERR_ID_NUM=%d,ERR_REC_NUM=%d,"
                         "TOTAL_FEE_SUM=%lld,FILTER_FEE_SUM=%lld",
                         m_accountLogInfo.m_idNum,m_accountLogInfo.m_errorIdNum,
                         m_accountLogInfo.m_errorRecordNum,
                         m_accountLogInfo.m_fee,
                         m_accountLogInfo.m_filterFee);
                             
    //处理文件中无错误记录
    if (m_accountAddupInfo->m_errorRecordNum == 0)
    {
        //删除无效文件
        if (::unlink(m_invalidFileFullName.c_str()) != 0)
        {
            theErrorMessages->insert(ERROR_LINK_UNLINKE,m_invalidFileFullName +
                                                        "-- file unlink error");
            
            return false;
        }        
    }
    else
    {
        //针对输出文件日志处理
        theLogger->pBeginOutput(m_performId,m_invalidFileFullName.c_str(),
                                "TOTAL_ID_NUM=0,ERR_ID_NUM=0,ERR_REC_NUM=0,"
                                "TOTAL_FEE_SUM=0,FILTER_FEE_SUM=0");          
    
        theLogger->pEndOutput(m_performId,m_invalidFileFullName.c_str(),
                              m_accountAddupInfo->m_errorRecordNum,
                              "TOTAL_ID_NUM=0,ERR_ID_NUM=0,ERR_REC_NUM=0,"
                              "TOTAL_FEE_SUM=0,FILTER_FEE_SUM=0");        
    }
        
    return true;
}
