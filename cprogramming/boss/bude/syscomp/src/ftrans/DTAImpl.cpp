
#include "DTAImpl.h"
#include "base/StringUtil.h"


const int MAXFILELIMIT = 50;


DTAImpl::DTAImpl()
{
  m_bConnected = false;
}

DTAImpl::~DTAImpl()
{
  m_bConnected = false;
}

int DTAImpl::initialize()
{
  m_bConnected = false;
  //if (DTA_INIT()) {
    //return -1;
  //}
  size_t i = 0;
  int val =30;
  DTA_SETOPT(DTAOPT_CONNTIMEO,&val,i);
  DTA_SETOPT(DTAOPT_RCVTIMEO,&val,i);
  DTA_SETOPT(DTAOPT_SNDTIMEO,&val,i);

  if (DTA_RESULT((char *)result, RESULT_LENG)) {
    return -1;
  }

  m_maxFileLimit = MAXFILELIMIT;

  return 0;
}

int DTAImpl::end()
{
  if (m_bConnected) {
    DTA_DISC(m_hconn);
  }
  m_bConnected = false;
  
  if (DTA_EXIT()) {
    return -1;
  }

  return 0;
}

int DTAImpl::connect(const string & host, const string & port,
                     const string & user, const string & passwd)
{
  if (m_bConnected) {
    DTA_DISC(m_hconn);
    m_bConnected = false;
  }
  
  if (DTA_CONN(&m_hconn, host.c_str())) {
    return -1;
  }
  m_bConnected = true;
  
  return 0;
}

int DTAImpl::disconnect()
{
  if (m_bConnected) {
    if (DTA_DISC(m_hconn)) {
      return -1;
    }
    m_bConnected = false;
  }

  return 0;
}

int DTAImpl::getFile(const string & source, const string & dest)
{
  DTAFTINFO ftinfo;
  
  if (DTA_GET(m_hconn, source.c_str(), dest.c_str(), &m_ftinfo)) {
    return -1;
  }
  
  return 0;
}

int DTAImpl::putFile(const string & dest, const string & source)
{
  DTAFTINFO ftinfo;

  if (DTA_PUT(m_hconn, dest.c_str(), source.c_str(), &ftinfo)) {
    return -1;
  }
  
  return 0;
}

int DTAImpl::setMode(const int & mode)
{
  switch(mode) {
    default:
//      printf("mode does not exits\n");
      return -1;
  }
    
  return 0;
}

int DTAImpl::setOption(const int & type, const int & value)
{
  switch(type) {
    //传输文件数设置
    case MAXTRANSFILE:
      m_maxFileLimit = value; 
    default:
//      printf(" option does not exits\n");
      return -1;
  }

  return 0;  
}

int DTAImpl::changeDirectory(const string & dir)
{
  return 0;
}

int DTAImpl::listDirectory(const string & dir, vector<string> & files,
                           const string & pattern)
{  
  int i,count;

  sprintf(rcommand,"ls -1rtp %s%s | awk -F/ \'{print $(NF)}\'",
          dir.c_str(), pattern.c_str());
  if (DTA_RCMD(m_hconn, rcommand, RESULT_LENG, &resultrow)) {
    return -1;
  }

  count = min(resultrow, m_maxFileLimit);
  
  files.clear();
  for (i = 0; i < count; ++i) {
    StringUtil::trim(result[i]);
    if (strlen(result[i]) > 0)
      files.push_back(result[i]);
  }
  
  return 0;
}


int DTAImpl::getFileInfo(const string & file, unsigned long & size)
{
  size = m_ftinfo.filesize;
  return size;
}


int DTAImpl::delFile(const string & file)
{
  sprintf(rcommand, "rm -f %s", file.c_str());
  if (DTA_RCMD(m_hconn, rcommand, RESULT_LENG, &resultrow)) {
    return -1;
  }
    
  return 0;
}


int DTAImpl::rename(const string & old_file,const string & new_file)
{
  if (delFile(new_file) < 0) {
    return -1;
  }
  
  sprintf(rcommand, "mv -f %s %s", old_file.c_str(), new_file.c_str());
  if (DTA_RCMD(m_hconn, rcommand, RESULT_LENG, &resultrow)) {
    return -1;
  }
  


  return 0;
}


void DTAImpl::getErrInfo(int & code, string & msg)
{
}
