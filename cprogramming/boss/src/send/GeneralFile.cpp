// ##########################################
// Source file : GeneralFile.cpp
// System      : NEW GENERATION BILLING SYSTEM
// Version     : 0.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Zhao Liyang
// E_mail      : zhaoly@lianchuang.com
// Create      : 20081018
// Update      : 20081029
// Copyright(C): 2008 by Zhao Liyang, Linkage.
// ##########################################

#include <stdlib.h>
#include "GeneralFile.h"

GeneralFile::GeneralFile()
{
	m_BlockBuffer=0;
	m_theCdrExtractor=0;
	//CDMA话单块的不定长特殊性 yuanjun 20060703
	m_BlockFlag = 0;
	m_BlockNo = 0;
}

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


bool GeneralFile::initialize(SendParameterFromFile *aSendParameter,int cdrType)
{
	string Tmp, TheCdrParameter, TheBlockParameter;
	int BlockHeadSize;
	vector <string> theBlockFields, theFieldHeadFields;

	aSendParameter->getValue("block", "block_size",TheBlockParameter);	
  aSendParameter->getValue("block", "block_header_size",Tmp);
	BlockHeadSize		= atoi( Tmp.c_str() );
	
	if ( createExtractCdr(cdrType) == 0 )		//指针为空
	{
	   theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create exterat cdr object!");
		return false;
	}

	aSendParameter->getValue("cdr_size", "cdr_size",TheCdrParameter);
	m_theCdrExtractor->initialize(TheCdrParameter,BlockHeadSize);
	m_BlockSize = atoi( TheBlockParameter.c_str() );
	return true;
}


int GeneralFile::getCdr(unsigned char *&CdrBuffer)
{
	int cdrLength;

FP_BEGIN(getcdr)
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
FP_END(getcdr)	

	return cdrLength;
}

int GeneralFile::getNextBlock()
{
    //定长块
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

int GeneralFile::postValid()
{
	return 0;
}

CdrExtractor *GeneralFile::createExtractCdr(int aCdrType)
{
	switch (aCdrType) {
		case 2 :
			m_theCdrExtractor = new TokenCdrExtractor;
			m_SendLog->ShowMessage("Create Token Extract cdr object.\n",0);
			break;
		default :
			m_theCdrExtractor=0;
			break;
	}
	m_SendLog->ShowMessage("\n",0);

	return m_theCdrExtractor;
}

bool GeneralFile::setRawFile(const string SrcFileName,
							 const unsigned char *FileBuffer,const int FileLength)
{
    int blockTotalSize;
	
    RawFileProcessor::setRawFile(SrcFileName,FileBuffer,FileLength);
        
    m_BufferOffset 	= 0;  
    
    //CDMA话单块的不定长特殊性 yuanjun 20060703
	if( m_BlockFlag == 0 )
	{

	    //get real block length
	    if ( m_BlockSize == 0 ) 
	    {
	    	m_RealBlockSize = m_FileLength;
	    } 
	    else 
	    {
	    	m_RealBlockSize = m_BlockSize;
	    }

	    if ( m_RealBlockSize <= 0 ) 
	    {
	        #ifdef DEBUG
                cout<<" file "<<SrcFileName<<"block size error!"<<endl;
          #endif
	        if(m_RealBlockSize<0)
	            return false;
	        else
	        {
	        	m_theCdrExtractor->setBlockBuffer(0,0);    
	            return true;
	        }
	    }
    
    	blockTotalSize 	= m_FileLength;
    
    	if ( blockTotalSize % m_RealBlockSize !=0 ) 
    	{
    	    theErrorMessages->insert(PP_ERROR_PHONE_FILE_BLOCK, " file "+SrcFileName+"block size error!");
    	    return false;
    	}
    
    	m_TotalBlocks 	= blockTotalSize / m_RealBlockSize;
      m_BlockNo 		= m_TotalBlocks;

      if(m_BlockSize>0) 
      {
          if(m_BlockBuffer!=0) 
          {
              free(m_BlockBuffer);
              m_BlockBuffer=0;
          }

          // 申请定长块缓存
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
  }
    
    //CDMA话单块的不定长特殊性 yuanjun 20060703
	if ( getNextBlock() == 0 ) 			// 得到第一个块的原始数据
	{
	    //#ifdef DEBUG
        //    cout<<"块数为零! "<<endl;
        //#endif
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

