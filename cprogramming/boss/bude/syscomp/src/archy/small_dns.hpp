#ifndef ARCHY_SMALL_DNS_HEADER
#define ARCHY_SMALL_DNS_HEADER

#ifdef _MBCS
#pragma warning( disable : 4786)
#endif

#include <map>
#include <list>
#include <string>

#include "archy_config.hpp"
USING_STD_NAMESPACE;

BEGIN_NAMESPACE_ARCHY


class CSmallDNS
{
public:
    // creates small DNS service from the /etc/hosts-like file
    CSmallDNS(const string& local_hosts_file = "./hosts.ini");
        
    // Validate if "ip" contains a legal IP address in a dot notation
    // (a la "255.255.255.255")
    static bool IsValidIP(const string& ip);

    // Get local (current) host name (uname call)
    static string GetLocalHost(void);
    
    // Get local (current) host ip address from registry
    static string GetLocalIP(void);
    
	static list<string> GetLocalIPList(void);

    // Given host name "hostname", return its IP address in dot notation.
    // If the "hostname" is not in the dot notation, then look it up in
    // the registry. Return empty string if "hostname" cannot be resolved.
    string LocalResolveDNS(const string& hostname) const;

    // Given ip address in dot notation, return host name by look up in
    // the registry. Return empty string if "ip" cannot be resolved to hostname.
    string LocalBackResolveDNS(const string& ip) const;

protected:

 //   static string sm_localHostName;
    
    map<string, string> m_map;
};

END_NAMESPACE_ARCHY

#endif // ARCHY_SMALL_DNS_HEADER


