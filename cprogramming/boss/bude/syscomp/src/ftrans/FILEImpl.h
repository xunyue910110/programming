#ifndef __FILEIMPL_H__
#define __FILEIMPL_H__

/**
 * \class FILEImpl 
 * \brief 文件FILE协议实现
 *
 * 该FILE协议代表连接的远程系统为本地.通过该协议实现对本地文件的访问.
 */



#include "FSystemAgent.h"
#include "base/Directory.h"


class FILEImpl : public FSystemAgent
{
  public:
    FILEImpl();
    ~FILEImpl();

    virtual int initialize();

    virtual int connect(const string & host, const string & port,
                        const string & user, const string & passwd);
    virtual int disconnect();
    virtual int getFile(const string & source, const string & dest); 
    virtual int putFile(const string & dest, const string & source);
    virtual int setMode(const int & mode);
    virtual int setOption(const int & type, const int & value);

    virtual int changeDirectory(const string & dir);
    virtual int listDirectory(const string & dir, vector<string> & files,
                              const string & pattern);
    virtual int getFileInfo(const string & file, unsigned long & size);
    virtual int getFileTime(const string & file, time_t & time) ;//新增根据特定结束文件标识,获取此文件时间前的文件列表  add by xiel  2009.10.30
    virtual int delFile(const string & file);
    virtual int rename(const string & old_file, const string & new_file);         
    
    virtual int end();
    virtual void getErrInfo(int & code, string & msg); 

  public:
    void setFileSortMethod(Directory::SortFlags = Directory::SF_NONE);

  private:
    int getFile(char *path, const int length);
    int getFile(char *path, const int length , unsigned long & filesize); 
    
  private:
    Directory m_dir;  
    Directory::SortFlags m_fSortMethod;     
};

#endif //__FILEIMPL_H__
