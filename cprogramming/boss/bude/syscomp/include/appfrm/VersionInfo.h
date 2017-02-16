/*
* Copyright (c) 2003,Linkage All rights reserved.
*
*/

#ifndef VERSIONINFO_H_INCLUDED
#define VERSIONINFO_H_INCLUDED

#include "base/config-all.h"
#include <string>
#include <iostream>



USING_NAMESPACE(std)


class VersionInfo
{
public:
	VersionInfo();

	string m_provider;        // default: "NLC"
	string m_province;        // default: areacode of the province, "898".
	string m_name;            // billing default: "YJFXT"
	string m_safelevel;       // default: "D1"	
	
	string m_prefix;            // default: "V"
	short  m_main;              // 0~99
	short  m_mini;              // 0~999
	short  m_patch;             // 0~999
	int    m_build;             // 0~99999
	string m_appname;           //
	string m_date;              //
	string m_author;            //
	string m_declare;           //
	string m_comment;           //

	const VersionInfo& operator=(const VersionInfo &l);
	friend ostream& operator<<(ostream& os, const VersionInfo& a);
};


#endif // VERSIONINFO_H_INCLUDED
