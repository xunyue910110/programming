#if !defined(AFX_STRINGBUFFER_HPP__E313914F_5ED7_422F_A8D2_A5625E4A6DEC__INCLUDED_)
#define AFX_STRINGBUFFER_HPP__E313914F_5ED7_422F_A8D2_A5625E4A6DEC__INCLUDED_

#include "archy_config.hpp"
#include "rcptr.hpp"
USING_STD_NAMESPACE;

BEGIN_NAMESPACE_ARCHY

class StringBuffer  
{
public:
	StringBuffer(const char *buf = 0);
	StringBuffer(const char *buf, unsigned int len);
	~StringBuffer();

	StringBuffer(const StringBuffer& s);
	//	生成单个字符的构造函数
	StringBuffer(const char ch, unsigned int nRepeat = 1);

	explicit StringBuffer(int i);
	explicit StringBuffer(unsigned int i);
	explicit StringBuffer(short i);
	explicit StringBuffer(unsigned short i);
	explicit StringBuffer(long l);
	explicit StringBuffer(unsigned long l);
	explicit StringBuffer(float f);
	explicit StringBuffer(double d);

	StringBuffer& operator = (const StringBuffer& s);

	int GetLength()const;

	//	将一个字符串对象的长度强制设为零。
	void Empty ();

	//	测试一个字符串对象是否是空
	int IsEmpty () const;

	char GetAt (int nIndex ) const;

	char operator [] (int nIndex ) const;

	void SetAt (int nIndex, const char ch);

	const char* c_str() const;
	operator const char*()const {
		return c_str();
	}

	StringBuffer& operator += (const StringBuffer& s);

	int Compare (const StringBuffer& right) const;

	int CompareNoCase (const StringBuffer& right) const;

	StringBuffer  Mid (int nFirst, int nCount = 0) const;

	StringBuffer Left (int nCount) const;

	StringBuffer Right (int nCount) const;

	void MakeUpper ();

	void MakeLower ();

	void MakeReverse ();

	int Replace (const char* pstrOld, const char* pstrNew = 0);

	int Replace (const char cOld, const char cNew );

	int Remove (char ch);

	int Insert (int nIndex, const char* pstrInsert);

	int Insert (int nIndex, const char cInsert);
	
	int Delete (int nIndex, int nCount = 1);

	void TrimLeft (const char* pstrTarget = " \t\r\n");

	void TrimLeft (const char cTarget);

	void TrimRight (const char* pstrTarget = " \t\r\n");

	void TrimRight (const char cTarget);

	//	从指定位置开始搜索字符，默认从头开始
	int Find (const char ch, int nStart = 0) const;

	int Find (const char* str, int nStart = 0) const;

	int ReverseFind (const char ch) const;

	int FindOneOf (const char* pstrCharSet);

    inline int ToInteger() const {
		return atoi(magic->buf);
    }
    inline unsigned int ToUnsignedInteger() const {
		return strtoul(magic->buf,0,0);
    }
    inline double ToDouble() const {
		return atof(magic->buf);
    }
	void Append(const char *bytes,unsigned int len);

    inline StringBuffer &operator<< (const char *s) {
		Append(s, strlen(s));
		return *this;
    }
    inline StringBuffer &operator<< (char c) {
		Append(&c,1);
		return *this;
    }
    inline StringBuffer &operator<< (unsigned char c) {
		Append((char *)&c,1);
		return *this;
    }
    inline void Append(const StringBuffer &s) {
		Append(s.c_str(), s.GetLength());
    }

    StringBuffer &operator<< (int);
    StringBuffer &operator<< (unsigned int);
    StringBuffer &operator<< (long);
    StringBuffer &operator<< (unsigned long);
    inline StringBuffer &operator<< (short i) {
		return *this<<(int)i;
    }
    inline StringBuffer &operator<< (unsigned short i) {
		return *this<<(unsigned int)i;
    }
    StringBuffer &operator<< (const double &f);

    inline StringBuffer &operator<< (const StringBuffer &s) {
		Append(s);
		return *this;
    }
    inline StringBuffer &operator<< (const StringBuffer *s) {
		Append(*s);
		return *this;
    }
    char operator>>(char c);

private:
	class MutableStringBuffer{
	public:
	  unsigned int len,maxlen;
	  char *buf;
  
	  MutableStringBuffer(const char *ibuf,unsigned int len);
	  MutableStringBuffer(const MutableStringBuffer &other);
	  MutableStringBuffer(unsigned int max);
	  MutableStringBuffer();
  
	  inline ~MutableStringBuffer() {
		  delete [] buf;
	  }
	  void append(const char *ap,unsigned int amount);
	  inline void append(MutableStringBuffer *other) {
		  append(other->buf,other->len);
	  }
	  void reallocate(unsigned int min);
	};
	rcptr<MutableStringBuffer> magic;
};

END_NAMESPACE_ARCHY


ostream &operator << (ostream &o, const archy::StringBuffer &s);

archy::StringBuffer operator + (const archy::StringBuffer& left, const archy::StringBuffer& right);
bool operator <=(const archy::StringBuffer& left, const archy::StringBuffer& right);
bool operator <=(const archy::StringBuffer& left, const char* right);
bool operator <=(const char* left, const archy::StringBuffer& right);
bool operator <(const archy::StringBuffer& left, const archy::StringBuffer& right);
bool operator <(const archy::StringBuffer& left, const char* right);
bool operator <(const char* left, const archy::StringBuffer& right);
bool operator >=(const archy::StringBuffer& left, const archy::StringBuffer& right);
bool operator >=(const archy::StringBuffer& left, const char* right);
bool operator >=(const char* left, const archy::StringBuffer& right);
bool operator >(const archy::StringBuffer& left, const archy::StringBuffer& right);
bool operator >(const archy::StringBuffer& left, const char* right);
bool operator >(const char* left, const archy::StringBuffer& right);
bool operator ==(const archy::StringBuffer& left, const archy::StringBuffer& right);
bool operator ==(const archy::StringBuffer& left, const char* right);

bool operator ==(const char* left, const archy::StringBuffer& right);
bool operator !=(const archy::StringBuffer& left, const archy::StringBuffer& right);
bool operator !=(const archy::StringBuffer& left, const char* right);
bool operator !=(const char* left, const archy::StringBuffer& right);


#endif // !defined(AFX_STRINGBUFFER_HPP__E313914F_5ED7_422F_A8D2_A5625E4A6DEC__INCLUDED_)
