// ##########################################
// Source file : GeneralFile.cpp
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

#include <stdlib.h>
#include "GeneralFile.h"
#include "GAsnCdrExtractor.h"
#include "SasnCdrExtractor.h"

//##ModelId=48523A7F0177
GeneralFile::GeneralFile()
{
    m_BlockBuffer           = 0;
    m_FileHeadBuffer        = 0;
    m_FileTailBuffer        = 0;
    m_theCdrExtractor       = 0;
    m_theFileHeadExtractor  = 0;
    m_theFileTailExtractor  = 0;
    m_isMotoGprs            = false;
    m_isCNGggsn             = false;
    m_isHuawTail            = false;
    m_isHuawTDTail			= false;	//added by zaiww 20090805
    m_AlertByEmptyFile      = false;
    m_isParseFileHead		= false;	//added by zaiww 20090820
    m_isParseFileTail		= false;	//added by zaiww 20090820
    m_BlockFlag             = 0;
    m_BlockNo               = 0;
}

//##ModelId=48523A8102BF
GeneralFile::~GeneralFile()
{
    if(m_BlockBuffer != 0 && m_BlockSize != 0)
    {
       free(m_BlockBuffer);
       m_BlockBuffer = 0;
    }
    if(m_FileHeadBuffer != 0 && m_FileHeaderSize != 0)
    {
       free(m_FileHeadBuffer);
       m_FileHeadBuffer = 0;
    }
    if(m_FileTailBuffer != 0 && m_FileTailSize != 0)
    {
       free(m_FileTailBuffer);
       m_FileTailBuffer = 0;
    }
    if(m_theCdrExtractor != 0)
    {
       delete m_theCdrExtractor;
       m_theCdrExtractor = 0;
    }
    if(m_theFileHeadExtractor != 0)
    {
        delete m_theFileHeadExtractor;
        m_theFileHeadExtractor = 0;
    }
    if(m_theFileTailExtractor != 0)
    {
        delete m_theFileTailExtractor;
        m_theFileTailExtractor = 0;
    }
}

//##ModelId=4855B547034B
int GeneralFile::getCdr(unsigned char *&CdrBuffer)
{
    int cdrLength;

    cdrLength = m_theCdrExtractor->getNextSrcCdr(CdrBuffer);
    if (cdrLength > 0)
    {
        return cdrLength;
    }
    else if( cdrLength == -1)     //对应token类型的cdr,-1代表字段数错
    {
        theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, "文件 " + m_FileName + " 字段数错!");
        return -1;
    }

    // 当前记录块的cdr已经取完
    if (getNextBlock() != 0)    // 取下一个记录块
    {
        m_theCdrExtractor->setBlockBuffer((unsigned char *)m_BlockBuffer, m_RealBlockSize);
        cdrLength = m_theCdrExtractor->getNextSrcCdr(CdrBuffer);
    }

    //CDMA lucent 话单块的不定长特殊性
    if (m_BlockNo < 0)
    {
        if(m_BlockBuffer != 0)
        {
            free(m_BlockBuffer);        // no next block
            m_BlockBuffer = 0;
            return -1;
        }
    }

    return cdrLength;
}

//##ModelId=4855B56A00FA
bool GeneralFile::setRawFile(const string &SrcFileName, const unsigned char *FileBuffer, const int FileLength)
{
    int blockTotalSize;

    // 处理moto-gprs话单的特殊文件头
    if( m_isMotoGprs == true)
    {
        // 动态求取m_FileHeaderSize的值
        dealMotoGprsFileHead(FileBuffer);
    }
    else if(m_isCNGggsn == true)
    {
        // 动态求取m_FileHeaderSize的值
        dealCngGgsnFileHead(FileBuffer);
    }
    // 处理华为软交换文件尾
    if (m_isHuawTail == true )
    {
        // 动态求取m_FileTailSize的值
        dealHuawTail(FileBuffer);
    }
    // 处理华为TD交换机文件尾	added by zaiww 20090805
    if (m_isHuawTDTail == true )
    {
        // 动态求取m_FileTailSize的值
        dealHuaweiTDFileTail(FileBuffer);
    }

    if (m_isParseFileHead == true)
    {
        RawFileProcessor::setRawFile(SrcFileName, FileBuffer, FileLength);
        #ifdef DEBUG
                cout<<"m_FileHeaderSize is  "<<m_FileHeaderSize<<endl;
        #endif
        m_FHBufferOffset = 0;
        if(m_FileHeadBuffer != 0)
        {
            free(m_FileHeadBuffer);
            m_FileHeadBuffer = 0;
        }
        if ((m_FileHeadBuffer = (unsigned char *)malloc(m_FileHeaderSize)) == 0)
        {
            #ifdef DEBUG
                cout<<"malloc m_FileHeaderSize error! "<<endl;
            #endif
            theErrorMessages->insert(PP_ERROR_MEMORY_ALLOCATE, "malloc file head buffer error!");
            return false;
        }
        memcpy(m_FileHeadBuffer, m_FHBufferOffset + m_FileBuffer, m_FileHeaderSize);
        m_theFileHeadExtractor->setBlockBuffer(m_FileHeadBuffer, m_FileHeaderSize);
    }
    if (m_isParseFileTail == true)
    {
        RawFileProcessor::setRawFile(SrcFileName, FileBuffer, FileLength);
        #ifdef DEBUG
                cout<<"m_FileTailSize is  "<<m_FileTailSize<<" and m_FileLength is "<<m_FileLength<<endl;
        #endif
        m_FTBufferOffset = m_FileLength - m_FileTailSize;
        if(m_FileTailBuffer != 0)
        {
            free(m_FileTailBuffer);
            m_FileTailBuffer = 0;
        }
        if ((m_FileTailBuffer = (unsigned char *)malloc(m_FileTailSize)) == 0)
        {
            #ifdef DEBUG
                cout<<"malloc m_FileHeaderSize error! "<<endl;
            #endif
            theErrorMessages->insert(PP_ERROR_MEMORY_ALLOCATE, "malloc file tail buffer error!");
            return false;
        }
        memcpy(m_FileTailBuffer, m_FTBufferOffset + m_FileBuffer, m_FileTailSize);
        m_theFileTailExtractor->setBlockBuffer(m_FileTailBuffer, m_FileTailSize);
    }
	
    RawFileProcessor::setRawFile(SrcFileName, FileBuffer + m_FileHeaderSize, FileLength);
    m_BufferOffset  = 0;

    //CDMA话单块的不定长特殊性
    if(m_BlockFlag == 0)
    {

        //get real block length
        if (m_BlockSize ==0)
        {
            m_RealBlockSize = m_FileLength - m_FileHeaderSize - m_FileTailSize;
        }
        else
        {
            m_RealBlockSize = m_BlockSize;
        }
        if (m_RealBlockSize <= 0 || m_FileLength <= 0)
        {
            #ifdef DEBUG
                cout << " file " << SrcFileName << "block size error!" << endl;
            #endif
            if(m_AlertByEmptyFile)
            {
                theErrorMessages->insert(PP_ERROR_PHONE_FILE_BLOCK, " file "+SrcFileName+"block size error!");
            }
            if(m_RealBlockSize < 0)
            {
                return false;
            }
            else
            {
                m_theCdrExtractor->setBlockBuffer(0, 0);
                return true;
            }
        }
        blockTotalSize  = m_FileLength - m_FileHeaderSize - m_FileTailSize;

        if (blockTotalSize % m_RealBlockSize !=0)
        {
            theErrorMessages->insert(PP_ERROR_PHONE_FILE_BLOCK, " file " + SrcFileName + "block size error!");
            return false;
        }

        m_TotalBlocks = blockTotalSize / m_RealBlockSize;
        m_BlockNo = m_TotalBlocks;

        if(m_BlockSize > 0)
        {
            if(m_BlockBuffer != 0)
            {
                free(m_BlockBuffer);
                m_BlockBuffer = 0;
            }

            // 申请定长块缓存
            if ((m_BlockBuffer = (unsigned char *)malloc(m_RealBlockSize)) == 0)
            {
                #ifdef DEBUG
                    cout << "申请块缓存失败! " << endl;
                #endif
                theErrorMessages->insert(PP_ERROR_MEMORY_ALLOCATE, "malloc block buffer error!");
                return false;
            }
        }
        else
        {
            m_BlockBuffer = 0;
        }
        #ifdef DEBUG
            cout<<"real block size is "<<m_RealBlockSize<<" . the total block is "<<m_TotalBlocks<<endl;
        #endif
    }

    //CDMA话单块的不定长特殊性
    if (getNextBlock() == 0)            // 得到第一个块的原始数据
    {
        #ifdef DEBUG
            cout << "块数为零! " << endl;
        #endif
        theErrorMessages->insert(PP_ERROR_MEMORY_ALLOCATE, " file " + SrcFileName + "块数为零!");
    }

    m_theCdrExtractor->setBlockBuffer(m_BlockBuffer, m_RealBlockSize);
    return true;
}

//##ModelId=4855B5A30138
int GeneralFile::postValid()
{
    return 0;
}

//##ModelId=4855B5BB02DE
bool GeneralFile::initialize(PpParameterFromFile *aPpParameter, int cdrType)
{
    string Tmp, TheCdrParameter, TheBlockParameter, TheCheckCdrSumParameter,TheFileHeadRecParameter, TheFileTailRecParameter;
    int BlockHeadSize;
    vector <string> theBlockFields, theFieldHeadFields;

    aPpParameter->getValue("file_header", "file_header_size", Tmp);    //得到文件头的大小

    // 处理moto-gprs话单的特殊文件头
    if(Tmp.compare("MOTO_GPRS") == 0)
    {
        m_FileHeaderSize = 0;
        m_isMotoGprs     = true;
    }
    // 江苏移动处理CNG_GGSN话单需要切割大文件
    else if(Tmp.compare("CNG_GGSN") == 0)
    {
        m_FileHeaderSize = 0;
        m_isCNGggsn      = true;
    }
    else
    {
        m_FileHeaderSize = atoi(Tmp.c_str());
    }

  //  m_FileHeaderSize = atoi(Tmp.c_str());
    aPpParameter->getValue("file_header", "file_tail_size", Tmp);   //得到文件尾的大小
    // 处理华为软交换特殊文件尾
    if(Tmp.compare("HUAW_TAIL") == 0)
    {
        m_FileTailSize = 0;
        m_isHuawTail   = true;
    }
    // 处理华为TD交换机特殊文件尾	added by zaiww 20090805
    else if(Tmp.compare("HUAW_TD_TAIL") == 0)
    {
        m_FileTailSize = 0;
        m_isHuawTDTail   = true;
    }
    else
    {
        m_FileTailSize = atoi(Tmp.c_str());
    }
    //m_FileTailSize = atoi(Tmp.c_str());
    aPpParameter->getValue("block", "block_size", TheBlockParameter);
    if (!isdigit(TheBlockParameter[0]))
    {
        m_BlockFlag = 1;
        m_BlockSize = 0;
    }
  //  else
   // {
     //   m_BlockSize = atoi(Tmp.c_str());
    //}
    aPpParameter->getValue("block", "block_header_size", Tmp);
    BlockHeadSize = atoi(Tmp.c_str());

    if (createExtractCdr(cdrType) == 0)     //指针为空
    {
        theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create extract cdr object!");
        return false;
    }

    aPpParameter->getValue("cdr_size", "cdr_size", TheCdrParameter);   //得到记录的大小
    m_theCdrExtractor->initialize(TheCdrParameter, BlockHeadSize);        //初始化记录的长度
    
    m_theBlock.initialize(theBlockFields, BlockHeadSize, TheBlockParameter, m_BlockSize);
    
    aPpParameter->getValue("file_header", "file_head_receipt",TheFileHeadRecParameter);
    aPpParameter->getValue("file_header", "file_tail_receipt",TheFileTailRecParameter);
    aPpParameter->getValue("file_header", "check_cdr_sum",TheCheckCdrSumParameter);
    
    // CheckCdrSumParameter=1 或者TheFileHeadRecepeiptParameter=1时 需要解析文件头
    // CheckCdrSumParameter=2 或者TheFileTailRecepeiptParameter=1时 需要解析文件尾
    //added by zaiww 20090820
    if ((TheCheckCdrSumParameter.compare("1") == 0)||(TheFileHeadRecParameter.compare("1") == 0))
    {
        m_isParseFileHead = true;
    }	
    if ((TheCheckCdrSumParameter.compare("2") == 0)||(TheFileTailRecParameter.compare("1") == 0))
    {
    	m_isParseFileTail = true;
    }
        
    if ( m_isParseFileHead == true )
    {
        if (createExtractFileHead(cdrType) == 0)        //指针为空
        {
            theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create exterat file_head object!");
            return false;
        }
        m_theFileHeadExtractor->initialize("1", 0);
    }
    if ( m_isParseFileTail == true )
    {
        if ( createExtractFileTail(cdrType) == 0 )      //指针为空
        {
            theErrorMessages->insert(PP_ERROR_CREATE_OBJECT, "cannot create exterat file_tail object!");
            return false;
        }
        m_theFileTailExtractor->initialize("1", 0);
    }
    
    aPpParameter->getValue("rawFile_type", "empty_file_alert", Tmp);
    if(Tmp.compare("YES") == 0)
    {
        m_AlertByEmptyFile = true;
    }
    return true;
}

//##ModelId=4855B5DD01A5
void GeneralFile::setToken(const string token)
{
    m_theCdrExtractor->setToken(token);
}

//##ModelId=4855B612036B
CdrExtractor *GeneralFile::createExtractCdr(int aCdrType)
{
    switch (aCdrType)
    {
    case 1 :
        m_theCdrExtractor = new FixedCdrExtractor;
        m_PpLog->ShowMessage("Create Fixed Extract cdr object.\n", 0);
        break;
    case 2 :
        m_theCdrExtractor = new TokenCdrExtractor;
        m_PpLog->ShowMessage("Create Token Extract cdr object.\n", 0);
        break;
    case 3 :
        m_theCdrExtractor = new AsnCdrExtractor;
        m_PpLog->ShowMessage("Create Asn Extract cdr object.\n", 0);
        break;
    case 4 :
        m_theCdrExtractor = new GAsnCdrExtractor;
        m_PpLog->ShowMessage("Create GAsn Extract cdr object.\n", 0);
        break;
    case 5 :
        m_theCdrExtractor = new SasnCdrExtractor;
        m_PpLog->ShowMessage("Create Sasn Extract cdr object.\n", 0);
        break;
    default :
        m_theCdrExtractor = 0;
        break;
    }
    m_PpLog->ShowMessage("\n", 0);
    return m_theCdrExtractor;
}

//##ModelId=4855B612036C
CdrExtractor *GeneralFile::createExtractFileHead(int aCdrType)
{
    switch (aCdrType)
    {
    case 1 :
        m_theFileHeadExtractor = new FixedCdrExtractor;
        m_PpLog->ShowMessage("Create Fixed Extract File_Head object.\n", 0);
        break;
    case 2 :
        m_theFileHeadExtractor = new TokenCdrExtractor;
        m_PpLog->ShowMessage("Create Token Extract File_Head object.\n", 0);
        break;
    case 3 :
        m_theFileHeadExtractor = new AsnCdrExtractor;
        m_PpLog->ShowMessage("Create Asn Extract File_Head object.\n", 0);
        break;
    case 4 :
        m_theFileHeadExtractor = new GAsnCdrExtractor;
        m_PpLog->ShowMessage("Create GAsn Extract File_Head object.\n", 0);
        break;
    case 5 :
        m_theFileHeadExtractor = new SasnCdrExtractor;
        m_PpLog->ShowMessage("Create Sasn Extract File_Head object.\n", 0);
        break;
    default :
        m_theFileHeadExtractor = 0;
        break;
    }
    m_PpLog->ShowMessage("\n", 0);

    return m_theFileHeadExtractor;
}

//##ModelId=4855B612036D
CdrExtractor *GeneralFile::createExtractFileTail(int aCdrType)
{
    switch (aCdrType)
    {
    case 1 :
        m_theFileTailExtractor = new FixedCdrExtractor;
        m_PpLog->ShowMessage("Create Fixed Extract File_Tail object.\n", 0);
        break;
    case 2 :
        m_theFileTailExtractor = new TokenCdrExtractor;
        m_PpLog->ShowMessage("Create Token Extract File_Tail object.\n", 0);
        break;
    case 3 :
        m_theFileTailExtractor = new AsnCdrExtractor;
        m_PpLog->ShowMessage("Create Asn Extract File_Tail object.\n", 0);
        break;
    case 4 :
        m_theFileTailExtractor = new GAsnCdrExtractor;
        m_PpLog->ShowMessage("Create GAsn Extract File_Tail object.\n", 0);
        break;
    case 5 :
        m_theFileTailExtractor = new SasnCdrExtractor;
        m_PpLog->ShowMessage("Create Sasn Extract File_Tail object.\n", 0);
        break;
    default :
        m_theFileTailExtractor = 0;
        break;
    }
    m_PpLog->ShowMessage("\n", 0);

    return m_theFileTailExtractor;
}

//##ModelId=4855B6290280
int GeneralFile::getNextBlock()
{
    //定长块
    if (m_BlockNo > 0)
    {
        if(m_BlockSize == 0)
        {
           m_BlockBuffer = (unsigned char*)m_FileBuffer;
        }
        else
        {
           memcpy(m_BlockBuffer, m_BufferOffset + m_FileBuffer, m_RealBlockSize);
        }

        m_BufferOffset += m_RealBlockSize;
        m_BlockNo--;
        return m_RealBlockSize;
    }

    //CDMA lucent 话单块的不定长特殊性
    if(m_BlockFlag == 1 && m_BufferOffset < (m_FileLength - 1))
    {
        m_RealBlockSize = m_theBlock.getBlockSize(m_FileBuffer + m_BufferOffset);

        if(m_BlockBuffer != 0)
        {
            free(m_BlockBuffer);
            m_BlockBuffer = 0;
        }
        // 申请变长块缓存
        if ( (m_BlockBuffer = (unsigned char *)malloc(m_RealBlockSize)) == 0 )
        {
            #ifdef DEBUG
                cout<<"申请块缓存失败! "<<endl;
            #endif
            theErrorMessages->insert(PP_ERROR_MEMORY_ALLOCATE, "malloc block buffer error!");
            return false;
        }

        memcpy(m_BlockBuffer, m_BufferOffset + m_FileBuffer, m_RealBlockSize);
        m_BufferOffset += m_RealBlockSize;
        return m_RealBlockSize;
    }
    return 0;
}

//##ModelId=4855B656032C
void GeneralFile::dealMotoGprsFileHead(const unsigned char *FileBuffer)
{
    AsnFormat tmpAsnFormat;
    int iAsnLengthSize, iAsnValueSize;

    iAsnLengthSize = iAsnValueSize = 0;

    // 跳过 tag:'30'和长度指示长度
    iAsnLengthSize = tmpAsnFormat.getAsnLength(FileBuffer + 1, iAsnValueSize);
    m_FileHeaderSize = 1 + iAsnLengthSize;

    // 跳过 tag:'A0',长度指示长度和包含内容长度
    iAsnLengthSize = tmpAsnFormat.getAsnLength(FileBuffer + m_FileHeaderSize + 1, iAsnValueSize);
    m_FileHeaderSize += 1 + iAsnLengthSize + iAsnValueSize;

    // 跳过 tag:'A1',长度指示长度
    iAsnLengthSize = tmpAsnFormat.getAsnLength(FileBuffer + m_FileHeaderSize + 1, iAsnValueSize);
    m_FileHeaderSize += 1 + iAsnLengthSize ;

    return;
}

//##ModelId=4855B6AB0261
void GeneralFile::dealCngGgsnFileHead(const unsigned char *FileBuffer)
{
    AsnFormat tmpAsnFormat;
    int iAsnLengthSize, iAsnValueSize, iAsnTagSize;
    unsigned char signTmp[10];

    iAsnLengthSize = iAsnValueSize = 0;

    if(FileBuffer[0] == 'b')  // CNG空文件,里面只含有 notification
    {
        iAsnTagSize  = tmpAsnFormat.getAsnTag(FileBuffer, signTmp);
        iAsnLengthSize = tmpAsnFormat.getAsnLength(FileBuffer + iAsnTagSize, iAsnValueSize);
        m_FileHeaderSize = iAsnTagSize + iAsnLengthSize + iAsnValueSize;
    }
    else
    {
        // 跳过 tag:'30'和长度指示长度
        iAsnTagSize  = tmpAsnFormat.getAsnTag(FileBuffer, signTmp);
        iAsnLengthSize = tmpAsnFormat.getAsnLength(FileBuffer + iAsnTagSize, iAsnValueSize);
        m_FileHeaderSize = iAsnTagSize + iAsnLengthSize;

        // 跳过 tag:'A0',长度指示长度和包含内容长度
        iAsnTagSize  = tmpAsnFormat.getAsnTag(FileBuffer + m_FileHeaderSize, signTmp);
        iAsnLengthSize = tmpAsnFormat.getAsnLength(FileBuffer + m_FileHeaderSize + iAsnTagSize, iAsnValueSize);
        m_FileHeaderSize += iAsnTagSize + iAsnLengthSize + iAsnValueSize;

        // 跳过 tag:'A1',长度指示长度
        iAsnTagSize  = tmpAsnFormat.getAsnTag(FileBuffer + m_FileHeaderSize, signTmp);
        iAsnLengthSize = tmpAsnFormat.getAsnLength(FileBuffer + m_FileHeaderSize + iAsnTagSize, iAsnValueSize);
        m_FileHeaderSize += iAsnTagSize + iAsnLengthSize ;
    }
    return;
}

//##ModelId=4855B6BD00FA
void GeneralFile::dealHuawTail(const unsigned char *FileBuffer)
{
    AsnFormat tmpAsnFormat;
    int iAsnLengthSize, iAsnValueSize, file_HSize, records_Size, file_Lenth;

    iAsnLengthSize = iAsnValueSize = 0;

    // 跳过 tag:'30'和长度指示长度
    iAsnLengthSize = tmpAsnFormat.getAsnLength(FileBuffer + 1, iAsnValueSize);
    file_HSize = 1 + iAsnLengthSize;

    // 跳过 tag:'A0',长度指示长度和包含内容长度
    iAsnLengthSize = tmpAsnFormat.getAsnLength(FileBuffer + file_HSize + 1, iAsnValueSize);
    file_HSize += 1 + iAsnLengthSize + iAsnValueSize;

    // 跳过 tag:'A1',长度指示长度
    iAsnLengthSize = tmpAsnFormat.getAsnLength(FileBuffer + file_HSize + 1, iAsnValueSize);
    file_HSize += 1 + iAsnLengthSize ;
    records_Size = iAsnValueSize;

    // 跳过通话记录长度以及 tag:'A2',长度指示长度和包含内容长度
    iAsnLengthSize = tmpAsnFormat.getAsnLength(FileBuffer + file_HSize + records_Size + 1, iAsnValueSize);
    m_FileTailSize = 1 + iAsnLengthSize + iAsnValueSize;

    // 跳过 tag:'A3',长度指示长度和包含内容长度
    iAsnLengthSize = tmpAsnFormat.getAsnLength(FileBuffer + file_HSize + records_Size + m_FileTailSize + 1, iAsnValueSize);
    m_FileTailSize += 1 + iAsnLengthSize + iAsnValueSize;
}

// 针对华为TD交换机文件尾的特殊处理		added by zaiww 20090805
void GeneralFile::dealHuaweiTDFileTail(const unsigned char *FileBuffer)
{
	AsnFormat tmpAsnFormat;
	int iAsnLengthSize,iAsnValueSize,iAsnTagSize,iFileHeaderSize;
	unsigned char signTmp[10];
	
	iAsnLengthSize=iAsnValueSize=0;
	
	// 取出tag:'30'的指示长度
	iAsnTagSize  = tmpAsnFormat.getAsnTag(FileBuffer,signTmp);
	iAsnLengthSize = tmpAsnFormat.getAsnLength(FileBuffer+iAsnTagSize,iAsnValueSize);
	iFileHeaderSize = iAsnTagSize + iAsnLengthSize;
	m_FileTailSize = iAsnValueSize;//先置tag=30所指示的整个文件长度(不包括30和length的长度)
	
	// 取出tag:'A0'的指示长度
	iAsnTagSize  = tmpAsnFormat.getAsnTag(FileBuffer+iFileHeaderSize,signTmp);
	iAsnLengthSize = tmpAsnFormat.getAsnLength(FileBuffer+iFileHeaderSize+iAsnTagSize,iAsnValueSize);
	iFileHeaderSize += iAsnTagSize + iAsnLengthSize + iAsnValueSize;
	m_FileTailSize -= iAsnTagSize + iAsnLengthSize + iAsnValueSize;//再减去tag=A0所指示的整个模块长度(包括A0和length的长度)
	
	// 取出tag:'A1'的指示长度
	iAsnTagSize  = tmpAsnFormat.getAsnTag(FileBuffer+iFileHeaderSize,signTmp);
	iAsnLengthSize = tmpAsnFormat.getAsnLength(FileBuffer+iFileHeaderSize+iAsnTagSize,iAsnValueSize);
	//m_FileHeaderSize += iAsnTagSize + iAsnLengthSize ;		
	m_FileTailSize -= iAsnTagSize + iAsnLengthSize + iAsnValueSize;//再减去tag=A1所指示的整个模块长度(包括A1和length的长度)
	
	return;	
}
