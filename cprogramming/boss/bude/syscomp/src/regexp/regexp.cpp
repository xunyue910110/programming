/*  
 * ===========================================================================
 *
 * Author: Clifford Clausen
 * File Description:
 *         C++ wrappers for Perl Compatible Regular Expression (pcre) library
 *
 * ===========================================================================
 */



#include <regexp/regexp.h>
#include <fnmatch.h>

#include <memory>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>



/// Non constant version of iterate macro.
///
/// This upper case style is prefered over the "non_const_iterate" macro.
/// @sa
///   non_const_iterate
#define NON_CONST_ITERATE(Type, Var, Cont) \
    for ( Type::iterator Var = (Cont).begin();  Var != (Cont).end();  ++Var )

template <typename T>
string s_NStr_Join(const T& arr, const string& delim)
{
    if (arr.empty()) {
        return kEmptyStr;
    }

    string                       result = arr.front();
    typename T::const_iterator   it     = arr.begin();
    size_t needed = result.size();
    while (++it != arr.end()) {
        needed += delim.size() + it->size();
    }
    result.reserve(needed);
    it = arr.begin();
    while (++it != arr.end()) {
        result += delim;
        result += *it;
    }
    return result;
}



string NStr_Join(const list<string>& arr, const string& delim)
{
    return s_NStr_Join(arr, delim);
}



//////////////////////////////////////////////////////////////////////////////
//
//  Regexp
//

Regexp::Regexp(const string& pattern, TCompile flags)
    : m_NumFound(0)
{
    const char *err;
    int err_offset;
    m_PReg = pcre_compile(pattern.c_str(), flags, &err, &err_offset, NULL);
    if (m_PReg == NULL) {
        //_THROW(RegexpException, eCompile, "Compilation of the pattern '" +
        //           pattern + "' failed: " + err);
    }
}


Regexp::~Regexp()
{
    (*pcre_free)(m_PReg);
}


void Regexp::Set(const string& pattern, TCompile flags)
{
    if (m_PReg != NULL) {
        (*pcre_free)(m_PReg);
    }
    const char *err;
    int err_offset;
    m_PReg = pcre_compile(pattern.c_str(), flags, &err, &err_offset, NULL);
    if (m_PReg == NULL) {
        //_THROW(RegexpException, eCompile, "Compilation of the pattern '" +
        //           pattern + "' failed: " + err);
    }
}


string Regexp::GetSub(const string& str, size_t idx) const
{
    int start = m_Results[2 * idx];
    int end   = m_Results[2 * idx + 1];

    if ((int)idx >= m_NumFound  ||  start == -1  ||  end == -1) {
        return kEmptyStr;
    }
    return str.substr(start, end - start);
}


string Regexp::GetMatch(
    const string& str,
    int           offset,
    size_t        idx,
    TMatch        flags,
    bool          noreturn)
{
    m_NumFound = pcre_exec(m_PReg, NULL, str.c_str(), (int)str.length(),
                           (int)offset, flags, m_Results,
                           (int)(kRegexpMaxSubPatterns +1) * 3);
    if ( noreturn ) {
        return kEmptyStr;
    } else {
        return GetSub(str, idx);
    }
}



//////////////////////////////////////////////////////////////////////////////
//
//  RegexpUtil
//

RegexpUtil::RegexpUtil(const string& str) 
    : m_Content(str), m_IsDivided(false),
      m_RangeStart(kEmptyStr), m_RangeEnd(kEmptyStr), m_Delimiter("\n")
{
    return;
}


void RegexpUtil::SetRange(
        const string& addr_start,
        const string& addr_end,
        const string& delimiter)
{
    m_RangeStart = addr_start;
    m_RangeEnd   = addr_end;
    x_Divide(delimiter);
}


size_t RegexpUtil::Replace(
    const string&     search,
    const string&     replace,
    Regexp::TCompile compile_flags,
    Regexp::TMatch   match_flags,
    size_t            max_replace)
{
    if ( search.empty() ) {
        return 0;
    }
    size_t n_replace = 0;

    // Fill shure that string is not divided.
    x_Join();

    // Compile regular expression.
    Regexp re(search, compile_flags);
    size_t  start_pos = 0;

    for (size_t count = 0; !(max_replace && count >= max_replace); count++) {

        // Match pattern.
        re.GetMatch(m_Content.c_str(), (int)start_pos, 0, match_flags, true);
        int num_found = re.NumFound();
        if (num_found <= 0) {
            break;
        }

        // Substitute all subpatterns "$<digit>" to values in the "replace"
        // string.
        const int* result;
        string     x_replace = replace;
        size_t     pos = 0;

        for (;;) {
            // Find "$"
            pos = x_replace.find("$", pos);
            if (pos == string::npos) {
                break;
            }
            // Try to convert string after the "$" to number
            errno = 0;
            const char* startptr = x_replace.c_str() + pos + 1;
            char* endptr = 0;
            long value = strtol(startptr, &endptr, 10);

            if ( errno  ||  endptr == startptr  ||  !endptr  ||
                 value < INT_MIN  ||  value > INT_MAX) {
                // Format error, skip single "$".
                pos++;
                continue;

            }
            int n = (int)value;

            // Get subpattern value
            string subpattern;
            if ( n > 0  &&  n < num_found ) {
                result = re.GetResults(n);
                if (result[0] >= 0  &&  result[1] >= 0) {
                    subpattern = m_Content.substr(result[0],
                                                  result[1] - result[0]);
                }
            }

            // Check braces {$...}
            size_t sp_start = pos;
            size_t sp_end   = endptr - x_replace.c_str();
            if ( sp_start > 0  &&  x_replace[sp_start-1] == '{') {
                sp_start--;
                if ( sp_end <  x_replace.length()  &&
                     x_replace[sp_end] == '}') {
                    sp_end++;
                } else {
                    // Format error -- missed closed brace.
                    sp_start++;
                }
            }
            // Replace $n with subpattern value.
            x_replace.replace(sp_start, sp_end - sp_start, subpattern);

            pos += subpattern.length();
        }

        // Replace pattern with "x_replace".
        result = re.GetResults(0);
        m_Content.replace(result[0], result[1] - result[0], x_replace);
        n_replace++;
        start_pos = result[0] + max(x_replace.length(), (size_t) 1);
    }
    return n_replace;
}


size_t RegexpUtil::ReplaceRange(
    const string&       search,
    const string&       replace,
    Regexp::TCompile   compile_flags,
    Regexp::TMatch     match_flags,
    RegexpUtil::ERange process_inside,
    size_t              max_replace
    )
{
    if ( search.empty() ) {
        return 0;
    }
    size_t n_replace = 0;

    // Split source string to parts by delimiter
    x_Divide();

    // Flag which denote that current line is inside "range"
    bool inside = m_RangeStart.empty();

    NON_CONST_ITERATE (list<string>, i, m_ContentList) {
        // Get new line
        string line = *i;

        // Check beginning of block [addr_re_start:addr_re_end]
        if ( !inside  &&  !m_RangeStart.empty() ) {
            Regexp re(m_RangeStart.c_str());
            re.GetMatch(line.c_str(), 0, 0, 0, true);
            inside = (re.NumFound() > 0);
        } else {
            inside = true;
        }

        // Process current line
        if ( (inside  &&  process_inside == eInside)  ||
             (!inside  &&  process_inside == eOutside) ) {
            RegexpUtil re(line);
            n_replace += re.Replace(search, replace,
                                    compile_flags, match_flags, max_replace);
            *i = re;
        }

        // Check ending of block [addr_re_start:addr_re_end]
        if ( inside  &&  !m_RangeEnd.empty() ) {
            // Two addresses
            Regexp re(m_RangeEnd.c_str());
            re.GetMatch(line.c_str(), 0, 0, 0, true);
            inside = (re.NumFound() <= 0);
        } else {
            // One address -- process one current string only
            inside = false;
        }
    }

    return n_replace;
}


void RegexpUtil::x_Divide(const string& delimiter)
{
    string x_delimiter = delimiter.empty() ? m_Delimiter : delimiter;
    if ( m_IsDivided  ) {
        if ( x_delimiter == m_Delimiter ) {
            return;
        }
        x_Join();
    }
    m_ContentList.clear();

    // Split source string to parts by delimiter
    size_t pos;
    size_t start_pos = 0;
    for (;;) {
        pos = m_Content.find(x_delimiter, start_pos);
        if (pos == string::npos) {
            m_ContentList.push_back(m_Content.substr(start_pos));
            break;
        } else {
            m_ContentList.push_back(m_Content.substr(start_pos,
                                                     pos - start_pos));
            start_pos = pos + x_delimiter.length();
        }
    }
    m_IsDivided = true;
    // Save delimiter for consecutive joining
    m_Delimiter = x_delimiter;
}


void RegexpUtil::x_Join(void)
{
    if ( m_IsDivided ) {
        m_Content = NStr_Join(m_ContentList, m_Delimiter);
        m_IsDivided = false;
    }
}






