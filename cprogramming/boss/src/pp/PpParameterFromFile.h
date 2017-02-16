// ##########################################
// Source file : PpParameterFromFile.h
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

#ifndef PPPARAMETERFROMFILE_H_HEADER_INCLUDED_B7A6332C
#define PPPARAMETERFROMFILE_H_HEADER_INCLUDED_B7A6332C

#include "PpLog.h"
#include "config-all.h"
#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <vector>
#include <string>

USING_NAMESPACE(std);

//##ModelId=4850D53C0196
//##Documentation
//## 从配置文件中获取参数
class PpParameterFromFile
{
  public:
    //##ModelId=4850D5C403C8
    PpParameterFromFile(const string &configFilePath, const string &configFileName,
                        const string &theDataSource);

    //##ModelId=4850D5F8001F
    //PpParameterFromFile(const string &configFilePath, const string &configFileName, int theDataSource);

    //##ModelId=4850D6150271
    ~PpParameterFromFile();

    //##ModelId=4850D63702DE
    virtual bool initialize();

    //##ModelId=4850D6480232
    virtual bool initialize(const string &switchName, const vector <string> &pathMessage);

    //##ModelId=4850D66502EE
    bool getValue(const string name, string &value);

    //##ModelId=4850D68C00FA
    bool getValue(const string section, const string name, string &value);

    //##ModelId=4850D6BC0213
    bool getSectionList(const string section, vector <string> aPpParameterString,
                        vector <string> &theSectionList);

    //##ModelId=4850D70D0138
    bool getSectionList(const string section, vector <string> &theSectionList);

    //##ModelId=4850D7290157
    bool end();

    //##ModelId=4850D735032C
    string lTrim(string TheString);

  private:
    //##ModelId=4850D74E0261
    //##Documentation
    //## 用于读取数据源配置文件的全部内容，存到PpParameterString容器中
    int getDataSourceConfig();

    //##ModelId=4850D75903D8
    int getDataSourceConfig(string theSwitchName);

    //##ModelId=4850D7750186
    int setParameterString(string ConfigFileName, vector <string> &theParameterString);

    //##ModelId=4850D7B001B5
    //##Documentation
    //## 数据源配置文件名称
    string m_ConfigFileName_;

    //##ModelId=4850D7BB03C8
    //##Documentation
    //## 配置文件路径
    string m_ConfigPath_;

    //##ModelId=4850D7C600EA
    //##Documentation
    //## 预处理数据源ID
    string m_DataSource_;

    //##ModelId=4850D7D4006D
    PpLog *m_PpLog_;

    //##ModelId=4850D7E50222
    vector<string> m_PpParameterString_;

};



#endif /* PPPARAMETERFROMFILE_H_HEADER_INCLUDED_B7A6332C */
