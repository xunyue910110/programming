#include "DirFileCtl.h"
#include "PageInfo.h"

CDirectory::CDirectory()
{
}
CDirectory::~CDirectory()
{
}
CDirectory::CDirectory(const string& r_dirName)
{
	if (access(r_dirName.c_str(),0) == -1)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "无效的路径:%s",r_dirName.c_str());
	}
	m_dirName = r_dirName;
}
bool CDirectory::setDir(const string& r_dirName)
{
	if (access(r_dirName.c_str(),0) == -1)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "无效的路径:%s",r_dirName.c_str());
	}
	m_dirName = r_dirName;
	return true;
}
bool CDirectory::getFileList(vector<string>& r_vFile)
{
	if (m_dirName.length() == 0)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "请先使用setDir设置路径");
	}
	
	DIR * dir = opendir(m_dirName.c_str());
	if (dir == NULL)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "无法打开目录:%s!",m_dirName.c_str());
	}
	struct dirent* pDirent = 0;
	while ((pDirent = readdir(dir)) != NULL)
	{
		if ((strcmp(pDirent->d_name,".")  == 0) || (strcmp(pDirent->d_name,"..") == 0))
			continue;
		string t_fileName=m_dirName+string("/")+string(pDirent->d_name);
		struct stat t_type;
		if (lstat(t_fileName.c_str(),&t_type) < 0)
			continue;
		if ((t_type.st_mode & S_IFMT) == S_IFLNK)
			continue;
		if ((t_type.st_mode & S_IFMT) == S_IFDIR)
			continue;
		r_vFile.push_back(t_fileName);
	}
	closedir(dir);
	return true;
}
bool CDirectory::getSubDirList(vector<string>& r_vSubDir)
{
	if (m_dirName.length() == 0)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "请先使用setDir设置路径");
	}
	
	DIR * dir = opendir(m_dirName.c_str());
	if (dir == NULL)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "无法打开目录:%s!",m_dirName.c_str());
	}
	struct dirent* pDirent = 0;
	while ((pDirent = readdir(dir)) != NULL)
	{
		if ((strcmp(pDirent->d_name,".")  == 0) || (strcmp(pDirent->d_name,"..") == 0))
			continue;
		string t_fileName=m_dirName+string("/")+string(pDirent->d_name);
		struct stat t_type;
		if (lstat(t_fileName.c_str(),&t_type) < 0)
			continue;
		if ((t_type.st_mode & S_IFMT) == S_IFLNK)
			continue;
		if ((t_type.st_mode & S_IFMT) != S_IFDIR)
			continue;
		r_vSubDir.push_back(t_fileName);
	}
	closedir(dir);
	return true;
}
CFile::CFile()
{
}
CFile::CFile(const string& r_fileName)
{
	if (!fileExists(r_fileName))
	{
		throw Mdb_Exception(__FILE__, __LINE__, "无效的文件名:%s",r_fileName.c_str());
	}
	m_fileName = r_fileName;
}
CFile::~CFile()
{
}
bool CFile::fileExists(const string& r_fileName)
{
	string t_fileName = r_fileName;
	if (t_fileName == "") t_fileName = m_fileName;
	if (access(t_fileName.c_str(),0) == -1)
		return false;
	return true;
}
long CFile::getFileSize(const string& r_fileName)
{
	string t_fileName = r_fileName;
	if (t_fileName == "") t_fileName = m_fileName;
	struct stat t_type;
	if (lstat(t_fileName.c_str(),&t_type) < 0)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "获取文件信息失败:%s",strerror(errno));
	}
	return t_type.st_size;
}
bool CFile::setFileName(const string& r_fileName)
{
	if (!fileExists(r_fileName))
	{
		throw Mdb_Exception(__FILE__, __LINE__, "无效的文件名:%s",r_fileName.c_str());
	}
	m_fileName = r_fileName;
	return true;
}
bool CFile::updateFile(long r_offSet,char * r_buf,long r_len)
{
	//#ifdef _DEBUG_
	//cout<<"updateFile(),offSet="<<r_offSet<<",Len="<<r_len<<",File="<<m_fileName<<endl;
	//#endif
	long t_size=getFileSize();
	if (r_offSet + r_len + 1024> t_size)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "长度越界:文件长度[%ld],写入偏移量[%ld],写入长度[%ld],文件:%s",t_size,r_offSet,r_len,m_fileName.c_str());
	}
	FILE * t_pFile;
	t_pFile = fopen(m_fileName.c_str(),"rb+");
	if (t_pFile == NULL)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "打开文件失败:%s",m_fileName.c_str());
	}
	if (fseek(t_pFile,r_offSet+1024,SEEK_SET) != 0)
	{
		fclose(t_pFile);
		throw Mdb_Exception(__FILE__, __LINE__, "文件定位错误!");
	}
	if (fwrite(r_buf,r_len,1,t_pFile) != 1)
	{
		fclose(t_pFile);
		throw Mdb_Exception(__FILE__, __LINE__, "文件写入错误!");		
	}
	fclose(t_pFile);
	return true;
}
bool CFile::initFile(long r_offSet,long r_len)
{
	long t_size=getFileSize();
	if (r_offSet + r_len + 1024> t_size)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "长度越界:文件长度[%ld],写入偏移量[%ld],写入长度[%ld],文件:%s",t_size,r_offSet,r_len,m_fileName.c_str());
	}
	FILE * t_pFile;
	t_pFile = fopen(m_fileName.c_str(),"rb+");
	if (t_pFile == NULL)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "打开文件失败:%s",m_fileName.c_str());
	}
	if (fseek(t_pFile,r_offSet+1024,SEEK_SET) != 0)
	{
		fclose(t_pFile);
		throw Mdb_Exception(__FILE__, __LINE__, "文件定位错误!");
	}
	char *t_char=new char[r_len];
	memset(t_char,0,r_len);	
	if (fwrite(t_char,r_len,1,t_pFile) != 1)
	{
		delete []t_char;
		fclose(t_pFile);
		throw Mdb_Exception(__FILE__, __LINE__, "文件写入错误!");		
	}
	delete []t_char;
	fclose(t_pFile);
	return true;
}
bool CFile::initPage(long r_offSet,long r_len)
{
	long t_size=getFileSize();

	FILE * t_pFile;
	t_pFile = fopen(m_fileName.c_str(),"rb+");
	if (t_pFile == NULL)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "打开文件失败:%s",m_fileName.c_str());
	}
	if (fseek(t_pFile,r_offSet+1024,SEEK_SET) != 0)
	{
		fclose(t_pFile);
		throw Mdb_Exception(__FILE__, __LINE__, "文件定位错误!");
	}
	
	PageInfo *t_page=new PageInfo;
	if (fread(t_page,sizeof(PageInfo),1,t_pFile) != 1)
	{
		fclose(t_pFile);
		delete t_page;
		throw Mdb_Exception(__FILE__, __LINE__, "文件读取错误!");		
	}
	size_t t_pageSize=t_page->m_pageSize;
	delete t_page;
	
	if (fseek(t_pFile,sizeof(PageInfo) * -1,SEEK_CUR)!=0)
	{
		fclose(t_pFile);
		throw Mdb_Exception(__FILE__, __LINE__, "文件定位错误!");
	}	
	char * t_pageMem=new char[t_pageSize];
	if (fread(t_pageMem,t_pageSize,1,t_pFile) != 1)
	{
		fclose(t_pFile);
		delete [] t_pageMem;
		throw Mdb_Exception(__FILE__, __LINE__, "文件读取错误!");		
	}
	t_page=(PageInfo *)t_pageMem;
	t_page->initSlotList(t_pageMem,r_len,false);
	
	if (fseek(t_pFile,t_pageSize * -1,SEEK_CUR)!=0)
	{
		delete [] t_pageMem;		
		fclose(t_pFile);
		throw Mdb_Exception(__FILE__, __LINE__, "文件定位错误!");
	}

	if (fwrite(t_pageMem,t_pageSize,1,t_pFile) != 1)
	{
		delete [] t_pageMem;
		fclose(t_pFile);
		throw Mdb_Exception(__FILE__, __LINE__, "文件写入错误!");		
	}
	delete [] t_pageMem;
	fclose(t_pFile);
	return true;
}
bool CFile::readFile(long r_offSet,char * r_buf,long r_len)
{
	//#ifdef _DEBUG_
	//cout<<"readFile(),offSet="<<r_offSet<<",Len="<<r_len<<",File="<<m_fileName<<endl;
	//#endif	
	long t_size=getFileSize();
	if (r_offSet + r_len + 1024 > t_size)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "长度越界:文件长度[%ld],读取偏移量[%ld],读取长度[%ld],文件:%s",t_size,r_offSet,r_len,m_fileName.c_str());
	}
	FILE * t_pFile;
	t_pFile = fopen(m_fileName.c_str(),"rb");
	if (t_pFile == NULL)
	{
		throw Mdb_Exception(__FILE__, __LINE__, "打开文件失败:%s",m_fileName.c_str());
	}
	if (fseek(t_pFile,r_offSet + 1024,SEEK_SET) != 0)
	{
		fclose(t_pFile);
		throw Mdb_Exception(__FILE__, __LINE__, "文件定位错误!");
	}
	if (fread(r_buf,r_len,1,t_pFile) != 1)
	{
		fclose(t_pFile);
		throw Mdb_Exception(__FILE__, __LINE__, "文件读取错误!");		
	}
	fclose(t_pFile);
	return true;	
}
string CTools::sprintf(const char *format,...)
{
    const int iMaxLen = 1024*8;
    char str[iMaxLen];

    memset(str,0,iMaxLen);
    va_list vm;
    va_start(vm,format);
    int n=vsnprintf(str,iMaxLen,format,vm);
    va_end(vm);
    if (n < iMaxLen)
    {
	    string ret(str);
	    return ret;
	}
	else
	{
		memset(str,0,iMaxLen);
		return "";
	}
}
template <class T> 
string CTools::toString(T a)
{
	string ret("");
	static stringstream _s_s;
	_s_s.str("");
	_s_s.clear();
	_s_s<<a;
	_s_s>>ret;
	return ret;
}
int CTools::userLog(const string& strLogFileName, const char * formate,...)
{
    int ret;
	
    FILE* fp=fopen(strLogFileName.c_str(),"a+");
    if (fp == NULL)
        return -1;

    va_list marker;
    va_start( marker,formate);
    char *c_formate=new char[strlen(formate)+3];
    memset(c_formate,0,strlen(formate)+3);
    strcpy(c_formate,formate);
    strcat(c_formate,"\n");
    
    ret=vfprintf(fp,(const char *)c_formate,marker);
    if (ret==EOF)
    {
        va_end( marker );
        fclose(fp);
        delete []c_formate;
        return -1;
    }
    va_end( marker );
    fclose(fp);
    delete []c_formate;
    return 0;
}
string CTools::toUpper(const string& str)
{
	string strRet;
	for (unsigned int i=0;i<str.length();i++)
	{
		strRet+=toupper(str[i]);
	}
	return strRet;
}
string CTools::toLower(const string& str)
{
	string strRet;
	for (unsigned int i=0;i<str.length();i++)
	{
		strRet+=tolower(str[i]);
	}
	return strRet;
}
string CTools::lTrim(const string& src)
{
    int ipos=0;
    int iLen=src.length();
    for(int i=0;i<iLen;i++)
    {
        if (src[i]==10 || src[i]==13 || src[i]==9 || src[i]==32)
            ipos ++;
        else
            break;
    }
    return src.substr(ipos,iLen - ipos + 1);
}
string CTools::rTrim(const string &src)
{
    int  i,iLen;
    iLen=src.length();
    int iCount = 0;
    for(i=iLen - 1;i>=0;i--)
    {
        if (src[i]==10 || src[i]==13 || src[i]==9 || src[i]==32)
            iCount ++;
        else
            break;
    }
    return src.substr(0,iLen - iCount);	
}
bool CTools::isInteger(const string& str, bool allowNeg /*允许负数*/)
{
	int len = str.length();
	for (int i=0; i<len; i++)
	{
		if ((str[i]>='0' && str[i]<='9') || (i == 0 && allowNeg && str[0] == '-'))
			continue;
		return false;
	}
	return true;
}
bool CTools::fileExists(const string& filename)
{
	return (access(filename.c_str(),0) == 0 ? true:false);
}
long CTools::fileSize(FILE *fp)
{
	long lCurrPos=0, lLength=0;
	//保存当前位置
	lCurrPos = ftell(fp);
	//移动指针到文件末尾
	fseek(fp, 0L, SEEK_END);
	//获取文件长度
	lLength = ftell(fp);
	//指针恢复到初始位置
	fseek(fp, lCurrPos, SEEK_SET);
	
	return lLength;
}
long CTools::fileSize(const string& filename)
{
	if (!fileExists(filename.c_str()))
		return -1;
	struct stat t_type;
	if (lstat(filename.c_str(),&t_type) < 0)
		return -1;
	return t_type.st_size;	
}

void CTools::daemon_init()
{
	int i,MAXFD;
	pid_t pid;
	char str[10];

	if (pid=fork())
	{
		exit(0);
	}

	setsid();
	signal(SIGHUP,SIG_IGN);//忽略SIGHUP信号

	if (pid=fork())
	{
		exit(0);
	}

	//关闭所有打开的文件描述字 //重定向到/dev/null也可以
	MAXFD=sysconf(_SC_OPEN_MAX);
	for (i=0;i<MAXFD;i++ )
	{
		close(i);
	}
}
string CTools::getCurrentDir()
{
	char t_path[256];
	memset(t_path,0,sizeof(t_path));
	getcwd(t_path,sizeof(t_path));
	return string(t_path);	
}
string CTools::getLenStr(size_t len)
{
	string b,k,m,g,t;
	int ib=0,ik=0,im=0,ig=0,it=0;
	size_t q=len,r=0;
	if (q>0)
	{
		r = q % 1024;
		ib=r;
		q = q/1024;
		b = CTools::sprintf("%dB",r);
		if (q>0)
		{
			r = q % 1024;
			ik=r;
			q = q /1024;
			k = CTools::sprintf("%dK",r);
			if (q>0)
			{
				r = q % 1024;
				im=r;
				q = q/1024;
				m = CTools::sprintf("%dM",r);
				if (q>0)
				{
					r = q % 1024;
					ig=r;
					q = q/1024;
					g = CTools::sprintf("%dG",r);
					if (q>0)
					{
						r = q % 1024;
						it=r;
						q = q/1024;
						t = CTools::sprintf("%dT",r);
					}
				}
			}
		}
	}
	string ret;
	if (it>0) ret += t;
	if (ig>0) ret += g;
	if (im>0) ret += m;
	if (ik>0) ret += k;
	if (ib>0) ret += b;
	if (ret.length() == 0)
		ret="0B";
	return ret;
}
string CTools::getTimeStr(const string & format,time_t *t)
{
	//format:"%Y%m%d%H%M%S","%Y%m%d","%Y-%m-%d %H:%M:%S"
	string fmt;
	if (format == "yyyymmddhh24miss")
		fmt="%Y%m%d%H%M%S";
	else if (format == "yyyymmdd")
		fmt="%Y%m%d";
	else if (format == "hh24miss")
		fmt="%H%M%S";	
	else if (format == "yyyy-mm-dd hh24:mi:ss")
		fmt="%Y-%m-%d %H:%M:%S";
	else if (format == "yyyy/mm/dd hh24:mi:ss")
		fmt="%Y/%m/%d %H:%M:%S";
	else if (format == "yyyy-mm-dd")
		fmt="%Y-%m-%d";	
	else if (format == "yyyy/mm/dd")
		fmt="%Y/%m/%d";
	else if (format == "hh24:mi:ss")
		fmt="%H:%M:%S";
	else
		fmt = format;
	string strDate;
	tzset();

	struct tm *time_now;
	char vv_localtime[100]="\0";
	memset(vv_localtime,0,100);
	if (t!=NULL)
	{
		time_now = localtime(t);
	}
	else
	{
		time_t secs_now;
		time(&secs_now);
		time_now = localtime(&secs_now);
	}
	strftime(vv_localtime,100,fmt.c_str(),time_now);
	strDate=vv_localtime;
	return strDate;	
}
 
time_t CTools::getTime(const string& strTime)
{
	struct tm time_check;
	//20081112235959
	time_check.tm_year =atoi(strTime.substr(0,4).c_str()) - 1900;
	time_check.tm_mon=atoi(strTime.substr(4,2).c_str()) - 1;
	time_check.tm_mday  =atoi(strTime.substr(6,2).c_str());
	time_check.tm_hour =atoi(strTime.substr(8,2).c_str());
	time_check.tm_min  =atoi(strTime.substr(10,2).c_str());
	time_check.tm_sec  =atoi(strTime.substr(12,2).c_str());
	time_check.tm_isdst=-1;
	return mktime(&time_check);
}
