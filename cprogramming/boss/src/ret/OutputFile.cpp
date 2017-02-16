// ##########################################
// Source file : OutputFile.cpp
// System      : Mobile Billing System
// Version     : 0.1.12
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010925
// Update      : 20020325
// Copyright(C): 2002 by Yang Zhang, Linkage.
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

#include "RetLog.h"
#include "OutputFile.h"
#include "MyApplication.h"
extern MyApplication g_application;

ERRORINFO theERRORINFO;

//##ModelId=3BB18B4C0257
OutputFile::~OutputFile()
{
	closeSrcFiles(0);
	closeDesFiles(0);
}

//##ModelId=3BC516710364
OutputFile::OutputFile()
{
    m_srcfd           = 0;
    m_cdrFlag         = CdrFlag::getInstance();
    m_RetLog_          = RetLog::getInstance();
    m_isCheckCdrNo    = false;
    theERRORINFO.no   = 230;
}

//##ModelId=3BB30DE50044
int OutputFile::setFileBuffer(string aSrcFileName,unsigned char *&srcFileBuffer)
{
	int len,size;
	struct stat attr;
	string srcFileNameWithPath,srcFileName,srcErrorName;

	size=aSrcFileName.size();
	if( (len=aSrcFileName.find_last_of("/") ) < 0 ) 
	{
		m_srcFileName		= aSrcFileName;
		srcFileNameWithPath	= m_srcFilePath + m_srcFileName;
	} 
	else 
	{
		m_srcFileName		= aSrcFileName.substr(len+1,size-len-1);
		srcFileNameWithPath	= aSrcFileName;
	}

#ifdef DEBUG
    cout<<"\nopen src file "<<srcFileNameWithPath<<"...";
#endif

	//打开原始话单文件
	if(m_srcfd!=0) 
	{
	    if(close(m_srcfd)!=0) 
	    {
	        theErrorMessages->insert(PP_CLOSE_FILE_ERROR,
	                                "致命错误：关闭文件"+m_srcFileName+"失败，程序退出 !");
#ifndef PPTEST
            g_application.emergyExitAlert();
#endif 
            exit(1);
        }
	   m_srcfd=0;
	}

	if ( (m_srcfd  = open( srcFileNameWithPath.c_str(), O_RDONLY )) <= 0 ) //打开文件错误
	{
        theErrorMessages->insert(PP_ERROR_OPEN_PHONE_FILE, "open "+m_srcFileName+" error!");
		return -1;		                                
	}

	stat(srcFileNameWithPath.c_str(), &attr);			// 得到文件长度
	m_fileLength = attr.st_size;

	if(m_fileLength==0)                                 //话单文件长度为0。
	{
       if(close(m_srcfd)!=0) 
       {
	        theErrorMessages->insert(PP_CLOSE_FILE_ERROR,
	                                "致命错误：关闭文件"+m_srcFileName+"失败，程序退出 !");
#ifndef PPTEST
            g_application.emergyExitAlert();
#endif 
            exit(1);
        }
	   m_srcfd=0;
	   mvToErrDir(m_srcFileName);

		return -1;		
	}

	m_srcFileBuffer=(unsigned char *)mmap(0,m_fileLength,PROT_READ,MAP_PRIVATE,m_srcfd,0);
	srcFileBuffer=m_srcFileBuffer;

	return m_fileLength;
}


//##ModelId=3BB08B9500B9
int OutputFile::write(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    char tmp[10];
    int len,cdrNo=0;
	    
    if(m_isCheckCdrNo)                   // 校验话单连续性   
    {                 
 		FieldInfo *tmpFieldInf = 0;
 		string tmpStr = string("CDR_NO");
 		tmpFieldInf = FieldConvertRule::getFieldMap(theFieldInfoMaps,tmpStr);
		if( tmpFieldInf != 0 )
		{
			cdrNo = atoi (tmpFieldInf->m_FieldDesValue.c_str() );	
		}
       

       if(cdrNo>0)                       // 如果 字段"CDR_NO"存在，对话单进行话单连续性校验 
       {                    
          if(m_RetLog_->getBeginCdrNo()==0) 
          {
                m_RetLog_->setBeginCdrNo(cdrNo);
          }
          m_RetLog_->setEndCdrNo(cdrNo);
          if( (m_cdrFlag->m_cdrNo != -1) && (m_cdrFlag->m_cdrNo + 1 != cdrNo) ) 
          {
             sprintf(tmp,"%d",cdrNo);
             theErrorMessages->insert(PP_ERROR_CDR_NO,"话单连续性错误(文件："+
                                     m_srcFileName+",No."+tmp+")!");
          }
          m_cdrFlag->m_cdrNo = cdrNo;
       }
    }

    return m_StandardFile.write(theFieldInfoMaps);
}

int OutputFile::writeFileHead(string sRetHead)
{
    return m_StandardFile.writeFileHead(sRetHead);
}

int OutputFile::writeFileTail(string sRetTail)
{
    return m_StandardFile.writeFileTail(sRetTail);
}

//##ModelId=3BB14B5D034D
bool OutputFile::reset(string fileName)
{
	return  m_StandardFile.reset(fileName);
}

//##ModelId=3BB14B8A01D5
//##Documentation
//## 关闭全部输出文件句柄
//## flag=0,正确 ； flag =1,错误
void OutputFile::closeSrcFiles(int flag)
{
    float theErrorRate=0;
    string srcFileName,srcBakName,srcErrorName;
    char cmd[100];

    munmap( (char *)m_srcFileBuffer, m_fileLength );

    if(m_srcfd!=0) 
    {
	    if(close(m_srcfd)!=0) 
	    {
	        theErrorMessages->insert(PP_CLOSE_FILE_ERROR,
	                                "致命错误：关闭文件"+m_srcFileName+"失败，程序退出 !");
#ifndef PPTEST
            g_application.emergyExitAlert();
#endif 
            exit(1);
        }
	    m_srcfd=0;
    }

    if( flag !=0 ) 
    {
      theErrorMessages->insert(PP_CLOSE_FILE_ERROR,
	                             "文件"+m_srcFileName+"处理错误！请检查该文件!");
      
      //m_StandardFile.close(1);
	    mvToErrDir(m_srcFileName);
	    return ;
    }

    if( m_RetLog_->getTotalCdrs() > 0 ) 
    {
	    theErrorRate=(float) m_RetLog_->getErrorSum() / (float)m_RetLog_->getTotalCdrs();

	    if( theErrorRate > m_errorRate ) 
	    {
	       //m_StandardFile.close(1);
	       theErrorMessages->insert(PP_ERROR_TOO_MANY_ERROR, "file "+m_srcFileName+" 错误记录过多 !");
	       mvToErrDir(m_srcFileName);
	    } 
	    else 
	    {
	       //m_StandardFile.close(0);
         srcFileName = m_srcFilePath    + m_srcFileName;
	           
	       if( m_isBackupSrcFile )
           {
	           srcBakName  = m_srcFileBakPath + m_srcFileName;
	           if(link(srcFileName.c_str(),srcBakName.c_str()) != 0 )
	            {
	                theErrorMessages->insert(PP_ERROR_LINK_FILE, "原始文件 "+m_srcFileName+" 不能备份!");
#ifdef DEBUG
	cout<<"原始文件 "<<m_srcFileName<<" 不能备份!";  
#endif

		        }
	       }
	       if( unlink(srcFileName.c_str()) != 0 )
	       {
	           	theErrorMessages->insert(PP_ERROR_UNLINK_FILE, "原始文件 "+m_srcFileName+" 不能删除 !");
           }
           // 压缩备份的原始文件
           //if( m_isBackupSrcFile )
           //{
	         //  sprintf(cmd,"compress -f %s",srcBakName.c_str());
	         //  system(cmd);
	       //}
	    }
	 } 
	 else 
	 {
	     //m_StandardFile.close(1);
	     //disabled by chenm 2003-4-12 
	     //theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, "file "+m_srcFileName+" 话单数为0 !");
	     mvToErrDir(m_srcFileName);
	 }
}

void OutputFile::closeDesFiles(int flag)
{
        m_StandardFile.close(0);
}

//##ModelId=3BB19885038B
bool OutputFile::initialize(vector<string> aStandardFile,vector<string> aFilePath,
							string isBackupSrc,string isBackupDes,string isCheckCdrNo,
							string outFileType,float errorRate)
{
	if (isBackupSrc.compare("yes")==0)
		m_isBackupSrcFile=true;
	else
		m_isBackupSrcFile=false;
		
	if (isCheckCdrNo.compare("yes")==0)
		m_isCheckCdrNo=true;

	m_srcFilePath		= getPath(aFilePath,"source_path");
	m_srcFileBakPath	= getPath(aFilePath,"src_backup_path");
	m_fileErrorPath	    = getPath(aFilePath,"file_error_path");
    m_errorRate         = errorRate;

	m_StandardFile.initialize(aStandardFile,isBackupDes,outFileType,
	                           getPath(aFilePath,"destination_path"),
							         getPath(aFilePath,"des_backup_path"),
							         getPath(aFilePath,"invalid_path") );

	return true;
}

//##ModelId=3BB3D0CB0373
string OutputFile::getPath(vector<string> aFilePath,string pathName)
{
	int len,size;
	char tmp[100];
	string apathName,apath;

	for(int i=0;i<aFilePath.size();i++)
	{
		size	= aFilePath[i].size();
		strcpy( tmp,aFilePath[i].c_str() );
		for(len=0;len<size;len++)
		{
			if( tmp[len]==' ' || tmp[len]=='\t')
				break;
		}
		apathName	= aFilePath[i].substr(0,len);
		apath		= aFilePath[i].substr(len,size-len);
		
		if(apathName==pathName) 
		{
			size	= apath.size();
			len		= apath.find_first_not_of(" \t");
			if(len>0) 
			{
				return apath.substr(len,size-len);
			}
		}
	}

	return string("");
}

//##ModelId=3BFCB54F018C
void OutputFile::mvToErrDir(string & filename)
{
    string srcFileName,srcErrorName;
    
    srcFileName = m_srcFilePath    + filename;
    srcErrorName= m_fileErrorPath  +  filename;
    
    link(srcFileName.c_str(),srcErrorName.c_str());
    unlink( srcFileName.c_str() );
}

void OutputFile::mvToBakDir(string & filename)
{
    string srcFileName,srcBakName;
    char cmd[256];
    
    srcFileName = m_srcFilePath    + filename;
    srcBakName= m_srcFileBakPath  +  filename;
    
    link(srcFileName.c_str(),srcBakName.c_str());
    unlink( srcFileName.c_str() );
}
