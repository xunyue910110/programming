
#ifndef __REGEXP_H__
#define __REGEXP_H__


/// @file regexp.h
/// C++ wrappers for the Perl-compatible regular expression (PCRE) library.
///
/// Regexp     -  wrapper class for the PCRE library.
/// RegexpUtil -  utility functions.
///
/// For more details see PCRE documentation: http://www.pcre.org/pcre.txt
///

#include "base/config-all.h"

#include <pcre.h>
#include <string>
#include <list>
#include <stdexcept>

USING_NAMESPACE(std);


static const string kEmptyStr = "";

/// Specifies the maximum number of subpatterns that can be found.
const size_t kRegexpMaxSubPatterns = 100;

///
///  @class Regexp 
///
///  @breif Perl 兼容正则表达式解析
///   
/// 
///
/// Define a wrapper class for the Perl-compatible regular expression (PCRE)
/// library.
///
/// Internally, this class holds a compiled regular expression used for
/// matching with strings passed as an argument to the GetMatch()
/// member function. The regular expression is passed as a argument
/// to the constructor or to the Set() member function.
///
/// Throw exception on error.
/// 
/// 
/// Example code:
/// @code
/// 
/// #include "regexp.h"
/// #include <iostream>
/// 
/// 
/// int test(void)
/// {
///     // Simple way to use regular expressions
///     Regexp pattern("D\\w*g");
///     cout << pattern.GetMatch("The Dodgers play baseball.") << endl;
///     cout << "" << endl;
///     // Perl compatible regular expression pattern to match
///     string pat("(q.*k).*f?x");
///     pattern.Set(pat);
///     
///     // String to find matching pattern in
///     string text("The quick brown fox jumped over the lazy dogs.\n");
///     text += "Now is the time for all good men to come to the aid of their ";
///     text += "country.\nTwas the night before Christmas and all through the ";
///     text += "house, not a\n creature was stirring, not even a mouse.\n";
///     
///     // Display pattern and sub pattern matches
///     cout << pattern.GetMatch(text.c_str()) << endl;
///     for (int k = 1; k < pattern.NumFound(); k++) {
///         cout << pattern.GetSub(text.c_str(), 1) << endl;
///     }    
///     
///     // Set new pattern and ignore case
///     pattern.Set("t\\w*e", Regexp::eCompile_ignore_case);
///     
///     // Find all matches to pattern
///     size_t start = 0;
///     while (start != string::npos) {
///         string match = pattern.GetMatch(text.c_str(), start);
///         if (pattern.NumFound() > 0) {
///             cout << match << endl;
///             start = text.find(match, start) + 1;            
///         } else {
///             break;
///         }
///     }
///     
///     // test for RegexpUtil
///     if (RegexpUtil::Exists("my this","^m\\w*"))
///       cout << "exits" << endl;
///     else
///       cout << "not exists" << endl;
///     
///     
///     // Same as above but with cstrings and elinates string return
///     start = 0;
///     char *txt = new char[text.length() + 1];
///     strcpy(txt, text.c_str());
///     while (true)
///     {
///         pattern.GetMatch(txt, start, 0, 0, true);
///         if (pattern.NumFound() > 0) {
///             const int *rslt = pattern.GetResults(0);
///             start = rslt[1];
///             for (int i = rslt[0]; i < rslt[1]; i++) {
///                 cout << txt[i];
///             }
///             cout << endl;
///         } else {
///             break;
///         }
///     }
///     delete[] txt;                  
///     return 0;
/// }
/// @endcode
/// 
/// 


class Regexp
{
public:
    /// Type definitions used for code clarity.
    typedef int TCompile;     ///< Compilation options.
    typedef int TMatch;       ///< Match options.

    /// Flags for compile regular expressions.
    ///
    /// PCRE compiler flags used in the constructor and in Set().
    /// If eCompile_ignore_case is set, matches are case insensitive.
    /// If eCompile_dotall is set, a dot metacharater in the pattern matches
    /// all characters, including newlines. Without it, newlines are excluded.
    /// If eCompile_newline is set then ^ matches the start of a line and
    /// $ matches the end of a line. If not set, ^ matches only the start
    /// of the entire string and $ matches only the end of the entire string.
    /// If eCompile_ungreedy inverts the "greediness" of the quantifiers so
    /// that they are not greedy by default, but become greedy if followed by
    /// "?".
    /// It is not compatible with Perl. 
    ///
    /// The settings can be changed from within the pattern by a sequence of
    /// Perl option letters enclosed between "(?" and ")".
    /// The option letters are:
    ///   i  for PCRE_CASELESS
    ///   m  for PCRE_MULTILINE
    ///   s  for PCRE_DOTALL
    ///   x  for PCRE_EXTENDED
    ///   U  for PCRE_UNGREEDY
    enum ECompile {
        eCompile_default     = 0,
        eCompile_ignore_case = PCRE_CASELESS,
        eCompile_dotall      = PCRE_DOTALL,
        eCompile_newline     = PCRE_MULTILINE,
        eCompile_ungreedy    = PCRE_UNGREEDY
    };

    /// Flags for match string against a pre-compiled pattern.
    ///
    /// Setting eMatch_not_begin causes ^ not to match before the
    /// first character of a line. Without setting eCompile_newline,
    /// ^ won't match anything if eMatch_not_begin is set.
    /// Setting eMatch_not_end causes $ not to match immediately before a new
    /// line. Without setting eCompile_newline, $ won't match anything
    /// if eMatch_not_end is set.
    enum EMatch {
        eMatch_default   = 0,
        eMatch_not_begin = PCRE_NOTBOL,      ///< ^ won't match string begin.
        eMatch_not_end   = PCRE_NOTEOL,      ///< $ won't match string end.
        eMatch_not_both  = PCRE_NOTBOL | PCRE_NOTEOL
    };

    /// Constructor.
    ///
    /// Set and compile the PCRE pattern specified by argument according
    /// to compile options. Also allocate memory for compiled PCRE.
    /// @param pattern
    ///   Perl regular expression to compile.
    /// @param flags
    ///   Regular expression compilation flags.
    /// @sa
    ///   ECompile
    Regexp(const string& pattern, TCompile flags = 0);

    /// Destructor.
    ///
    /// Deallocate compiled Perl-compatible regular expression.
    virtual ~Regexp();

    /// Set and compile PCRE.
    ///
    /// Set and compile the PCRE pattern specified by argument according
    /// to compile options. Also deallocate/allocate memory for compiled PCRE.
    /// @param pattern
    ///   Perl regular expression to compile.
    /// @param flags
    ///   Regular expression compilation flags.
    /// @sa
    ///   ECompile
    void Set(const string& pattern, TCompile flags = 0);

    /// Get matching pattern and subpatterns.
    ///
    /// Return a string corresponding to the match to pattern or subpattern.
    /// Set noreturn to true when GetSub() or GetResults() will be used
    /// to retrieve pattern and subpatterns. Calling GetMatch() causes
    /// the entire search to be performed again. If you want to retrieve
    /// a different pattern/subpattern from an already performed search,
    /// it is more efficient to use GetSub or GetResults.
    /// @param str
    ///   String to search.
    /// @param offset
    ///   Starting offset in str.
    /// @param idx
    ///   (Sub) match to return.
    ///   Use idx = 0 for complete pattern. Use idx > 0 for subpatterns.
    /// @param flags
    ///   Flags to match.
    /// @param noreturn
    ///   Return empty string if noreturn is true.
    /// @return
    ///   Return (sub) match with number idx or empty string when no match
    ///   found or if noreturn is true.
    /// @sa
    ///   EMatch, GetSub(), GetResult()
    string GetMatch(
        const string& str,
        int          offset   = 0,
        size_t        idx      = 0,
        TMatch        flags    = 0,
        bool          noreturn = false
    );

    /// Get pattern/subpattern from previous GetMatch().
    ///
    /// Should only be called after GetMatch() has been called with the
    /// same string. GetMatch() internally stores locations on string where
    /// pattern and subpatterns were found. 
    /// @param str
    ///   String to search.
    /// @param idx
    ///   (Sub) match to return.
    /// @return
    ///   Return the substring at location of pattern match (idx 0) or
    ///   subpattern match (idx > 0). Return empty string when no match.
    /// @sa
    ///   GetMatch(), GetResult()
    string GetSub(const string& str, size_t idx = 0) const;

    /// Get number of patterns + subpatterns.
    ///
    /// @return
    ///   Return the number of patterns + subpatterns found as a result
    ///   of the most recent GetMatch() call.
    /// @sa
    ///   GetMatch()
    int NumFound() const;

    /// Get location of pattern/subpattern.
    ///
    /// @param idx
    ///   Index of pattern/subpattern to obtaining.
    ///   Use idx = 0 for pattern, idx > 0 for sub patterns.
    /// @return
    ///   Return array where index 0 is location of first character in
    ///   pattern/sub pattern and index 1 is 1 beyond last character in
    ///   pattern/sub pattern.
    ///   Throws if called with idx >= NumFound().
    /// @sa
    ///   GetMatch(), NumFound()
    const int* GetResults(size_t idx) const;

private:
    // Disable copy constructor and assignment operator.
    Regexp(const Regexp &);
    void operator= (const Regexp &);

    /// Pointer to compiled PCRE pattern.
    pcre*  m_PReg;

    /// Array of locations of patterns/subpatterns resulting from
    /// the last call to GetMatch(). Also contains 1/3 extra space used
    /// internally by the PCRE C library.
    int m_Results[(kRegexpMaxSubPatterns +1) * 3];

    /// The total number of pattern + subpatterns resulting from
    /// the last call to GetMatch.
    int m_NumFound;
};


/////////////////////////////////////////////////////////////////////////////
///
/// RegexpUtil --
///
/// Throw exception on error.

class  RegexpUtil
{
public:
    /// Constructor.
    ///
    /// Set string for processing.
    /// @param str
    ///   String to process.
    /// @sa
    ///   Exists(), Extract(), Replace(), ReplaceRange()
    RegexpUtil(const string& str = kEmptyStr);

    /// Reset the content of the string to process.
    ///
    /// @param str
    ///   String to process.
    /// @sa
    ///   operator =
    void Reset(const string& str);

    /// Reset the content of the string to process.
    ///
    /// The same as Reset().
    /// @param str
    ///   String to process.
    /// @sa
    ///   Reset()
    void operator= (const string& str);

    /// Get result string.
    ///
    /// @sa
    ///   operator string
    string GetResult(void);

    /// Get result string.
    ///
    /// The same as GetResult().
    /// @sa
    ///   GetResult()
    operator string(void);
 
    /// Check existence substring which match a specified pattern.
    ///
    /// @param pattern
    ///   Perl regular expression to search.
    /// @param compile_flags
    ///   Regular expression compilation flags.
    /// @param match_flags
    ///   Flags to match.
    /// @return
    ///   Return TRUE if  a string corresponding to the match to pattern or
    ///   subpattern.
    /// @sa
    ///   Regexp, Regexp::GetMatch()
    bool Exists(
        const string&      pattern,
        Regexp::TCompile  compile_flags = Regexp::eCompile_default,
        Regexp::TMatch    match_flags   = Regexp::eMatch_default
    );

 
    /// Check existence substring which match a specified pattern.
    ///
    /// @param str
    ///   string for processing
    /// @param pattern
    ///   Perl regular expression to search.
    /// @param compile_flags
    ///   Regular expression compilation flags.
    /// @param match_flags
    ///   Flags to match.
    /// @return
    ///   Return TRUE if  a string corresponding to the match to pattern or
    ///   subpattern.
    /// @sa
    ///   Regexp, Regexp::GetMatch()
    static bool Exists(
        const string&     str,
        const string&     pattern,
        Regexp::TCompile  compile_flags = Regexp::eCompile_default,
        Regexp::TMatch    match_flags   = Regexp::eMatch_default
    );

    /// Get matching pattern/subpattern from string.
    ///
    /// @param pattern
    ///   Perl regular expression to search.
    /// @param compile_flags
    ///   Regular expression compilation flags.
    /// @param match_flags
    ///   Flags to match.
    /// @param pattern_idx
    ///   Index of pattern/subpattern to extract.
    ///   Use pattern_idx = 0 for pattern, pattern_idx > 0 for sub patterns.
    /// @return
    ///   Return the substring at location of pattern/subpatter match with
    ///   index pattern_idx. Return empty string when no match.
    /// @sa
    ///   Regexp, Regexp::GetMatch()
    string Extract(
        const string&      pattern,
        Regexp::TCompile  compile_flags = Regexp::eCompile_default,
        Regexp::TMatch    match_flags   = Regexp::eMatch_default,
        size_t             pattern_idx   = 0
    );

    /// Replace occurrences of a substring within a string by pattern.
    ///
    /// @param search
    ///   Reqular expression to match a substring value that is replaced.
    /// @param replace
    ///   Replace "search" substring with this value. The matched subpatterns
    ///   (if any) can be found and inserted into replace string using
    ///   variables $1, $2, $3, and so forth. The variable can be enclosed
    ///   in the curly brackets {}, that will be deleted on substitution.
    /// @param compile_flags
    ///   Regular expression compilation flags.
    /// @param match_flags
    ///   Flags to match.
    /// @param max_replace
    ///   Replace no more than "max_replace" occurrences of substring "search".
    ///   If "max_replace" is zero (default), then replace all occurrences with
    ///   "replace".
    /// @return
    ///   Return the count of replacements.
    /// @sa
    ///   Regexp, ReplaceRange()
    size_t Replace(
        const string&      search,
        const string&      replace,
        Regexp::TCompile  compile_flags = Regexp::eCompile_default,
        Regexp::TMatch    match_flags   = Regexp::eMatch_default,
        size_t             max_replace   = 0
    );

    //
    // Range functions.
    //

    /// Range processing type.
    /// Defines which part of the specified range should be processed.
    enum ERange {
        eInside,    ///< Process substrings inside range.
        eOutside    ///< Process substrings outside range.
    };

    /// Set new range for range-dependent functions.
    ///
    /// The mached string will be splitted up by "delimeter".
    /// And then in range-dependent functions every part (substring) is checked
    /// to fall into the range, specified by start and end adresses.
    ///
    /// The addresses works similare the Unix utility SED, except that regular
    /// expressions is Perl-compatible:
    ///    - empty address in the range correspond to any substring.
    ///    - command with one address correspond to any substring that matches
    ///      the address.
    ///    - command with two addresses correspond to inclusive range from the
    ///      start address to through the next pattern space that maches the
    ///      end address.
    ///
    /// Specified range have effect only for range-dependent functions.
    /// Otherwise range is ignored.
    /// @param addr_start
    ///   Regular expression which assign a starting address of range.
    /// @param addr_end
    ///   Regular expression which assign an ending address of range.
    ///   Should be empty if the start address is empty.
    /// @param delimiter
    ///   Split a source string by "delimiter.
     /// @sa
    ///   ClearRange, ReplaceRange()
    void SetRange(
        const string&  addr_start = kEmptyStr,
        const string&  addr_end   = kEmptyStr,
        const string&  delimiter  = "\n"
    );

    /// Clear range for range-dependent functions.
    ///
    /// Have the same effect as SetRange() without parameters.
    /// @sa
    ///   SetRange()
    void ClearRange(void);

    /// Replace all occurrences of a substring within a string by pattern.
    ///
    /// Use range specified by SetRange() method. Work like SED command s/.
    /// @param search
    ///   Reqular expression to match a substring value that is replaced.
    /// @param replace
    ///   Replace "search" substring with this value. The matched subpatterns
    ///   (if any) can be found and inserted into replace string using
    ///   variables $1, $2, $3, and so forth. The variable can be enclosed
    ///   in the curly brackets {}, that will be deleted on substitution.
    /// @param compile_flags
    ///   Regular expression compilation flags.
    /// @param match_flags
    ///   Flags to match.
    /// @param process_within
    ///   Define which part of the range should be processed.
    /// @param max_replace
    ///   Replace no more than "max_replace" occurrences of substring "search"
    ///   in the every substring. If "max_replace" is zero (default),
    ///   then replace all occurrences with "replace".
    /// @return
    ///   Return the count of replacements.
    /// @sa
    ///   ERange, SetRange(), ClearRange()
    size_t ReplaceRange(
        const string&       search,
        const string&       replace,
        Regexp::TCompile   compile_flags  = Regexp::eCompile_default,
        Regexp::TMatch     match_flags    = Regexp::eMatch_default,
        RegexpUtil::ERange process_within = eInside,
        size_t              max_replace    = 0
    );

private:
    /// Divide source string to substrings by delimiter.
    /// If delimiter is empty string that use early defined delimiter.
    void x_Divide(const string& delimiter = kEmptyStr);

    /// Join substrings back to entire string.
    void x_Join(void);

private:
    string       m_Content;       ///< Content string.
    list<string> m_ContentList;   ///< Content list.
    bool         m_IsDivided;     ///< TRUE if m_ContentList is newer than
                                  ///< m_Content, and FALSE otherwise.
    string       m_RangeStart;    ///< Regexp to determine start of range.
    string       m_RangeEnd;      ///< Regexp to determine end of range.
    string       m_Delimiter;     ///< Delimiter used to split string.
};



//////////////////////////////////////////////////////////////////////////////
//
// Inline
//

//
// Regexp
//

inline
int Regexp::NumFound() const
{
    return m_NumFound;
}


inline
const int* Regexp::GetResults(size_t idx) const
{
    if ((int)idx >= m_NumFound) {
        throw runtime_error("idx >= NumFound()");
    }
    return m_Results + 2 * idx;
}

//
// RegexpUtil
//

inline
string RegexpUtil::GetResult(void)
{
    if ( m_IsDivided ) {
        x_Join();
    }
    return m_Content;
}

inline
void RegexpUtil::Reset(const string& str)
{
    m_Content   = str;
    m_IsDivided = false;
    m_ContentList.clear();
}

inline
RegexpUtil::operator string(void)
{
    return GetResult();
}

inline
void RegexpUtil::operator= (const string& str)
{
    Reset(str);
}

inline
void RegexpUtil::ClearRange(void)
{
    SetRange();
}

inline
bool RegexpUtil::Exists(
    const string&     pattern,
    Regexp::TCompile compile_flags,
    Regexp::TMatch   match_flags)
{
    // Fill shure that string is not divided
    x_Join();
    // Check the pattern existence
    Regexp re(pattern, compile_flags);
    re.GetMatch(m_Content.c_str(), 0, match_flags, 0, true);
    return re.NumFound() > 0;
}


inline 
bool RegexpUtil:: Exists(
    const string&     str,
    const string&     pattern,
    Regexp::TCompile  compile_flags ,
    Regexp::TMatch    match_flags ) 
{  
	RegexpUtil reutil(str);
	return reutil.Exists(pattern,compile_flags,match_flags);
}

inline
string RegexpUtil::Extract(
    const string&     pattern,
    Regexp::TCompile compile_flags,
    Regexp::TMatch   match_flags,
    size_t            pattern_idx)
{
    // Fill shure that string is not divided
    x_Join();
    // Get the pattern/subpattern
    Regexp re(pattern, compile_flags);
    return re.GetMatch(m_Content.c_str(), 0, pattern_idx, match_flags);
}



#endif  /* __REGEXP_H__  */
