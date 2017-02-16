#ifdef _MBCS
#pragma warning( disable : 4786)
#endif

#include "ustr.hpp"
#include <string>
#include "debug.hpp"
#include <errno.h>

BEGIN_NAMESPACE_ARCHY

USING_STD_NAMESPACE;

const char * const AEmptyCStr = "";

inline
std::string::size_type s_DiffPtr(const char* end, const char* start)
{
    if (end) {
        return end - start;
    }
    return 0;
}

string& UStr::TrimLeft(string& str, const string& strSet/* = " \r\n\t"*/)
{
        string::size_type iPos = str.find_first_not_of(strSet);
        // Not found
        if( iPos == string::npos ) {
		str.erase();
                return str;
        }
        else {
                return str.erase(0, iPos);
        }
}

string& UStr::TrimRight(string& str, const string& strSet /*= " \r\t"*/)
{
        string::size_type iPos = str.find_last_not_of(strSet);
        // Not found
        if( iPos == string::npos ) {
				str.erase();
                return str;
        }
        else{
                return str.erase(iPos + 1);
        }
}

string& UStr::Trim(string& str, const string& strSet /*= " \r\n\t"*/)
{
	TrimLeft(str, strSet);
	return TrimRight(str, strSet);
}

int UStr::CompareCase(const string& str, SIZE_TYPE pos, SIZE_TYPE n,
                      const char* pattern)
{
    if (pos == NPOS  ||  !n  ||  str.length() <= pos) {
        return *pattern ? -1 : 0;
    }
    if ( !*pattern ) {
        return 1;
    }

    if (n == NPOS  ||  n > str.length() - pos) {
        n = str.length() - pos;
    }

    const char* s = str.data() + pos;
    while (n  &&  *pattern  &&  *s == *pattern) {
        s++;  pattern++;  n--;
    }

    if (n == 0) {
        return *pattern ? -1 : 0;
    }

    return *s - *pattern;
}


int UStr::CompareNocase(const string& str, SIZE_TYPE pos, SIZE_TYPE n,
                        const char* pattern)
{
    if (pos == NPOS  ||  !n  ||  str.length() <= pos) {
        return *pattern ? -1 : 0;
    }
    if ( !*pattern ) {
        return 1;
    }

    if (n == NPOS  ||  n > str.length() - pos) {
        n = str.length() - pos;
    }

    const char* s = str.data() + pos;
    while (n  &&  *pattern  &&  toupper(*s) == toupper(*pattern)) {
        s++;  pattern++;  n--;
    }

    if (n == 0) {
        return *pattern ? -1 : 0;
    }

    return toupper(*s) - toupper(*pattern);
}


int UStr::CompareCase(const string& str, SIZE_TYPE pos, SIZE_TYPE n,
                      const string& pattern)
{
    if (pos == NPOS  ||  !n  ||  str.length() <= pos) {
        return pattern.empty() ? 0 : -1;
    }
    if ( pattern.empty() ) {
        return 1;
    }

    if (n == NPOS  ||  n > str.length() - pos) {
        n = str.length() - pos;
    }

    SIZE_TYPE n_cmp = n;
    if (n_cmp > pattern.length()) {
        n_cmp = pattern.length();
    }
    const char* s = str.data() + pos;
    const char* p = pattern.data();
    while (n_cmp  &&  *s == *p) {
        s++;  p++;  n_cmp--;
    }

    if (n_cmp == 0) {
        if (n == pattern.length())
            return 0;
        return n > pattern.length() ? 1 : -1;
    }

    return *s - *p;
}


int UStr::CompareNocase(const string& str, SIZE_TYPE pos, SIZE_TYPE n,
                        const string& pattern)
{
    if (pos == NPOS  ||  !n  ||  str.length() <= pos) {
        return pattern.empty() ? 0 : -1;
    }
    if ( pattern.empty() ) {
        return 1;
    }

    if (n == NPOS  ||  n > str.length() - pos) {
        n = str.length() - pos;
    }

    SIZE_TYPE n_cmp = n;
    if (n_cmp > pattern.length()) {
        n_cmp = pattern.length();
    }
    const char* s = str.data() + pos;
    const char* p = pattern.data();
    while (n_cmp  &&  toupper(*s) == toupper(*p)) {
        s++;  p++;  n_cmp--;
    }

    if (n_cmp == 0) {
        if (n == pattern.length())
            return 0;
        return n > pattern.length() ? 1 : -1;
    }

    return toupper(*s) - toupper(*p);
}


char* UStr::ToLower(char* str)
{
    char* s;
    for (s = str;  *str;  str++) {
        *str = tolower(*str);
    }
    return s;
}


string& UStr::ToLower(string& str)
{
	string::iterator it;
	for( it = str.begin(); it != str.end(); ++it) {
        *it = tolower(*it);
    }
    return str;
}


char* UStr::ToUpper(char* str)
{
    char* s;
    for (s = str;  *str;  str++) {
        *str = toupper(*str);
    }
    return s;
}


string& UStr::ToUpper(string& str)
{
    string::iterator it;
    for (it = str.begin(); it != str.end(); ++it) {
        *it = toupper(*it);
    }
    return str;
}


int UStr::StringToNumeric(const string& str)
{
    if (str.empty()  ||  !isdigit(*str.begin())) {
        return -1;
    }
    errno = 0;
    char* endptr = 0;
    unsigned long value = strtoul(str.c_str(), &endptr, 10);
    if (errno  ||  !endptr  ||  value > (unsigned long) INT_MAX  ||
        *endptr != '\0'  ||  endptr == str.c_str()) {
        return -1;
    }
    return (int) value;
}

# define CHECK_ENDPTR()                                     \
    if (check_endptr == eCheck_Need  &&  *endptr != '\0') { \
        throw runtime_error("No symbols should follow the number"); \
    }

int UStr::StringToInt(const string& str, int base /* = 10 */,
                      ECheckEndPtr check_endptr   /* = eCheck_Need */ )
{
    errno = 0;
    char* endptr = 0;
    long value = strtol(str.c_str(), &endptr, base);
    if (errno  ||  !endptr  ||  endptr == str.c_str()  ||
        value < INT_MIN || value > INT_MAX) {
        throw runtime_error("String cannot be converted");
    }
    CHECK_ENDPTR();
    return (int) value;
}


unsigned int UStr::StringToUInt(const string& str, int base /* =10 */,
                                ECheckEndPtr check_endptr   /* =eCheck_Need */)
{
    errno = 0;
    char* endptr = 0;
    unsigned long value = strtoul(str.c_str(), &endptr, base);
    if (errno  ||  !endptr  ||  endptr == str.c_str()  ||  value > UINT_MAX) {
        throw runtime_error("String cannot be converted");
    }
    CHECK_ENDPTR();
    return (unsigned int) value;
}


long UStr::StringToLong(const string& str, int base /* = 10 */,
                        ECheckEndPtr check_endptr   /* = eCheck_Need */ )
{
    errno = 0;
    char* endptr = 0;
    long value = strtol(str.c_str(), &endptr, base);
    if (errno  ||  !endptr  ||  endptr == str.c_str()) {
        throw runtime_error("String cannot be converted");
    }
    CHECK_ENDPTR();
    return value;
}


unsigned long UStr::StringToULong(const string& str, int base /*=10 */,
                                  ECheckEndPtr check_endptr   /*=eCheck_Need*/)
{
    errno = 0;
    char* endptr = 0;
    unsigned long value = strtoul(str.c_str(), &endptr, base);
    if (errno  ||  !endptr  ||  endptr == str.c_str()) {
        throw runtime_error("String cannot be converted");
    }
    CHECK_ENDPTR();
    return value;
}


double UStr::StringToDouble(const string& str,
                            ECheckEndPtr check_endptr /* = eCheck_Need */ )
{
    errno = 0;
    char* endptr = 0;
    double value = strtod(str.c_str(), &endptr);
    if (errno  ||  !endptr  ||  endptr == str.c_str()) {
        throw runtime_error("String cannot be converted");
    }
    if (*(endptr - 1) != '.'  &&  *endptr == '.')
        endptr++;
    CHECK_ENDPTR();
    return value;
}


string UStr::IntToString(long value, bool sign /* = false */ )
{
    char buffer[64];
    ::sprintf(buffer, sign ? "%+ld" : "%ld", value);
    return buffer;
}


string UStr::UIntToString(unsigned long value)
{
    char buffer[64];
    ::sprintf(buffer, "%lu", value);
    return buffer;
}


string UStr::Int8ToString(ARCHY_Int8 value, bool sign /* = false */ )
{
    const size_t kBufSize = (sizeof(value) * CHAR_BIT) / 3 + 2;
    char buffer[kBufSize];
    char* pos = buffer + kBufSize;

    if (value == 0) {
        *--pos = '0';
    }
    else {
        bool is_negative = value < 0;
        if ( is_negative )
            value = -value;

        do {
            *--pos = char('0' + (value % 10));
            value /= 10;
        } while ( value );

        if ( is_negative )
            *--pos = '-';
        else if ( sign )
            *--pos = '+';
    }

    return string(pos, buffer + kBufSize - pos);
}


ARCHY_Int8 UStr::StringToInt8(const string& str)
{
    bool sign = false;
    const char* pc = str.c_str();

    switch (*pc) {
    case '-':
        sign = true;
    case '+':
        ++pc;
        break;
    }

    if (!isdigit(*pc)) {
        throw runtime_error("String cannot be converted");
    }

    ARCHY_Int8 n = 0;
    ARCHY_Int8 limdiv = ARCHY_MAX_I8 / 10;
    ARCHY_Int8 limoff = ARCHY_MAX_I8 % 10;

    while (*pc) {
        if (!isdigit(*pc)) {
		   throw runtime_error("String cannot be converted");
        }
        int delta = *pc++ - '0';
        n *= 10;

        // Overflow checking
        if (n > limdiv || (n == limdiv && delta > limoff)) {
			throw runtime_error("String cannot be converted");
        }

        n += delta;
    }

    return sign ? -n : n;
}


ARCHY_Uint8 UStr::StringToUInt8(const string& str, int base /* = 10  */)
{
    const char* pc = str.c_str();

    if (*pc == '+')
        ++pc;

 
    ARCHY_Uint8 n = 0;
    ARCHY_Uint8 limdiv = ARCHY_MAX_UI8 / base;
    int   limoff = int(ARCHY_MAX_UI8 % base);

    do {
        // Do a sanity check for common radixes
        int ch = *pc; 
        if (base == 10  &&  !isdigit(ch)              ||
            base == 16  &&  !isxdigit(ch)             ||
            base == 8   &&  (ch < '0'  ||  ch > '7')  ||
            base == 2   &&  (ch < '0'  ||  ch > '1')) {

			 throw runtime_error("String cannot be converted");
        }

        int delta;  // corresponding numeric value of *pc
        if (isdigit(ch)) {
            delta = ch - '0';
        } else {
            ch = tolower(ch);
            // Got to be 'a' to 'f' because of previous sanity checks
            delta = ch - 'a' + 10;
        }
		
        n *= base;

        // Overflow checking
        if (n > limdiv  ||  (n == limdiv  &&  delta > limoff)) {
		    throw runtime_error("String cannot be converted");
        }

        n += delta;
    } while (*++pc);

    return n;
}


string UStr::UInt8ToString(ARCHY_Uint8 value)
{
    const size_t kBufSize = (sizeof(value) * CHAR_BIT) / 3 + 2;
    char buffer[kBufSize];
    char* pos = buffer + kBufSize;
    if ( value == 0 ) {
        *--pos = '0';
    }
    else {
        do {
            *--pos = char('0' + (value % 10));
            value /= 10;
        } while ( value );
    }

    return string(pos, buffer + kBufSize - pos);
}


string UStr::DoubleToString(double value)
{
    char buffer[64];
    ::sprintf(buffer, "%g", value);
    return buffer;
}


// A maximal double precision used in the double to string conversion
#if defined(WIN32)
const unsigned int kMaxDoublePrecision = 200;
#else
const unsigned int kMaxDoublePrecision = 308;
#endif
// A maximal size of a double value in a string form.
// Exponent size + sign + dot + ending '\0' + max.precision
const unsigned int kMaxDoubleStringSize = 308 + 3 + kMaxDoublePrecision;


string UStr::DoubleToString(double value, unsigned int precision)
{
    char buffer[kMaxDoubleStringSize];
    SIZE_TYPE n = DoubleToString(value, precision, buffer,
                                 kMaxDoubleStringSize);
    buffer[n] = '\0';
    return buffer;
}


SIZE_TYPE UStr::DoubleToString(double value, unsigned int precision,
                               char* buf, SIZE_TYPE buf_size)
{
    char buffer[kMaxDoubleStringSize];
    if (precision > kMaxDoublePrecision) {
        precision = kMaxDoublePrecision;
    }
    int n = ::sprintf(buffer, "%.*f", (int) precision, value);
    SIZE_TYPE n_copy = min((SIZE_TYPE) n, buf_size);
    memcpy(buf, buffer, n_copy);
    return n_copy;
}


string UStr::PtrToString(const void* value)
{
    char buffer[64];
    ::sprintf(buffer, "%p", value);
    return buffer;
}


const void* UStr::StringToPtr(const string& str)
{
    void *ptr = NULL;
    ::sscanf(str.c_str(), "%p", &ptr);
    return ptr;
}


static const string s_kTrueString  = "true";
static const string s_kFalseString = "false";
static const string s_kTString     = "t";
static const string s_kFString     = "f";
static const string s_kYesString   = "yes";
static const string s_kNoString    = "no";
static const string s_kYString     = "y";
static const string s_kNString     = "n";

const string& UStr::BoolToString(bool value)
{
    return value ? s_kTrueString : s_kFalseString;
}


bool UStr::StringToBool(const string& str)
{
    if ( AStrEquiv(str, s_kTrueString,  PNocase())  ||
         AStrEquiv(str, s_kTString,     PNocase())  ||
         AStrEquiv(str, s_kYesString,   PNocase())  ||
         AStrEquiv(str, s_kYString,     PNocase()) )
        return true;

    if ( AStrEquiv(str, s_kFalseString, PNocase())  ||
         AStrEquiv(str, s_kFString,     PNocase())  ||
         AStrEquiv(str, s_kNoString,    PNocase())  ||
         AStrEquiv(str, s_kNString,     PNocase()) )
        return false;

        throw runtime_error("String cannot be converted");
}


string UStr::TruncateSpaces(const string& str, ETrunc where)
{
    SIZE_TYPE beg = 0;
    if (where == eTrunc_Begin  ||  where == eTrunc_Both) {
        while (beg < str.length()  &&  isspace(str[beg]))
            beg++;
        if (beg == str.length())
            return "";
    }
    SIZE_TYPE end = str.length() - 1;
    if (where == eTrunc_End  ||  where == eTrunc_Both) {
        while ( isspace(str[end]) )
            end--;
    }
    ARCHY_ASSERT( beg <= end );
    return str.substr(beg, end - beg + 1);
}


string& UStr::Replace(const string& src,
                      const string& search, const string& replace,
                      string& dst, SIZE_TYPE start_pos, size_t max_replace)
{
    // source and destination should not be the same
    if (&src == &dst) {
			throw runtime_error("String method called with inappropriate arguments");
    }

    dst = src;

    if( start_pos + search.size() > src.size() ||
        search == replace)
        return dst;

    for(size_t count = 0; !(max_replace && count >= max_replace); count++) {
        start_pos = dst.find(search, start_pos);
        if(start_pos == NPOS)
            break;
        dst.replace(start_pos, search.size(), replace);
        start_pos += replace.size();
    }
    return dst;
}


string UStr::Replace(const string& src,
                     const string& search, const string& replace,
                     SIZE_TYPE start_pos, size_t max_replace)
{
    string dst;
    return Replace(src, search, replace, dst, start_pos, max_replace);
}


list<string>& UStr::Split(const string& str, const string& delim,
                          list<string>& arr, EMergeDelims merge)
{
    for (size_t pos = 0; ; ) {
        size_t prev_pos = (merge == eMergeDelims
                           ? str.find_first_not_of(delim, pos)
                           : pos);
        if (prev_pos == NPOS) {
            break;
        }
        pos = str.find_first_of(delim, prev_pos);
        if (pos == NPOS) {
            arr.push_back(str.substr(prev_pos));
            break;
        } else {
            arr.push_back(str.substr(prev_pos, pos - prev_pos));
            ++pos;
        }
    }
    return arr;
}

vector<string>& UStr::Tokenize(const string& str, const string& delim,
                               vector<string>& arr, EMergeDelims merge)
{
    if (delim.empty()) {
        arr.push_back(str);
        return arr;
    }

    size_t pos, prev_pos;

    // Count number of tokens to determine the array size
    size_t tokens = 0;

    for (pos = prev_pos = 0; pos < str.length(); ++pos) {
        char c = str[pos];
        size_t dpos = delim.find(c);
        if (dpos != string::npos) ++tokens;
    }

    arr.reserve(arr.size() + tokens + 1);

    // Tokenization
    for (pos = 0; ; ) {
        prev_pos = (merge == eMergeDelims ? str.find_first_not_of(delim, pos)
                    : pos);
        if (prev_pos == NPOS) {
            break;
        }
        pos = str.find_first_of(delim, prev_pos);
        if (pos == NPOS) {
            arr.push_back(str.substr(prev_pos));
            break;
        } else {
            arr.push_back(str.substr(prev_pos, pos - prev_pos));
            ++pos;
        }
    }

    return arr;
}


string UStr::Join(const list<string>& arr, const string& delim)
{
    if (arr.empty()) {
        return "";
    }

    string                       result = arr.front();
    list<string>::const_iterator it     = arr.begin();
    while (++it != arr.end()) {
        result += delim;
        result += *it;
    }
    return result;
}


string UStr::PrintableString(const string&      str,
                             UStr::ENewLineMode nl_mode)
{
    string s;
	string::const_iterator it;
	for( it = str.begin(); it != str.end(); ++it ) {
        if (*it == '\\') {
            s += "\\\\";
        } else if (*it == '\n') {
            s += nl_mode == eNewLine_Quote ? "\\n" : "\n";
        } else if (*it == '\t') {
            s += "\\t";
        } else if (*it == '\r') {
            s += "\\r";
        } else if (*it == '\v') {
            s += "\\v";
        } else if (*it == '"') {
            s += "\\\"";
        } else if ( isprint(*it) ) {
            s += *it;
        } else {
            static const char s_Hex[] = "0123456789ABCDEF";
            s += "\\x";
            s += s_Hex[(unsigned char) *it / 16];
            s += s_Hex[(unsigned char) *it % 16];
        }
    }
    return s;
}


// Determines the end of an HTML <...> tag, accounting for attributes
// and comments (the latter allowed only within <!...>).
static SIZE_TYPE s_EndOfTag(const string& str, SIZE_TYPE start)
{
    ARCHY_ASSERT(start < str.size()  &&  str[start] == '<');
    bool comments_ok = (start + 1 < str.size()  &&  str[start + 1] == '!');
    for (SIZE_TYPE pos = start + 1;  pos < str.size();  ++pos) {
        switch (str[pos]) {
        case '>': // found the end
            return pos;

        case '\"': // start of "string"; advance to end
            pos = str.find('\"', pos + 1);
            if (pos == NPOS) {
                throw runtime_error("Unclosed string in HTML tag");
                // return pos;
            }
            break;

        case '-': // possible start of -- comment --; advance to end
            if (comments_ok  &&  pos + 1 < str.size()
                &&  str[pos + 1] == '-') {
                pos = str.find("--", pos + 2);
                if (pos == NPOS) {
                    throw runtime_error("Unclosed comment in HTML tag");
                    // return pos;
                } else {
                    ++pos;
                }
            }
        }
    }
    throw runtime_error("Unclosed HTML tag");
    // return NPOS;
}


// Determines the end of an HTML &foo; character/entity reference
// (which might not actually end with a semicolon :-/)
static SIZE_TYPE s_EndOfReference(const string& str, SIZE_TYPE start)
{
    ARCHY_ASSERT(start < str.size()  &&  str[start] == '&');
#ifdef NCBI_STRICT_HTML_REFS
    return str.find(';', start + 1);
#else
    SIZE_TYPE pos = str.find_first_not_of
        ("#0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
         start + 1);
    if (pos == NPOS  ||  str[pos] == ';') {
        return pos;
    } else {
        return pos - 1;
    }
#endif
}


static SIZE_TYPE s_VisibleWidth(const string& str, bool is_html)
{
    if (is_html) {
        SIZE_TYPE width = 0, pos = 0;
        for (;;) {
            SIZE_TYPE pos2 = str.find_first_of("<&", pos);
            if (pos2 == NPOS) {
                width += str.size() - pos;
                break;
            } else {
                width += pos2 - pos;
                if (str[pos2] == '&') {
                    ++width;
                    pos = s_EndOfReference(str, pos);
                } else {
                    pos = s_EndOfTag(str, pos);
                }
                if (pos == NPOS) {
                    break;
                } else {
                    ++pos;
                }
            }
        }
        return width;
    } else {
        return str.size();
    }
}


list<string>& UStr::Wrap(const string& str, SIZE_TYPE width,
                         list<string>& arr, UStr::TWrapFlags flags,
                         const string* prefix, const string* prefix1)
{
	static const string aprefex;
    if (prefix == 0) {
        prefix = &aprefex;
    }

    const string* pfx = prefix1 ? prefix1 : prefix;
    SIZE_TYPE     pos = 0, len = str.size();
    string        hyphen; // "-" or empty
    bool          is_html  = flags & fWrap_HTMLPre ? true : false;

    enum EScore { // worst to best
        eForced,
        ePunct,
        eSpace,
        eNewline
    };

    while (pos < len) {
        SIZE_TYPE column     = s_VisibleWidth(*pfx, is_html);
        SIZE_TYPE column0    = column;
        // the next line will start at best_pos
        SIZE_TYPE best_pos   = NPOS;
        EScore    best_score = eForced;
        for (SIZE_TYPE pos2 = pos;  pos2 < len && column <= width;
             ++pos2, ++column) {
            EScore    score     = eForced;
            SIZE_TYPE score_pos = pos2;
            char      c         = str[pos2];

            if (c == '\n') {
                best_pos   = pos2;
                best_score = eNewline;
                break;
            } else if (isspace(c)) {
                if (pos2 > 0  &&  isspace(str[pos2 - 1])) {
                    continue; // take the first space of a group
                }
                score = eSpace;
            } else if (is_html  &&  c == '<') {
                // treat tags as zero-width...
                pos2 = s_EndOfTag(str, pos2);
                --column;
            } else if (is_html  &&  c == '&') {
                // ...and references as single characters
                pos2 = s_EndOfReference(str, pos2);
            } else if (ispunct(c)) {
                if (c == '('  ||  c == '['  ||  c == '{'  ||  c == '<'
                    ||  c == '`') { // opening element
                    score = ePunct;
                } else if (score_pos < len - 1) {
                    // Prefer breaking *after* most types of punctuation.
                    score = ePunct;
                    ++score_pos;
                }
            }

            if (score >= best_score) {
                best_pos   = score_pos;
                best_score = score;
            }

            while (pos < len - 1  &&  str[pos + 1] == '\b') {
                // Account for backspaces
                ++pos;
                if (column > column0) {
                    --column;
                }
            }
        }

        if (best_score != eNewline  &&  column <= width) {
            // If the whole remaining text can fit, don't split it...
            best_pos = len;
        } else if (best_score == eForced  &&  (flags & fWrap_Hyphenate)) {
            hyphen = "-";
            --best_pos;
        }
        arr.push_back(*pfx);
        {{ // eat backspaces and the characters (if any) that precede them
            string    line(str, pos, best_pos - pos);
            SIZE_TYPE bs = 0;
            while ((bs = line.find('\b', bs)) != NPOS) {
                if (bs > 0) {
                    line.erase(bs - 1, 2);
                } else {
                    line.erase(0, 1);
                }
            }
            arr.back() += line;
        }}
        arr.back() += hyphen;
        pos    = best_pos;
        pfx    = prefix;
        hyphen = "";

        if (best_score == eSpace) {
            // If breaking at a group of spaces, skip over the whole group
            while (pos < len  &&  isspace(str[pos])  &&  str[pos] != '\n') {
                ++pos;
            }
        } else if (best_score == eNewline) {
            ++pos;
        }
        while (pos < len  &&  str[pos] == '\b') {
            ++pos;
        }
    }

    return arr;
}


list<string>& UStr::WrapList(const list<string>& l, SIZE_TYPE width,
                             const string& delim, list<string>& arr,
                             UStr::TWrapFlags flags, const string* prefix,
                             const string* prefix1)
{
    if (l.empty()) {
        return arr;
    }

    const string* pfx      = prefix1 ? prefix1 : prefix;
    string        s        = *pfx;
    bool          is_html  = flags & fWrap_HTMLPre ? true : false;
    SIZE_TYPE     column   = s_VisibleWidth(s,     is_html);
    SIZE_TYPE     delwidth = s_VisibleWidth(delim, is_html);
    bool          at_start = true;
	list<string>::const_iterator it = l.begin();

    for ( ; it != l.end(); ++it ) {
        SIZE_TYPE term_width = s_VisibleWidth(*it, is_html);
        if (at_start) {
            if (column + term_width <= width) {
                s += *it;
                column += term_width;
                at_start = false;
            } else {
                // Can't fit, even on its own line; break separately.
                Wrap(*it, width, arr, flags, prefix, pfx);
                pfx      = prefix;
                s        = *prefix;
                column   = s_VisibleWidth(s, is_html);
                at_start = true;
            }
        } else if (column + delwidth + term_width <= width) {
            s += delim;
            s += *it;
            column += delwidth + term_width;
            at_start = false;
        } else {
            // Can't fit on this line; break here and try again.
            arr.push_back(s);
            pfx      = prefix;
            s        = *prefix;
            column   = s_VisibleWidth(s, is_html);
            at_start = true;
            --it;
        }
    }

    arr.push_back(s);
    return arr;
}


#if !defined(ARCHY_HAVE_STRDUP)
extern char* strdup(const char* str)
{
    if ( !str )
        return 0;

    size_t size   = strlen(str) + 1;
    void*  result = malloc(size);
    return (char*) (result ? memcpy(result, str, size) : 0);
}
#endif


END_NAMESPACE_ARCHY


