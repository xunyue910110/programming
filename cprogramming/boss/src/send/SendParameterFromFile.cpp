// ##########################################
// Source file : SendParameterFromFile.cpp
// System      : Mobile Billing System
// Version     : 0.1.36
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010731
// Update      : 20011021
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################


#include "SendLog.h"
#include "SendParameterFromFile.h"

//##ModelId=3BD42C9C0223
SendParameterFromFile::SendParameterFromFile(string configFilePath,
										 string configFileName,string theDataSource)
{
	m_ConfigFileName_	= configFileName;
	m_ConfigPath_		= configFilePath;
	m_DataSource_		= theDataSource;
	m_SendLog_			= SendLog::getInstance();
}

//##ModelId=3B6FF60D0131
SendParameterFromFile::~SendParameterFromFile()
{
}

//##ModelId=3B7B98480220
int SendParameterFromFile::getDataSourceConfig()
{
	int iLen;
	string SwitchCfgName;
	vector <string> tmp;

	m_ConfigFileName_ 	= m_ConfigPath_ + m_ConfigFileName_;
	iLen = setParameterString(m_ConfigFileName_,tmp);
	if ( iLen == -1 )
		return -1;

	if (getSectionList(m_DataSource_,tmp,m_SendParameterString_)!=true)
		return -1;

	getValue("switch_name", SwitchCfgName);
	SwitchCfgName = m_ConfigPath_ + SwitchCfgName + string(".cfg");

	iLen = setParameterString(SwitchCfgName,m_SendParameterString_);
	if ( iLen == -1 )
		return -1;

	return 0;
}

//##ModelId=3BD42E0B00B8
int SendParameterFromFile::getDataSourceConfig(string theSwitchName)
{
	int iLen;

	iLen = setParameterString(theSwitchName,m_SendParameterString_);
	if ( iLen == -1 )
		return -1;

	return 0;
}
//##ModelId=3B7BDD480158
int SendParameterFromFile::setParameterString(string ConfigFileName,
											vector <string> &theParameterString)
{
	int ilen,i=0,x;
	string tmp;
	ifstream iCfgFile;

	iCfgFile.open( ConfigFileName.c_str() );
	if ( iCfgFile.fail() ) {
		m_SendLog_->ShowMessage( "Open "+ConfigFileName+" ERROR!\n" , 0 );
		return -1;
	}

	while ( !iCfgFile.eof() ) 
	{
		getline(iCfgFile,tmp);				//读取一行
		tmp=lTrim(tmp);
		if ( tmp.substr(0,1)==string("#") || tmp.length()==0 )
			continue;
		ilen = tmp.find("#");
		x = tmp.find("\\#");
		if ( ilen != -1 && x==-1 )
			tmp=tmp.substr(0,ilen);
		tmp=lTrim(tmp);
		if (tmp.size()!=0)
			theParameterString.push_back(tmp);
		i++;
	}
	iCfgFile.close();
	return i-1;
}

//====================================================================

//##ModelId=3B8E03D800D9
bool SendParameterFromFile::getValue(const string name, string &value)
{
	int nameLen,valueLen;

	value=string("");
	nameLen=name.size();

	for (int i=0;i<m_SendParameterString_.size();i++)
	{
		if ( m_SendParameterString_[i].substr(0,nameLen)==name ) 
		{
			valueLen	= m_SendParameterString_[i].size() - nameLen;
			value		= m_SendParameterString_[i].substr(nameLen,valueLen);
			break;
		}
	}
	value=lTrim(value);
	if (value.size()<=0)
		return false;
	else
		return true;
}

//##ModelId=3B8E03D9012B
bool SendParameterFromFile::getValue(const string section, const string name, string &value)
{
	int nameLen,valueLen;
	vector <string> sectionList;
	vector <string> :: iterator iter;

	value=string("");
	nameLen=name.size();

	getSectionList(section,m_SendParameterString_,sectionList);

	for ( iter=sectionList.begin(); iter!=sectionList.end(); iter++ )
	{
		if ( (*iter).substr(0,nameLen)==name ) 
		{
			valueLen=(*iter).size() - nameLen;
			value=(*iter).substr(nameLen,valueLen);
			break;
		}
	}
	value=lTrim(value);
	if (value.size()<=0)
		return false;
	else
		return true;
}

//##ModelId=3B8E03DA0082
bool SendParameterFromFile::getSectionList(const string section,
							  vector <string> aSendParameterString,
							  vector <string> &theSectionList)
{
	int endSectionLen;
	bool sectionFlag=false;
	string startSection,endSection;
	vector <string> :: iterator iter;

	startSection=string("<")+section+string(">");
	endSection=string("</")+section+string(">");
	endSectionLen=endSection.size();


	for ( iter=aSendParameterString.begin(); iter!=aSendParameterString.end(); iter++ )
	{
		if (endSection == *iter )
			break;
		if (sectionFlag==true)
			theSectionList.push_back(*iter);

		if (startSection == *iter && sectionFlag == false)
			sectionFlag=true;
	}
	return true;
}

//##ModelId=3B8E03DB000B
bool SendParameterFromFile::getSectionList(const string section,vector <string> &theSectionList)
{
	return getSectionList(section,m_SendParameterString_,theSectionList);
}

//##ModelId=3B7CC76202A4
bool SendParameterFromFile::initialize()
{
	if ( getDataSourceConfig() == -1 ) 
	{
		m_SendLog_->ShowMessage( "Get parameter from file ERROR!\n" , 0 );
		return false;
	} 
	else 
	{
		m_SendLog_->ShowMessage( "Get parameter from file ok.\n" , 0 );
		return true;
	}
}

//##ModelId=3BD42C9C02B9
bool SendParameterFromFile::initialize(string switchName,vector <string> &pathMessage)
{
	for(int i=0;i<pathMessage.size();i++)
	{
	   m_SendParameterString_.push_back(pathMessage[i]);
	}

	if ( getDataSourceConfig(switchName) == -1 ) 
	{
		m_SendLog_->ShowMessage( "Get parameter from file ERROR!\n" , 0 );
		return false;
	} 
	else 
	{
		m_SendLog_->ShowMessage( "Get parameter from file ok.\n" , 0 );
		return true;
	}
}

//##ModelId=3B8E03DB035E
bool SendParameterFromFile::end()
{
	return true;
}

//##ModelId=3B8E03DB03E1
string SendParameterFromFile::lTrim(string TheString)
{
	int len,size;

	size	= TheString.size();
	len		= TheString.find_first_not_of(" \t");
	if(len>0)
		TheString	= TheString.substr(len,size-len);	// 去掉头部空格和Tab

	len		= TheString.find_last_not_of(" \t");
	if(len>0)
		TheString	= TheString.substr(0,len+1);		// 去掉尾部空格和Tab

	return TheString;
}



