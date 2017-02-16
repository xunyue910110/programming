#ifndef __DTAIMPL_H__
#define __DTAIMPL_H__

/**
 * \class DTAImpl
 * \brief 文件DTA协议实现(目前没有实现该协议)
 */

#include "FSystemAgent.h"

#include "dta/dta.h"

const int RESULT_LENG = 512;
const int CMD_LENG = 512;

class DTAImpl : public FSystemAgent
{
  public:
    DTAImpl();
    ~DTAImpl();

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
    virtual int getFileTime(const string & file, time_t & time){return -1;} ;//新增根据特定结束文件标识,获取此文件时间前的文件列表  add by xiel  2009.10.30
    virtual int delFile(const string & file);
    virtual int rename(const string & old_file, const string & new_file);         

    virtual int end();
    
    virtual void getErrInfo(int & code, string & msg);
    
  private:
    DTAHCONN m_hconn;
    DTAFTINFO m_ftinfo;
    bool m_bConnected;
    char result[1024*16][RESULT_LENG];
    mutable char rcommand[CMD_LENG];
    mutable int resultrow;
};

#endif //__DTAIMPL_H__
