#include "CException.h"
#include <typeinfo>
#include <sstream>

CException::CException(int code)
    : m_File(""),m_Line(-1),m_Nested(0), m_Code(code)
{
}


CException::CException(const string & file, int line,const std::string& msg, int code)
    :m_File(file),m_Line(line),m_Msg(msg), m_Nested(0), m_Code(code)
{
}


CException::CException(const string & file, int line,const std::string& msg, const std::string& arg, int code)
    : m_File(file),m_Line(line),m_Msg(msg),m_Nested(0), m_Code(code)
{
	if (!arg.empty())
	{
		m_Msg.append(": ");
		m_Msg.append(arg);
	}
}


CException::CException(const string & file, int line,const std::string& msg, const CException& nested, int code)
    :  m_File(file),m_Line(line),m_Msg(msg), m_Nested(nested.clone()), m_Code(code)
{
}


CException::CException(const CException& exc):
	std::exception(exc),
	m_File(exc.m_File),
	m_Line(exc.m_Line),
	m_Msg(exc.m_Msg),
	m_Code(exc.m_Code)
{
	m_Nested = exc.m_Nested ? exc.m_Nested->clone() : 0;
}

	
CException::~CException() throw()
{
	delete m_Nested;
}


CException& CException::operator = (const CException& exc)
{
	if (&exc != this)
	{
		delete m_Nested;
		m_File   = exc.m_File;
		m_Line   = exc.m_Line;
		m_Msg    = exc.m_Msg;
		m_Nested = exc.m_Nested ? exc.m_Nested->clone() : 0;
		m_Code   = exc.m_Code;
	}
	return *this;
}


const char* CException::name() const throw()
{
	return "CException";
}


const char* CException::className() const throw()
{
	return typeid(*this).name();
}

	
const char* CException::what() const throw()
{
	return name();
}

	
std::string CException::toString() const
{
	std::string txt = name();
	if (!m_Msg.empty())
	{
		txt.append(": ");
		txt.append(m_File);
		txt.append(": ");
		ostringstream oss;
		oss << m_Line;
		txt.append(oss.str());
		txt.append(m_Msg);
	}
	return txt;
}


CException* CException::clone() const
{
	return new CException(*this);
}


void CException::rethrow() const
{
	throw *this;
}


IMPLEMENT_CEXCEPTION(AppException, CException, "Application exception")

IMPLEMENT_CEXCEPTION(ScriptException, CException, "Script exception")

