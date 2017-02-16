#include "MwsvcConf.h"

MwsvcConf::MwsvcConf()
{
}

MwsvcConf::~MwsvcConf()
{
    xmlApi.close();
}

bool MwsvcConf::Init(const string &szXmlFileName)
{
    XmlFileName = szXmlFileName;
    
    if(xmlApi.open(XmlFileName)==false)
    {
       //cout << "ERROR:open file:" << XmlFileName << endl;
       return false;
    }
    
    return true;
}

bool MwsvcConf::getValue(const string &xpath,string &value)
{
    multimap<string,string> resultSet;
    
    if(xmlApi.getXPathResultSet(xpath.c_str(),resultSet)==false)
    {
        //cout << "ERROR:can't get!" << endl;
        return false;
    }
    
    multimap<string,string>::iterator iter = resultSet.begin();//取第一个的值
    
    value = (*iter).second;
    
    return true;
}

bool MwsvcConf::getValue(const string &xpath,multimap<string,string> &resultSet)
{
    if(xmlApi.getXPathResultSet(xpath.c_str(),resultSet)==false)
    {
        //cout << "ERROR:can't get!" << endl;
        return false;
    }
    
    return true;
}

bool MwsvcConf::ParserMwsvcConfXmlFile()
{
    try
    {
		string tempXpath = "/MWSVC/CONTACT/NAME/text()";
		if(!getValue(tempXpath, contact.Name))
		{
			//cout << "获取基本信息Name出错!" << endl;
			return false;
		}

		tempXpath = "/MWSVC/CONTACT/DESC/text()";
		if(!getValue(tempXpath, contact.Desc))
		{
			//cout << "获取基本信息Name出错!" << endl;
			return false;
		}

		tempXpath = "/MWSVC/CONTACT/VALUE/text()";
		if(!getValue(tempXpath, contact.Value))
		{
			//cout << "获取基本信息Name出错!" << endl;
			return false;
		}

		multimap<string,string> tempColumnSet;
		int countColumn = 0;

		tempXpath = "/MWSVC/CONTACT/INMODE/descendant-or-self::*";
		if(getValue(tempXpath,tempColumnSet))
		{
			countColumn = tempColumnSet.size()/4;
		}
		else
		{
			//cout << "取对帐文件首行记录数出错!" << endl;
			return false;
		}
		for(int h=0;h<countColumn;h++)
		{
			Row hRow;
			tempXpath = "/MWSVC/CONTACT/INMODE/Column[" + convert<string>(h+1) + "]/NAME";
			if(!getValue(tempXpath,hRow.Name))
			{
				//cout << "获取字段名称（Name）出错!" << endl;
				return false;
			}
			tempXpath = "/MWSVC/CONTACT/INMODE/Column[" + convert<string>(h+1) + "]/DESC";
			if(!getValue(tempXpath,hRow.Desc))
			{
				//cout << "获取字段描述出错!" << endl;
				return false;
			}
			tempXpath = "/MWSVC/CONTACT/INMODE/Column[" + convert<string>(h+1) + "]/VALUE";
			if(!getValue(tempXpath,hRow.Value))
			{
				//cout << "获取字段值出错!" << endl;
				return false;
			}
			contact.Inmode.insert(map<string, Row>::value_type(hRow.Name, hRow));

			//print(hRow);
		}

		tempXpath = "/MWSVC/CONTACT/EPARCHYCODE/descendant-or-self::*";
		if(getValue(tempXpath,tempColumnSet))
		{
			countColumn = tempColumnSet.size()/4;
		}
		else
		{
			//cout << "取对帐文件首行记录数出错!" << endl;
			return false;
		}
		for(int h=0;h<countColumn;h++)
		{
			Row hRow;
			tempXpath = "/MWSVC/CONTACT/EPARCHYCODE/Column[" + convert<string>(h+1) + "]/NAME";
			if(!getValue(tempXpath,hRow.Name))
			{
				//cout << "获取字段名称（Name）出错!" << endl;
				return false;
			}
			tempXpath = "/MWSVC/CONTACT/EPARCHYCODE/Column[" + convert<string>(h+1) + "]/DESC";
			if(!getValue(tempXpath,hRow.Desc))
			{
				//cout << "获取字段描述出错!" << endl;
				return false;
			}
			tempXpath = "/MWSVC/CONTACT/EPARCHYCODE/Column[" + convert<string>(h+1) + "]/VALUE";
			if(!getValue(tempXpath,hRow.Value))
			{
				//cout << "获取字段值出错!" << endl;
				return false;
			}
			contact.Eparchycode.insert(map<string, Row>::value_type(hRow.Name, hRow));

			//print(hRow);
		}

		tempXpath = "/MWSVC/CONTACT/SERVICE/descendant-or-self::*";	
		if(getValue(tempXpath,tempColumnSet))
		{
			countColumn = tempColumnSet.size()/4;
		}
		else
		{
			//cout << "取对帐文件首行记录数出错!" << endl;
			return false;
		}

		for(int h=0;h<countColumn;h++)
		{
			Row hRow;
			tempXpath = "/MWSVC/CONTACT/SERVICE/Column[" + convert<string>(h+1) + "]/NAME";
			if(!getValue(tempXpath,hRow.Name))
			{
				//cout << "获取字段名称（Name）出错!" << endl;
				return false;
			}

			tempXpath = "/MWSVC/CONTACT/SERVICE/Column[" + convert<string>(h+1) + "]/DESC";
			if(!getValue(tempXpath,hRow.Desc))
			{
				//cout << "获取字段描述出错!" << endl;
				return false;
			}

			tempXpath = "/MWSVC/CONTACT/SERVICE/Column[" + convert<string>(h+1) + "]/VALUE";
			if(!getValue(tempXpath,hRow.Value))
			{
				//cout << "获取字段值出错!" << endl;
				return false;
			}
			contact.Service.insert(map<string, Row>::value_type(hRow.Name, hRow));

			//print(hRow);
		}

		tempXpath = "/MWSVC/BUSIAPI/descendant-or-self::*";	
		if(getValue(tempXpath,tempColumnSet))
		{
			countColumn = tempColumnSet.size()/4;
		}
		else
		{
			//cout << "取对帐文件首行记录数出错!" << endl;
			return false;
		}

		for(int h=0;h<countColumn;h++)
		{
			LibApi hLibApi;
			tempXpath = "/MWSVC/BUSIAPI/Column[" + convert<string>(h+1) + "]/FUNC";
			if(!getValue(tempXpath,hLibApi.Func))
			{
				//cout << "获取字段名称（Name）出错!" << endl;
				return false;
			}

			tempXpath = "/MWSVC/BUSIAPI/Column[" + convert<string>(h+1) + "]/LIB";
			if(!getValue(tempXpath,hLibApi.Lib))
			{
				//cout << "获取字段描述出错!" << endl;
				return false;
			}

			tempXpath = "/MWSVC/BUSIAPI/Column[" + convert<string>(h+1) + "]/API";
			if(!getValue(tempXpath,hLibApi.Api))
			{
				//cout << "获取字段值出错!" << endl;
				return false;
			}
			busiapi.Api.insert(map<string, LibApi>::value_type(hLibApi.Func, hLibApi));			

			//print(hLibApi);
		}
	}
	catch(...)
	{
	    //cout << "Parser Error!" << endl;
	    return false;
	}
	
	return true;
}


/* 接触管理 功能函数 */

bool MwsvcConf::chkPrintTag()
{
	if(contact.Value.compare("Y")==0)
		return true;
	else
		return false;

}

bool MwsvcConf::chkInmodePrintTag(string inmode)
{
	CIT cit = contact.Inmode.find(inmode);
	if (cit==contact.Inmode.end())
	{
		cout << "sorry, no such key" << endl;
		return false;
	}
	else
	{
		string strTag = (cit->second).Value;
		if(strTag.compare("Y")==0)
			return true;
		else
			return false;
	}
}

bool MwsvcConf::chkEparchyPrintTag(string eparchy)
{
	CIT cit = contact.Eparchycode.find(eparchy);
	if (cit==contact.Eparchycode.end())
	{
		cout << "sorry, no such key" << endl;
		return false;
	}
	else
	{
		string strTag = (cit->second).Value;
		if(strTag.compare("Y")==0)
			return true;
		else
			return false;
	}
}

bool MwsvcConf::chkServicePrintTag(string service)
{
	CIT cit = contact.Service.find(service);
	if (cit==contact.Service.end())
	{
		cout << "sorry, no such key" << endl;
		return NULL;
	}
	else
	{
		string strTag = (cit->second).Value;
		if(strTag.compare("Y")==0)
			return true;
		else
			return false;
	}
}

/* 业务层模块儿 功能函数 */
//这两个函数可以通过指针函数返回两个值的方法来合并成一个函数，
string MwsvcConf::getLibName(string func)
{
	CILA cila =  busiapi.Api.find(func);
	if (cila==busiapi.Api.end())
	{
		cout << "sorry, no such key" << endl;
		return NULL;
	}
	else
	{
		return (cila->second).Lib;
	}
}

string MwsvcConf::getApiName(string func)
{
	CILA cila =  busiapi.Api.find(func);
	if (cila==busiapi.Api.end())
	{
		cout << "sorry, no such key" << endl;
		return NULL;
	}
	else
	{
		return (cila->second).Api;
	}
}


/* 打印配置信息 */

void MwsvcConf::print()
{
	print(contact);
	print(busiapi);
}


/* "接触管理"配置信息打印函数 */

void MwsvcConf::print(Row& row)
{
	cout << "   Row Begin" << endl;
    
    cout << "       Row.Name=[" << row.Name << "]" << endl;
    cout << "       Row.Desc=[" << row.Desc << "]" << endl;
	cout << "       Row.Value=[" << row.Value << "]" << endl;
    
    cout << "   Row End" << endl;
    cout << endl;
}

void MwsvcConf::print(map<string, Row>& mRow)
{

	map<string, Row>::iterator it;   
    for(it = mRow.begin(); it != mRow.end(); it++)   
	{
		cout<<"it->first="<<it->first<<endl;
		print(it->second);
	}
}

void MwsvcConf::print(Contact& contact)
{
	cout << "   Row Name : "<< contact.Name<< endl;
	cout << "   Row Desc : "<< contact.Desc<< endl;
	cout << "   Row Value : "<< contact.Value<< endl;

	print(contact.Inmode);
	print(contact.Eparchycode);
	print(contact.Service);
}

/* "业务层功能"配置信息打印函数 */

void MwsvcConf::print(LibApi& libapi)
{
	cout << "   LibApi Begin" << endl;
    
    cout << "       Row.Name=[" << libapi.Func << "]" << endl;
    cout << "       Row.Desc=[" << libapi.Lib << "]" << endl;
	cout << "       Row.Value=[" << libapi.Api << "]" << endl;
    
    cout << "   LibApi End" << endl;
    cout << endl;
}

void MwsvcConf::print(map<string, LibApi>& mLibApi)
{

	map<string, LibApi>::iterator it;   
    for(it = mLibApi.begin(); it != mLibApi.end(); it++)   
	{
		cout<<"it->first="<<it->first<<endl;
		print(it->second);
	}
}

void MwsvcConf::print(BusiApi& busiapi)
{
	print(busiapi.Api);
}

/*	测试程序	*/
int main (int argc, char* argv[])
{
	MwsvcConf mwsvcconf;
	
	string sLogFilePath = getenv("HOME");	
	if(sLogFilePath.length()==0)
        return false;
	sLogFilePath = sLogFilePath + "/etc/MwsvcConf.xml";

	cout<<"LANG="<<getenv("LANG")<<endl;
	putenv("LANG=ZH_CN.UTF-8");
	cout<<"LANG="<<getenv("LANG")<<endl;

	mwsvcconf.Init(sLogFilePath);
	mwsvcconf.ParserMwsvcConfXmlFile();
	mwsvcconf.print();

	cout<<"LANG="<<getenv("LANG")<<endl;
	putenv("LANG=C");
	cout<<"LANG="<<getenv("LANG")<<endl;

	string strInmode = "33";
	//cout<<"strInmode="<<strInmode<<"; Value="<<mwsvcconf.getInmodePrintTag(strInmode).c_str()<<endl;

	string strEparchy = "0022";
	//cout<<"strEparchy="<<strEparchy<<"; Value="<<mwsvcconf.chkEparchyPrintTag(strEparchy).c_str()<<endl;

	string strService = "ITF_CCHQ_CheckUserPWD-";
	//cout<<"strService="<<strService<<"; Value="<<mwsvcconf.chkServicePrintTag(strService).c_str()<<endl;
}

