// ##########################################
// Source file : RetParameterFromFile.cpp
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


#include "RetLog.h"
#include "RetParameterFromFile.h"

//##ModelId=3BD42C9C0223
RetParameterFromFile::RetParameterFromFile(string configFilePath,
										 string configFileName,string theDataSource)
{
	m_ConfigFileName_	= configFileName;
	m_ConfigPath_		= configFilePath;
	m_DataSource_		= theDataSource;
	m_RetLog_			= RetLog::getInstance();
}

//##ModelId=3B6FF60D0131
RetParameterFromFile::~RetParameterFromFile()
{
}

//##ModelId=3B7B98480220
int RetParameterFromFile::getDataSourceConfig()
{
	int iLen;
	string SwitchCfgName;
	vector <string> tmp;

	m_ConfigFileName_ 	= m_ConfigPath_ + m_ConfigFileName_;
	iLen = setParameterString(m_ConfigFileName_,tmp);
	if ( iLen == -1 )
		return -1;

	if (getSectionList(m_DataSource_,tmp,m_RetParameterString_)!=true)
		return -1;

	getValue("switch_name", SwitchCfgName);
	SwitchCfgName = m_ConfigPath_ + SwitchCfgName + string(".cfg");

	iLen = setParameterString(SwitchCfgName,m_RetParameterString_);
	if ( iLen == -1 )
		return -1;

	return 0;
}

//##ModelId=3BD42E0B00B8
int RetParameterFromFile::getDataSourceConfig(string theSwitchName)
{
	int iLen;

	iLen = setParameterString(theSwitchName,m_RetParameterString_);
	if ( iLen == -1 )
		return -1;

	return 0;
}
//##ModelId=3B7BDD480158
int RetParameterFromFile::setParameterString(string ConfigFileName,
											vector <string> &theParameterString)
{
	int ilen,i=0,x;
	string tmp;
	ifstream iCfgFile;

	iCfgFile.open( ConfigFileName.c_str() );
	if ( iCfgFile.fail() ) {
		m_RetLog_->ShowMessage( "Open "+ConfigFileName+" ERROR!\n" , 0 );
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
bool RetParameterFromFile::getValue(const string name, string &value)
{
	int nameLen,valueLen;

	value=string("");
	nameLen=name.size();

	for (int i=0;i<m_RetParameterString_.size();i++)
	{
		if ( m_RetParameterString_[i].substr(0,nameLen)==name ) 
		{
			valueLen	= m_RetParameterString_[i].size() - nameLen;
			value		= m_RetParameterString_[i].substr(nameLen,valueLen);
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
bool RetParameterFromFile::getValue(const string section, const string name, string &value)
{
	int nameLen,valueLen;
	vector <string> sectionList;
	vector <string> :: iterator iter;

	value=string("");
	nameLen=name.size();

	getSectionList(section,m_RetParameterString_,sectionList);

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
bool RetParameterFromFile::getSectionList(const string section,
							  vector <string> aRetParameterString,
							  vector <string> &theSectionList)
{
	int endSectionLen;
	bool sectionFlag=false;
	string startSection,endSection;
	vector <string> :: iterator iter;

	startSection=string("<")+section+string(">");
	endSection=string("</")+section+string(">");
	endSectionLen=endSection.size();


	for ( iter=aRetParameterString.begin(); iter!=aRetParameterString.end(); iter++ )
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
bool RetParameterFromFile::getSectionList(const string section,vector <string> &theSectionList)
{
	return getSectionList(section,m_RetParameterString_,theSectionList);
}

//##ModelId=3B7CC76202A4
bool RetParameterFromFile::initialize()
{
	if ( getDataSourceConfig() == -1 ) 
	{
		m_RetLog_->ShowMessage( "Get parameter from file ERROR!\n" , 0 );
		return false;
	} 
	else 
	{
		m_RetLog_->ShowMessage( "Get parameter from file ok.\n" , 0 );
		return true;
	}
}

//##ModelId=3BD42C9C02B9
bool RetParameterFromFile::initialize(string switchName,vector <string> &pathMessage)
{
	for(int i=0;i<pathMessage.size();i++)
	{
	   m_RetParameterString_.push_back(pathMessage[i]);
	}

	if ( getDataSourceConfig(switchName) == -1 ) 
	{
		m_RetLog_->ShowMessage( "Get parameter from file ERROR!\n" , 0 );
		return false;
	} 
	else 
	{
		m_RetLog_->ShowMessage( "Get parameter from file ok.\n" , 0 );
		return true;
	}
}

//##ModelId=3B8E03DB035E
bool RetParameterFromFile::end()
{
	return true;
}

//##ModelId=3B8E03DB03E1
string RetParameterFromFile::lTrim(string TheString)
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



