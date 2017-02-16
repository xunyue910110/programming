#include "file.hpp"
#include <stdio.h>
#if defined(WIN32)
#include <windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif

namespace archy {

#ifdef WIN32
date_time to_date_time1(const SYSTEMTIME& sysTime)
{
	if (sysTime.wYear < 1900)
	{
		time_t time0 = 0L;
		return date_time(time0);
	}
	return date_time(
		(int)sysTime.wYear, (int)sysTime.wMonth, (int)sysTime.wDay,
		(int)sysTime.wHour, (int)sysTime.wMinute, (int)sysTime.wSecond
		);
}

date_time to_date_time2(const FILETIME& fileTime)
{
	time_t time0 = 0L;
	// first convert file time (UTC time) to local time
	FILETIME localTime;
	if (!FileTimeToLocalFileTime(&fileTime, &localTime))
	{
		return date_time(time0);
	}

	// then convert that time to system time
	SYSTEMTIME sysTime;
	if (!FileTimeToSystemTime(&localTime, &sysTime))
	{
		return date_time(time0);
	}

	// then convert the system time to a time_t (C-runtime local time)
	return to_date_time1(sysTime);

}
#endif // WIN32

bool file::GetStatus(const char* filename, CFileStatus& rStatus)
{
#ifdef WIN32
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((LPTSTR)filename, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hFind);


	// get just the low DWORD of the file size
	rStatus.m_size = (long)findFileData.nFileSizeLow;

	
	// convert times as appropriate
	rStatus.m_ctime = to_date_time2(findFileData.ftCreationTime);
	rStatus.m_atime = to_date_time2(findFileData.ftLastAccessTime);
	rStatus.m_mtime = to_date_time2(findFileData.ftLastWriteTime);


#else
	struct stat statbuf;
	int ret = ::stat(filename, &statbuf);
	if( ret < 0 )
		return false;
	rStatus.m_size = (long)statbuf.st_size;
	
	// convert times as appropriate
	rStatus.m_ctime = date_time(statbuf.st_ctime);
	rStatus.m_atime = date_time(statbuf.st_atime);
	rStatus.m_mtime = date_time(statbuf.st_mtime);

#endif
	return true;
}
file::file()
:m_fp(INVALID_FILE_HANDLE)
,b_autoclose(true)
{
}

file::file(FILE_HANDLE fp)
:m_fp(INVALID_FILE_HANDLE)
,b_autoclose(true)
{
}

void file::set_autoclose(bool autoclose)
{
	b_autoclose = autoclose;
}

int file::close()
{
	if( m_fp != INVALID_FILE_HANDLE) {
#ifdef WIN32
		::CloseHandle((HANDLE)m_fp);
#else
		::fclose(m_fp);
#endif
		m_fp = INVALID_FILE_HANDLE;
		m_filename = "";
	}
        return 0;	
}

file::~file()
{
	if( b_autoclose ) {
		file::close();
	}
}


int file::open(const std::string& strFileName, const std::string& flags)
{
	m_filename = strFileName;
#ifdef WIN32
	DWORD dwAccess = 0;
	DWORD dwCreateFlag;

	if( flags == "r" ) {
		dwAccess = GENERIC_READ;
        }
	else if( flags == "w" || flags == "a" ) {
		dwAccess = GENERIC_WRITE;
        }
	else if( flags == "r+" || flags == "w+" || flags == "a+" ) 
		dwAccess = GENERIC_READ|GENERIC_WRITE;	
	else return -1;

	if( flags == "r" || flags == "r+" ) {
                dwCreateFlag = OPEN_EXISTING;
        }
	else if( flags == "w" || flags == "w+" ) {
                dwCreateFlag = CREATE_ALWAYS;
        }
	else if( flags == "a" || flags == "a+" ) {
                dwCreateFlag = OPEN_ALWAYS;
        }

	HANDLE hFile = ::CreateFile(m_filename.c_str(), dwAccess, FILE_SHARE_READ, NULL,
		dwCreateFlag, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			0, // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);
		// Process any inserts in lpMsgBuf.
		// ...
		// Display the string.
		printf("Error:%s\n", (LPCTSTR)lpMsgBuf);
		// Free the buffer.
		LocalFree( lpMsgBuf );

		return -1;
	}
	m_fp = (HFILE)hFile;

	if( flags == "a" || flags == "a+") {
		DWORD dwNew = ::SetFilePointer((HANDLE)m_fp, 0, NULL, (DWORD)FILE_END);
		if (dwNew  == (DWORD)-1) return false;
	}
	else if( flags == "w+" ) {
		DWORD dwNew = ::SetFilePointer((HANDLE)m_fp, 0, NULL, (DWORD)FILE_BEGIN);
		if (dwNew  == (DWORD)-1) return false;
		if (!::SetEndOfFile((HANDLE)m_fp)) {
			return false;
		}
	}
	
	return 0;
#else	
	m_fp = ::fopen(m_filename.c_str(), flags.c_str());
        if( m_fp == NULL ) perror("fopen");
		return m_fp != NULL ? 0 : -1;
#endif

}

int file::read_line(std::string& strLine)
{
	strLine = "";
	char chChar;

	long iRet(0);
	while(1) {		
	#ifdef WIN32
		if (!::ReadFile((HANDLE)m_fp, &chChar, 1, (DWORD*)&iRet, NULL))
			return -1;
		// Win32s will not return an error all the time (usually DISK_FULL)
	#else
		iRet = ::fread(&chChar, 1, 1, m_fp);
	#endif
		if( iRet < 0 ) return iRet;
		else if( iRet == 0 ) break;
                
		strLine += chChar;
		if( chChar == '\n' ) 
			break;
	}
	if( strLine.empty() )
		return 0;

	return 1;
}


int file::read(void* buf, int bufsize)
{
	long iRet;
#ifdef WIN32
	if (!::ReadFile((HANDLE)m_fp, buf, bufsize, (DWORD*)&iRet, NULL))
		return -1;
	// Win32s will not return an error all the time (usually DISK_FULL)
#else
	iRet = ::fread(buf, 1, bufsize, m_fp);
#endif
	return iRet;
}

int file::write_line(const std::string& strLine)
{
	long iRet;
#ifdef WIN32
	if (!::WriteFile((HANDLE)m_fp, strLine.c_str(), strLine.length(), (DWORD*)&iRet, NULL))
		return -1;
	// Win32s will not return an error all the time (usually DISK_FULL)
#else
	iRet = ::fwrite(strLine.c_str(), 1, strLine.length(), m_fp);
#endif
	if( iRet < strLine.length() )
		return -1;
		
	if( strLine[strLine.length() - 1] != '\n' ) {
		
	#ifdef WIN32
		if (!::WriteFile((HANDLE)m_fp, "\n", 1, (DWORD*)&iRet, NULL))
			return -1;
		// Win32s will not return an error all the time (usually DISK_FULL)
	#else
		iRet = ::fwrite("\n", 1, 1, m_fp);
	#endif
		if( iRet != 1 )
			return -1;
	}
	return 1;	
}

int file::write(const void* buf, int bufsize)
{
	long iRet;
#ifdef WIN32
	if (!::WriteFile((HANDLE)m_fp, buf, bufsize, (DWORD*)&iRet, NULL))
		return -1;
	// Win32s will not return an error all the time (usually DISK_FULL)
#else
	iRet = ::fwrite(buf, 1, bufsize, m_fp);
#endif
	return iRet;
}

int file::flush()
{
#ifdef WIN32
	return ::FlushFileBuffers((HANDLE)m_fp);
#else
	return ::fflush(m_fp);
#endif
}

// 获取文件大小
long file::get_length()
{
	long curpos = file::tell();
	file::seek(0, seek_end);
	long size = file::tell();

	file::seek(curpos, seek_begin);
	return size; 
}
	
long file::get_length(const std::string& filename)
{
	file ofile;
	int ret = ofile.open(filename, "r");
	if( ret < 0) return -1;
	return ofile.get_length();
}

long file::seek( long lOff, seekposition nFrom )
{
#ifdef WIN32
	return ::SetFilePointer((HANDLE)m_fp, lOff, NULL, (DWORD)nFrom);
#else
	return ::fseek(m_fp, lOff, nFrom);
#endif
}

long file::tell()
{
#ifdef WIN32
	return ::SetFilePointer((HANDLE)m_fp, 0, NULL, FILE_CURRENT);
#else
	return ::ftell(m_fp);
#endif
}

bool file::lock_range( long pos, long count )
{
#if defined(WIN32)
	if (!::LockFile((HANDLE)m_fp, pos, 0, count, 0))
		return false;
	return true;
#else
	struct flock fk;
	fk.l_type = F_WRLCK;
	fk.l_start = pos;
	fk.l_whence = SEEK_SET;
	fk.l_len = count ;
	fk.l_pid = getpid() ;	
	return ::fcntl(fileno(m_fp), F_SETLKW, &fk) == 0;
#endif	
}

bool file::unlock_range( long pos, long count )
{
#if defined(WIN32)
	if (!::UnlockFile((HANDLE)m_fp, pos, 0, count, 0))
		return false;
	return true;
#else
	struct flock fk;
	fk.l_type = F_UNLCK ;
	return ::fcntl(fileno(m_fp), F_SETLKW, &fk) == 0;	
#endif	
}

} // namespace archy
