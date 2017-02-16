// ##########################################
// Source file : PreProcApplication.cpp
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

#include <vector>
#include "PreProcApplication.h"
#include "PpConstDef.h"
#include "FieldInfos.h"
#include "OutputFile.h"
#include "RawCdrProcessor.h"
#include "AsnRawCdrProcessor.h"
#include "FixedRawCdrProcessor.h"
#include "SasnRawCdrProcessor.h"
#include "TokenRawCdrProcessor.h"
#include "RawFileProcessor.h"
#include "FieldInfo.h"
#include "PpParameterFromFile.h"
#include "CdrFlag.h"
#include "PpLog.h"
#include "CdrErrorCode.h"
#include "RepeatableFieldInfo.h"
#include "Pp_Exception.h"
#include "GeneralFile.h"

Baf g_theBaf;

//OutputFile PreProcApplication::m_pOutputFile;

//##ModelId=48508FB1030D
PreProcApplication::PreProcApplication(const string &theDataSource, int isShow)
{
    string configName, configPath;
    m_thePpParameter_   = NULL;
    m_theRawFileProcess = NULL;
    m_theCdrProcess     = NULL;
    m_theFileHeadProcess= NULL;
    m_theFileTailProcess= NULL;
    m_srcFileHeadBuffer = NULL;
    m_srcFileTailBuffer = NULL;
    m_pOutputFile       = NULL;
	  m_srcCdrBuffer      = NULL;
	  m_FileHeadParse		= false;	//added by zaiww 20090820
    m_FileTailParse		= false;	//added by zaiww 20090820
    m_CheckCdrSum       = 0;

    m_roamlog  = "";
    configPath = "../config/";
    configName = "ppdatesource.cfg";

    m_PpLog_     = PpLog::getInstance();
    m_cdrFlag    = CdrFlag::getInstance();
    m_dataSource = atoi(theDataSource.c_str());

    if (m_PpLog_ != 0)
    {
        if (isShow == 1)
        {
            m_PpLog_->setShowFlag(true);    //显示调试信息
        }
        else
        {
            m_PpLog_->setShowFlag(false);   //不显示调试信息
        }
    }

    m_thePpParameter_ = new PpParameterFromFile(configPath, configName, theDataSource);
    
    m_pFieldInfos = FieldInfos::getInstance(m_dataSource);
    
    m_pField_FileName_ = m_pFieldInfos->getFieldInfoByName(FILE_NAME);     
    
    m_pOutputFile = new OutputFile(m_dataSource);
}


//##ModelId=4850C2310148
PreProcApplication::~PreProcApplication()
{
    free(m_srcCdrBuffer);
    if(m_srcFileHeadBuffer != 0 && m_file_head_size != 0)
    {
        free(m_srcFileHeadBuffer);
        m_srcFileHeadBuffer = 0;
    }
    if(m_srcFileTailBuffer != 0 && m_file_tail_size != 0)
    {
        free(m_srcFileTailBuffer);
        m_srcFileTailBuffer = 0;
    }
    if(m_theRawFileProcess != 0)
    {
        delete m_theRawFileProcess;
    }
    if(m_theCdrProcess != 0)
    {
        m_PpLog_->clear();
        delete m_theCdrProcess;
    }
    if( m_theFileHeadProcess != 0)
    {
        delete m_theFileHeadProcess;
    }
    if( m_theFileTailProcess != 0)
    {
        delete m_theFileTailProcess;
    }
    if(m_pOutputFile != 0)
    {
        delete m_pOutputFile;
    }
    if(m_thePpParameter_ != 0)
    {
        delete m_thePpParameter_;
    }
}

//##ModelId=4850C3D401F4
int PreProcApplication::run(const string &aSrcFileName)
{
    string filename;
    bool error = false;
    int srcCdrLength, size, len;
    string File_Head,File_Tail;
    int iTotal_Cdrs,iTotal_Fee;

    if (!reset(aSrcFileName))
    {
        #ifdef DEBUG
            cout << "PreProcApplication reset error!" << endl;
        #endif
        return -1;
    }

    size = aSrcFileName.size();
    if((len = aSrcFileName.find_last_of("/")) < 0)
    {
        filename = aSrcFileName;
    }
    else
    {
        filename = aSrcFileName.substr(len + 1, size - len - 1);
    }

    FP_BEGIN(run)
    int i=0;
    while ((srcCdrLength = m_theRawFileProcess->getCdr(m_srcCdrBuffer)) > 0)
    {
        #ifdef _DEBUG_
        	showSrcCdrBuffer(m_srcCdrBuffer,srcCdrLength);
        #endif
   
		    m_pFieldInfos->reset();
        	CdrFlag::getInstance()->clear();
	        m_pField_FileName_->m_IsFormat       = true;
        	m_pField_FileName_->setDesValue( filename );
        
        FP_BEGIN(getDesCdr)
        if(srcCdrLength==1094)
        {
        	 i++;        	
        }
        if(i==57)
        {
        	 int kkk =0;
        }
        // 得到原始字段值
        if(m_theCdrProcess->getDesCdr(m_srcCdrBuffer, srcCdrLength) < 0)
        {
            continue;
        }
        FP_END(getDesCdr)
        
        FP_BEGIN(formatField)
            m_theCdrProcess->formatField();          // 字段值数据格式标准化
        FP_END(formatField)
        
        FP_BEGIN(convertCdr)
            m_theCdrProcess->convertCdr();           // 对字段值进行转换
        FP_END(convertCdr)
        
        FP_BEGIN(validCdr)
            m_theCdrProcess->validCdr();             // 对字段进行校验
        FP_END(validCdr)
        
        m_PreProApi.processCdr(m_dataSource);       // 编程接口
 
        FP_BEGIN(write)
        if(m_pOutputFile->write() < 0)             // 写话单到标准话单
        {
            error = true;
            break;
        }
        FP_END(write)
        	writePpLog();                                               // 写日志
        
        #ifdef DEBUG
            showSrcFields();
        #endif
    }
    FP_END(run)

    if (m_FileHeadParse == true)
    {
        if (m_file_head_size <= 0)
        {
            throw Pp_Exception (__FILE__, __LINE__,"FileHead Need Parse But File_Head_Size <= 0!");	
        }
        else
        {
       		m_pFieldInfos->reset();
            CdrFlag::getInstance()->clear();

            memcpy(m_srcFileHeadBuffer, m_theRawFileProcess->m_FileHeadBuffer, m_file_head_size);
        #ifdef DEBUG
            showSrcCdrBuffer(m_srcFileHeadBuffer, m_file_head_size);
        #endif
            m_theFileHeadProcess->getDesCdr(m_srcFileHeadBuffer, m_file_head_size);
            m_theFileHeadProcess->formatField();
            m_theFileHeadProcess->convertCdr();
            m_theFileHeadProcess->validCdr();

        	File_Head = m_pField_FileHead_->m_FieldDesValue;
            iTotal_Cdrs = atoi( m_pField_HTotalCDRS->m_FieldDesValue==NULL?"":m_pField_HTotalCDRS->m_FieldDesValue);

        #ifdef DEBUG
            showSrcFields();
        #endif
        }
    }
    if (m_FileTailParse == true)
    {
        FieldInfo *pFTFieldInfo;
        if (m_file_tail_size <= 0)
        {
			throw Pp_Exception (__FILE__, __LINE__,"FileTail Need Parse But FileTail <= 0!");	
        }
        else
        {
            m_pFieldInfos->reset();
            CdrFlag::getInstance()->clear();
            	
            memcpy(m_srcFileTailBuffer, m_theRawFileProcess->m_FileTailBuffer, m_file_tail_size);
        #ifdef DEBUG
            showSrcCdrBuffer(m_srcFileTailBuffer, m_file_tail_size);
        #endif
            m_theFileTailProcess->getDesCdr(m_srcFileTailBuffer, m_file_tail_size);
            m_theFileTailProcess->formatField();
            m_theFileTailProcess->convertCdr();
            m_theFileTailProcess->validCdr();
            
			File_Tail  = m_pField_FileTail->m_FieldDesValue;
        	iTotal_Cdrs = atoi( m_pField_TTotalCDRS->m_FieldDesValue==NULL?"":m_pField_TTotalCDRS->m_FieldDesValue);
			iTotal_Fee  = atoi( m_pField_TotalFee->m_FieldDesValue==NULL?"":m_pField_TotalFee->m_FieldDesValue);

        #ifdef DEBUG
            showSrcFields();
        #endif
        }
    }

    if (m_FileHeadReceipt || m_FileTailReceipt)
    {
        string thisFileName, thisTmpFileName;
        string tmpPath, desPath, inputStr;
        FILE *fDes = NULL;
        struct stat attr;

        desPath = m_roamlog;
        tmpPath = m_roamlog;

        if( tmpPath.compare(tmpPath.length()-1,1,"/") != 0)
        {
            tmpPath.erase(tmpPath.find_last_of("/")+1,tmpPath.npos);
        }
        else
        {
            tmpPath.erase(tmpPath.find_last_of("/")-1,tmpPath.npos);
            tmpPath.erase(tmpPath.find_last_of("/")+1,tmpPath.npos);
        }
        //正在处理的文件
        thisFileName = filename;

        // 将"文件头,错误类型,错误代码,原始话单文件名,文件尾"输出到与原始文件同名的文件中

        inputStr.clear();
        //正在处理的临时输出文件,带路径
        thisTmpFileName  =  tmpPath + thisFileName;

        //打开输出文件
        if ( ( fDes = fopen(thisTmpFileName.c_str(),(char *)"w")) == NULL )
        {
            throw Pp_Exception (__FILE__, __LINE__,"open output file(%s) error!",thisTmpFileName.c_str());
        }
        //输出字符串
        if (m_PpLog_->getErrorSum() >= 1)
        {
            inputStr = File_Head+",2,204,"+thisFileName+","+File_Tail+"\n";
            if ( fputs(inputStr.c_str(),fDes) == EOF )
            {
                throw Pp_Exception (__FILE__, __LINE__,"write output file(%s) error!",thisTmpFileName.c_str());
            }
        }

        if (m_PpLog_->getTotalCdrs() != iTotal_Cdrs)
        {
            inputStr = File_Head+",1,103,"+thisFileName+","+File_Tail+"\n";
            if ( fputs(inputStr.c_str(),fDes) == EOF )
            {
            	throw Pp_Exception (__FILE__, __LINE__,"write output file(%s) error!",thisTmpFileName.c_str());
            }
        }

        if ( (m_PpLog_->getErrorSum() == 0)&&(m_PpLog_->getTotalCdrs() == iTotal_Cdrs))
        {
            inputStr = File_Head+",0,000,"+thisFileName+","+File_Tail+"\n";
            if ( fputs(inputStr.c_str(), fDes) == EOF)
            {
                throw Pp_Exception (__FILE__, __LINE__,"write output file(%s) error!",thisTmpFileName.c_str());
            }
        }

        if (fDes != NULL)
        {
            fclose(fDes);
            fDes = 0;
        }

        string desFileName = desPath + thisFileName;
        if( link(thisTmpFileName.c_str(),desFileName.c_str())!=0 )
        {
            string repeatDesName;
            repeatDesName = desFileName + ".01";
            //如果是由于stdpath目录下有重名文件而link不成功的话,该成"*.01"重试
            if( link(thisTmpFileName.c_str(),repeatDesName.c_str())!=0 )
            {
                throw Pp_Exception (__FILE__, __LINE__,"File %s 不能移动到输出目录 %s!",thisTmpFileName.c_str(),desPath.c_str());
            }
            else
            {
                if( unlink( thisTmpFileName.c_str() )!=0)   //删除
                {
                    throw Pp_Exception (__FILE__, __LINE__,"临时文件 %s 不能删除!",thisTmpFileName.c_str());
                }
            }
        }
        else if( unlink( thisTmpFileName.c_str() )!=0)   //删除
        {
            throw Pp_Exception (__FILE__, __LINE__,"临时文件 %s 不能删除!",thisTmpFileName.c_str());
        }
    }
    
    //added by zaiww 20090820 m_CheckCdrSum :0,不校验话单条数;1,通过文件头校验;2,通过文件尾校验
    if ( m_CheckCdrSum == 1 )
    {
    	if (m_PpLog_->getTotalCdrs() != iTotal_Cdrs)
        {
    		throw Pp_Exception (__FILE__, __LINE__,"文件 %s 话单条数校验错误!文件头: %d ;实际条数:%d",filename.c_str(),iTotal_Cdrs,m_PpLog_->getTotalCdrs());
        }
    }
    if ( m_CheckCdrSum == 2 )
    {
    	if (m_PpLog_->getTotalCdrs() != iTotal_Cdrs)
        {
    		throw Pp_Exception (__FILE__, __LINE__,"文件 %s 话单条数校验错误!文件尾: %d ;实际条数:%d",filename.c_str(),iTotal_Cdrs,m_PpLog_->getTotalCdrs());
        }
    }

    if(error)
    {
        #ifdef DEBUG
            cout << "文件处理错误！" << endl;
        #endif
        m_pOutputFile->closeFiles(1);
    }
    else
    {
        #ifdef DEBUG
            cout << "-----------------------------------------------"       << endl;
            cout << "\tMOC   record sum is \t"<<m_PpLog_->getMocSum()       << endl;
            cout << "\tMTC   record sum is \t"<<m_PpLog_->getMtcSum()       << endl;
            cout << "\tPOC   record sum is \t"<<m_PpLog_->getPocSum()       << endl;
            cout << "\tSMMO  record sum is \t"<<m_PpLog_->getSmocSum()      << endl;
            cout << "\tSMTC  record sum is \t"<<m_PpLog_->getSmtcSum()      << endl;
            cout << "\tFORW  record sum is \t"<<m_PpLog_->getForwSum()      << endl;
            cout << "\tOther record sum is \t"<<m_PpLog_->getOtherSum()     << endl;
            cout << "\tError record sum is \t"<<m_PpLog_->getErrorSum()     << endl;
            cout << "-----------------------------------------------"       << endl;
            cout << "\tTotal     CDR    is \t"<<m_PpLog_->getTotalCdrs()    << endl;
            if (m_PpLog_->getTotalFee() != 0)
            {
                cout << "\tTotal     FEE    is \t"<<m_PpLog_->getTotalFee() << endl;
            }
            cout << "-----------------------------------------------"       << endl;
            cout << "\tstart number     is \t"<<m_PpLog_->getBeginCdrNo()   << endl;
            cout << "\tend   number     is \t"<<m_PpLog_->getEndCdrNo()     << endl;
            cout << "-----------------------------------------------"       << endl;
        #endif
        m_pOutputFile->closeFiles(0);
    }
    return 0;
}

//##ModelId=4850C407034B
bool PreProcApplication::initialize()
{
    float errorRate = 1;
    int maxCdrNum   = 0;
    vector<string> theFilePath;
    string aRate, isBackupDes, isBackupSrc, isCheckCdrNo, isSplitDes, aMaxCdrNum, roamPath, isSpliteLong, longPath;
    bool splitLong;

    if (m_thePpParameter_ == 0)
    {
    	throw Pp_Exception (__FILE__, __LINE__,"cannot create thePpParameter object!");
    }

    if (m_PpLog_ == 0)
    {
    	throw Pp_Exception (__FILE__, __LINE__,"cannot create PpLog object!");
    }

    if (m_cdrFlag == 0)
    {
    	throw Pp_Exception (__FILE__, __LINE__,"cannot create cdrFlag object!");
    }

    if (!m_thePpParameter_->initialize())
    {
    	throw Pp_Exception (__FILE__, __LINE__,"cannot initialize PpParameter!");
    }

    //得到section为"path"的配置文件内容
    if (!m_thePpParameter_->getSectionList("path", theFilePath))
    {
    	throw Pp_Exception (__FILE__, __LINE__,"cannot get file path PpParameter!");
    }

    m_thePpParameter_->getValue("error_rate", aRate);
    errorRate=atof(aRate.c_str());
    m_thePpParameter_->getValue("max_cdrNum",aMaxCdrNum);
    maxCdrNum=atoi(aMaxCdrNum.c_str());
    m_thePpParameter_->getValue("split_longcdr", isSpliteLong);
    if (isSpliteLong.compare("yes") == 0)
    {
        splitLong = true;
        m_thePpParameter_->getValue("long_cdr_path", longPath);
    }
    else
    {
        splitLong = false;
        longPath  = "";
    }
    m_thePpParameter_->getValue("check_cdrNo", isCheckCdrNo);
    m_thePpParameter_->getValue("backup_desfile", isBackupDes);
    m_thePpParameter_->getValue("backup_srcfile", isBackupSrc);
    m_thePpParameter_->getValue("split_desfile", isSplitDes);
    m_thePpParameter_->getValue("roamlog_path", roamPath);

    return initialize("", theFilePath, errorRate, isBackupSrc, isBackupDes, isCheckCdrNo, isSplitDes, maxCdrNum, roamPath, splitLong, longPath);
    
    
}

//##ModelId=4850C41A0138
bool PreProcApplication::initialize(const string &theSwitchName, const vector <string> &pathMessage,
                                    const float errorRate, const string &isBackupSrc, const string &isBackupDes,
                                    const string &isCheckCdrNo, const string &isSplitDes, int maxCdrNum,
                                    const string &roamPath, bool isSplitLongCdr, const string &longPath)
{
    vector<string> theConvertRule, theValidRule;
    vector<string> theFHTConvertRule, theFHTValidRule;
    string aRawFileType, aCdrType, out_file_type, token, aEmptyfile;
    string file_head_receipt, file_tail_receipt, file_header_size, file_tail_size, check_cdr_sum;
    vector<string> theStandardFile, theRawFileParameter, theCdrParameter, theBafParameter;
    vector<string> theFileHeadParameter,theFileTailParameter;

    m_roamlog = roamPath;

    if (m_thePpParameter_ == 0)
    {
        throw Pp_Exception( __FILE__, __LINE__,"cannot create thePpParameter object!" );
    }
    if(!theSwitchName.empty())
    {
        if (!m_thePpParameter_->initialize(theSwitchName, pathMessage) )
        {
			throw Pp_Exception( __FILE__, __LINE__,"cannot initialize PpParameter!" );
        }
    }

    m_thePpParameter_->getValue("token", "token", token);
    m_thePpParameter_->getValue(CDR_TYPE, CDR_TYPE, aCdrType);
    m_thePpParameter_->getValue(OUTPUT_FILE_TYPE, OUTPUT_FILE_TYPE, out_file_type);
    m_thePpParameter_->getValue(RAWFILE_TYPE, RAWFILE_TYPE, aRawFileType);
    m_thePpParameter_->getValue(RAWFILE_TYPE, EMPTY_FILE_ALERT, aEmptyfile);
    m_thePpParameter_->getValue(FILE_HEANDER, FILE_HEADER_SIZE,file_header_size);
    m_thePpParameter_->getValue(FILE_HEANDER, FILE_TAIL_SIZE,file_tail_size);
    m_thePpParameter_->getValue(FILE_HEANDER, FILE_HEAD_RECEIPT,file_head_receipt);
    m_thePpParameter_->getValue(FILE_HEANDER, FILE_TAIL_RECEIPT,file_tail_receipt);
    m_thePpParameter_->getValue(FILE_HEANDER, CHECK_CDR_SUM,check_cdr_sum);

   
    if (aEmptyfile.compare("YES") == 0)
    {
        m_AlertByEmptyFile = true;
    }
    else
    {
        m_AlertByEmptyFile = false;
    }
    //added by zaiww 20090820 m_CheckCdrSum :0,不校验话单条数;1,通过文件头校验;2,通过文件尾校验
    if (check_cdr_sum.compare("0") == 0)
    {
        m_CheckCdrSum   = 0;
    }
    else if (check_cdr_sum.compare("1") == 0)
    {
        m_CheckCdrSum   = 1;
        m_FileHeadParse = true;
    }
    else if (check_cdr_sum.compare("2") == 0)
    {
    	m_CheckCdrSum   = 2;
    	m_FileTailParse = true;
    }
    //modified by zaiww 20090820 file_head_receipt: 0,不产生回执文件;1,产生回执文件
    if (file_head_receipt.compare("1") == 0)
    {
        m_FileHeadReceipt = true;
        m_FileHeadParse   = true;
    }
    else
    {
        m_FileHeadReceipt = false;
    }
    //modified by zaiww 20090820 file_tail_receipt: 0,不产生回执文件;1,产生回执文件
    if (file_tail_receipt.compare("1") == 0)
    {
    	m_FileTailReceipt = true;
		m_FileTailParse   = true;
	}
	else
	{
		m_FileTailReceipt = false;
	}
		
    if ( m_FileHeadParse == true )
    {
        if (m_srcFileHeadBuffer != 0)
        {
            free(m_srcFileHeadBuffer);
            m_srcFileHeadBuffer = 0;
        }
        if ( (m_srcFileHeadBuffer = (unsigned char *)malloc(atoi(file_header_size.c_str()))) == 0)    // 文件头申请缓存
        {
			throw Pp_Exception (__FILE__, __LINE__,"malloc src file head buffer error!");	
        }
        m_pField_FileHead_  =  m_pFieldInfos->getFieldInfoByName(H_FILE_HEAD);
        m_pField_HTotalCDRS =  m_pFieldInfos->getFieldInfoByName(H_TOTAL_CDRS);
    }
    if ( m_FileTailParse == true )
    {
        if (m_srcFileTailBuffer != 0)
        {
            free(m_srcFileTailBuffer);
            m_srcFileTailBuffer = 0;
        }
        if ( (m_srcFileTailBuffer = (unsigned char *)malloc(atoi(file_tail_size.c_str()))) == 0)    // 文件尾申请缓存
        {
			throw Pp_Exception (__FILE__, __LINE__,"malloc src file tail buffer error!");	
        }
		m_pField_FileTail  =  m_pFieldInfos->getFieldInfoByName(T_FILE_TAIL);
        m_pField_TTotalCDRS =  m_pFieldInfos->getFieldInfoByName(T_TOTAL_CDRS);
        m_pField_TotalFee =  m_pFieldInfos->getFieldInfoByName(T_TOTAL_FEE);
    }
    m_file_head_size = atoi(file_header_size.c_str());
    m_file_tail_size = atoi(file_tail_size.c_str());
    m_cdrType = atoi(aCdrType.c_str());

    if (createFileObject(atoi(aRawFileType.c_str())) == 0)
    {
		throw Pp_Exception (__FILE__, __LINE__,"cannot create file object!");	
    }
    else if (createCdrProcessObject(atoi(aCdrType.c_str())) == 0)
    {
		throw Pp_Exception (__FILE__, __LINE__,"cannot create CDR Process object!");	
    }

    if (m_FileHeadParse)
    {
        if (createFileHeadProcessObject(atoi(aCdrType.c_str())) == 0)
        {
            throw Pp_Exception (__FILE__, __LINE__,"cannot create File_Head Process object!");
        }
        // 得到section为"file_head"的配置文件内容
        if (!m_thePpParameter_->getSectionList("file_head", theFileHeadParameter))//theFileHeadParameter存放文件头的配置信息
        {
            throw Pp_Exception (__FILE__, __LINE__,"cannot get file_head PpParameter!");
        }
        //得到section为"fht_convert"的配置文件内容
        if (!m_thePpParameter_->getSectionList("fht_convert", theFHTConvertRule))
        {
            throw Pp_Exception (__FILE__, __LINE__,"cannot get fht_convert  PpParameter!");
        }
        //得到section为"fht_valid"的配置文件内容
        if (!m_thePpParameter_->getSectionList("fht_valid", theFHTValidRule))
        {
            throw Pp_Exception (__FILE__, __LINE__,"cannot get fht_valid  PpParameter!");
        }
        if (!m_theFileHeadProcess->setToken(token))
    	{
            throw Pp_Exception (__FILE__, __LINE__,"cannot initialize FileHeadProcess setToken!");  
    	} 
    	else if (!m_theFileHeadProcess->initialize(theFileHeadParameter)) //读取配置信息，创建字段提取对象，初始化各个参数
        {
            throw Pp_Exception (__FILE__, __LINE__,"cannot initialize FileHeadProcess!");  
        }
    }

    if (m_FileTailParse)
    {
        if (createFileTailProcessObject(atoi(aCdrType.c_str())) == 0)
        {
            throw Pp_Exception (__FILE__, __LINE__,"cannot create File_Tail Process object!");
        }
        // 得到section为"file_tail"的配置文件内容
        if (!m_thePpParameter_->getSectionList("file_tail", theFileTailParameter))
        {
            throw Pp_Exception (__FILE__, __LINE__,"cannot get file_tail PpParameter!");
        }
        if (!m_FileHeadParse)
        {
            //得到section为"fht_convert"的配置文件内容
            if (!m_thePpParameter_->getSectionList("fht_convert", theFHTConvertRule))
            {
            	throw Pp_Exception (__FILE__, __LINE__,"cannot get fht_convert  PpParameter!");
            }
            //得到section为"fht_valid"的配置文件内容
            if (!m_thePpParameter_->getSectionList("fht_valid", theFHTValidRule))
            {
            	throw Pp_Exception (__FILE__, __LINE__,"cannot get fht_valid  PpParameter!");
            }
        }
        if (!m_theFileTailProcess->setToken(token))
    	{
            throw Pp_Exception (__FILE__, __LINE__,"cannot initialize FileTailProcess setToken!");
    	} 
    	else if (!m_theFileTailProcess->initialize(theFileTailParameter)) //创建字段提取对象，初始化各个数据成员
        {
            throw Pp_Exception (__FILE__, __LINE__,"cannot initialize FileTailProcess!");
        }
    }

    //得到section为"record"的配置文件内容
    if (!m_thePpParameter_->getSectionList("record", theCdrParameter))
    {
        throw Pp_Exception (__FILE__, __LINE__,"cannot get record  PpParameter!");
    }

    //得到section为"BAF"的配置文件内容
    if (!m_thePpParameter_->getSectionList("BAF", theBafParameter))
    {
        throw Pp_Exception (__FILE__, __LINE__,"cannot get BAF PpParameter!");
    }
    //得到section为"convert"的配置文件内容
    if (!m_thePpParameter_->getSectionList("convert", theConvertRule))
    {
        throw Pp_Exception (__FILE__, __LINE__,"cannot get convert  PpParameter!");
    }
    //得到section为"valid"的配置文件内容
    if (!m_thePpParameter_->getSectionList("valid", theValidRule))
    {
        throw Pp_Exception (__FILE__, __LINE__,"cannot get valid  PpParameter!");
    }
    //得到section为"output_file"的配置文件内容
    if (!m_thePpParameter_->getSectionList("output_file", theStandardFile))
    {
        throw Pp_Exception (__FILE__, __LINE__,"cannot get output file  PpParameter!");
    }
    else if (!m_theCdrProcess->setToken(token))
    {
        throw Pp_Exception (__FILE__, __LINE__,"cannot initialize CdrProcess setToken!");
    } 
    else if (m_theRawFileProcess->initialize(m_thePpParameter_, atoi( aCdrType.c_str() )) == 0)//初始化文件头，尾的长度和块的长度
    {
        throw Pp_Exception (__FILE__, __LINE__,"cannot initialize RawFileProcessor!");
    } 
    else if (!m_theCdrProcess->initialize(theCdrParameter))
    {
        throw Pp_Exception (__FILE__, __LINE__,"cannot initialize CdrProcess!");
    } 
    else if (!m_theCdrProcess->setRule(theConvertRule,theValidRule))
    {
        throw Pp_Exception (__FILE__, __LINE__,"cannot initialize CdrProcess setRule!");
    } 
    else if (!m_pOutputFile->initialize(theStandardFile, pathMessage, isBackupSrc, isBackupDes,
                                         isCheckCdrNo, out_file_type, errorRate, isSplitDes, maxCdrNum, isSplitLongCdr, longPath))
    {
        throw Pp_Exception (__FILE__, __LINE__,"cannot initialize OutputFile!");
    } 
    else if (!g_theBaf.initialize(theBafParameter))
    {
        throw Pp_Exception (__FILE__, __LINE__,"cannot initialize OutputFile!");
    }

    if (m_FileHeadParse)
    {
        if (!m_theFileHeadProcess->setRule(theFHTConvertRule, theFHTValidRule))
        {
            throw Pp_Exception (__FILE__, __LINE__,"cannot initialize FileHeadProcess setRule!");
            #ifdef DEBUG
                cout << "cannot initialize FileHeadProcess setRule!" << endl;
            #endif
        }
    }
    if (m_FileTailParse)
    {
    	if (!m_theFileTailProcess->setRule(theFHTConvertRule, theFHTValidRule))
    	{
            throw Pp_Exception (__FILE__, __LINE__,"cannot initialize FileTailProcess setRule!");
    	}
    }

    //传入token话单的"分割符",用于token话单的"字段数"校验
    m_theRawFileProcess->setToken(token);
    return true;
}

//##ModelId=4850C5B80242
RawFileProcessor *PreProcApplication::createFileObject(int rawFileType)
{
    switch (rawFileType)
    {
    case 1 :
        m_theRawFileProcess = new GeneralFile();
        m_PpLog_->ShowMessage("Create General file object.\n", 0);
        break;
    default :
        m_theRawFileProcess = 0;
        break;
    }
    return m_theRawFileProcess;
}

//##ModelId=4850C5E0037A
RawCdrProcessor *PreProcApplication::createCdrProcessObject(int cdrType)
{
    switch (cdrType)
    {
    case 1 :
        m_theCdrProcess = new FixedRawCdrProcessor(m_dataSource);
        m_PpLog_->ShowMessage("Create Fixed cdr object.\n", 0);
        break;
    case 2 :
        m_theCdrProcess = new TokenRawCdrProcessor(m_dataSource);
        m_PpLog_->ShowMessage("Create Token cdr object.\n", 0);
        break;
    case 3 :
    case 4 :
        m_theCdrProcess = new AsnRawCdrProcessor(m_dataSource);
        m_PpLog_->ShowMessage("Create Asn cdr object.\n", 0);
        break;
    case 5 :
        m_theCdrProcess = new SasnRawCdrProcessor(m_dataSource);
        m_PpLog_->ShowMessage("Create Sasn cdr object.\n", 0);
        break;
    default :
        m_theCdrProcess = 0;
        break;
    }
    return m_theCdrProcess;
}

//##ModelId=4850C5E0037B
RawCdrProcessor *PreProcApplication::createFileHeadProcessObject(int cdrType)
{
    switch (cdrType)
    {
    case 1 :
        m_theFileHeadProcess = new FixedRawCdrProcessor(m_dataSource);
        m_PpLog_->ShowMessage("Create Fixed File_Head object.\n", 0);
        break;
    case 2 :
        m_theFileHeadProcess = new TokenRawCdrProcessor(m_dataSource);
        m_PpLog_->ShowMessage("Create Token File_Head object.\n", 0);
        break;
    case 3 :
    case 4 :
        m_theFileHeadProcess = new AsnRawCdrProcessor(m_dataSource);
        m_PpLog_->ShowMessage("Create Asn File_Head object.\n", 0);
        break;
    case 5 :
        m_theFileHeadProcess = new SasnRawCdrProcessor(m_dataSource);
        m_PpLog_->ShowMessage("Create Sasn File_Head object.\n", 0);
        break;
    default :
        m_theFileHeadProcess=0;
        break;
    }
    return m_theFileHeadProcess;
}

//##ModelId=4850C5E0037C
RawCdrProcessor *PreProcApplication::createFileTailProcessObject(int cdrType)
{
    switch (cdrType)
    {
    case 1 :
        m_theFileTailProcess = new FixedRawCdrProcessor(m_dataSource);
        m_PpLog_->ShowMessage("Create Fixed File_Tail object.\n", 0);
        break;
    case 2 :
        m_theFileTailProcess = new TokenRawCdrProcessor(m_dataSource);
        m_PpLog_->ShowMessage("Create Token File_Tail object.\n", 0);
        break;
    case 3 :
    case 4 :
        m_theFileTailProcess = new AsnRawCdrProcessor(m_dataSource);
        m_PpLog_->ShowMessage("Create Asn File_Tail object.\n", 0);
        break;
    case 5 :
        m_theFileTailProcess = new SasnRawCdrProcessor(m_dataSource);
        m_PpLog_->ShowMessage("Create Sasn File_Tail object.\n", 0);
        break;
    default :
        m_theFileTailProcess=0;
        break;
    }
    return m_theFileTailProcess;
}

//##ModelId=4850C5F80213
void PreProcApplication::showSrcCdrBuffer(const unsigned char *theSrcCdrBuffer, int size)
{
    int i;
    //char tmp[1024];
    char *tmp;
    string srcCdr;

    if (!m_PpLog_->isShowFlag())
    {
        return ;
    }

    tmp = (char *)malloc(size+32);     //add by zhangch2 2009-12-26 16:20

    sprintf(tmp, "%d", size);
    m_PpLog_->ShowMessage("\nCdr size is " + string(tmp) + ", ", 0);
    if (m_cdrType == 2)
    {
        memcpy(tmp, theSrcCdrBuffer, size);
        tmp[size] = 0;
        srcCdr = tmp;
    }
    else
    {
        for(i = 0; i < size; i++)
        {
            sprintf(tmp, "%02X ", theSrcCdrBuffer[i]);
            srcCdr += tmp;
        }
    }
    m_PpLog_->ShowMessage(srcCdr+string(" .\n"), 0);
    free(tmp);
}

//##ModelId=4850C68E005D
void PreProcApplication::showSrcFields()
{
    char tmp[4], show[2000];
    string srcField;
    int iFieldCount;

    if (!m_PpLog_->isShowFlag())
        return ;

    for (iFieldCount = 0; iFieldCount<m_pFieldInfos->getFieldNum(); ++iFieldCount)
    {
        srcField = string("[ ");
        for(int i = 0; i < ((*m_pFieldInfos)[iFieldCount])->m_iSrcValueLen; ++i)
        {
            sprintf(tmp, "%02X ", ((*m_pFieldInfos)[iFieldCount])->m_FieldSrcValue[i]);
            srcField += tmp;
        }
        srcField += "]";
        sprintf(show, "%-24s {%d} %-22s %-40s \n", m_pFieldInfos->getFieldNameBySeq(iFieldCount)
        									, iFieldCount
        									, ((*m_pFieldInfos)[iFieldCount])->m_FieldDesValue==NULL?"":((*m_pFieldInfos)[iFieldCount])->m_FieldDesValue
        									, srcField.c_str());
        m_PpLog_->ShowMessage(show, 0);
    }
    for (iFieldCount = 0; iFieldCount<m_pFieldInfos->getRepeatableFieldNum(); ++iFieldCount)
    {
        RepeatableFieldInfo *pTmpFieldInfo;
        
        pTmpFieldInfo = dynamic_cast<RepeatableFieldInfo*>( (*m_pFieldInfos)[iFieldCount+DIVIDE] );
        for(int j=0;j<pTmpFieldInfo->getNumCount();++j)
        {
	        srcField = string("[ ");
	        for(int i = 0; i < pTmpFieldInfo->m_fieldInfoVect[j]->m_iSrcValueLen; ++i)
	        {
	            sprintf(tmp, "%02X ", pTmpFieldInfo->m_fieldInfoVect[j]->m_FieldSrcValue[i]);
	            srcField += tmp;
	        }
	        srcField += "]";
	        sprintf(show, "%-24s %-22s %-40s \n", m_pFieldInfos->getFieldNameBySeq(iFieldCount+DIVIDE)
	        									, pTmpFieldInfo->m_fieldInfoVect[j]->m_FieldDesValue==NULL?"":pTmpFieldInfo->m_fieldInfoVect[j]->m_FieldDesValue
	        									, srcField.c_str());
	        m_PpLog_->ShowMessage(show, 0);
    	}
    }
    sprintf(show, "%-24s %03d %-40s\n", (char *)"ERROR_CODE"
    								  , m_cdrFlag->m_errorCode
    								  , m_cdrFlag->m_fieldName.c_str());
    m_PpLog_->ShowMessage(show, 0);
}

//##ModelId=4850C6AE003E
bool PreProcApplication::reset( const string &srcFileName)
{
    int fileLength;
    unsigned char *srcFileBuffer;

    m_PpLog_->clear();
    m_PpLog_->setFileName(srcFileName);
	m_cdrFlag->resetCdrNo(); 

    if ((fileLength = m_pOutputFile->setFileBuffer(srcFileName, srcFileBuffer)) <= 0)
    {
        if(m_AlertByEmptyFile == true)
        {
			throw Pp_Exception (__FILE__, __LINE__,"file %s is empty or open error!!",srcFileName.c_str());	
        }
        return true;
    }
    // 记录输入文件长度
    m_PpLog_->setFileLenth(fileLength);

    if (!m_theRawFileProcess->setRawFile(srcFileName, srcFileBuffer, fileLength))
    {
        m_pOutputFile->mvToErrDir(srcFileName);    // 移动文件到错误目录
		throw Pp_Exception (__FILE__, __LINE__,"cannot initialize setRawFile");	
    }

    if (!m_pOutputFile->reset(srcFileName))
    {
		throw Pp_Exception (__FILE__, __LINE__,"cannot initialize m_pOutputFile");
    }
    return true;
}

//##ModelId=4850C6C90280
void PreProcApp
    }
    // 璁板綍杈撳叆鏂囦欢闀垮害
    m_PpLog_->setFileLenth(fileLength);

    if (!m_theRawFileProcess->setRawFile(srcFileName, srcFileBuffer, fileLength))
    {
        m_pOutputFile->mvToErrDir(srcFileName);    // 绉诲姩鏂囦欢鍒伴敊璇