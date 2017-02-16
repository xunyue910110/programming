#ifndef ARCHY_FILE_INFORMATION_DEFINE
#define ARCHY_FILE_INFORMATION_DEFINE
#include <list>
#include "archy_config.hpp"

USING_STD_NAMESPACE;

struct file_info
{
	string name;
	string attribute;
	int    size;
	string modiftime;
};
// ret -1 parse error 0 ok
int ParseUnixFtpListLine(const string& line, file_info& info);

// ret the count of line which parsed error
// ignore . and ..
int ParseUnixFtpList(const string& fileName, list<file_info>& infoList);


// ignore . and ..
int ParseUnixFtpNList(const string& fileName, list<string>& infoList);

#endif
