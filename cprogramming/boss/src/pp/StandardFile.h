// ##########################################
// Source file : StandardFile.h
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

#ifndef STANDARDFILE_H_HEADER_INCLUDED_B7A66AD2
#define STANDARDFILE_H_HEADER_INCLUDED_B7A66AD2

#include "config-all.h"

#if defined(STDIOSTREAM_DISABLE)
    #include <iostream.h>
    #include <fstream.h>
#else
    #include <iostream>
    #include <fstream>
#endif

#include <string>
#include <vector>
#include <map>
#include <stdio.h>

#include "CdrFlag.h"
#include "PpLog.h"
#include "FieldInfo.h"
#include "StandardField.h"
#include "CdrErrorCode.h"
#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;
USING_NAMESPACE(std);


const string TMP_PRIFIX ("#");
const int MAX_STDFILE_CDR_NUM (20000);

class FieldInfos;

//## 标准话单记录文件输出类
class StandardFile
{
  public:
    //##ModelId=485233970148
    StandardFile(const int iSubChannel);

    //##ModelId=48523314035B
    int write();

    //##ModelId=4852333A00CB
    //##Documentation
    //## 重置输出标准文件句柄
    bool reset(const string &outputFileName);

    //##ModelId=4852335801A5
    //##Documentation
    //## 关闭标准输出文件,将它从临时目录下删除,并移动到标准输出目录下
    void close(int flag);

    //##ModelId=48523562006D
    bool initialize(const vector<string> &rOutPutFieldRules
    					, const string &isBackup
    					, const string &outFileType
    					, const string &desPath
    					, const string &desBakPath
    					, const string &invalidPath
    					, const string &isSplit
    					, int maxCdrNum);

  protected:
    //##ModelId=485236DB0271
    //##Documentation
    //## 标准话单分隔符
    char m_token[3];

  private:
    //##ModelId=485235CC0109
    void assembleOutPutStr(int i, char *const&outFields);

    //##ModelId=4852362D0261
    bool getOutCdrWithToken(char *pOutPutStr);

    //##ModelId=4852364E03B9
    bool getOutCdrWithFix(char *pOutPutStr);

    //##ModelId=4852366B03D8
    void getNowTime(void);

    //##ModelId=485236800157
    //##Documentation
    //## 进行大小标准文件间话单转储
    bool conveyCdr(const int iFileNameSequnce, const int iFileSize);

    //##ModelId=4852369D001F
    //##Documentation
    //## 对超大文件进行拆分,即将desbak目录下的大文件,分成若干des目录下的小文件
    bool splitStdFile(void);

    //add by zhangch2  2009-12-22 拷贝文件
    bool linkFile(const string &srcPathName,const string &desPathName,const string &desFileName);

  public:
    //##ModelId=4852373B00AB
    string m_desPath;		/*目标文件路径*/

    //##ModelId=4852375503C8
    string m_FileName;		/*目标文件名*/

    //add by zhangch2 2009-12-22  标准话单输出目录容器
    vector<string> m_dispenPath;

  private:
    //##ModelId=485236F20167
    FILE *m_fDes;			/*目标文件句柄*/

    //##ModelId=48523706004E
    FILE *m_fInvalid;		/*非法文件句柄*/

    //##ModelId=4852371C0203
    char m_desBuffer[1024*4];		/*目标文件缓存*/

    //##ModelId=4852372801A5
    char m_invalidBuffer[1024*4];/*非法文件缓存*/

    //##ModelId=4852373103A9
    string m_desBakPath;			/*目标文件备份路径*/

    //##ModelId=48523745032C
    string m_invalidPath;			/*非法文件路径*/

    //##ModelId=48523761031C
    bool m_isBackupDesFile;		/*备份目标文件标志*/

    //##ModelId=48523770036B
    CdrFlag *m_cdrFlag;			/*话单记录标志类对象*/

    //##ModelId=4852377D01D4
    PpLog *m_PpLog;				/*日志对象*/

    //##ModelId=4852378B0138
    vector<StandardField> m_StandardField;	/*输出字段集合*/	

    //##ModelId=4852379D02DE
    float m_errorRate;			/*最大错误率*/

    //##ModelId=485237A9007D
    string m_outFileType;		/*输出文件类型*/

    //##ModelId=485237B400AB
    char m_nowTime[11];

    //##ModelId=485237D3497B
    int m_maxCdrNum;			/*最大话单条数*/

    //##ModelId=485237D3497C
    bool m_isSplitDes;			/*是否切割目标文件标识*/
    
    FieldInfos *m_pFieldInfos;
    FieldInfo  *m_pField_ErrCode;   
    FieldInfo  *m_pFieldStartDate;
    FieldInfo  *m_pFieldStartTime;
    
    int m_iSeq_CALL_TOTAL_FEE; 
};



#endif /* STANDARDFILE_H_HEADER_INCLUDED_B7A66AD2 */
