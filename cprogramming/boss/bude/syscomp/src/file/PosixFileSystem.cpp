
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <utime.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>

#include <errno.h>

#include <iostream>
using namespace std;



#include "PosixFileSystem.h"

const size_t MaxPathLen = 256;


int PosixFileSystem::canonicalize(const string& path, string& ret_path) const
{
  if (FileSystem::canonicalize(path, ret_path) < 0)
    return -1;
  
  return 0;
}


int PosixFileSystem::getCurrentDirectory(string& path) const
{
  char buffer[MaxPathLen];
  if (::getcwd(buffer, MaxPathLen) == 0) {
    return -1;
  }
  
  path = string(buffer);
  
  return 0;
}


char PosixFileSystem::getSeparatorChar() const
{
  return '/';
}


size_t PosixFileSystem::getPrefixLength(const string& path) const
{
  if ( !path.empty() && path[0] == getSeparatorChar()) {
    return 1;
  }
  
  return 0; 
}


bool PosixFileSystem::isAbsolute(const string& path) const
{
  return (!path.empty() && path[0] == getSeparatorChar());  
}


bool PosixFileSystem::isCaseSensitive() const
{
#ifndef WIN32
  return false;
#else
  return true;
#endif  
}


int PosixFileSystem::getFileAttributeFlags(const string& path) const
{
  struct stat myStat;
  if (::stat(path.c_str(), &myStat) == 0) {
    int ret = Exists;
    if (myStat.st_mode & S_IFDIR)       ret |= Directory;
    if (myStat.st_mode & S_IFREG)       ret |= RegularFile;
    if ((myStat.st_mode & S_IWUSR)==0)  ret |= ReadOnly;
    return ret;
  }
  
  return 0; 
}


bool PosixFileSystem::checkAccess(const string& path, AccessMode mode) const
{
  struct stat myStat;
  if (::stat(path.c_str(), &myStat) == 0) {
    return ((mode == ReadAccess  && myStat.st_mode & S_IRUSR) ||
            (mode == WriteAccess && myStat.st_mode & S_IWUSR));
  }

  return false;

}


int PosixFileSystem::getLastModifiedTime(const string& path, time_t& time) const
{
  struct stat myStat;
  if (::stat(path.c_str(), &myStat) < 0) {
    return -1;
  }

  time = myStat.st_mtime;
  return 0;
}


size_t PosixFileSystem::getLength(const string& path) const
{
  struct stat myStat;

  if (::stat(path.c_str(), &myStat) == 0) {
    return myStat.st_size;
  }

  return 0;
}


int PosixFileSystem::openFile(const string& path, const int accessMode, 
                              const CreationDisp creationDisp, 
                              const int attributes) const
{
  int flags = 0;
  int permissionFlags = 0;

  if ( (accessMode & ReadAccess) && (accessMode & WriteAccess) ) {
    flags |= O_RDWR;
  }
  else if (accessMode & ReadAccess) {
    flags |= O_RDONLY;
  }
  else if (accessMode & WriteAccess) {
    flags |= O_WRONLY;
  }
  else {
    cout<<"PosixFileSystem file open error "<< errno <<endl;
    return -1;
  }

  switch(creationDisp)
  {
    case OpenExisting:
      if (accessMode & WriteAccess) flags |= O_APPEND;
      break;
    case OpenCreateAppend:
      flags |= O_CREAT | O_APPEND;
      break;
    case OpenCreateExclusive:
      flags |= O_CREAT | O_EXCL;
      break;
    case OpenCreateTruncate:
      flags |= O_CREAT | O_TRUNC;
      break;
  }

  if (creationDisp == OpenCreateExclusive || creationDisp == OpenCreateTruncate)
  {
    if (attributes & ReadOnly)
      permissionFlags = S_IRUSR;
    else
      permissionFlags = (S_IRUSR | S_IWUSR);

#ifndef WIN32
    // Under Unix it is standard to give read permissions
    // to the group and others
    permissionFlags |= (S_IROTH | S_IRGRP);
#endif

  }

  int fd = ::open(path.c_str(), flags, permissionFlags);

  if (fd == -1) {
    
    	cout<<"file open(path.c_str(), flags, permissionFlags) error=  "<< errno <<endl;
    	cout << "file=" << path.c_str() <<endl;
    	cout << "flags=" << flags << endl;
    	cout << "permissionFlags=" << permissionFlags << endl;
      return -1;
  }

#ifndef CREATEFILE_SETS_MODTIME
  // Some platforms do not set the file's modification time to the current 
  // time when it is first created.  This is unusual (only darwin so far),
  // so for consitency with otherplatforms OpenTop sets the modtime to the
  // current time.

  if (::utime(path.c_str(), 0) < 0) {
    return -1;
  }
#endif

  return fd;

}


int PosixFileSystem::closeFile(int fd) const
{
  if (fd < 0 || ::close(fd) < 0) {
    return -1;
  }
  
  return 0;
}


int PosixFileSystem::deleteFile(const string& path) const
{
  int result = 0;

  if (getFileAttributeFlags(path) & Directory) {  
    result = ::rmdir(path.c_str());
  }
  else {
    result = ::remove(path.c_str());
  }
  
  if (result < 0) {
    return -1;
  }
  
  return 0;
}


int PosixFileSystem::listDirectory(const string& path,
                                   std::list<string> & file_list) const
{
  if (! (getFileAttributeFlags(path) & Directory)) {
    return -1;
  }
  
  file_list.clear();

#ifndef WIN32
  DIR* pDir = ::opendir(path.c_str());
  
  if (pDir == 0) {
    return -1;
  }

  struct dirent* pEntry;
  
  while( (pEntry = ::readdir(pDir)) != NULL)
  {
    string foundPath = string(pEntry->d_name);
    if (foundPath != (".") && foundPath != ("..")) {
      file_list.push_back(foundPath);
    }
  }

  ::closedir(pDir);
#endif //WIN32

  return 0; 
}


int PosixFileSystem::createDirectory(const string& path) const
{
#if defined(WIN32)
  int result = ::mkdir(path.c_str());
#else
  int mode = 0777;
  int result = ::mkdir(path.c_str(), mode);
#endif

  if (result < 0) {
    return -1;
  }

  return 0;
}


int PosixFileSystem::rename(const string& old_path, 
                            const string& new_path) const
{
  if (::rename(old_path.c_str(), new_path.c_str()) < 0) {
    return -1;
  }

  return 0;
}


int PosixFileSystem::setLastModifiedTime(const string& path, 
                                         const time_t& time) const
{
#ifdef _MSC_VER
  _utimbuf buf;
#else
  struct utimbuf buf;
#endif

  time_t currentTime = ::time(NULL);
  buf.actime  = currentTime;
  buf.modtime = time;
  
#ifdef _MSC_VER
  int result = ::_utime(path.c_str(), &buf);
#else
  int result = ::utime(path.c_str(), &buf);
#endif
  
  if (result < 0) {
    return -1;
  }
  
  return 0;
}


int PosixFileSystem::setReadOnly(const string& path, const bool bReadOnly) const
{
  struct stat myStat;
  if (::stat(path.c_str(), &myStat) < 0) {
    return -1;
  }
  int mode = myStat.st_mode;

  if (bReadOnly) {
#ifdef WIN32
    mode &= ~(S_IWUSR);
#else
    mode &= ~(S_IWUSR | S_IWOTH | S_IWGRP);
#endif
  }
  else {
    mode |= (S_IWUSR);
  }
  if (::chmod(path.c_str(), mode) < 0) {
    return -1;
  }
  
  return 0;
}

int PosixFileSystem::readFile(int fd, Byte* pBuffer, size_t bufLen) const
{
  if (fd < 0 || !pBuffer) {
    return -1;
  }

  size_t bytesRead = ::read(fd, pBuffer, bufLen);
  if (bytesRead < 0) {
    return -1;
  }

  return bytesRead;
}


int PosixFileSystem::writeFile(int fd, const Byte* pBuffer, size_t bufLen) const
{
  if (fd < 0 || !pBuffer) {
    return -1;
  }
 
 /* 
  int bytesWritten = ::write(fd, pBuffer, bufLen);
  
  
  if (bytesWritten != (int)bufLen) {
    cout << "write file error,errno=" << errno <<endl;
    cout << "write file error,bytesWritten=" << bytesWritten <<endl;
    cout << "write file error,bufLen=" << bufLen <<endl;
    cout << "write file error,pBuffer=" << (char *)pBuffer <<endl;
    
    
    return -1;
  }
  
  */
  
  int bytesWritten=0, bytesCurrWrite=0;
  
  while(bytesWritten<bufLen)
  {
  	
  	bytesCurrWrite = ::write(fd, pBuffer+bytesWritten, bufLen-bytesWritten);
  	if(bytesCurrWrite <0)
  	{
  		cout << "write file error,errno=" << errno <<endl;
    		cout << "write file error,bytesWritten=" << bytesWritten <<endl;
    		cout << "write file error,bufLen=" << bufLen <<endl;
    		cout << "write file error,pBuffer=" << (char *)pBuffer+bytesWritten <<endl;
        return -1;
  	}
  	bytesWritten += bytesCurrWrite;
  	
  }
  
  
  return 0;
}
