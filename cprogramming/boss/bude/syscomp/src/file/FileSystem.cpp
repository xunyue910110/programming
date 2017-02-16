
#include <algorithm>

#include "FileSystem.h"
#include "File.h"

#if defined(WIN32)
# include "Win32FileSystem.h"
#else 
# include "PosixFileSystem.h"
#endif

FileSystem* FileSystem::s_pFileSystem = 0;


void FileSystem::setFileSystem(FileSystem* pFileSystem)
{

  FileSystem* pExisting;

  pExisting = s_pFileSystem;
  s_pFileSystem = pFileSystem;

  if (pExisting) {
    delete pExisting;
    pExisting = NULL;
  }
}


FileSystem* FileSystem::getFileSystem()
{
  if(!s_pFileSystem)
  {
    setFileSystem
#if defined(WIN32)
    (new Win32FileSystem);
#else 
    (new PosixFileSystem);
#endif
  }

  return s_pFileSystem;
}


string FileSystem::normalize(const string& path) const
{
  string ret = path;
  
  if ( !ret.empty()) {
    //
    // Replace separator characters with the appropriate type
    //
    const char sep = getSeparatorChar();
    const char badSep = (sep == '/') ? '\\' : '/';
    std::replace(ret.begin(), ret.end(), badSep, sep);

    // Replace strings of separators with a single separator
    // starting at position 0 for UNIX and 1 for Windows!
    size_t pos = (sep == '\\') ? 1 : 0;
    const char doubleSep[] = {sep, sep, 0};
    while (pos != string::npos) {
      pos = ret.find(doubleSep, pos);
      if (pos != string::npos) {
        ret.erase(pos, 1);
      }
    }

    //
    // Remove terminating separator if any
    //
    if (sep == ret[ret.length()-1] && getPrefixLength(ret) < ret.length())
    {
      ret = ret.erase(ret.length()-1);
    }    
  }
  
  return ret;
}


string FileSystem::resolve(const string& parent, const string& child) const
{
  string normalizedChild = normalize(child);
  if (isAbsolute(normalizedChild)) {
    return normalizedChild;
  }
  else
  {
    size_t prefixLen = getPrefixLength(normalizedChild);
    string normalizedParent = normalize(parent);
    const char lastChar = normalizedParent[normalizedParent.length()-1];
    if(lastChar == getSeparatorChar())
    {
      return normalizedParent + normalizedChild.substr(prefixLen);
    }
    else
    {
      return normalizedParent + getSeparator() 
            + normalizedChild.substr(prefixLen);
    }
  }
}


string FileSystem::getSeparator() const
{
  return string(1, getSeparatorChar());
}


int FileSystem::canonicalize(const string& path, string& ret_path) const
{
  string tempPath = path;
  
  if (resolve(tempPath) < 0) {
    return -1;
  }
  
  File::getCanonicalPath(tempPath, ret_path);
  return 0;
}


int FileSystem::resolve(string& path) const
{
  string currentDir;
  if (isAbsolute(path)) {
    return 0;
  }
  
  if (getCurrentDirectory(currentDir) < 0) {
    return -1;
  }
  
  path = resolve(currentDir, path);
  return 0;
}

