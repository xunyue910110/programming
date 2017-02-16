
#include <algorithm>

#include "Win32FileSystem.h"


int Win32FileSystem::canonicalize(const string& path, string& ret_path) const
{
  if (FileSystem::canonicalize(path, ret_path) < 0)
    return -1;
  
  return 0;
}


int Win32FileSystem::getCurrentDirectory(string& path) const
{
  return 0;
}


char Win32FileSystem::getSeparatorChar() const
{
  return '\\';
}


size_t Win32FileSystem::getPrefixLength(const string& path) const
{
  return 0;
}


bool Win32FileSystem::isAbsolute(const string& path) const
{
  return true;
}


bool Win32FileSystem::isCaseSensitive() const
{
  return false;
}


int Win32FileSystem::getFileAttributeFlags(const string& path) const
{
  return 0;
}


bool Win32FileSystem::checkAccess(const string& path, AccessMode mode) const
{
  return true;
}


int Win32FileSystem::getLastModifiedTime(const string& path, time_t& time) const
{
  return 0;
}


size_t Win32FileSystem::getLength(const string& path) const
{
  return 0;
}


int Win32FileSystem::openFile(const string& path, const int accessMode, 
                              const CreationDisp creationDisp, 
                              const int attributes) const
{
  return 0;
}


int Win32FileSystem::closeFile(int pFD) const
{
  return 0;
}


int Win32FileSystem::deleteFile(const string& path) const
{
  return 0;
}


int Win32FileSystem::listDirectory(const string& path,
                                   std::list<string> & fileList) const
{
  return 0;
}


int Win32FileSystem::createDirectory(const string& path) const
{
  return 0;
}


int Win32FileSystem::rename(const string& path1, const string& path2) const
{
  return 0;
}


int Win32FileSystem::setLastModifiedTime(const string& path, 
                                         const time_t& time) const
{
  return 0;
}


int Win32FileSystem::setReadOnly(const string& path, const bool bReadOnly) const
{
  return 0;
}


int Win32FileSystem::readFile(int fd, Byte* pBuffer, size_t bufLen) const
{
  return 0;
}


int Win32FileSystem::writeFile(int fd, const Byte* pBuffer, 
                                size_t bufLen) const
{
  return 0;
}
