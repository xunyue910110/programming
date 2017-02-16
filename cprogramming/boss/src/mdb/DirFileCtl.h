#ifndef __DIRFILECTL_H__
#define __DIRFILECTL_H__
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/timeb.h>
#include <ctype.h>
#include <time.h>
#include "Mdb_Exception.h"
#include <iostream>
#include <vector>
#include <sstream>
using namespace std;
class CTools
{
	public:
		CTools(){}
		~CTools(){}
		static string sprintf(const char *format,...);
		template <class T> 
		static string toString(T a);		
		static int userLog(const string& strLogFileName, const char * formate,...);//最后一个参数为NULL
		static string toUpper(const string& str);
		static string toLower(const string& str);
		static string lTrim(const string& src);
		static string rTrim(const string &src);
		static bool isInteger(const string& str, bool allowNeg = true/*允许负数*/);
		static bool fileExists(const string& filename);
		static long fileSize(FILE *fp);
		static long fileSize(const string& filename);
		//format:"%Y%m%d%H%M%S","%Y%m%d","%Y-%m-%d %H:%M:%S"
		static string getTimeStr(const string & forma,time_t *t=NULL);
		static void daemon_init();
		static string getLenStr(size_t len);
		static string getCurrentDir();
		static time_t getTime(const string& strTime);//yyyymmddhh24miss
};
class CDirectory
{
	public:
		CDirectory();
		CDirectory(const string& r_dirName);
		~CDirectory();
		bool setDir(const string& r_dirName);
		bool getFileList(vector<string>& r_vFile);
		bool getSubDirList(vector<string>& r_vSubDir);
	private:
		string m_dirName;
};
class CFile
{
	public:
		CFile();
		CFile(const string& r_fileName);
		~CFile();
		bool fileExists(const string& r_fileName="");
		long getFileSize(const string& r_fileName="");
		bool setFileName(const string& r_fileName);
		bool updateFile(long r_offSet,char * r_buf,long r_len);
		bool initFile(long r_offSet,long r_len);
		bool initPage(long r_offSet,long r_len);
		bool readFile(long r_offSet,char * r_buf,long r_len);
	private:
		string m_fileName;
};

#endif

