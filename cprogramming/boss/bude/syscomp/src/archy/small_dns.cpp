#ifdef _MBCS
#pragma warning( disable : 4786)
#endif

#include "archy_config.hpp"
USING_STD_NAMESPACE;

#include "small_dns.hpp"
#ifndef WIN32
	#include <sys/utsname.h>
#endif
#include "ustr.hpp"
#include "debug.hpp"
#include <errno.h>
#include "tcp.hpp"
#include "file.hpp"
BEGIN_NAMESPACE_ARCHY


CSmallDNS::CSmallDNS(const string& local_hosts_file /* = "./hosts.ini" */)
{
	archy::file f;
	int ret = f.open(local_hosts_file, "r");
	
	if (ret < 0 ) {
		ARCHY_CERR << "CSmallDNS: cannot open file: " << local_hosts_file << endl;
		return;
	}

    string s;
    while (1) {
    		ret = f.read_line(s);
    		if( ret <= 0 ) return;

		UStr::Trim(s);
		if( s.empty() || s[0] == '#')
			continue;
		list<string> l;
		UStr::Split(s, " ", l);
		if( l.size() >= 2 && IsValidIP(l.front()) ) {
			list<string>::const_iterator it2, it1 = l.begin();
			it2 = it1;
			++it2;
			m_map[*it1] = *it2;
			m_map[*it2] = *it1;
		}
	}

   
}
    

// static
bool CSmallDNS::IsValidIP(const string& ip)
{
    list<string> lstDig;
    
    UStr::Split(ip, ".", lstDig);
    if( lstDig.size() != 4 )
        return false;
    list<string>::iterator it = lstDig.begin();
    for(; it != lstDig.end(); ++it) {
        try {
            unsigned long i = UStr::StringToULong(*it);
            if( i > 255 )
                return false;
        } catch(...) {
            return false;
        }
    }
    return true;
}

list<string> CSmallDNS::GetLocalIPList(void)
{
	// Get host adresses 
	list<string> l;
	try {
		string sm_localHostName = GetLocalHost();
		struct hostent * pHost; 
		pHost = gethostbyname(sm_localHostName.c_str()); 
		for(int i = 0; pHost!= NULL && pHost->h_addr_list[i] != NULL; i++ ) { 
			const char* psz = inet_ntoa(*(struct in_addr *)pHost->h_addr_list[i]); 
			l.push_back(psz);
		} 
	}
	catch(...) {
	}
	return l;

}
string CSmallDNS::GetLocalIP(void) 
{
	// Get host adresses 
	try {
		string sm_localHostName = GetLocalHost();
		struct hostent * pHost; 
		pHost = gethostbyname(sm_localHostName.c_str()); 
		if( pHost!= NULL && pHost->h_addr_list[0] != NULL ) { 
			const char* psz = inet_ntoa(*(struct in_addr *)pHost->h_addr_list[0]); 
			return psz;
		} 
	}
	catch(...) {
	}
	return "";
}

// static
string CSmallDNS::GetLocalHost(void)
{
	static string sm_localHostName;
    if( sm_localHostName.empty() ) {
#ifdef WIN32
		char szHostName[128]; 
		if( gethostname(szHostName, 128) == 0 ) {
			sm_localHostName = szHostName;
		}
#else
        struct utsname unm;
        errno = 0;
        if( uname(&unm) != -1 ) {
            sm_localHostName = unm.nodename;
        }
#endif
		else {
            throw runtime_error("CSmallDNS: Cannot detect host name, errno:");
        }
		
    }
    return sm_localHostName;
}

string CSmallDNS::LocalResolveDNS(const string& host) const
{
    if( IsValidIP(host) ) {
        return host;
    }
    
    map<string, string>::const_iterator it = m_map.find(host);
    if( it != m_map.end() ) {
        return it->second;
    }
    return "";
}

string CSmallDNS::LocalBackResolveDNS(const string& ip) const
{
    if( !IsValidIP(ip) ) {
        return "";
    }
    
    map<string, string>::const_iterator it = m_map.find(ip);
    if( it != m_map.end() ) {
        return it->second;
    }
    return "";
}

END_NAMESPACE_ARCHY
