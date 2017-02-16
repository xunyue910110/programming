// ##########################################
// Source file : StandardFile.cpp
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

#include <unistd.h>
#include <sys/stat.h>

#include "StandardFile.h"
#include "FieldInfo.h"
#include "MyApplication.h"
#include "base/StringUtil.h"
#include "FieldInfos.h"
#include "AlertCode.h"

extern MyApplication g_application;

//##ModelId=485233970148
StandardFile::StandardFile(const int iSubChannel)
{
    m_fDes    = 0;
    m_cdrFlag = CdrFlag::getInstance();
    m_PpLog   = PpLog::getInstance();
    m_pFieldInfos = FieldInfos::getInstance(iSubChannel);

}

//##ModelId=48523314035B
int StandardFile::write()
{
    int len;
    FILE *fout;
    char pOutPutStr[1024];

    pOutPutStr[0] = 0;
    fout = m_fDes;

    if(m_outFileType.compare("TOKEN") == 0)
    {
        getOutCdrWithToken(pOutPutStr);
    }
    else if(m_outFileType.compare("FIX") == 0)
    {
        getOutCdrWithFix(pOutPutStr);
    }

    len = strlen(pOutPutStr);
    if(len <= 0)
    {
        theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, "file " + m_FileName + " 话单长度为0 !");;
    }

    if (fputs(pOutPutStr, fout) == EOF)
    {
        theErrorMessages->insert(PP_ERROR_WRITE_FILE,"致命错误：写文件" + m_FileName + "错误，程序退出!");
#ifndef PPTEST
        g_application.emergyExitAlert();
#endif
        exit(1);
    }
    return 1;
}

//##ModelId=4852333A00CB
bool StandardFile::reset(const string &outputFileName)
{
    char tmp[30];
    string theDesFileName;

    m_FileName = outputFileName;

    // 修改文件名前缀
    //m_FileName = "1" + m_FileName;

    theDesFileName = m_desBakPath + m_FileName;

    getNowTime();

    if(m_fDes != 0)
    {
       if(fclose(m_fDes) != 0)
       {
            theErrorMessages->insert(PP_CLOSE_FILE_ERROR,
                                     "fatal error: close the previous" + m_FileName + "failed, exit !");
#ifndef PPTEST
            g_application.emergyExitAlert();
#endif
            exit(1);
        }
        m_fDes = 0;
    }

    if ( (m_fDes = fopen(theDesFileName.c_str(), (char *)"w")) == NULL)         //打开输出文件错误
    {
        theErrorMessages->insert(PP_ERROR_OPEN_PHONE_FILE, "open des file(" + m_FileName + ")  error.?");
        return false;
    }
    setbuf(m_fDes, m_desBuffer);
    return true;
}

//##ModelId=4852335801A5
void StandardFile::close(int flag)
{
    int status;
    struct stat attr;
    string desBakName, rmCommand; // desName, repeatDesName;

    if(m_fDes != 0)                                                             //关闭输出标准文件
    {
       if(fclose(m_fDes) != 0)
       {
            theErrorMessages->insert(PP_CLOSE_FILE_ERROR,
                                     "fatal error: close the output file when finish" + m_FileName + "failed, exit !");
#ifndef PPTEST
            g_application.emergyExitAlert();
#endif
            exit(1);
        }

        m_fDes = 0;
        desBakName = m_desBakPath + m_FileName;
        stat(desBakName.c_str(), &attr);
        if(attr.st_size == 0)                                                   // 如果文件长度为零，删除该文件
        {
            rmCommand = "rm " + desBakName;
            system(rmCommand.c_str());
        }
        else
        {
            m_PpLog->setDesFileLenth(attr.st_size);                             // 记录输出文件大小
            //desName = m_desPath + m_FileName;
            //将标准话单从备份目录link到相应目录
            if(flag == 1)                                                       // 错误原始话单文件错误率大于阀值,删除标准话单
            {
                if(unlink(desBakName.c_str()) != 0)                             //删除标准话单
                {
                    theErrorMessages->insert(PP_ERROR_UNLINK_FILE,
                                               "临时标准话单文件(大于阀值)" + m_FileName + "不能删除!");
                }
            }
            else
            {
                // 切割大文件
                if(m_isSplitDes)
                {
                    if(!splitStdFile())
                    {
                        if(!linkFile(desBakName,m_desPath,m_FileName))
                        {
                          exit(1);
                        }
                    }
                }
                else
                {
                    if(!linkFile(desBakName,m_desPath,m_FileName))
                    {
                      exit(1);
                    }
                }

                //分发文件,不切割   add by zhangch2 2009-12-22
                for( int i=1; i<m_dispenPath.size(); i++ )
                {
                    if(!linkFile(desBakName,m_dispenPath[i],m_FileName))
                    {
                      exit(1);
                    }
                }
                
                if (!(m_isBackupDesFile == true))
                {
                    if(unlink( desBakName.c_str()) != 0)                        //删除临时标准话单
                    {
                       theErrorMessages->insert(PP_ERROR_UNLINK_FILE,
                                                  "临时标准话单文件(备份)" + m_FileName + "不能删除!");
                    }
                }
            }
        }
    }
}

bool StandardFile::linkFile(const string &srcPathName,const string &desPathName,const string &desFileName)
{
   string repeatDesName, desName;

   desName = desPathName + desFileName;
   if(link(srcPathName.c_str(), desName.c_str()) != 0)
   {
       repeatDesName = desName + ".01";
       //如果是由于stdpath目录下有重名文件而link不成功的话,改成"*.01"重试,如果还不成功,则退出
       if(link(srcPathName.c_str(), repeatDesName.c_str()) != 0)
       {
           theErrorMessages->insert(PP_ERROR_LINK_FILE, "file " + desFileName + "cannot move to the destination directory "+desPathName+"! exit!");
       #ifndef PPTEST
           g_application.emergyExitAlert();
       #endif
           return false;
           //exit(1);
       }
       else
       {
           theErrorMessages->insert(PP_ERROR_LINK_FILE, "file " + desFileName + "in the destination directory "+desPathName+"already exists! new file rename:"+desName+".01"+" output ");
       }
   }
   return true;
}
//##ModelId=48523562006D
bool StandardFile::initialize(const vector<string> &rOutPutFieldRules
			    					, const string &isBackup
			    					, const string &outFileType
			    					, const string &desPath
			    					, const string &desBakPath
			    					, const string &invalidPath
			    					, const string &isSplit
			    					, int maxCdrNum)
{
    StandardField tStandardField;

    if(isSplit.compare("yes")==0)
    {
        m_isSplitDes = true;
        m_maxCdrNum = maxCdrNum;
    }
    else
    {
        m_isSplitDes = false;
        m_maxCdrNum = 0;
    }

    if(outFileType.compare("FIX") == 0)
    {
        m_outFileType = "FIX";
    }
    else
    {
        m_outFileType = "TOKEN";

        if(outFileType.compare(0,2,"\\t") == 0)
        {
           m_token[0] = '\t';
           m_token[1] = 0;
        }
        else
        {
           strcpy(m_token, outFileType.c_str());                    // 标准话单字段分隔符初始化
        }
    }
    if (isBackup.compare("yes") == 0)
    {
        m_isBackupDesFile = true;
    }
    else
    {
        m_isBackupDesFile = false;
    }
    
    m_dispenPath.clear();
    StringUtil::split(desPath,";",m_dispenPath);
    m_desPath       = m_dispenPath[0];                //add by zhangch2 2009-12-22 第一个目录为预处理标准话单输出目录
    m_desBakPath    = desBakPath;
    m_invalidPath   = invalidPath;

    for(int i = 0; i < rOutPutFieldRules.size(); i++)
    {
        tStandardField.initialize(m_pFieldInfos,rOutPutFieldRules[i]);
        m_StandardField.push_back(tStandardField);
    }
    
    m_pField_ErrCode = m_pFieldInfos->getFieldInfoByName(ERROR_CODE);
	m_pFieldStartDate    = m_pFieldInfos->getFieldInfoByName(START_DATE);
    m_pFieldStartTime    = m_pFieldInfos->getFieldInfoByName(START_TIME);
    m_iSeq_CALL_TOTAL_FEE= m_pFieldInfos->getSequnceByName(CALL_TOTAL_FEE);    

    return true;
}

//##ModelId=485235CC0109
void StandardFile::assembleOutPutStr(int i, char *const &pOutPutStr)
{
    int len, filedLen, j;
    FieldInfo *tmpFieldInfo;

    len = m_StandardField[i].m_inputFieldNames.size();
    for(j = 0; j < len; ++j)                                // 此循环用于把多个字段值组合到一个字段里
    {
        if( m_StandardField[i].m_inPutFieldSeq[j] == OUTOF_FIELD_RANGE)
        {
        	continue;	
        }
        
        tmpFieldInfo    = 0;
        tmpFieldInfo    = (*m_pFieldInfos)[ m_StandardField[i].m_inPutFieldSeq[j] ];
        if(tmpFieldInfo != 0
        	&& tmpFieldInfo->m_iDesValueLen != 0 )
        {
            filedLen = m_StandardField[i].m_fieldLength;
            if(tmpFieldInfo->m_iDesValueLen > filedLen)
            {
                //若字段置长度大于配置文件中的配置长度,则从后往前截取
                strcat(pOutPutStr, tmpFieldInfo->m_FieldDesValue+tmpFieldInfo->m_iDesValueLen-filedLen);
            }
            else
            {
                // 给输出字段 填充字符
                if(m_StandardField[i].m_isLeftFill != NO_FILL)
                {
                    string tmpStr;
                    if(m_StandardField[i].m_isLeftFill == LEFT_FILL)
                    {
                        tmpStr = StringUtil::pad( tmpFieldInfo->m_FieldDesValue,
                                                   filedLen, m_StandardField[i].m_fillChar, true);
                    }
                    else if(m_StandardField[i].m_isLeftFill == RIGHT_FILL)
                    {
                        tmpStr = StringUtil::pad(tmpFieldInfo->m_FieldDesValue,
                                                   filedLen, m_StandardField[i].m_fillChar, false);
                    }
                    strcat(pOutPutStr,tmpStr.c_str());
                }
                else
                {
                    strcat(pOutPutStr, tmpFieldInfo->m_FieldDesValue);
                }
            }
            if(m_StandardField[i].m_inputFieldNames[j].compare(m_cdrFlag->m_fieldName) == 0)
            {
                if(m_cdrFlag->m_errorCode > 0)              // 输出的字段有错误
                {
                    m_cdrFlag->m_errorFlag = true;
                }
                else
                {
                    m_cdrFlag->m_errorFlag = false;
                }
            }
        }
        else
        {
            // 给输出字段 填充字符
            if((m_StandardField[i].m_isLeftFill != NO_FILL))
            {
                string tmpStr;
                if(m_StandardField[i].m_isLeftFill == LEFT_FILL)
                {
                    tmpStr = StringUtil::pad( "", m_StandardField[i].m_fieldLength, m_StandardField[i].m_fillChar, true);
                }
                else if(m_StandardField[i].m_isLeftFill == RIGHT_FILL)
                {
                    tmpStr = StringUtil::pad("", m_StandardField[i].m_fieldLength, m_StandardField[i].m_fillChar, false);
                }
                strcat(pOutPutStr, tmpStr.c_str());
            }
            else
            {
                strcat(pOutPutStr, "");
            }
        }
    }
}

//##ModelId=4852362D0261
bool StandardFile::getOutCdrWithToken(char *pOutPutStr)
{
    int len, count;
    char tmp[4];

    len = m_StandardField.size();
    for(int i = 0; i < len; i++)
    {
        count = m_StandardField[i].m_inFieldCount;
        if((m_StandardField[i].m_outputFieldName).compare(ERROR_CODE) == 0)
        {
            if(!m_cdrFlag->m_errorFlag)                     // 如果不是要输出的字段，则错误号置0
            {
                m_cdrFlag->m_errorCode = 0 ;
                m_cdrFlag->m_isError   = false;
            }
            if( m_pField_ErrCode->m_iDesValueLen == 0)
            {
                sprintf(tmp, "%03d", m_cdrFlag->m_errorCode);
                strcat(pOutPutStr, tmp);
            }
            else
            {
                // 由于convert段中,会对ERROR_CODE重新付值,会对日志统计有影响
                // 这里把 m_cdrFlag->m_isError 的值置成true,便于日志统计
                if(strncmp(m_pField_ErrCode->m_FieldDesValue, "000", 3) != 0)
                {
                    m_cdrFlag->m_isError = true;
                }
                else
                {
                    m_cdrFlag->m_isError = false;
                }
                strcat(pOutPutStr, m_pField_ErrCode->m_FieldDesValue);
            }
        }
        else if((m_StandardField[i].m_outputFieldName).compare("DEALTIME") == 0)
        {
            strcat(pOutPutStr, m_nowTime);
        }
        else if (count > 0)
        {
            assembleOutPutStr(i, pOutPutStr);
        }
        strcat(pOutPutStr, (char *)m_token);
    }

    // 记录当前原始文件中cdr的最早开始时间和最晚开始时间
    if(m_pFieldStartDate->m_iDesValueLen != 0 
    	&& m_pFieldStartTime->m_iDesValueLen != 0)
    {
        m_PpLog->setMinMaxStartDate(m_pFieldStartDate->m_FieldDesValue
        								, m_pFieldStartTime->m_FieldDesValue);
    }
    
    if(m_iSeq_CALL_TOTAL_FEE!=OUTOF_FIELD_RANGE
    	&& ((*m_pFieldInfos)[m_iSeq_CALL_TOTAL_FEE])->m_iDesValueLen !=0 )
    {
    	m_cdrFlag->m_cdrFee = atoi( ((*m_pFieldInfos)[m_iSeq_CALL_TOTAL_FEE])->m_FieldDesValue);	
    }
    
    pOutPutStr[strlen(pOutPutStr) - 1] = 0;                   // 去除行尾多余的分割符号
    strcat(pOutPutStr, "\n");
    return true;
}

//##ModelId=4852364E03B9
bool StandardFile::getOutCdrWithFix( char *pOutPutStr)
{
    return true;
}

//##ModelId=4852366B03D8
void StandardFile::getNowTime(void)
{
    memset(m_nowTime, 0, sizeof(m_nowTime));
    if((m_PpLog->getStartDateTime()).size() >= 14)
    {
        strncpy(m_nowTime, m_PpLog->getStartDateTime().c_str() + 4, 10);
    }
}

//##ModelId=485236800157
bool StandardFile::conveyCdr(const int iFileNameSequnce, const int iFileSize)
{
    //const char TMP_PRIFIX[2] = {'#','\0'};
    char cTmp[5], cdrBuffer[1024];
    string desTmpFileName, desFileName;
    FILE *fp;

    sprintf(cTmp, "_%03d", iFileNameSequnce);
    desTmpFileName = m_desPath + TMP_PRIFIX + m_FileName + cTmp;

    if ((fp = fopen(desTmpFileName.c_str(), (char *)"w")) == NULL)  //打开输出文件错误
    {
        theErrorMessages->insert(PP_ERROR_OPEN_PHONE_FILE, "open des file(" + desTmpFileName + ")  error.?");
        return false;
    }

    for(int i = 0; i < iFileSize; i++)
    {
        if (fgets(cdrBuffer, 1024, m_fDes) == NULL)
        {
            return false;
        }
        fputs(cdrBuffer, fp);
    }

    if(fclose(fp) != 0)
    {
        theErrorMessages->insert(PP_CLOSE_FILE_ERROR,
                                  "致命错误：关闭文件" + desTmpFileName + "失败!");
        return false;
    }

    desFileName = m_desPath + m_FileName + cTmp;;
    rename(desTmpFileName.c_str(), desFileName.c_str());

    return true;
}

//##ModelId=4852369D001F
bool StandardFile::splitStdFile(void)
{
    int iCdrCount,i, iMaxCdrNum, iFileNameSequnce;
    string theDesBakFileName;

    iCdrCount  = iFileNameSequnce = 0;
    iMaxCdrNum = m_PpLog->getTotalCdrs();

    if(m_maxCdrNum <= 0)
  {
      return false;
  }

    //如果话单中的cdr数量大于拆分值10%以上时,才拆分(防止例如:m_maxCdrNum + 1条话单也拆成两个文件的情况出现)
    if( (iMaxCdrNum - m_maxCdrNum) / (float)m_maxCdrNum > 0.1)
    {
        theDesBakFileName = m_desBakPath + m_FileName;

        if ((m_fDes = fopen(theDesBakFileName.c_str(), (char *)"r")) == NULL)   //打开输出文件错误
        {
            return false;
        }

        while(iCdrCount < iMaxCdrNum)
        {
            //如果话单中剩余的cdr数量大于拆分值10%以上时,才拆分
            if((iMaxCdrNum - iCdrCount) / (float)m_maxCdrNum > 1.1)
            {
                if(!conveyCdr(iFileNameSequnce, m_maxCdrNum))
                {
                    fclose(m_fDes);
                    m_fDes = 0;
                    return false;
                }
            }
            //否则将剩余的话单一次性输出到一个标准文件中
            else
            {
                if(!conveyCdr(iFileNameSequnce,iMaxCdrNum- iCdrCount))
                {
                    fclose(m_fDes);
                    m_fDes = 0;
                    return false;
                }
                break;
            }
            iCdrCount = iCdrCount + m_maxCdrNum;
            iFileNameSequnce++;
        }

        fclose(m_fDes);
        m_fDes = 0;
        return true;
    }
    else
    {
        return false;
    }
}

