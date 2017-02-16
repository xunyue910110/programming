// ##########################################
// Source file : PreProcApplication.h
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

#ifndef PREPROCAPPLICATION_H_HEADER_INCLUDED_B7A661D5
#define PREPROCAPPLICATION_H_HEADER_INCLUDED_B7A661D5

#include "base/supdebug.h"
#include "config-all.h"

#include <string>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include "PreProApi.h"
#include "appfrm/ErrorHandle.h"
#include "Baf.h"

USING_NAMESPACE(std);

extern Baf g_theBaf;        //处理nortel等话单中的module

double gettime();
class FieldInfos;
class OutputFile;
class RawCdrProcessor;
class RawFileProcessor;
class FieldInfo;
class PpParameterFromFile;
class CdrFlag;
class PpLog;

//##ModelId=48508C85035B
//##Documentation
//## 文件管理类
//## 此类负责查找所有的原始话单文件,调用RawFileProcess对象,对话单文件进行处理
//## 此类由外部实现,在此仅表示逻辑关系
class PreProcApplication
{
  public:
    //##ModelId=48508FB1030D
    PreProcApplication(const string &theDataSource, int isShow);

    //##ModelId=4850C2310148
    ~PreProcApplication();

    //##ModelId=4850C3D401F4
    int run(const string &aSrcFileName);

    //##ModelId=4850C407034B
    bool initialize();

    //##ModelId=4850C41A0138
    bool initialize(const string &theSwitchName, const vector <string> &pathMessage,
                    const float errorRate, const string &isBackupSrc, const string &isBackupDes,
                    const string &isCheckCdrNo, const string &isSplitDes, int maxCdrNum,
                    const string &roamPath, bool isSplitLongCdr, const string &longPath);

  public:
    //##ModelId=4850C5050242
    RawFileProcessor *m_theRawFileProcess;

    //##ModelId=4850C52E0000
    RawCdrProcessor *m_theCdrProcess;
    
    //##ModelId=4850C52E0001
    RawCdrProcessor *m_theFileHeadProcess;
    
    //##ModelId=4850C52E0002
    RawCdrProcessor *m_theFileTailProcess;

    //##ModelId=4850C54302FD
    FieldInfos *m_pFieldInfos;

    //##ModelId=4850C55C009C
    OutputFile *m_pOutputFile;

    //##ModelId=4850C581006D
    PreProApi m_PreProApi;

  private:
    //##ModelId=4850C5B80242
    RawFileProcessor *createFileObject(int rawFileType);

    //##ModelId=4850C5E0037A
    RawCdrProcessor *createCdrProcessObject(int cdrType);

    //##ModelId=4850C5E0037B
    RawCdrProcessor *createFileHeadProcessObject(int cdrType);

    //##ModelId=4850C5E0037C
    RawCdrProcessor *createFileTailProcessObject(int cdrType);

    //##ModelId=4850C5F80213
    void showSrcCdrBuffer(const unsigned char *theSrcCdrBuffer, int size);

    //##ModelId=4850C68E005D
    void showSrcFields();

    //##ModelId=4850C6AE003E
    bool reset(const string &srcFileName);

    //##ModelId=4850C6C90280
    void writePpLog();

  private:
    //##ModelId=4850C10D029F
    PpLog *m_PpLog_;

    //##ModelId=4850C1470280
    PpParameterFromFile *m_thePpParameter_;

    //##ModelId=4850C16F01A5
    unsigned char *m_srcCdrBuffer;

    //##ModelId=4850C18E033C
    CdrFlag *m_cdrFlag;

    //##ModelId=4850C1A00157
    int m_cdrType;

    //##ModelId=4850C1AA005D
    int m_dataSource;

    //##ModelId=4850C9510148
    bool m_AlertByEmptyFile;

    //##ModelId=4850C9510149
    int m_file_head_size;

    //##ModelId=4850C951014A
    int m_file_tail_size;

    //##ModelId=4850C951014B
    bool m_FileHeadParse;

    //##ModelId=4850C951014C
    bool m_FileTailParse;
        
    //通过文件头或者文件尾校验文件话单总量: 0,不校验话单条数;1,通过文件头校验;2,通过文件尾校验
    //added by zaiww 20090820
    int m_CheckCdrSum;
    
    //通过文件头产生回执文件	added by zaiww 20090820
    bool m_FileHeadReceipt;
    
    //通过文件尾产生回执文件	added by zaiww 20090820
    bool m_FileTailReceipt;

    //##ModelId=4850C951014D
    unsigned char *m_srcFileHeadBuffer;

    //##ModelId=4850C951014E
    unsigned char *m_srcFileTailBuffer;

    //##ModelId=4850C951014F
    string m_roamlog;
    
    FieldInfo * m_pField_FileName_; // 文件名字段 
    FieldInfo * m_pField_FileHead_; 
    FieldInfo * m_pField_HTotalCDRS; 
    FieldInfo * m_pField_FileTail; 
    FieldInfo * m_pField_TTotalCDRS; 
    FieldInfo * m_pField_TotalFee;
};

#endif /* PREPROCAPPLICATION_H_HEADER_INCLUDED_B7A661D5 */
