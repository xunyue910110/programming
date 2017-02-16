 #ifndef ARCHY_UTILS_STRING_
#define ARCHY_UTILS_STRING_

#ifdef _MBCS
#pragma warning( disable : 4786)
#endif

#include <string.h>
#include <ctype.h>
#include <time.h>
#include <string>
#include <list>
#include <vector>

#include "archy_config.hpp"
#include "archy_type.hpp"



BEGIN_NAMESPACE_ARCHY

extern const char * const AEmptyCStr;
#define kEmptyCStr NCBI_NS_NCBI::AEmptyCStr

USING_STD_NAMESPACE;


// SIZE_TYPE and NPOS

typedef std::string::size_type SIZE_TYPE;
static const SIZE_TYPE NPOS = std::string::npos;


/////////////////////////////////////////////////////////////////////////////
//  UStr::
//    String-processing utilities
//

class UStr
{
public:
    /// Convert "str" to a (non-negative) "int" value.
    /// Return "-1" if "str" contains any symbols other than [0-9], or
    /// if it represents a number that does not fit into "int".
    static int StringToNumeric(const string& str);

    /// Whether to prohibit trailing symbols (any symbol but '\0')
    /// in the StringToXxx() conversion functions below
    enum ECheckEndPtr {
        eCheck_Need,   /// Check is necessary
        eCheck_Skip    /// Skip this check
    };

    /// String-to-X conversion functions (throw exception on conversion error).
    /// If "check" is eCheck_Skip then "str" can have trailing symbols
    /// after the number;  otherwise exception will be thrown if there
    /// are trailing symbols present after the converted number.
    static int           StringToInt    (const string& str, int base = 10,
                                         ECheckEndPtr check = eCheck_Need);
    static unsigned int  StringToUInt   (const string& str, int base = 10,
                                         ECheckEndPtr check = eCheck_Need);
    static long          StringToLong   (const string& str, int base = 10,
                                         ECheckEndPtr check = eCheck_Need);
    static unsigned long StringToULong  (const string& str, int base = 10,
                                         ECheckEndPtr check = eCheck_Need);
    static double        StringToDouble (const string& str,
                                         ECheckEndPtr check = eCheck_Need);
    static ARCHY_Int8          StringToInt8   (const string& str);
    static ARCHY_Uint8         StringToUInt8  (const string& str, int base = 10);
    static const void*   StringToPtr    (const string& str);

    /// X-to-String conversion functions
    static string    IntToString   (long value, bool sign = false);
    static string    UIntToString  (unsigned long value);
    static string    Int8ToString  (ARCHY_Int8 value, bool sign = false);
    static string    UInt8ToString (ARCHY_Uint8 value);
    static string    DoubleToString(double value);
    /// Note: If precission is more that maximum for current platform,
    //        then it will be truncated to this maximum.
    static string    DoubleToString(double value, unsigned int precision);
    /// Put result of the conversion into buffer "buf" size of "buf_size".
    /// Return the number of bytes stored in "buf", not counting the
    /// terminating '\0'.
    static SIZE_TYPE DoubleToString(double value, unsigned int precision,
                                    char* buf, SIZE_TYPE buf_size);
    static string PtrToString      (const void* ptr);

    /// Return one of: 'true, 'false'
    static const string& BoolToString(bool value);
    /// Can recognize (case-insensitive) one of:  'true, 't', 'false', 'f'
    static bool          StringToBool(const string& str);

	static string& TrimLeft(string& str) {
		return TrimLeft(str, " \r\n\t");
	}
	static string& TrimRight(string& str) {
		return TrimRight(str, " \r\n\t");
	}
	static string& Trim(string& str) {
		return Trim(str, " \r\n\t");
	}
	static string& TrimLeft(string& str, const string& strSet/* = " \r\n\t"*/);
	static string& TrimRight(string& str, const string& strSet/* = " \r\n\t"*/);
	static string& Trim(string& str, const string& strSet/* = " \r\n\t"*/);


    /// String comparison
    enum ECase {
        eCase,
        eNocase  /// ignore character case
    };

    /// ATTENTION.  Be aware that:
    ///
    /// 1) "Compare***(..., SIZE_TYPE pos, SIZE_TYPE n, ...)" functions
    ///    follow the ANSI C++ comparison rules a la "basic_string::compare()":
    ///       str[pos:pos+n) == pattern   --> return 0
    ///       str[pos:pos+n) <  pattern   --> return negative value
    ///       str[pos:pos+n) >  pattern   --> return positive value
    ///
    /// 2) "strn[case]cmp()" functions follow the ANSI C comparison rules:
    ///       str[0:n) == pattern[0:n)   --> return 0
    ///       str[0:n) <  pattern[0:n)   --> return negative value
    ///       str[0:n) >  pattern[0:n)   --> return positive value

    static int CompareCase(const string& str, SIZE_TYPE pos, SIZE_TYPE n,
                           const char* pattern);
    static int CompareCase(const string& str, SIZE_TYPE pos, SIZE_TYPE n,
                           const string& pattern);
    static int CompareCase(const char* s1, const char* s2);

    static int CompareCase(const string& s1, const string& s2);

    static int CompareNocase(const string& str, SIZE_TYPE pos, SIZE_TYPE n,
                             const char* pattern);
    static int CompareNocase(const string& str, SIZE_TYPE pos, SIZE_TYPE n,
                             const string& pattern);
    static int CompareNocase(const char* s1, const char* s2);

    static int CompareNocase(const string& s1, const string& s2);

    static int Compare(const string& str, SIZE_TYPE pos, SIZE_TYPE n,
                       const char* pattern, ECase use_case = eCase);
    static int Compare(const string& str, SIZE_TYPE pos, SIZE_TYPE n,
                       const string& pattern, ECase use_case = eCase);
    static int Compare(const char* s1, const char* s2,
                       ECase use_case = eCase);
    static int Compare(const string& s1, const char* s2,
                       ECase use_case = eCase);
    static int Compare(const char* s1, const string& s2,
                       ECase use_case = eCase);
    static int Compare(const string& s1, const string& s2,
                       ECase use_case = eCase);

    // NOTE.  On some platforms, "strn[case]cmp()" can work faster than their
    //        "Compare***()" counterparts.
    static int strcmp      (const char* s1, const char* s2);
    static int strncmp     (const char* s1, const char* s2, size_t n);
    static int strcasecmp  (const char* s1, const char* s2);
    static int strncasecmp (const char* s1, const char* s2, size_t n);


    /// The following 4 methods change the passed string, then return it
    static string& ToLower(string& str);
    static char*   ToLower(char*   str);
    static string& ToUpper(string& str);
    static char*   ToUpper(char*   str);
    // ...and these two are just dummies to prohibit passing constant C strings
private:
    static void/*dummy*/ ToLower(const char* /*dummy*/);
    static void/*dummy*/ ToUpper(const char* /*dummy*/);
public:

    static bool StartsWith(const string& str, const string& start,
                           ECase use_case = eCase);
    static bool EndsWith(const string& str, const string& end,
                         ECase use_case = eCase);

    enum EOccurrence {
        eFirst,
        eLast
    };

    // Return the start of the first or last (depending on WHICH)
    // occurrence of PATTERN in STR that *starts* within [START, END],
    // or NPOS if it does not occur.
    static SIZE_TYPE Find      (const string& str, const string& pattern,
                                SIZE_TYPE start = 0, SIZE_TYPE end = NPOS,
                                EOccurrence which = eFirst,
                                ECase use_case = eCase);
    static SIZE_TYPE FindCase  (const string& str, const string& pattern,
                                SIZE_TYPE start = 0, SIZE_TYPE end = NPOS,
                                EOccurrence which = eFirst);
    static SIZE_TYPE FindNoCase(const string& str, const string& pattern,
                                SIZE_TYPE start = 0, SIZE_TYPE end = NPOS,
                                EOccurrence which = eFirst);

    enum ETrunc {
        eTrunc_Begin,  /// truncate leading  spaces only
        eTrunc_End,    /// truncate trailing spaces only
        eTrunc_Both    /// truncate spaces at both begin and end of string
    };
    static string TruncateSpaces(const string& str, ETrunc where=eTrunc_Both);

    /// Starting from position "start_pos", replace no more than "max_replace"
    /// occurrences of substring "search" by string "replace".
    /// If "max_replace" is zero -- then replace all occurrences.
    /// Result will be put to string "dst", and it will be returned as well.
    static string& Replace(const string& src,
                           const string& search,
                           const string& replace,
                           string& dst,
                           SIZE_TYPE start_pos = 0, size_t max_replace = 0);

    /// The same as the above Replace(), but return new string
    static string Replace(const string& src,
                          const string& search,
                          const string& replace,
                          SIZE_TYPE start_pos = 0, size_t max_replace = 0);

    // Whether to merge adjacent delimiters in Split and Tokenize
    enum EMergeDelims {
        eNoMergeDelims,
        eMergeDelims
    };


    /// Split string "str" using symbols from "delim" as delimiters.
    /// Delimiters which immediately follow each other are treated as one
    /// delimiter by default (unlike that in Tokenize()).
    /// Add the resultant tokens to the list "arr". Return "arr".
    static list<string>& Split(const string& str,
                               const string& delim,
                               list<string>& arr,
                               EMergeDelims  merge = eMergeDelims);

    /// Tokenize string "str" using symbols from "delim" as delimeters.
    /// Add the resultant tokens to the vector "arr". Return ref. to "arr".
    /// If delimiter is empty, then input string is appended to "arr" as is.
    static vector<string>& Tokenize(const string&   str,
                                    const string&   delim,
                                    vector<string>& arr,
                                    EMergeDelims    merge = eNoMergeDelims);

    /// Join the strings in "arr" into a single string, separating
    /// each pair with "delim".
    static string Join(const list<string>& arr, const string& delim);

    /// Make a printable version of "str". The non-printable characters will
    /// be represented as "\r", "\n", "\v", "\t", "\0", "\\", or
    /// "\xDD" where DD is the character's code in hexadecimal.
    enum ENewLineMode {
        eNewLine_Quote,         // display "\n" instead of actual linebreak
        eNewLine_Passthru       // break the line on every "\n" occurrance
    };

    static string PrintableString(const string& str,
                                  ENewLineMode  nl_mode = eNewLine_Quote);

    enum EWrapFlags {
        fWrap_Hyphenate  = 0x1, // add a hyphen when breaking words?
        fWrap_HTMLPre    = 0x2  // wrap as preformatted HTML?
    };
    typedef int TWrapFlags; // binary OR of "EWrapFlags"

    /// Split string "str" into lines of width "width" and add the
    /// resulting lines to the list "arr" (returned).  Normally, all
    /// lines will begin with "prefix" (counted against "width"),
    /// but the first line will instead begin with "prefix1" if
    /// you supply it.
    static list<string>& Wrap(const string& str, SIZE_TYPE width,
                              list<string>& arr, TWrapFlags flags = 0,
                              const string* prefix = 0,
                              const string* prefix1 = 0);

    static list<string>& Wrap(const string& str, SIZE_TYPE width,
                              list<string>& arr, TWrapFlags flags,
                              const string& prefix, const string* prefix1 = 0);

    static list<string>& Wrap(const string& str, SIZE_TYPE width,
                              list<string>& arr, TWrapFlags flags,
                              const string& prefix, const string& prefix1);

    /// Similar to the above, but tries to avoid splitting any elements of l.
    /// Delim only applies between elements on the same line; if you want
    /// everything to end with commas or such, you should add them first.
    static list<string>& WrapList(const list<string>& l, SIZE_TYPE width,
                                  const string& delim, list<string>& arr,
                                  TWrapFlags flags = 0,
                                  const string* prefix = 0,
                                  const string* prefix1 = 0);

    static list<string>& WrapList(const list<string>& l, SIZE_TYPE width,
                                  const string& delim, list<string>& arr,
                                  TWrapFlags flags, const string& prefix,
                                  const string* prefix1 = 0);

    static list<string>& WrapList(const list<string>& l, SIZE_TYPE width,
                                  const string& delim, list<string>& arr,
                                  TWrapFlags flags, const string& prefix,
                                  const string& prefix1);
}; // class NStr


/////////////////////////////////////////////////////////////////////////////
//  Predicates
//


/// Case-sensitive string comparison
struct PCase
{
    /// Return difference between "s1" and "s2"
    int Compare(const string& s1, const string& s2) const;

    /// Return TRUE if s1 < s2
    bool Less(const string& s1, const string& s2) const;

    /// Return TRUE if s1 == s2
    bool Equals(const string& s1, const string& s2) const;

    /// Return TRUE if s1 < s2
    bool operator()(const string& s1, const string& s2) const;
};


/// Case-INsensitive string comparison
struct PNocase
{
    /// Return difference between "s1" and "s2"
    int Compare(const string& s1, const string& s2) const;

    /// Return TRUE if s1 < s2
    bool Less(const string& s1, const string& s2) const;

    /// Return TRUE if s1 == s2
    bool Equals(const string& s1, const string& s2) const;

    /// Return TRUE if s1 < s2 ignoring case
    bool operator()(const string& s1, const string& s2) const;
};



/////////////////////////////////////////////////////////////////////////////
//  Algorithms
//


/// Check equivalence of arguments using predicate
template<class Arg1, class Arg2, class Pred>
inline
bool AStrEquiv(const Arg1& x, const Arg2& y, Pred pr)
{
    return pr.Equals(x, y);
}




/////////////////////////////////////////////////////////////////////////////
//  UStr::
//


inline
int UStr::strcmp(const char* s1, const char* s2)
{
    return ::strcmp(s1, s2);
}

inline
int UStr::strncmp(const char* s1, const char* s2, size_t n)
{
    return ::strncmp(s1, s2, n);
}

inline
int UStr::strcasecmp(const char* s1, const char* s2)
{
#if defined(ARCHY_HAVE_STRICMP)
    return ::stricmp(s1, s2);

#elif defined(ARCHY_HAVE_STRCASECMP)
    return ::strcasecmp(s1, s2);

#else
    int diff = 0;
    for ( ;; ++s1, ++s2) {
        char c1 = *s1;
        // calculate difference
        diff = toupper(c1) - toupper(*s2);
        // if end of string or different
        if (!c1  ||  diff)
            break; // return difference
    }
    return diff;
#endif
}

inline
int UStr::strncasecmp(const char* s1, const char* s2, size_t n)
{
#if defined(HAVE_STRICMP)
    return ::strnicmp(s1, s2, n);

#elif defined(HAVE_STRCASECMP)
    return ::strncasecmp(s1, s2, n);

#else
    int diff = 0;
    for ( ; ; ++s1, ++s2, --n) {
        char c1 = *s1;
        // calculate difference
        diff = toupper(c1) - toupper(*s2);
        // if end of string or different
        if (!c1  ||  diff)
            break; // return difference
        if (n == 0)
            return 0;
    }
    return diff;
#endif
}


inline
int UStr::Compare(const string& str, SIZE_TYPE pos, SIZE_TYPE n,
                  const char* pattern, ECase use_case)
{
    return use_case == eCase ?
        CompareCase(str, pos, n, pattern): CompareNocase(str, pos, n, pattern);
}

inline
int UStr::Compare(const string& str, SIZE_TYPE pos, SIZE_TYPE n,
                  const string& pattern, ECase use_case)
{
    return use_case == eCase ?
        CompareCase(str, pos, n, pattern): CompareNocase(str, pos, n, pattern);
}

inline
int UStr::CompareCase(const char* s1, const char* s2)
{
    return UStr::strcmp(s1, s2);
}

inline
int UStr::CompareNocase(const char* s1, const char* s2)
{
    return UStr::strcasecmp(s1, s2);
}

inline
int UStr::Compare(const char* s1, const char* s2, ECase use_case)
{
    return use_case == eCase ? CompareCase(s1, s2): CompareNocase(s1, s2);
}

inline
int UStr::Compare(const string& s1, const char* s2, ECase use_case)
{
    return Compare(s1.c_str(), s2, use_case);
}

inline
int UStr::Compare(const char* s1, const string& s2, ECase use_case)
{
    return Compare(s1, s2.c_str(), use_case);
}

inline
int UStr::Compare(const string& s1, const string& s2, ECase use_case)
{
    return Compare(s1.c_str(), s2.c_str(), use_case);
}

inline
int UStr::CompareCase(const string& s1, const string& s2)
{
    return CompareCase(s1.c_str(), s2.c_str());
}

inline
int UStr::CompareNocase(const string& s1, const string& s2)
{
    return CompareNocase(s1.c_str(), s2.c_str());
}

inline
bool UStr::StartsWith(const string& str, const string& start, ECase use_case)
{
    return str.size() >= start.size()  &&
        Compare(str, 0, start.size(), start, use_case) == 0;
}

inline
bool UStr::EndsWith(const string& str, const string& end, ECase use_case)
{
    return str.size() >= end.size()  &&
        Compare(str, str.size() - end.size(), end.size(), end, use_case) == 0;
}


inline
SIZE_TYPE UStr::Find(const string& str, const string& pattern,
                     SIZE_TYPE start, SIZE_TYPE end, EOccurrence where,
                     ECase use_case)
{
    return use_case == eCase ? FindCase(str, pattern, start, end, where)
        : FindNoCase(str, pattern, start, end, where);
}

inline
SIZE_TYPE UStr::FindCase(const string& str, const string& pattern,
                         SIZE_TYPE start, SIZE_TYPE end, EOccurrence where)
{
    if (where == eFirst) {
        SIZE_TYPE result = str.find(pattern, start);
        return (result == NPOS  ||  result > end) ? NPOS : result;
    } else {
        SIZE_TYPE result = str.rfind(pattern, end);
        return (result == NPOS  ||  result < start) ? NPOS : result;
    }
}

inline
SIZE_TYPE UStr::FindNoCase(const string& str, const string& pattern,
                           SIZE_TYPE start, SIZE_TYPE end, EOccurrence where)
{
    string str2 = str, pat2 = pattern;
    return FindCase(ToLower(str2), ToLower(pat2), start, end, where);
}


inline
list<string>& UStr::Wrap(const string& str, SIZE_TYPE width, list<string>& arr,
                         UStr::TWrapFlags flags, const string& prefix,
                         const string* prefix1)
{
    return Wrap(str, width, arr, flags, &prefix, prefix1);
}


inline
list<string>& UStr::Wrap(const string& str, SIZE_TYPE width, list<string>& arr,
                         UStr::TWrapFlags flags, const string& prefix,
                         const string& prefix1)
{
    return Wrap(str, width, arr, flags, &prefix, &prefix1);
}


inline
list<string>& UStr::WrapList(const list<string>& l, SIZE_TYPE width,
                             const string& delim, list<string>& arr,
                             UStr::TWrapFlags flags, const string& prefix,
                             const string* prefix1)
{
    return WrapList(l, width, delim, arr, flags, &prefix, prefix1);
}


inline
list<string>& UStr::WrapList(const list<string>& l, SIZE_TYPE width,
                             const string& delim, list<string>& arr,
                             UStr::TWrapFlags flags, const string& prefix,
                             const string& prefix1)
{
    return WrapList(l, width, delim, arr, flags, &prefix, &prefix1);
}




/////////////////////////////////////////////////////////////////////////////
//  PCase::
//

inline
int PCase::Compare(const string& s1, const string& s2) const
{
    return UStr::Compare(s1, s2, UStr::eCase);
}

inline
bool PCase::Less(const string& s1, const string& s2) const
{
    return Compare(s1, s2) < 0;
}

inline
bool PCase::Equals(const string& s1, const string& s2) const
{
    return Compare(s1, s2) == 0;
}

inline
bool PCase::operator()(const string& s1, const string& s2) const
{
    return Less(s1, s2);
}



/////////////////////////////////////////////////////////////////////////////
//  PNocase::
//

inline
int PNocase::Compare(const string& s1, const string& s2) const
{
    return UStr::Compare(s1, s2, UStr::eNocase);
}

inline
bool PNocase::Less(const string& s1, const string& s2) const
{
    return Compare(s1, s2) < 0;
}

inline
bool PNocase::Equals(const string& s1, const string& s2) const
{
    return Compare(s1, s2) == 0;
}

inline
bool PNocase::operator()(const string& s1, const string& s2) const
{
    return Less(s1, s2);
}


END_NAMESPACE_ARCHY



#endif 
