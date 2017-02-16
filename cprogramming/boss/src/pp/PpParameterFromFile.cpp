// ##########################################
// Source file : PpParameterFromFile.cpp
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

#include "PpLog.h"
#include "PpParameterFromFile.h"

//##ModelId=4850D5C403C8
PpParameterFromFile::PpParameterFromFile(const string &configFilePath,
                                        const string &configFileName, const string &theDataSource)
{
    m_ConfigFileName_   = configFileName;
    m_ConfigPath_       = configFilePath;
    m_DataSource_       = theDataSource;
    m_PpLog_            = PpLog::getInstance();
}

//##ModelId=4850D5F8001F
//PpParameterFromFile::PpParameterFromFile(const string configFilePath, const string configFileName, int theDataSource)
//{
//}

//##ModelId=4850D6150271
PpParameterFromFile::~PpParameterFromFile()
{
}

//##ModelId=4850D63702DE
bool PpParameterFromFile::initialize()
{
    if (getDataSourceConfig() == -1) 
	{
		m_PpLog_->ShowMessage("Get parameter from file ERROR!\n", 0);
		return false;
	} 
	else 
	{
		m_PpLog_->ShowMessage("Get parameter from file ok.\n", 0);
		return true;
	}
}

//##ModelId=4850D6480232
bool PpParameterFromFile::initialize(const string &switchName, const vector <string> &pathMessage)
{
    for(int i = 0; i < pathMessage.size(); i++)
	{
	   m_PpParameterString_.push_back(pathMessage[i]);
	}

	if (getDataSourceConfig(switchName) == -1) 
	{
		m_PpLog_->ShowMessage("Get parameter from file ERROR!\n", 0);
		return false;
	} 
	else 
	{
		m_PpLog_->ShowMessage("Get parameter from file ok.\n", 0);
		return true;
	}
}

//##ModelId=4850D66502EE
bool PpParameterFromFile::getValue(const string name, string &value)
{
    int nameLen, valueLen;

	value = string("");
	nameLen = name.size();
	for (int i = 0; i < m_PpParameterString_.size(); i++)
	{
		if (m_PpParameterString_[i].substr(0, nameLen) == name)
		{
			valueLen = m_PpParameterString_[i].size() - nameLen;
			value    = m_PpParameterString_[i].substr(nameLen, valueLen);
			break;
		}
	}
	value = lTrim(value);
	if (value.size() <= 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//##ModelId=4850D68C00FA
bool PpParameterFromFile::getValue(const string section, const string name, string &value)
{
    int nameLen, valueLen;
	vector <string> sectionList;
	vector <string> :: iterator iter;

	value = string("");
	nameLen = name.size();

	getSectionList(section, m_PpParameterString_, sectionList);
	for (iter = sectionList.begin(); iter != sectionList.end(); iter++)
	{
		if ((*iter).substr(0, nameLen) == name) 
		{
			valueLen = (*iter).size() - nameLen;
			value = (*iter).substr(nameLen,valueLen);
			break;
		}
	}
	value = lTrim(value);
	if (value.size() <= 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//##ModelId=4850D6BC0213
bool PpParameterFromFile::getSectionList(const string section,
                                        vector <string> aPpParameterString,
                                        vector <string> &theSectionList)
{
    int endSectionLen;
	bool sectionFlag = false;
	string startSection, endSection;
	vector <string> :: iterator iter;
	int i;

    startSection=string("<")+section+string(">");
	endSection=string("</")+section+string(">");
	endSectionLen=endSection.size();

	for (iter = aPpParameterString.begin(); iter != aPpParameterString.end(); iter++)
	{
	    
		if (endSection == *iter)
		{
			break;
		}
		if (sectionFlag == true)
		{   
			theSectionList.push_back(*iter);
		}

		if (startSection == *iter && sectionFlag == false)
		{
			sectionFlag = true;
		}
	}
	return true;
}

//##ModelId=4850D70D0138
bool PpParameterFromFile::getSectionList(const string section, vector <string> &theSectionList)
{
    return getSectionList(section, m_PpParameterString_, theSectionList);
}

//##ModelId=4850D7290157
bool PpParameterFromFile::end()
{
    return true;
}

//##ModelId=4850D735032C
string PpParameterFromFile::lTrim(string TheString)
{
    int len,size;

	size = TheString.size();
	len  = TheString.find_first_not_of(" \t");
	if(len > 0)
	{
		TheString	= TheString.substr(len, size - len);        // 去掉头部空格和Tab
	}

	len = TheString.find_last_not_of(" \t");
	if(len > 0)
	{
		TheString = TheString.substr(0, len + 1);               // 去掉尾部空格和Tab
	}

	return TheString;
}

//##ModelId=4850D74E0261
int PpParameterFromFile::getDataSourceConfig()
{
    int iLen;
	string SwitchCfgName;
	vector <string> tmp;

	m_ConfigFileName_ = m_ConfigPath_ + m_ConfigFileName_;
	iLen = setParameterString(m_ConfigFileName_, tmp);     //tmp中存放的是通道配置文件的每一行信息，返回值为实际读取的有效行数
	if (iLen == -1)
	{
		return -1;
	}

	if (getSectionList(m_DataSource_, tmp, m_PpParameterString_) != true) //m_PpParameterString_存放具体通道的每一行信息
	{
		return -1;
	}
	
	getValue("switch_name", SwitchCfgName);    //SwitchCfgName=具体交换机的名称
	SwitchCfgName  = m_ConfigPath_ + SwitchCfgName + string(".cfg"); //得到交换机配置文件（带路径）

	iLen = setParameterString(SwitchCfgName, m_PpParameterString_);//m_PpParameterString_接着存放具体交换机配置文件的每一行信息

	if (iLen == -1)
	{
		return -1;
    }
	return 0;
}

//##ModelId=4850D75903D8
int PpParameterFromFile::getDataSourceConfig(string theSwitchName)
{
    int iLen;

	iLen = setParameterString(theSwitchName, m_PpParameterString_);
	if (iLen == -1)
	{
		return -1;
    }
	return 0;
}

//##ModelId=4850D7750186
int PpParameterFromFile::setParameterString(string ConfigFileName, vector <string> &theParameterString)
{
    int ilen, i = 0, x;
	string tmp;
	ifstream iCfgFile;

	iCfgFile.open(ConfigFileName.c_str());
	if (iCfgFile.fail())
	{
		m_PpLog_->ShowMessage("Open " + ConfigFileName + " ERROR!\n" , 0);
		return -1;
	}

	while (!iCfgFile.eof()) 
	{
		getline(iCfgFile, tmp);                //读取一行
		tmp = lTrim(tmp);                      //去掉前后空格和tab
		if (tmp.substr(0,1) == string("#") || tmp.length() == 0)  //判断是否为注释或空行
		{
			continue;
		}
		ilen = tmp.find("#");
		x = tmp.find("\\#");
		if (ilen != -1 && x == -1)
		{
			tmp = tmp.substr(0, ilen);
		}
		tmp = lTrim(tmp);
		if (tmp.size() != 0)
		{
			theParameterString.push_back(tmp);
		}
		i++;
	}
	iCfgFile.close();
	return i - 1;
}

