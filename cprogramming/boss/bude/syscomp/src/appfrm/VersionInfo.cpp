
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <memory.h>
#include <errno.h>
#include <sys/stat.h>


#ifdef WIN32_OS_CONFIG  // for _getpid()
#include <process.h>
#else
#include <unistd.h>
#endif

#include <fstream>
#include <strstream>
#include <iostream>

#include "VersionInfo.h"


VersionInfo::VersionInfo() : m_provider("NLC"), m_province("025"),
                             m_name("YJFXT"), m_safelevel("D1"),    
                             m_prefix("V"), m_main(1), m_mini(1),
                             m_patch(0), m_build(1),
                             m_appname("BossBilling"),
                             m_date("2005/01/05"),
                             m_author("Linkage Boss Billing System Development Team"),
                             m_declare("Linkage All right reserved."),
                             m_comment("Default Version Information.")
{
}


const VersionInfo& VersionInfo::operator=(const VersionInfo &l)
{
    m_provider  = l.m_provider;          
    m_province  = l.m_province;        
    m_name      = l.m_name;           
    m_safelevel = l.m_safelevel;       
    m_prefix    = l.m_prefix;
    m_main      = l.m_main;
    m_mini      = l.m_mini;
    m_patch     = l.m_patch;
    m_build     = l.m_build;
    m_appname   = l.m_appname;
    m_date      = l.m_date;
    m_author    = l.m_author;
    m_declare   = l.m_declare;
    m_comment   = l.m_comment;
    return *this;
}


ostream& operator<<(ostream& os, const VersionInfo& a)
{
    os << "System Name: "
            << a.m_provider << "."
            << a.m_province << "."
            << a.m_name << "."
            << a.m_safelevel<<endl;
    os << "-------------------------------------------------" << endl;             
       
    os << "Version    : "
     << a.m_prefix;
    os.width(2);
    os.fill('0');
    os << a.m_main << ".";
    os.width(3);
    os << a.m_mini << ".";
    os.width(3);
    os << a.m_patch
     << endl;
    os << "-------------------------------------------------" << endl;
    os << "build  : " << a.m_build << endl
     << "update : " << a.m_date << endl
     << "comment: " << a.m_comment << endl
     << a.m_appname << endl
     << a.m_author << endl
     << a.m_declare;
    return os;
}


