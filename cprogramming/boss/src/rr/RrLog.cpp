#include "RrLog.h"

void RrLog::clear()
{
  m_totalCdr=m_errCdr=m_uniqCdr=m_repeatInFile=m_repeatHis=0;
}
//##ModelId=489A99DF02AF
void RrLog::getLogStr()
{
}


RrFileLog::RrFileLog()
{
}
RrFileLog::~RrFileLog()
{
}
RrFileLog::RrFileLog(const RrFileLog &r_right)
{
  *this = r_right;
}
RrFileLog& RrFileLog::operator=(const RrFileLog &r_right)
{
  m_fileName = r_right.m_fileName;
  m_totalCdr = r_right.m_totalCdr;
  return *this;
}

void  RrMergLog::addFileLog(const char *r_fileName,size_t r_cdrNum)
{
  size_t t_pos=m_srcFileList.size();
  //if(t_pos==0)
  //{
  //  m_fileName_merg = r_fileName;
  //}
  m_srcFileList.push_back(RrFileLog());
  m_srcFileList[t_pos].m_fileName=r_fileName;
  m_srcFileList[t_pos].m_totalCdr=r_cdrNum;
}
void  RrMergLog::clear()
{
  m_srcFileList.clear();
}
