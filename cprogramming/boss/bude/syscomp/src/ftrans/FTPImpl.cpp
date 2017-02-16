
#include <stdio.h>
#include "FTPImpl.h"
#include "FTPClientWrapper.h"

const int MAXFILELIMIT = 50;   


FTPImpl::FTPImpl()
{
  m_errorCode = 0;
  m_transfer = new FTPClientWrapper;
  /* 默认超时设为600秒 */
  /* setOption(TIMEOUT,600); */
  /* 默认传输数据类型为自动（这里即为二进制 */
  /* setOption(DATATYPE,AUTO); */
  /* 默认为非被动方式 */
  /* setOption(PASSIVE,YES); */
  /* 支持断点续传 */
  m_transfer->setOption(RESUME, YES);
  m_transfer->setOption(TIMEOUT, 60);
  m_transfer->setOption(MAXWAITTIME, 60); 
}

FTPImpl::~FTPImpl()
{
  if (m_transfer != NULL) {
    delete m_transfer;
    m_transfer = NULL;
  }
}

int FTPImpl::initialize()
{
  m_maxFileLimit = MAXFILELIMIT;
  
  return 0;
}

int FTPImpl::connect(const string & host, const string & port,
                     const string & user, const string & passwd)
{
  int ret;
  
  ret = (int)m_transfer->login(host.c_str(), port.c_str(), 
                               user.c_str(), passwd.c_str());
  if (ret < 0) {
    m_errorCode = ret;
    return -1;
  }

  m_transfer->setOption(DATATYPE, 1);   
    
  return 0;
}

int FTPImpl::disconnect()
{
  m_transfer->logout();
  return 0;
}

int FTPImpl::getFile(const string & source, const string & dest)
{
  int ret;
  
  m_transfer->setOption(TIMEOUT, 60);
  m_transfer->setOption(MAXWAITTIME, 10); 
  m_transfer->setOption(RESUME, YES);

  ret = (int)m_transfer->getFile(source.c_str(), dest.c_str());
  if (ret < 0) {
    m_errorCode = ret;
    return -1;
  }
  
  return 0; 
}

int FTPImpl::putFile(const string & dest, const string & source)
{
  int ret;
  
  m_transfer->setOption(TIMEOUT, 10);
  
  ret = (int)m_transfer->putFile(dest.c_str(), source.c_str());
  if (ret < 0) {
    m_errorCode = ret;
    return -1;
  }
  
  return 0;
}

int FTPImpl::setMode(const int & mode)
{
  int ret;
    
  ret = (int)m_transfer->setOption(DATATYPE, mode);
  if (ret < 0) {
    m_errorCode = ret;
    return -1;
  }
  
  return 0;
}

int FTPImpl::setOption(const int & type, const int & value)
{
  switch(type) {
    //传输文件数设置
    case MAXTRANSFILE:
      m_maxFileLimit = value; 
    default:
      printf(" option does not exits\n");
      m_errorCode = ERROR_FTPIMPL_OPTION;
      return -1;
  }

  return 0;
}

int FTPImpl::changeDirectory(const string & dir)
{
  int ret;

  ret = (int)m_transfer->executeCommand(CHDIR, dir.c_str());
  if (ret < 0) {
    m_errorCode = ret;
    return -1;
  }
  
  return 0; 
}

int FTPImpl::listDirectory(const string & dir, vector<string> & files,
                           const string & pattern)
{
  int ret;
  
  changeDirectory(dir);

  ret = (int)m_transfer->listFile(dir, files, pattern);
  
  if (ret < 0) {
    m_errorCode = ret;
    return -1;
  }

  if ((int)files.size() > m_maxFileLimit)
    files.resize(m_maxFileLimit);
        
  return 0;
}

int FTPImpl::getFileInfo(const string & file, unsigned long & size)
{
  size = m_transfer->m_fileInfo[file.substr(file.find_last_of('/')+1)];  
  return 0;
  
}

int FTPImpl::delFile(const string & file)
{
  int ret;
  
  ret = m_transfer->executeCommand(DEL, file.c_str());
  if (ret < 0) {
    m_errorCode = ret;   
    return -1;
  }
  
  return 0;
}

int FTPImpl::rename(const string & old_file, const string & new_file)
{
  int ret;
  
  ret = m_transfer->executeCommand(RENAME, old_file.c_str(), 
                                   const_cast<char *>(new_file.c_str()));
  if (ret < 0) {
    m_errorCode = ret;
    return -1;
  }

  delFile(old_file);

  return 0;

}

int FTPImpl::end()
{
  return 0;
}

void FTPImpl::getErrInfo(int & code, string & msg)
{
  code = m_errorCode;
  getErrorText(code, msg);
}

void FTPImpl::getErrorText(const int code, string & msg)
{
  switch(code) {
    case ERROR_FTPIMPL_OPTION:
      msg = "invalid ftpimpl option";
    default:
      m_transfer->getErrorText(code, msg);
  }
}

