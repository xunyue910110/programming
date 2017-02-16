#ifndef ARCHY_REGEXP__HPP
#define ARCHY_REGEXP__HPP
#include "noncopyable.hpp"
/// @file regexpr.hpp
/// C++ wrapper for the PCRE library
///
/// Class definition for class CRegexp which is a wrapper class for the
/// Perl-compatible regular expression (PCRE) library written in C.
#include "archy_config.hpp"
USING_STD_NAMESPACE;


namespace archy {

/// This class is a C++ wrapper for the Perl-compatible regular expression
/// library written in C. Internally, it holds a compiled regular expression
/// used for matching with char* strings passed as an argument to the
/// GetMatch member function. The regular expression is passed as a char*
/// argument to the constructor or to the Set member function.
class reg_expr:private noncopyable
{
public:
/// Specifies the maximum number of sub-patterns that can be found
    enum { kRegexpMaxSubPatterns = 100 };
    
    /// Type definitions used for code clarity.
    typedef int TCompile;
    typedef int TMatch;

    /// Flags for constructor and Set

    /// PCRE compiler flags usde in the constructor and in Set. If
    /// eCompile_ignore_case is set, matches are case insensitive. If
    /// eCompile_newline is set then ^ matches the start of a line and
    /// $ matches the end of a line. If not set, ^ matches only the start of
    /// the entire string and $ matches only the end of the entire string.
    enum ECompile {
        eCompile_default     = 0,
        eCompile_ignore_case = 0x0001,
        eCompile_newline     = 0x0002,
        eCompile_both        = 0x0002 | 0x0001
    };

    /// Constructor.

    /// Sets and compiles the PCRE pattern specified by pat
    /// according to compile options. See ECompile above. Also
    /// allocates memory for compiled PCRE.
    reg_expr(const string &pat,  // Perl regular expression
            TCompile flags = 0) // Flags
            throw (runtime_error);     
    reg_expr();
    /// Destructor.

    /// Deallocates compiled Perl-compatible regular expression.
    virtual ~reg_expr();

    /// Sets and compiles PCRE

    /// Sets and compiles the PCRE pattern specified by pat
    /// according to compile options. See ECompile above. Also
    /// deallocates/allocates memory for compiled PCRE.
    void set(const string &pat, // Perl regular expression
             TCompile flags = 0)    // Options
	   throw (runtime_error);
    /// GetMatch flags

    /// Setting eMatch_not_begin causes ^ not to match before the
    /// first character of a line. Without setting eCompile_newline,
    /// ^ won't match anything if eMatch_not_begin is set. Setting
    /// eMatch_not_end causes $ not to match immediately before a new
    /// line. Without setting eCompile_newline, $ won't match anything
    /// if eMatch_not_end is set.
    enum EMatch {
        eMatch_default   = 0,
        eMatch_not_begin = 0x0080,           // ^ won't match string begin
        eMatch_not_end   = 0x0100,           // $ won't match string end
        eMatch_not_both  = 0x0080 | 0x0100
    };

    /// Get matching pattern and sub-patterns.

    /// Returns a string corresponding to the match to pattern or sub-pattern.
    /// Use idx = 0 for complete pattern. Use idx > 0 for sub-patterns.
    /// Returns empty string when no match found or if noreturn is true.
    /// Set noreturn to true when GetSub or GetResults will be used
    /// to retrieve pattern and sub-patterns. Calling GetMatch causes
    /// the entire search to be performed again. If you want to retrieve
    /// a different pattern/sub-pattern from an already performed search,
    /// it is more efficient to use GetSub or GetResults.
    bool match (const string& str,             // String to search
                     TMatch flags = 0);            // Options

    string operator[]( int i ) const ;

    /// Get number of patterns + sub-patterns.

    /// Returns the number of patterns + sub-patterns found as a result
    /// of the most recent GetMatch call.
    int count() const;

    // Return the starting offset of the ith matched substring from the 
    // beginning of the string used in match().
    int sub_start( int i ) const;
		
    // Return the length of the ith matched substring
    int sub_len( int i ) const;
   
	// Get Replace string in which we use & as the whole matched string 
	// and \0 - \n as the number n substring
	string get_replacedstring(const string& strSrc);
	
private:
    string m_strSrc;
    
    /// Pointer to compiled PCRE
    void  *m_PReg;                                  // Compiled pattern

    /// Array of locations of patterns/sub-patterns resulting from
    /// the last call to GetMatch. Also contains 1/3 extra space used
    /// internally by the PCRE C library.
    int m_Results[(kRegexpMaxSubPatterns +1) * 3];  // Results of Match

    /// Holds the total number of pattern + sub-patterns resulting
    /// from the last call to GetMatch.
    int m_NumFound;                                 // #(sub) patterns
};


}

#endif 

