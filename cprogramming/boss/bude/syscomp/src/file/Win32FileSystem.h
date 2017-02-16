
#ifndef __WIN32FILESYSTEM_H__
#define __WIN32FILESYSTEM_H__

/**
 * \class Win32FileSystem 
 * \brief WIN32文件系统的实现
 *
 */

#include <list>
#include <string>

#include "FileSystem.h"

class Win32FileSystem : public FileSystem {
  public:
  
    virtual int getCurrentDirectory(string& path) const;
    virtual char getSeparatorChar() const;
    virtual size_t getPrefixLength(const string& path) const;
    virtual bool isAbsolute(const string& path) const;
    virtual bool isCaseSensitive() const;
    virtual int canonicalize(const string& path, string& ret_path) const;
    
    virtual int getFileAttributeFlags(const string& path) const;
      
    virtual bool checkAccess(const string& path, AccessMode mode) const;
  
    virtual int getLastModifiedTime(const string& path, time_t& time) const;
    virtual size_t getLength(const string& path) const;
  
    virtual int openFile(const string& path,
                         const int accessMode,
                         const CreationDisp creationDisp,
                         const int attributes) const;
  
    virtual int closeFile(int pFD) const;
    virtual int deleteFile(const string& path) const;
    virtual int listDirectory(const string& path,
                              std::list<string> & file_list) const;
    virtual int createDirectory(const string& path) const;
    virtual int rename(const string& old_path, const string& new_path) const;
    virtual int setLastModifiedTime(const string& path, 
                                    const time_t& time) const;
    virtual int setReadOnly(const string& path, const bool bReadOnly) const;
  
    virtual int readFile(int fd, Byte* pBuffer, size_t bufLen) const;
    virtual int writeFile(int fd, const Byte* pBuffer, size_t bufLen) const;

};

#endif //__WIN32FILESYSTEM_H__
