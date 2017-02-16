// ##########################################
// Source file : GeneralFile.cpp
// System      : Mobile Billing System
// Version     : 0.1.13
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010822
// Update      : 20020121
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include <stdlib.h>
#include "GeneralFile.h"
#include "GAsnCdrExtractor.h"

//##ModelId=3B6216660176
GeneralFile::GeneralFile()
{
	m_BlockBuffer=0;
	m_theCdrExtractor=0;
	m_isMotoGprs = false;
	m_AlertByEmptyFile = false;
	m_isCNGggsn  = false;
}

//##ModelId=3B6216660248
GeneralFile::~GeneralFile()
{
	if(m_BlockBuffer!=0 && m_BlockSize!=0) 
	{
	   free(m_BlockBuffer);
	   m_BlockBuffer=0;
	}
	if(m_theCdrExtractor!=0) 
	{
	   delete m_theCdrExtractor ;
	   m_theCdrExtractor=0;
	}
}


//##ModelId=3B832B7C011F
bool GeneralFile::initialize(RetParameterFromFile *aRetParameter,int cdrType)
{
	string Tmp,TheCdrParameter;
	int BlockHeadSize;
	vector <string> theBlockFields,theFieldHeadFields;

	aRetParameter->getValue("file_header", "file_header_size",Tmp);
  m_FileHeaderSize	= atoi( Tmp.c_str() );

	// over 2005-4-11 
	aRetParameter->getValue("file_header", "file_tail_size",Tmp);
	m_FileTailSize		= atoi( Tmp.c_str() );
	aRetParameter->getValue("block", "block_size",Tmp);
	m_BlockSize			= atoi( Tmp.c_str() );
	aRetParameter->getValue("block", "block_header_size",Tmp);
	BlockHeadSize		= atoi( Tmp.c_str() );

	if ( createExtractCdr(cdrType) == 0 )		//指针为空
	{
	   theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create exterat cdr object!");
		return false;
	}

	aRetParameter->getValue("cdr_size", "cdr_size",TheCdrParameter);
	m_theCdrExtractor->initialize(TheCdrParameter,BlockHeadSize);

	m_theBlock.initialize(theBlockFields,BlockHeadSize);
	//m_theFileHead.initialize(theFieldHeadFields);
	
	// add by chenm 2006-6-23
	aRetParameter->getValue("rawFile_type", "empty_file_alert",Tmp);
	if( Tmp.compare("YES") == 0 )
		m_AlertByEmptyFile = true;
	// over 2006-6-23 

	return true;
}


//##ModelId=3B832B7B02EA
int GeneralFile::getCdr(unsigned char *&CdrBuffer)
{
	int cdrLength;

	cdrLength=m_theCdrExtractor->getNextSrcCdr(CdrBuffer);
	if ( cdrLength > 0 )
	{
		return cdrLength;
	}
	else if( cdrLength == -1)     //对应token类型的cdr,-1代表字段数错
	{
		theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, "文件 "+ m_FileName +" 字段数错!" );
		return -1;
	}
	
		// 当前记录块的cdr已经取完
	if ( getNextBlock() != 0 ) 	// 取下一个记录块
	{
		m_theCdrExtractor->setBlockBuffer((unsigned char *)m_BlockBuffer,m_RealBlockSize);
		cdrLength = m_theCdrExtractor->getNextSrcCdr(CdrBuffer);
	}

	if ( m_BlockNo < 0 ) 
	{
		if(m_BlockBuffer!=0) 
		{
		   free(m_BlockBuffer);		// no next block
		   m_BlockBuffer=0;
		   return -1;
		}
	}
	return cdrLength;
}

//##ModelId=3B83782C03C7
int GeneralFile::getNextBlock()
{
	if ( m_BlockNo > 0 ) 
	{
		if(m_BlockSize==0) 
		{
		   m_BlockBuffer=(unsigned char*)m_FileBuffer;
		} 
		else 
		{
		   memcpy( m_BlockBuffer,m_BufferOffset+m_FileBuffer,m_RealBlockSize );
		}

		m_BufferOffset+=m_RealBlockSize;
		m_BlockNo--;
		return m_RealBlockSize;
	}
	return 0;
}

//##ModelId=3B832B7C00B0
int GeneralFile::postValid()
{
	return 0;
}

//##ModelId=3B820A000243
CdrExtractor *GeneralFile::createExtractCdr(int aCdrType)
{
	switch (aCdrType) {
		case 1 :
			m_theCdrExtractor = new FixedCdrExtractor;
			m_RetLog->ShowMessage("Create Fixed Extract cdr object.\n",0);
			break;
		case 2 :
			m_theCdrExtractor = new TokenCdrExtractor;
			m_RetLog->ShowMessage("Create Token Extract cdr object.\n",0);
			break;
		//case 3 :
			//m_theCdrExtractor = new AsnCdrExtractor;
			//m_RetLog->ShowMessage("Create Asn Extract cdr object.\n",0);
			//break;
		//case 4 :
			//m_theCdrExtractor = new GAsnCdrExtractor;
			//m_RetLog->ShowMessage("Create GAsn Extract cdr object.\n",0);
			//break;
		default :
			m_theCdrExtractor=0;
			break;
	}
	m_RetLog->ShowMessage("\n",0);

	return m_theCdrExtractor;
}

//##ModelId=3B839E6F03CC
bool GeneralFile::setRawFile(const string SrcFileName,
							 const unsigned char *FileBuffer,const int FileLength)
{
  int blockTotalSize;
	
	RawFileProcessor::setRawFile(SrcFileName,FileBuffer+m_FileHeaderSize,FileLength);

	//get real block length
	if ( m_BlockSize == 0 ) 
	{
		m_RealBlockSize = m_FileLength - m_FileHeaderSize - m_FileTailSize;
	} 
	else 
	{
		m_RealBlockSize = m_BlockSize;
	}
	if ( m_RealBlockSize <= 0 || m_FileLength <=0 ) 
	{
	    #ifdef DEBUG
            cout<<" file "<<SrcFileName<<"block size error!"<<endl;
        #endif
	    // disable by chenm 应江苏局方要求 2003-2-19 9:14
	    // add by chenm 2006-6-23 根据配置,告警
	    if( m_AlertByEmptyFile )
	    	theErrorMessages->insert(PP_ERROR_PHONE_FILE_BLOCK, " file "+SrcFileName+"block size error!");
	    // over 2006-6-23
	    
	    if(m_RealBlockSize<0)
	        return false;
	    else
	    {
	    	m_theCdrExtractor->setBlockBuffer(0,0);    
	        return true;
	    }
	}

	m_BufferOffset 	= 0;
	blockTotalSize 	= m_FileLength - m_FileHeaderSize - m_FileTailSize;

	if ( blockTotalSize % m_RealBlockSize !=0 ) 
	{
	    theErrorMessages->insert(PP_ERROR_PHONE_FILE_BLOCK, " file "+SrcFileName+"block size error!");
	    return false;
	}

	m_TotalBlocks 	= blockTotalSize / m_RealBlockSize;

    if(blockTotalSize % m_RealBlockSize != 0) 
    {
       // m_TotalBlocks++;
    }
    m_BlockNo 		= m_TotalBlocks;

    if(m_BlockSize>0) 
    {
      if(m_BlockBuffer!=0) 
      {
          free(m_BlockBuffer);
          m_BlockBuffer=0;
      }
      // 申请块缓存
	    if ( (m_BlockBuffer = (unsigned char *)malloc(m_RealBlockSize)) == 0 ) 
	    {
            #ifdef DEBUG
                cout<<"申请块缓存失败! "<<endl;
            #endif
	        theErrorMessages->insert(PP_ERROR_MEMORY_ALLOCATE, "malloc block buffer error!");
		    return false;
		  }
    } 
    else 
    {
        m_BlockBuffer=0;
    }
    #ifdef DEBUG
        cout<<"real block size is "<<m_RealBlockSize<<" . the total block is "<<m_TotalBlocks<<endl;
    #endif

	if ( getNextBlock() == 0 ) 			// 得到第一个块的原始数据
	{
	    #ifdef DEBUG
            cout<<"块数为零! "<<endl;
        #endif
        //zaiww disabled 20060612
        //theErrorMessages->insert(PP_ERROR_MEMORY_ALLOCATE, " file "+SrcFileName+"块数为零!");
    }
	m_theCdrExtractor->setBlockBuffer(m_BlockBuffer,m_RealBlockSize);

	return true;
}

void GeneralFile::setToken(const string token)
{
	m_theCdrExtractor->setToken(token);	
} 


