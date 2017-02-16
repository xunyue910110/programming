#include "regexpr.hpp"

#include "pcre.h"

namespace archy {

reg_expr::reg_expr()
: m_NumFound(0)
, m_PReg(0)
{
}

string reg_expr::get_replacedstring(const string& strSrc)
{
	const char *src = strSrc.c_str();
	char c;
	int no;


	// First compute the length of the string
	int replacelen = 0;
	while ((c = *src++) != '\0') {
		if (c == '&')
			no = 0;
		else if (c == '\\' && isdigit(*src))
			no = *src++ - '0';
		else
			no = -1;

		if (no < 0) {	
			// Ordinary character. 
			if (c == '\\' && (*src == '\\' || *src == '&' ) )
				c = *src++;
			replacelen++;
		} 
		else if ( no <= count() ) {
			// Get tagged expression
			replacelen += sub_len(no);
		}
	}
		
	char* buf = new char[ replacelen + 1 ];
	char* tmp = buf;
	memset(buf, 0 , replacelen + 1);
	// Now we can create the string
	src = strSrc.c_str();
	while ((c = *src++) != '\0') {
		if (c == '&')
			no = 0;
		else if (c == '\\' && isdigit(*src))
			no = *src++ - ('0');
		else
			no = -1;

		if (no < 0) {	
			// Ordinary character. 
			if (c == '\\' && (*src == '\\' || *src == '&'))
				c = *src++;
			*buf++ = c;
		} 
		else if ( no <= count() ) {
			// Get tagged expression
			int len = sub_len(no);
			strncpy(buf, m_strSrc.c_str() + sub_start(no), len);
			buf[len] = 0;
			buf += len;
			if (len != 0 && *(buf-1) ==  '\0' ) {	
				/* strncpy hit NUL. */
				fprintf(stderr, "damaged match string\n");
				delete []tmp;
				return "";
			}
		}
	}
	printf("temp:%s\n", tmp);
	string szReplace(tmp);
	delete []tmp;
	return szReplace;
}
reg_expr::reg_expr(const string &pat, TCompile flags) throw (runtime_error)
: m_NumFound(0)
{
    const char *error;
    int erroffset;
    m_PReg = pcre_compile(pat.c_str(), flags, &error, &erroffset, NULL);
    if (m_PReg == NULL) {
        throw runtime_error(error);
    }
}


reg_expr::~reg_expr()
{
    (*pcre_free)((pcre*)m_PReg);
}


void reg_expr::set(const string &pat, TCompile flags)throw (runtime_error)
{
    if (m_PReg != NULL) {
        (*pcre_free)((pcre*)m_PReg);
    }

    const char *error;
    int erroffset;
    m_PReg = pcre_compile(pat.c_str(), flags, &error, &erroffset, NULL);
    if (m_PReg == NULL) {
        throw runtime_error(error);
    }
}

string reg_expr::operator[]( int i ) const
{
    if ( i >= m_NumFound){
        return "";
    }
    size_t len = m_Results[2 * i + 1] - m_Results[2 * i];
    return m_strSrc.substr(m_Results[2 * i], len);
}

// Return the starting offset of the ith matched substring from the 
// beginning of the string used in match().
int reg_expr::sub_start( int i ) const
{
	return m_Results[2 * i];
}

	
// Return the length of the ith matched substring
int reg_expr::sub_len( int i ) const
{
	return m_Results[2 * i + 1] - m_Results[2 * i];
}

bool reg_expr::match(const string& str, TMatch flags)
{
    m_NumFound = pcre_exec((pcre*)m_PReg, NULL, str.c_str(),  str.length(), 0,
        flags, m_Results, (kRegexpMaxSubPatterns +1) * 3);
    if (m_NumFound <= 0) {
        return false;
    } else {
    	m_strSrc = str;
        return true;
    }
}


int reg_expr::count() const
{
    return m_NumFound;
}



}
