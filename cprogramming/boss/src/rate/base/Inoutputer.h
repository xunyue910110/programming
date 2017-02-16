#ifndef _INOUTPUTER_H_INCLUDE_20080625_
#define _INOUTPUTER_H_INCLUDE_20080625_
#include "config-all.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <fstream> 
#include "base/PString.h"

class Inoutputer
{
  public:
    Inoutputer();
    virtual ~Inoutputer();
  public:
    //设置文件路径、文件名
    void  setName(const char *r_fileName,const char *r_path);
    void  setName(const char *r_fullName);
    virtual bool  open(const int &r_mode) ;
    bool  is_open();
    bool  close() ;
    bool  unlinkFile() const;
    bool  linkFile(const char *r_desFileName,const char *r_desPath) const;
    bool  linkFile(const char *r_desFullName) const;
    bool  renameFile(const char *r_desFileName,const char *r_desPath) const;
    bool  renameFile(const char *r_desFullName) const;
    // 返回值：  -1－错误，0－文件结束，1－成功
    int   read(char * r_cdr,const int &r_buffLen,const char &r_lineDelimiter) ;
    bool  write(const char *r_cdr) ;
    const char* getFileName();
    const char* getFullFileName();
    //add by gaojf 2009-2-21 11:04
    bool  copyFile(const char *r_desFullName) const;
  public:
    fstream   m_fstream;
  protected:
    PString   m_sFileName;//不带路径的文件名
    PString   m_pathName; //路径
    PString   m_fullName; //全路径文件名
    bool      m_openFlag; //打开标记
    int       m_openMode; //打开模式
  private:
    int   copy(const char *r_srcFile,const char *r_desFile) const;
};
#endif //_INOUTPUTER_H_INCLUDE_20080625_
