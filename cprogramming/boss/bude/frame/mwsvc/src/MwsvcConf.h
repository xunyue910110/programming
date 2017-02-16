/**
* <p>Title: </p>
* <p>Description:</p>
* <p>Copyright: Copyright (c) 2007</p>
* <p>Company:Linkage </p>
* <p>File:MwsvcConf.h,MwsvcConf.cpp </p>
* <p>author:Wang Hongliang </p>
* @version 1.0
*/

#ifndef MWSVCCONF_H
#define MWSVCCONF_H

#include <iostream>
#include <string>
#include <sstream>
#include "xml/XmlApi.h"
#include "BaseConvert.h"

using namespace std;

struct ContactInfo
{
	string ct_eparchycode;
	string ct_citycode;
	string ct_contacttime;
	string ct_contactmode;
	string ct_inmodecode;
	string ct_channelid;
	string ct_contacterid;
	string ct_custid;
	string ct_contacttradetype;
	string ct_contacttradeid;
	string ct_contactdesc;
	string ct_contactfinishtime;
};
typedef struct ContactInfo ContactInfo;

struct Row
{
    string Name;
    string Desc;
	string Value;
};

struct LibApi
{
    string Func;
    string Lib;
	string Api;
};

struct Contact
{
    string Name;
	string Desc;
	string Value;
    map<string, Row> Inmode;
    map<string, Row> Eparchycode;
    map<string, Row> Service;
};

struct BusiApi
{
	map<string, LibApi> Api;
};

typedef map<string, Row>::const_iterator CIT;
typedef map<string, LibApi>::const_iterator CILA;

class MwsvcConf
{
    public:
        MwsvcConf();
        ~MwsvcConf();
        
    private:
        string XmlFileName;//xml文件名
        XmlApi xmlApi;
		Contact contact;
		BusiApi busiapi;
       
    public:
        bool Init(const string &szXmlFileName);
        bool getValue(const string &xpath,string &value);
        bool getValue(const string &xpath,multimap<string,string> &resultSet);

		//解析接触信息的XML格式配置文件
		bool ParserMwsvcConfXmlFile();

		//获取接触信息生成标志
		//string getPrintTag();
		bool chkPrintTag();

		//获取指定接入渠道的接触信息生成标志
		//string getInmodePrintTag(string inmode);
		bool chkInmodePrintTag(string inmode);

		//获取指定地州的接触信息生成标志
		//string chkEparchyPrintTag(string eparchy);
		bool chkEparchyPrintTag(string eparchy);

		//获取指定Tuxedo流程的接触信息生成标志
		//string chkServicePrintTag(string service);
		bool chkServicePrintTag(string service);

		//根据“功能名”获取动态库名
		string getLibName(string func);
		
		//根据“功能名”获取API名
		string getApiName(string func);

		void print();
		void print(Row& row);
		void print(map<string, Row>& mRow);
		void print(Contact& contact);

		void print(LibApi& libapi);
		void print(map<string, LibApi>& mLibApi);
		void print(BusiApi& busiapi);
};

#endif //MWSVCCONF_H
