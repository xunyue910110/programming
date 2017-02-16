#ifndef _CDRRELCOMMON_H_
#define _CDRRELCOMMON_H_

#include "base/Types.h"
#include "base/SysParam.h"
#include "base/StringUtil.h"    

const int LINEMAXLEN  = 1024*8;//一条话单最大长度

class FileType
{
  public :
    void reset(){m_filePrefix.clear();};
  public :                             
    string m_fileTypeName;
    vector<string> m_filePrefix;
    int m_msisdnIndex;
    int m_beginDateIndex;
    int m_beginTimeIndex;
    int m_key1Index;
    int m_key2Index;
    int m_key3Index;
    int m_key4Index;
    int m_key5Index;
    string m_table;
    string m_procedure;
};

class CdrRelConfig
{
  public :
    CdrRelConfig(){};
    ~CdrRelConfig(){};
    bool getSysParam(SysParam *r_sysParam,const char * r_appName,const int &r_channelNo);
    bool getFileType(const string fileName, FileType &fileType);
  public :
    string m_dbUser;
    string m_dbPassWord;
    string m_dbServerName;
    string m_inputPath;
    string m_backupPath;
    string m_outputPath;
    string m_errorPath;
    vector<FileType> m_fileType;
    friend ostream& operator<<(ostream& os,const CdrRelConfig& po);
};

#endif //_CDRRELCOMMON_H_
