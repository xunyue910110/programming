#include <fcntl.h>
#include <stdio.h>
#include <time.h>

#include <algorithm>

#include "File.h"
//#include "Utility.h"
#include "FileSystem.h"
#include <vector>
#include <iostream>

File::File(const string& pathname) :
  m_fileSystem(FileSystem::getFileSystem())
{
  m_path = m_fileSystem->normalize(pathname);
  m_prefixLength = m_fileSystem->getPrefixLength(m_path);
}


File::File(const File& parent, const string& child) :
  m_fileSystem(FileSystem::getFileSystem())
{
  m_path = m_fileSystem->resolve(parent.getPath(), child);
  m_prefixLength = m_fileSystem->getPrefixLength(m_path);
}


File::File(const string& parent, const string& child) :
  m_fileSystem(FileSystem::getFileSystem())
{
  m_path = m_fileSystem->resolve(parent, child);
  m_prefixLength = m_fileSystem->getPrefixLength(m_path);
}


File::File(const File& rhs) :
  m_fileSystem(rhs.m_fileSystem),
  m_path(rhs.m_path),
  m_prefixLength(rhs.m_prefixLength)
{
}
  

bool File::canRead() const
{
  return m_fileSystem->checkAccess(m_path, FileSystem::ReadAccess);
}


bool File::canWrite() const
{
  return m_fileSystem->checkAccess(m_path, FileSystem::WriteAccess);
}


int File::deleteFile() const
{
  return m_fileSystem->deleteFile(m_path);
}


bool File::exists() const
{
  return (m_fileSystem->getFileAttributeFlags(m_path) & FileSystem::Exists);
}


bool File::isDirectory() const
{
  return (( m_fileSystem->getFileAttributeFlags(m_path) &
            FileSystem::Directory)!=0);
}


bool File::isFile() const
{
  return ((m_fileSystem->getFileAttributeFlags(m_path) & 
           FileSystem::RegularFile)!=0);
}


size_t File::length() const
{
  return m_fileSystem->getLength(m_path);
}


int File::getAbsolutePath(string& path) const
{
  path = m_path;

  if (isAbsolute()) { 
    return 0;
  }

  if (m_fileSystem->resolve(path) < 0) {
    return -1;
  }
  
  return 0;
  
}


int File::getCurrentDirectory(string & path)
{
  return FileSystem::getFileSystem()->getCurrentDirectory(path);
}


int File::getCanonicalPath(string& path) const
{
  if (getAbsolutePath(path) < 0)
    return -1;
  
  const string tempPath = path;

  if (m_fileSystem->canonicalize(tempPath, path) < 0)
    return -1;
  
  return 0;
}


void File::getCanonicalPath(const string& path, string& ret_path)
{
  //
  // Deal with the empty string right away
  //
  if (path.empty()) {
    ret_path = string();
    return;
  }

  //
  // The prefix is copied to the output unchanged
  //
  size_t prefixLen = FileSystem::getFileSystem()->getPrefixLength(path);
  string ret = path.substr(0, prefixLen);

  //
  // The remainder of the path represents a sequence of names
  // separated by the path separator character.
  //
  string sequence;
  string tempPath = path.substr(prefixLen);
  const string separater = getSeparator();
  std::vector<string> tokens;
  size_t pos;

  tokens.clear();
  while ((pos = tempPath.find_first_of(separater)) != string::npos) {
    tokens.push_back(tempPath.substr(0, pos));
    tempPath = tempPath.substr(pos+1);
  }

  if (!tempPath.empty()) {
    tokens.push_back(tempPath);
  }

  for (int i = 0; i < (int)tokens.size(); i++)
  {
    const string& name = tokens[i];
    if (name == (".")) {
      continue;
    }
    else if (name == ("..")) {
      size_t pos = sequence.find_last_of(separater);
      if (pos != string::npos) {
        sequence.erase(pos);
      }
      else {
        // This is an error.  We have a ".." with no preceding
        // name.  We've gone back as far as we could!
        sequence += separater;
        sequence += name;
      }
    }
    else {
      sequence += separater;
      sequence += name;
    }
  }
  
  if (!sequence.empty()) {
    //
    // erase the trailing separator from the prefix (if any)
    //
    if (prefixLen && ret[prefixLen-1] == separater[0]) {
      ret.erase(prefixLen-1);
    }
    ret += sequence;
  }

  ret_path = ret;
}


string File::getName() const
{
  size_t sepPos = m_path.find_last_of(getSeparatorChar());
  if (sepPos != string::npos)
  {
    return m_path.substr(sepPos+1);
  }
  else
  {
    return m_path.substr(m_prefixLength);
  }
}


string File::getParent() const
{
  string absPath;
//  if (getAbsolutePath(absPath) < 0) {
  if (getCanonicalPath(absPath) < 0) {    
    return string();
  }
  
  size_t namePos = absPath.find_last_of(getSeparatorChar());
  if (namePos != string::npos) {
    if (namePos < m_prefixLength) {
      namePos = m_prefixLength;
    }
    return absPath.substr(0, namePos);
  }

  return string();

}


File File::getParentFile() const
{
  return File(getParent());
}


string File::getPath() const
{
  return m_path;
}


bool File::isAbsolute() const
{
  return m_fileSystem->isAbsolute(m_path);
}


int File::listDirectory(std::list<string> & file_list) const
{
  file_list.clear();
  
  if (isDirectory()) {
    return m_fileSystem->listDirectory(m_path, file_list);
  }

  return -1;
}


int File::mkdir() const
{
  return m_fileSystem->createDirectory(m_path);
}


int File::mkdirs() const
{
  //
  // tokenize the pathname excluding the prefix, and turn it into
  // a vector;
  //
  string tempPath = m_path.substr(m_prefixLength);

  const string separater = getSeparator();
  std::vector<string> tokens;
  size_t pos;

  tokens.clear();
  while ((pos = tempPath.find_first_of(separater)) != string::npos) {
    tokens.push_back(tempPath.substr(0, pos));
    tempPath = tempPath.substr(pos+1);
  }

  if (!tempPath.empty()) {
    tokens.push_back(tempPath);
  }

  //
  // For each token in the path, check if the directory exists.
  // and if not ask the FileSystem to create it.
  //
  string path = m_path.substr(0, m_prefixLength);
  for (size_t i = 0; i < tokens.size(); i++)
  {   
    path = path + tokens[i] + getSeparator();

    int fileAttr = m_fileSystem->getFileAttributeFlags(path);

    if (fileAttr && FileSystem::Exists) {
      // If the path exists, but isn't a directory then we have an
      // exceptional case
      if (! (fileAttr && FileSystem::Directory)) {
        return -1;
      }
    }
    else {
      if (m_fileSystem->createDirectory(path) < 0) {
        return -1;
      }
    }
  }
  
  return 0;
}


int File::renameTo(const File& file) const
{
  if (m_fileSystem->rename(m_path, file.getPath()) < 0)
    return -1;
  
  return 0;
}


char File::getSeparatorChar()
{
  return FileSystem::getFileSystem()->getSeparatorChar();
}


string File::getSeparator()
{
  return FileSystem::getFileSystem()->getSeparator();
}


string File::normalizePath(const string& path)
{
  string ret = path;

  if ( !ret.empty()) {   
    const char sep = getSeparatorChar();
    const char badSep = (sep == '/') ? '\\' : '/';
    std::replace(ret.begin(), ret.end(), badSep, sep);

    // Replace strings of separators with a single separator
    // starting at position 0 for UNIX and 1 for Windows!
    size_t pos = (sep == '\\') ? 1 : 0;
    const char doubleSep[] = {sep, sep, 0};
    while(pos != string::npos) {
      pos = ret.find(doubleSep, pos);
      if (pos != string::npos) {
        ret.erase(pos, 1);
      }
    }

    //
    // Remove terminating separator if any
    //
    size_t prefixLen = FileSystem::getFileSystem()->getPrefixLength(ret);
    if ((sep == ret[ret.length()-1]) && (prefixLen < ret.length()))
    {
      ret = ret.erase(ret.length()-1);
    }    
  }
  
  return ret;
}


int File::getLastModified(time_t& time) const
{
  if (m_fileSystem->getLastModifiedTime(m_path, time) < 0)
    return -1;
  
  return 0;
}


int File::setLastModified(const time_t& time) const
{
  if (m_fileSystem->setLastModifiedTime(m_path, time) < 0)
    return -1;

  return 0;
}


int File::setReadOnly(bool bReadOnly) const
{
  if (m_fileSystem->setReadOnly(m_path, bReadOnly) < 0)
    return -1;

  return 0;
}


int File::createNewFile() const
{
  // File will automatically be closed as the file descriptor goes
  // out of scope
  if (m_fileSystem->openFile(m_path, FileSystem::WriteAccess,
                             FileSystem::OpenCreateExclusive, 0) < 0)
  {
    return -1;
  }
  
  return 0;
}


File& File::operator=(const File& rhs)
{
  if (this != &rhs) {
    m_fileSystem = rhs.m_fileSystem;
    m_path = rhs.m_path;
    m_prefixLength = rhs.m_prefixLength;
  }
    
  return *this;
}


int File::concateFile(const string& file1, const string& file2)
{
  return concateFile(File(file1), File(file2));  
}


int File::concateFile(const File& file1, const File& file2)
{ 
  if (  !file1.isFile() || !file2.isFile()
      || !file1.canRead() || !file2.canRead() )
  {
    return -1;
  }

  File parent(getParentFile());

  if ( !parent.exists() ) {
    if (parent.mkdirs() < 0) {
      return -1;
    }
  }
  
  string path1,path2,path;  
  /* 获取文件路径 */
  if (  file1.getCanonicalPath(path1) < 0
      || file2.getCanonicalPath(path2) < 0
      || getCanonicalPath(path) < 0 )
  {
    return -1;
  }
  
  /* 打开文件 */
  int f1,f2,f;  
  if (  (f1 = m_fileSystem->openFile(path1, FileSystem::ReadAccess, 
                                     FileSystem::OpenExisting, 0)) < 0
      || (f2 = m_fileSystem->openFile(path2, FileSystem::ReadAccess, 
                                      FileSystem::OpenExisting, 0)) < 0 
      || (f = m_fileSystem->openFile(path, FileSystem::WriteAccess, 
                                     FileSystem::OpenCreateTruncate, 0)) < 0 )
  {
    return -1;
  }

  /* 合并文件 */
  Byte strBuff[1000];
  int bytesRead;  

  while ((bytesRead = m_fileSystem->readFile(f1, strBuff, sizeof(strBuff))) > 0)
  {
    if (m_fileSystem->writeFile(f, strBuff, bytesRead) < 0) {
      return -1;
    }
  }

  while ((bytesRead = m_fileSystem->readFile(f2, strBuff, sizeof(strBuff))) > 0)
  { 
    if (m_fileSystem->writeFile(f, strBuff, bytesRead) < 0) {
      return -1;
    }
  }

  /* 关闭文件 */
  m_fileSystem->closeFile(f1);
  m_fileSystem->closeFile(f2);
  m_fileSystem->closeFile(f);
  
  return 0;

}


int File::concateFile2(const File& file1)
{ 
  if ( !file1.isFile() || !file1.canRead() )
  {
    return -1;
  }

  File parent(getParentFile());

  if ( !parent.exists() ) {
    if (parent.mkdirs() < 0) {
      return -1;
    }
  }
  
  string path1,path;  
  /* 获取文件路径 */
  if (  file1.getCanonicalPath(path1) < 0
      || getCanonicalPath(path) < 0 )
  {
    return -1;
  }
  
  /* 打开文件 */
  int f1,f;  
  if (  (f1 = m_fileSystem->openFile(path1, FileSystem::ReadAccess, 
                                     FileSystem::OpenExisting, 0)) < 0
      || (f = m_fileSystem->openFile(path, FileSystem::WriteAccess, 
                                     FileSystem::OpenCreateAppend, 0)) < 0 )
  {
    return -1;
  }

  /* 合并文件 */
  Byte strBuff[8192+1];
  int bytesRead;  

  memset(strBuff,0,sizeof(strBuff));
  
  while ((bytesRead = m_fileSystem->readFile(f1, strBuff, sizeof(strBuff)-1)) > 0)
  {
    //cout << "Read strbuff,bytesRead=" << bytesRead <<endl;
    if (m_fileSystem->writeFile(f, strBuff, bytesRead) < 0) {
      return -1;
    }
  }

  /* 关闭文件 */
  m_fileSystem->closeFile(f1);
  m_fileSystem->closeFile(f);
  
  return 0;

}


void File::dump(ostream & os)
{
  string absPath,canonicalPath;
  
  getAbsolutePath(absPath);
  os << "the absolute path is " << absPath << endl; 
  getCanonicalPath(canonicalPath);  
  os << "the canonical path is " << canonicalPath << endl;
  os << (exists()?("exists"):("not exists")) << endl;
  os << (canRead()?("can read"):("cannot read")) << endl;
  os << (canWrite()?("can write"):("cannot write")) << endl;
  os << "name is " << getName() << endl;
  os << "parent is " << getParent() << endl;
  os << "path is " << getPath() << endl;
  os << "the size of file is " << length() << endl;
  os << (isFile()?("is file"):("is not file")) << endl;
  time_t last_mod_time;
  getLastModified(last_mod_time);
  os << "the last modified time is " << ctime(&last_mod_time);
  os << getSeparator() << endl;

  string currentDir;  
  getCurrentDirectory(currentDir);
  cout << "current directory is " << currentDir << endl;  

//  cout << Utility::compareNoCase("dsfdfsdf","DSFDFSDF") << endl;  
}

