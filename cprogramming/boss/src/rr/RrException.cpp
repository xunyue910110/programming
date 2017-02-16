#include "RrException.h"
#include <iostream>

RrException::RrException()
{
}

/*RrException::~RrException()
{
}
*/
RrException::RrException( char * errstr)
{
	m_msg = errstr;
}

RrException::RrException(char * filename, int linenum , const char * format ,...)
{
	m_filename = filename;
	m_linenum  = linenum;
	m_errCode  = 0;
	char *t_msg = new char[1024*2];
	va_list  pvar;
	va_start (pvar,format);
	vsprintf(t_msg,format,pvar);
	va_end (pvar);
	m_msg = t_msg;
	delete t_msg;
}

RrException::RrException(char * filename, int linenum ,int errCode, const char * format ,...)
{
	m_filename = filename;
	m_linenum  = linenum;
	m_errCode  = errCode;
	char *t_msg = new char[1024*2];
	va_list  pvar;
	va_start (pvar,format);
	vsprintf(t_msg,format,pvar);
	va_end (pvar);
	m_msg = t_msg;
	delete t_msg;
}

void RrException::toString()
{
  cout<<"File["<<m_filename<<":"<<m_linenum<<"]  ErrCode["
      <<m_errCode<<"] Msg[ "<<m_msg<<" ]"<<endl;
}
int   RrException::getErrcode()
{
  return m_errCode;
}
char* RrException::GetString()
{
  snprintf(m_errstr,sizeof(m_errstr)-1,
           "File[%12s:%4d] ErrCode[%d] Msg[ %s ]\n",
           m_filename.c_str(),
           m_linenum,
           m_errCode,
           m_msg.c_str());
  return m_errstr;
}




