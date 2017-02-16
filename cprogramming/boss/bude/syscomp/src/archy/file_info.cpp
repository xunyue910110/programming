#ifdef _MBCS
#pragma warning( disable : 4786)
#endif

#include "file_info.hpp"

int ParseUnixFtpListLine(const string& line, file_info& info)
{
	if( line.size() < 2 || (line[1] != 'r' && line[1] != 'w' && line[1] != '-'))
		return -1;

	string::size_type pos1, pos2, pos3;
	pos1 = line.find(' ', 0);
	if( pos1 == string::npos )
		return -1;
	info.attribute = line.substr(0, pos1);

	if( line[0] == 'l' ) {
		pos2 = line.find_last_of("->");
		if( pos2 == string::npos )
			return -1;
		pos2 -= 2;
		while(line[--pos2] == ' ');

		pos1 = line.find_last_of(' ', pos2 - 1);
	}
	else {
		pos1 = line.find_last_of(' ');
	}
	if( pos1 == string::npos )
		return -1;
	
	pos3 = line.find_first_of("\r\n", pos1 + 1);
	info.name = line.substr(pos1 + 1, pos3 - pos1 -1);
	
	while(line[--pos1] == ' ');
	pos3 = pos1;

	// 修改时间
	pos2 = line.find_last_of(' ', pos1);
	if( pos2 == string::npos )
		return -1;
	while(line[--pos2] == ' ');
	pos1 = line.find_last_of(' ', pos2);
	if( pos1 == string::npos )
		return -1;
	while(line[--pos1] == ' ');
	pos2 = line.find_last_of(' ', pos1);
	if( pos2 == string::npos )
		return -1;
	
	info.modiftime = line.substr(pos2 + 1, pos3 - pos2);
	while(line[--pos2] == ' ');
	pos1 = line.find_last_of(' ', pos2);
	if( pos1 == string::npos )
		return -1;
	info.size = atoi(line.substr(pos1 + 1, pos2 - pos1).c_str());

	return 0;

}
int ParseUnixFtpList(const string& fileName, list<file_info>& infoList)
{
	FILE* fp = fopen(fileName.c_str(), "r");
	if( fp == NULL )
		return -1;
	char buf[256];
	int ret;
	file_info info;
	int erroParseLines(0);
	while(!feof(fp)) {
		buf[0] = 0;
		fgets(buf, 255, fp);
		if( buf[0] == 0)
			continue;
		if(strcmp(buf, "." ) == 0 || strcmp(buf, "..") == 0 )continue;

		ret = ParseUnixFtpListLine(buf, info);
		if(ret == 0) {
			infoList.push_back(info);
		}
		else {
			++erroParseLines;
		}
	}
	fclose(fp);
	return erroParseLines;
}
int ParseUnixFtpNList(const string& fileName, list<string>& infoList)
{
	FILE* fp = fopen(fileName.c_str(), "r");
	if( fp == NULL )
		return -1;
	char buf[256];

	while(!feof(fp)) {
		buf[0] = 0;
		fgets(buf, 255, fp);
		if( buf[0] == 0)
			continue;
		if(strncmp(buf, "total", 5) == 0 ) continue;
		if(strcmp(buf, "." ) == 0 || strcmp(buf, "..") == 0 )continue;

		char* p = strchr(buf, '\r');
		if( p == 0)
			p = strchr(buf, '\n');
		if(p == 0) {
			fclose(fp);
			return -1;
		}
		*p = 0;
		infoList.push_back(string(buf));
	}
	fclose(fp);
	return 0;
}


