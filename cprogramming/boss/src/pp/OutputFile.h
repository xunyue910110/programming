// ##########################################
// Source file : OutputFile.h
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

#ifndef OUTPUTFILE_H_HEADER_INCLUDED_B7A65B5B
#define OUTPUTFILE_H_HEADER_INCLUDED_B7A65B5B

#include "CdrFlag.h"
#include "PpLog.h"
#include "FieldInfo.h"
#include "config-all.h"
#include "appfrm/ErrorHandle.h"

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <vector>
#include <string>
#include <map>

extern ErrorMessages * const theErrorMessages;

USING_NAMESPACE(std);

class StandardFile;
class FieldInfos;

//## 输出文件处理类
class OutputFile
{
  public:
    //##ModelId=4850D16C00AB
    OutputFile(const int iSubChannel);

    //##ModelId=4850D2290148
    ~OutputFile();

    //##ModelId=4850D255038A
    int write();

    //##ModelId=4850D27B0280
    bool reset(string fileName);

    //##ModelId=4850D2A30213
    int setFileBuffer(const string &aSrcFileName, unsigned char *&srcFileBuffer);

    //##ModelId=4850D2DA038A
    //##Documentation
    //## 关闭全部输出文件句柄
    //## flag=0,正确  flag =1,错误
    void closeFiles(int flag);

    //##ModelId=4850D2EC008C
    bool initialize(const vector<string> &aStandardFile, const vector<string> &aFilePath,
                    const string &isBackupSrc, const string &isBackupDes, const string &isCheckCdrNo,
                    const string &outFileType, float errorRate, const string &isSplitDes, int maxCdrNum,
                    bool isSpliteLongCdr, const string &longPath);

    //##ModelId=4850D36B038A
    void mvToErrDir(const string & filename);

  private:
    //##ModelId=4850D38E02EE
    string getPath(vector<string> aFilePath, string pathName);

    //##ModelId=4850D17101C5
    StandardFile *m_pStandardFile;

    //##ModelId=4850D17101C6
    StandardFile *m_pStandardLongFile;

    //##ModelId=4850D17F02DE
    string m_srcFilePath;

    //##ModelId=4850D18D038A
    int m_srcfd;

    //##ModelId=4850D19701D4
    int m_fileLength;

    //##ModelId=4850D1A1038A
    string m_srcFileBakPath;

    //##ModelId=4850D1A1038B
    string m_longCdrPath;

    //##ModelId=4850D1AF008C
    unsigned char *m_srcFileBuffer;

    //##ModelId=4850D1BD01E4
    string m_fileErrorPath;

    //##ModelId=4850D1C802EE
    string m_srcFileName;

    //##ModelId=4850D1D1034B
    bool m_isBackupSrcFile;

    //##ModelId=4850D1DD0119
    CdrFlag *m_cdrFlag;

    //##ModelId=4850D1E90280
    float m_errorRate;

    //##ModelId=4850D1F6004E
    PpLog *m_PpLog_;

    //##ModelId=4850D20301C5
    bool m_isCheckCdrNo;

    //##ModelId=4850D20301C6
    bool m_isSpliteLongCdr;

    FieldInfos *m_pFieldInfos;

    FieldInfo  *m_pField_ErrCode;   
    FieldInfo  *m_pField_CallType;
    FieldInfo  *m_pField_RecordType;

};



#endif /* OUTPUTFILE_H_HEADER_INCLUDED_B7A65B5B */
