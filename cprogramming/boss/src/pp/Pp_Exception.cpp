// Exception.cpp: implementation of the Exception class.
//
//////////////////////////////////////////////////////////////////////
#include "Pp_Exception.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Pp_Exception::Pp_Exception()
{}



//rem by chenzm for hp-ux
//Pp_Exception::~Pp_Exception()
//{}




Pp_Exception::Pp_Exception( char * errstr)
{
	m_msg.push_back(ExceptionMsgPp(0,"__FILE__",errstr));
}

Pp_Exception::Pp_Exception(char * filename, int linenum , const char * format ,...)
{
	char *ls_str = new char[1024 * 10];
	va_list  pvar;
	va_start (pvar, format);
	vsprintf(ls_str,format,pvar);
	va_end(pvar);
	//ExceptionMsgPp e(filename,linenum,"%s",ls_str);
	ExceptionMsgPp e(linenum,filename,ls_str);
	m_msg.push_back(e);
	delete ls_str;
}

void Pp_Exception::toString()
{
	vector<ExceptionMsgPp>::iterator it;
	for ( it = m_msg.begin(); it != m_msg.end(); it++)
		printf("File[%12s:%4d] Msg[ %s ]\n",it->m_filename,it->m_linenum,it->m_msg);
}

void Pp_Exception::Push(ExceptionMsgPp msg)
{
	m_msg.push_back(msg);
}

char* Pp_Exception::GetString()
{
        vector<ExceptionMsgPp>::iterator it;
        char *pstr = new char[1024];
        int curpos = 0;
        int leftsize = sizeof(m_errstr) -1;
        int length = 0;
	for ( it = m_msg.begin(); it != m_msg.end(); it++)
        {
                sprintf(pstr,"File[%12s:%4d] Msg[ %s ]\n",it->m_filename,it->m_linenum,it->m_msg);
                length = strlen(pstr);
                leftsize -= length;
                if (leftsize < 0)
                        break;
                strcpy(&m_errstr[curpos], pstr);
                curpos += length;
        }
        delete pstr;
        return m_errstr;
}

ExceptionMsgPp::ExceptionMsgPp(char * filename, int linenum , char * format, ...)
{
	strcpy(m_filename,filename);
	m_linenum = linenum;
	char *msg = new char[1024*2];
	va_list  pvar;
	va_start (pvar,format);
	vsprintf(msg,format,pvar);
	va_end (pvar);
	strncpy(m_msg,msg,sizeof(m_msg)-20);
	if (strlen(msg) > sizeof(m_msg)-20)
	{
		strcat(m_msg,"[[msg超长]]");
	}
	delete msg;
}



