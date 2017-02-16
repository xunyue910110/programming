// ##########################################
// Source file : OutputFile.cpp
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

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef WIN32
    //#include "win32.h"
#else
    #include <sys/mman.h>
#endif

#include "PpLog.h"
#include "OutputFile.h"
#include "MyApplication.h"
#include "StandardFile.h"
#include "FieldInfos.h"
#include "AlertCode.h"


extern MyApplication g_application;

//##ModelId=4850D16C00AB
OutputFile::OutputFile(const int iSubChannel)
{
    m_srcfd           = 0;
    m_cdrFlag         = CdrFlag::getInstance();
    m_PpLog_          = PpLog::getInstance();
    m_isCheckCdrNo    = false;
    
    m_pStandardFile     = new StandardFile(iSubChannel);
    m_pStandardLongFile = new StandardFile(iSubChannel);

    m_pFieldInfos = FieldInfos::getInstance(iSubChannel);
   
}

//##ModelId=4850D2290148
OutputFile::~OutputFile()
{
    closeFiles(0);
}

//##ModelId=4850D255038A
int OutputFile::write()
{
    char tmp[10];
    int len, cdrNo = 0;
    
	if(m_pField_CallType->m_iDesValueLen != 0)
    {
        // m_cdrFlag->m_callType 的类型是 char[4],要防止越界
        if(m_pField_CallType->m_iDesValueLen < 4)
        {
            strcpy( m_cdrFlag->m_callType, m_pField_CallType->m_FieldDesValue);
        }
        else
        {
            strncpy(m_cdrFlag->m_callType, m_pField_CallType->m_FieldDesValue, 3);
            m_cdrFlag->m_callType[3] = 0;
        }
    }

    if (m_isSpliteLongCdr)
    {
        int rec_type = 0;
        int errorCode = 0;

        if(m_pField_RecordType->m_iDesValueLen != 0)
        {
            rec_type = atoi(m_pField_RecordType->m_FieldDesValue);
        }
        if(m_pField_ErrCode->m_iDesValueLen != 0)
        {
            errorCode = atoi(m_pField_ErrCode->m_FieldDesValue);
        }
        if(  m_pField_CallType->m_iDesValueLen != 0
        	 && (strcmp(m_pField_CallType->m_FieldDesValue,"FF") != 0) 
        	 && (rec_type == 1 || rec_type == 2 || rec_type == 3) 
        	 && (errorCode == 0)
          )
        {
            return m_pStandardLongFile->write();
        }
        else
        {
            return m_pStandardFile->write();
        }
    }
    else
    {
        return m_pStandardFile->write();
    }
}

//##ModelId=4850D27B0280
bool OutputFile::reset(string fileName)
{
    if(m_isSpliteLongCdr)
    {
        if(!m_pStandardLongFile->reset("L"+fileName))
        {
            theErrorMessages->insert(PP_CLOSE_FILE_ERROR,
                                    "致命错误：打开文件" + string("L") + fileName + "失败，程序退出 !");
        }
    }

    return  m_pStandardFile->reset(fileName);
}

//##ModelId=4850D2A30213
int OutputFile::setFileBuffer(const string &aSrcFileName, unsigned char *&srcFileBuffer)
{
    int len, size;
    struct stat attr;
    string srcFileNameWithPath, srcFileName, srcErrorName;

    size = aSrcFileName.size();
    if((len = aSrcFileName.find_last_of("/")) < 0)
    {
        m_srcFileName       = aSrcFileName;
        srcFileNameWithPath = m_srcFilePath + m_srcFileName;
    }
    else
    {
        m_srcFileName       = aSrcFileName.substr(len + 1, size - len - 1);
        srcFileNameWithPath = aSrcFileName;
    }

#ifdef DEBUG
    cout<<"\nopen src file "<<srcFileNameWithPath<<"...";
#endif

    //打开原始话单文件
    if(m_srcfd != 0)
    {
        if(close(m_srcfd) != 0)
        {
            theErrorMessages->insert(PP_CLOSE_FILE_ERROR,
                                    "致命错误：关闭文件" + string(m_srcFileName) + "失败，程序退出 !");
#ifndef PPTEST
            g_application.emergyExitAlert();
#endif
            exit(1);
        }
       m_srcfd = 0;
    }

    if ((m_srcfd = open(srcFileNameWithPath.c_str(), O_RDONLY)) <= 0)   //打开文件错误
    {
        theErrorMessages->insert(PP_ERROR_OPEN_PHONE_FILE, "open " + string(m_srcFileName) + " error!");
        return -1;
    }

    stat(srcFileNameWithPath.c_str(), &attr);                           // 得到文件长度
    m_fileLength = attr.st_size;

    if(m_fileLength == 0)                                               //话单文件长度为0。
    {
       if(close(m_srcfd) != 0)
       {
            theErrorMessages->insert(PP_CLOSE_FILE_ERROR,
                                    "致命错误：关闭文件" + string(m_srcFileName) + "失败，程序退出 !");
#ifndef PPTEST
            g_application.emergyExitAlert();
#endif
            exit(1);
        }
        m_srcfd = 0;
        mvToErrDir(m_srcFileName);

        return -1;
    }

    m_srcFileBuffer=(unsigned char *)mmap(0,m_fileLength,PROT_READ,MAP_PRIVATE,m_srcfd,0);
    srcFileBuffer=m_srcFileBuffer;

    return m_fileLength;
}

//##ModelId=4850D2DA038A
void OutputFile::closeFiles(int flag)
{
    float theErrorRate = 0;
    string srcFileName, srcBakName;
    char cmd[100];

    munmap((char *)m_srcFileBuffer, m_fileLength);

    if(m_srcfd != 0)
    {
        if(close(m_srcfd) != 0)
        {
            theErrorMessages->insert(PP_CLOSE_FILE_ERROR,
                                    "致命错误：结束时关闭原始文件" + m_srcFileName + "失败，程序退出 !");
#ifndef PPTEST
            g_application.emergyExitAlert();
#endif
            exit(1);
        }
        m_srcfd = 0;
    }

    if( flag !=0 )
    {
        theErrorMessages->insert(PP_CLOSE_FILE_ERROR,
                                    "文件" + m_srcFileName + "处理错误！请检查该文件!");

        m_pStandardFile->close(1);
        if(m_isSpliteLongCdr)
        {
            m_pStandardLongFile->close(1);
        }
        mvToErrDir(m_srcFileName);
        return;
    }

    if(m_PpLog_->getTotalCdrs() > 0)
    {
        theErrorRate = (float) m_PpLog_->getErrorSum() / (float)m_PpLog_->getTotalCdrs();

        if(theErrorRate > m_errorRate)
        {
            m_pStandardFile->close(1);
            if(m_isSpliteLongCdr)
            {
                m_pStandardLongFile->close(1);
            }
            theErrorMessages->insert(PP_ERROR_TOO_MANY_ERROR, "file " + m_srcFileName + " too many error records !");
            mvToErrDir(m_srcFileName);
        }
        else
        {
            m_pStandardFile->close(0);
            if(m_isSpliteLongCdr)
            {
                m_pStandardLongFile->close(0);
            }
            srcFileName = m_srcFilePath + m_srcFileName;;
            if(m_isBackupSrcFile)
            {
                srcBakName = m_srcFileBakPath + m_srcFileName;
                if(link(srcFileName.c_str(), srcBakName.c_str()) != 0)
                {
                    theErrorMessages->insert(PP_ERROR_LINK_FILE, "source file " + m_srcFileName + " cannot copy!");
#ifdef DEBUG
    cout<<"原始文件 " << m_srcFileName << " cannot be saved!";
#endif
                }
            }
            if(unlink(srcFileName.c_str()) != 0)
            {
                theErrorMessages->insert(PP_ERROR_UNLINK_FILE, "source file " + m_srcFileName + " cannot be deleted !");
            }
            // 压缩备份的原始文件
            if(m_isBackupSrcFile)
            {
               sprintf(cmd, "compress -f %s", srcBakName.c_str());
               system(cmd);
            }
        }
    }
    else
    {
        m_pStandardFile->close(1);
        if(m_isSpliteLongCdr)
        {
            m_pStandardLongFile->close(1);
        }
        //theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, "file "+m_srcFileName+" 话单数为0 !");
        mvToErrDir(m_srcFileName);
    }
}

//##ModelId=4850D2EC008C
bool OutputFile::initialize(const vector<string> &aStandardFile, const vector<string> &aFilePath,
                            const string &isBackupSrc, const string &isBackupDes, const string &isCheckCdrNo,
                            const string &outFileType, float errorRate, const string &isSplitDes, int maxCdrNum,
                            bool isSpliteLongCdr, const string &longPath)
{
    if (isBackupSrc.compare("yes") == 0)
    {
        m_isBackupSrcFile = true;
    }
    else
    {
        m_isBackupSrcFile = false;
    }

    if (isCheckCdrNo.compare("yes") == 0)
    {
        m_isCheckCdrNo = true;
    }

    m_srcFilePath       = getPath(aFilePath, "source_path");
    m_srcFileBakPath    = getPath(aFilePath, "src_backup_path");
    m_fileErrorPath     = getPath(aFilePath, "file_error_path");
    m_errorRate         = errorRate;

    if (isSpliteLongCdr)
    {
        m_isSpliteLongCdr = true;
        m_longCdrPath     = longPath;
        m_pStandardLongFile->initialize(aStandardFile, isBackupDes, outFileType, longPath,
                                     getPath(aFilePath, "des_backup_path"),
                                     getPath(aFilePath, "invalid_path"), isSplitDes, maxCdrNum);
    }
    else
    {
        m_isSpliteLongCdr = false;
        m_longCdrPath     = "";
    }

    m_pStandardFile->initialize(aStandardFile, isBackupDes, outFileType,
                               getPath(aFilePath, "destination_path"),
                                     getPath(aFilePath, "des_backup_path"),
                                     getPath(aFilePath, "invalid_path"), isSplitDes, maxCdrNum);

    m_pField_ErrCode    = m_pFieldInfos->getFieldInfoByName(ERROR_CODE);
    m_pField_CallType   = m_pFieldInfos->getFieldInfoByName(CALL_TYPE);
    m_pField_RecordType = m_pFieldInfos->getFieldInfoByName(RECORD_TYPE);
    
    return true;
}

//##ModelId=4850D36B038A
void OutputFile::mvToErrDir(const string & filename)
{
    string srcFileName, srcErrorName;

    srcFileName = m_srcFilePath + filename;

    // 错误文件不输出到错误目录下,直接备份到备份目录下
    srcErrorName= m_srcFileBakPath  +  filename;;

    link(srcFileName.c_str(), srcErrorName.c_str());
    unlink(srcFileName.c_str());
}

//##ModelId=4850D38E02EE
string OutputFile::getPath(vector<string> aFilePath, string pathName)
{
    int len, size;
    char tmp[512];
    string apathName, apath;

    for(int i = 0; i < aFilePath.size(); i++)
    {
        size    = aFilePath[i].size();
        strcpy(tmp, aFilePath[i].c_str());
        for(len = 0; len < size; len++)
        {
            if(tmp[len] == ' ' || tmp[len] == '\t')
            {
                break;
            }
        }
        apathName = aFilePath[i].substr(0, len);
        apath     = aFilePath[i].substr(len, size - len);

        if(apathName == pathName)
        {
            size = apath.size();
            len  = apath.find_first_not_of(" \t");
            if(len > 0)
            {
                return apath.substr(len, size - len);
            }
        }
    }

    return string("");
}

