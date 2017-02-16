#ifndef __CEXCEPTION_H
#define __CEXCEPTION_H
#include <exception>
#include <string>
using namespace std;

class CException: public std::exception
{
public:
	CException(const string & file, int line,const std::string& msg, int code = 0);

	CException(const string & file, int line,const std::string& msg, const std::string& arg, int code = 0);

	CException(const string & file, int line,const std::string& msg, const CException& nested, int code = 0);

	CException(const CException& exc);
		
	~CException() throw();

	CException& operator = (const CException& exc);

	virtual const char* name() const throw();
		
	virtual const char* className() const throw();
			
	virtual const char* what() const throw();
		
	const CException* nested() const;
				
	const std::string& message() const;
	
	int code() const;
		
	std::string toString() const;

	virtual CException* clone() const;

	virtual void rethrow() const;

protected:
	CException(int code = 0);
		
private:
	string m_File;

    int m_Line;
	
	std::string m_Msg;
	    
	CException*  m_Nested;
	
	int			m_Code;
};

inline const CException* CException::nested() const
{
	return m_Nested;
}


inline const std::string& CException::message() const
{
	return m_Msg;
}


inline int CException::code() const
{
	return m_Code;
}


#define DECLARE_CEXCEPTION(CLS, BASE) \
	class CLS: public BASE														\
	{																				\
	public:																			\
		CLS(int code = 0);															\
		CLS(const string & file, int line,const std::string& msg, int code = 0);									\
		CLS(const string & file, int line,const std::string& msg, const std::string& arg, int code = 0);			\
		CLS(const string & file, int line,const std::string& msg, const CException& exc, int code = 0);		\
		CLS(const CLS& exc);														\
		~CLS() throw();																\
		CLS& operator = (const CLS& exc);											\
		const char* name() const throw();											\
		const char* className() const throw();										\
		CException* clone() const;												\
		void rethrow() const;														\
	};


#define IMPLEMENT_CEXCEPTION(CLS,BASE, NAME)													\
	CLS::CLS(int code): BASE(code)																	\
	{																								\
	}																								\
	CLS::CLS(const string & file, int line,const std::string& msg, int code): BASE(file,line,msg, code)										\
	{																								\
	}																								\
	CLS::CLS(const string & file, int line,const std::string& msg, const std::string& arg, int code): BASE(file,line,msg, arg, code)		\
	{																								\
	}																								\
	CLS::CLS(const string & file, int line,const std::string& msg, const CException& exc, int code): BASE(file,line,msg, exc, code)	\
	{																								\
	}																								\
	CLS::CLS(const CLS& exc): BASE(exc)																\
	{																								\
	}																								\
	CLS::~CLS() throw()																				\
	{																								\
	}																								\
	CLS& CLS::operator = (const CLS& exc)															\
	{																								\
		BASE::operator = (exc);																		\
		return *this;																				\
	}																								\
	const char* CLS::name() const throw()															\
	{																								\
		return NAME;																				\
	}																								\
	const char* CLS::className() const throw()														\
	{																								\
		return typeid(*this).name();																\
	}																								\
	CException* CLS::clone() const																\
	{																								\
		return new CLS(*this);																		\
	}																								\
	void CLS::rethrow() const																		\
	{																								\
		throw *this;																				\
	}


DECLARE_CEXCEPTION(AppException, CException)
DECLARE_CEXCEPTION(ScriptException, CException)

#define THROW_EXP(expclass, msg) throw expclass(__FILE__, __LINE__, msg,-1)
#define THROW_EXP_C(expclass, msg, code) throw expclass(__FILE__, __LINE__, msg,code)
#define THROW_EXP_ARG(expclass, msg, arg) throw expclass(__FILE__, __LINE__, msg,arg,-1)
#define THROW_EXP_ARG_C(expclass, msg, arg,code) throw expclass(__FILE__, __LINE__, msg,arg,code)


#endif // __CEXCEPTION_H
