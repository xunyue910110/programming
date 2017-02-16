// ##########################################
// Source file : RetParameterFromFile.h
// System      : Mobile Billing System
// Version     : 0.1.24
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010731
// Update      : 20011021
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef PPPARAMETERFROMFILE_H_INCLUDED_C4915E98
#define PPPARAMETERFROMFILE_H_INCLUDED_C4915E98

#include "config-all.h"
USING_NAMESPACE(std)

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <vector>
#include <string>
#include "RetLog.h"

//##ModelId=3B7B97BF015B
//##Documentation
//## 从配置文件中获取参数
class RetParameterFromFile
{
  public:
	//##ModelId=3B7B9847037D
	RetParameterFromFile(string configFilePath, string configFileName, string theDataSource);

	//##ModelId=3BD42C9C0223
   RetParameterFromFile(string configFilePath, string configFileName, int theDataSource);

	//##ModelId=3B7B984801DA
	~RetParameterFromFile();

	//##ModelId=3B7CC76202A4
    virtual bool initialize();

	//##ModelId=3BD42C9C02B9
    virtual bool initialize(string switchName,vector <string> &pathMessage);

	//##ModelId=3B8E03D800D9
    bool getValue(const string name, string &value);

	//##ModelId=3B8E03D9012B
    bool getValue(const string section, const string name, string &value);

	//##ModelId=3B8E03DA0082
	bool getSectionList(const string section,
						vector <string> aRetParameterString,
						vector <string> &theSectionList);

	//##ModelId=3B8E03DB000B
	bool getSectionList(const string section,vector <string> &theSectionList);

	//##ModelId=3B8E03DB035E
    bool end();

	//##ModelId=3B8E03DB03E1
    string lTrim(string TheString);

  private:

	//##ModelId=3B7B98480220
	//##Documentation
	//## 用于读取数据源配置文件的全部内容，存到RetParameterString容器中。
	int getDataSourceConfig();

	//##ModelId=3BD42E0B00B8
	int getDataSourceConfig(string theSwitchName);

	//##ModelId=3B7BDD480158
	int setParameterString(string ConfigFileName,
						   vector <string> &theParameterString);

  private:
	//##ModelId=3B7B9847014D
	//##Documentation
	//## 数据源配置文件名称
	string m_ConfigFileName_;

	//##ModelId=3B7B98470215
	//##Documentation
	//## 配置文件路径
	string m_ConfigPath_;

	//##ModelId=3B7B984702A1
	//##Documentation
	//## 预处理数据源ID
	string m_DataSource_;

	//##ModelId=3B7CBF4700B6
	RetLog *m_RetLog_;

	//##ModelId=3B8E0E0E0245
	vector<string> m_RetParameterString_;

};

#endif /* PpPARAMETERFROMFILE_H_INCLUDED_C4915E98 */
