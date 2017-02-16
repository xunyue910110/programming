  #include "StringBuffer.hpp"
#include "debug.hpp"
BEGIN_NAMESPACE_ARCHY


StringBuffer::StringBuffer(const char *buf /*= 0*/)
{
    if (buf) {
		magic = new MutableStringBuffer(buf, strlen(buf));
    }
    else magic = new MutableStringBuffer();

}

StringBuffer::StringBuffer(const char *buf, unsigned int len)
{
    if (buf) {
		magic = new MutableStringBuffer(buf, len);
    }
    else magic = new MutableStringBuffer();
}
StringBuffer::~StringBuffer()
{

}

StringBuffer::StringBuffer(const StringBuffer& s)
:magic(s.magic)
{
}

StringBuffer::StringBuffer(const char ch, unsigned int nRepeat /*= 1*/)
{
	magic = new MutableStringBuffer(nRepeat + 1);
	magic->len = nRepeat;
	while( nRepeat--) {
		magic->buf[nRepeat] = ch;
	}
}

StringBuffer::StringBuffer(int i)
{
    char str[16];
    unsigned int len = (unsigned int)sprintf(str,"%d",i);
	magic = new MutableStringBuffer(str, len);
}

StringBuffer::StringBuffer(unsigned int i)
{
    char str[16];
    unsigned int len = (unsigned int)sprintf(str,"%u",i);
	magic = new MutableStringBuffer(str, len);
}

StringBuffer::StringBuffer(short i)
{
    char str[16];
    unsigned int len = (unsigned int)sprintf(str,"%d",i);
	magic = new MutableStringBuffer(str, len);
}
StringBuffer::StringBuffer(unsigned short i)
{
    char str[16];
    unsigned int len = (unsigned int)sprintf(str,"%u",i);
	magic = new MutableStringBuffer(str, len);
}
StringBuffer::StringBuffer(long l)
{
    char str[32];
    unsigned int len = (unsigned int)sprintf(str,"%ld",l);
	magic = new MutableStringBuffer(str, len);
}

StringBuffer::StringBuffer(unsigned long l)
{
    char str[32];
    unsigned int len = (unsigned int)sprintf(str,"%lu",l);
	magic = new MutableStringBuffer(str, len);
}

StringBuffer::StringBuffer(float f)
{
    char str[512];
#if HAVE_SNPRINTF
    int len=snprintf(str,sizeof(str),"%.25f",f);
#else
    int len=sprintf(str,"%.25f",f);
#endif
    while (len--&&str[len]=='0') {
		str[len]=0;
    }
    if (str[len]=='.') str[len--]=0;
    ++len;
	magic = new MutableStringBuffer(str, (unsigned int)len);
}
StringBuffer::StringBuffer(double d)
{
    char str[512];
#if HAVE_SNPRINTF
    int len=snprintf(str,sizeof(str),"%.25f",d);
#else
    int len=sprintf(str,"%.25f",d);
#endif
    while (len--&&str[len]=='0') {
		str[len]=0;
    }
    if (str[len]=='.') str[len--]=0;
    ++len;
	magic = new MutableStringBuffer(str, (unsigned int)len);
}

void StringBuffer::MutableStringBuffer::reallocate(unsigned int min) {
    if (min<len) min=len;
    if (min==0) {
		if (buf) delete [] buf;
		buf=new char[16];
		buf[0]=0;
		maxlen=16;
		len=0;
		return;
    }
    maxlen=(unsigned int)((min+16)*1.2);
    //assert(maxlen>=len);
    char *newbuf=new char[maxlen];
    if (buf) {
		memcpy(newbuf,buf,len);
		delete [] buf;
    }
    newbuf[len]=0;
    buf=newbuf;
}


StringBuffer& StringBuffer::operator = (const StringBuffer& s)
{
	if( this != &s ) {
		magic = s.magic;
	}
	return *this;
}

int StringBuffer::GetLength()const
{
	return magic->len;
}

//	将一个字符串对象的长度强制设为零。
void StringBuffer::Empty ()
{
	magic->buf[0] = 0;
	magic->len = 0;
}

//	测试一个字符串对象是否是空
int StringBuffer::IsEmpty () const
{
	return magic->len == 0;
}

char StringBuffer::GetAt (int nIndex ) const
{
	ARCHY_ASSERT(nIndex >= 0 && nIndex < magic->len);
	return magic->buf[nIndex];
}

char StringBuffer::operator [] (int nIndex ) const
{
	ARCHY_ASSERT(nIndex >= 0 && nIndex < magic->len);
	return magic->buf[nIndex];
}

void StringBuffer::SetAt (int nIndex, const char ch)
{
	ARCHY_ASSERT(nIndex >= 0 && nIndex < magic->len);
	magic->buf[nIndex] = ch;
}

const char*StringBuffer::c_str() const
{
	return magic->buf;
}

StringBuffer& StringBuffer::operator += (const StringBuffer& s)
{
	magic->append(s.magic->buf, s.magic->len);
	return *this;
}

int StringBuffer::Compare (const StringBuffer& right) const
{
	/* 比较字符串缓冲区里的字符序列 */	
	const char* p = magic->buf;
	const char* q = right.magic->buf;
	
	for( ; *p != '\0' && *p == *q; ++p, ++q )
		;/* 空循环体，字符相同就继续比较 */
	return *p - *q;
}

int StringBuffer::CompareNoCase (const StringBuffer& right) const
{
	/* 比较字符串缓冲区里的字符序列 */	
	const char* p = magic->buf;
	const char* q = right.magic->buf;
	
	for( ; *p != '\0' && 
			( *p >= 'a' && *p <= 'z' ? *p - 32 : *p ) 
				== ( *q >= 'a' && *q <= 'z' ? *q - 32 : *q ); 
			++p, ++q )
		;/* 空循环体，字符相同就继续比较 */
	return ( *p >= 'a' && *p <= 'z' ? *p - 32 : *p ) 
		- (*q >= 'a' && *q <= 'z' ? *q - 32 : *q);
}

StringBuffer  StringBuffer::Mid (int nFirst, int nCount /*= 0*/) const
{
	ARCHY_ASSERT( nFirst >= 0 && nFirst < (int)magic->len );
	ARCHY_ASSERT( nCount >= 0 );
	if( nCount == 0 || nCount + nFirst > magic->len )
		nCount = magic->len - nFirst;

	return StringBuffer(magic->buf + nFirst, nCount);
}

StringBuffer StringBuffer::Left (int nCount) const
{
	ARCHY_ASSERT( nCount > 0 );
	if( nCount > magic->len )
		nCount = magic->len;

	return StringBuffer(magic->buf, nCount);
}

StringBuffer StringBuffer::Right (int nCount) const
{
	ARCHY_ASSERT( nCount > 0 );
	if( nCount > magic->len )
		nCount = magic->len;
	return StringBuffer(magic->buf + magic->len - nCount, nCount);
}


void StringBuffer::MakeUpper ()
{
	for( unsigned int i = 0; i  < magic->len; i++ ) {
		if( magic->buf[i] >= 'a' &&  magic->buf[i] <= 'z' )
			 magic->buf[i] -=  32;
	}

}

void StringBuffer::MakeLower ()
{
	for( unsigned int i = 0; i  < magic->len; i++ ) {
		if( magic->buf[i] >= 'A' &&  magic->buf[i] <= 'Z' )
			 magic->buf[i] +=  32;
	}
}

void StringBuffer::MakeReverse ()
{
	char chSingle;
	for( unsigned int i = 0; i < magic->len/2 ; i++ ) {
		chSingle = magic->buf[i];
		 magic->buf[i] =   magic->buf[magic->len - i - 1];
		 magic->buf[magic->len - i - 1] = chSingle;
	}	
}

int StringBuffer::Replace (const char* pstrOld, const char* pstrNew /*= 0*/)
{
	char* pSearch = strstr( magic->buf, pstrOld );
	int iCount = 0;

	while(pSearch != 0) {
		++iCount;
		unsigned int iRealLen = magic->len - strlen(pstrOld) + (pstrNew == 0 ? 0 : strlen(pstrNew));
	
		rcptr<MutableStringBuffer> newMagic = new MutableStringBuffer(iRealLen);

		// 复制前面部分
		strncpy( newMagic->buf, magic->buf, pSearch - magic->buf);
		newMagic->buf[pSearch - magic->buf] = 0;
		// 赋值替换的部分
		if(pstrNew)
			strcat(newMagic->buf, pstrNew);
		// 赋值后面的部分
		strcat(newMagic->buf, pSearch + strlen(pstrOld));

		newMagic->len = strlen(newMagic->buf);
		magic = newMagic;
		pSearch = strstr( pSearch, pstrOld );
	}

	return iCount;
}

int StringBuffer::Replace (const char cOld, const char cNew )
{
	if( cOld == cNew )return 0;

	int iCount = 0;
	for(int i = 0; i < magic->len; i++ ) {
		if( magic->buf[i] == cOld ) {
			++iCount;
			magic->buf[i] = cNew;
		}
	}
	return iCount;
}

int StringBuffer::Remove (char ch)
{
	rcptr<MutableStringBuffer> newMagic = new MutableStringBuffer(magic->len);

	int iCount = 0;
	char *pOffset = newMagic->buf;
	for( int i = 0; i < magic->len; i++ )
		if( magic->buf[i] != ch ) {
			*pOffset++ = magic->buf[i];
			++newMagic->len;
		}
		else {
			iCount++;
		}
	*pOffset = 0;

	magic = newMagic;
	return iCount;
}

int StringBuffer::Insert (int nIndex, const char* pstrInsert)
{
	int iLen = magic->len + strlen(pstrInsert);
	ARCHY_ASSERT( nIndex >= 0 );
	// 如果起始位置大于字符串的长度，则认为是在字符串末尾插入
	if( nIndex >= magic->len )
		nIndex = magic->len;
	rcptr<MutableStringBuffer> newMagic = new MutableStringBuffer(magic->len + strlen(pstrInsert));

	newMagic->append(magic->buf, nIndex);
	newMagic->append(pstrInsert, strlen(pstrInsert));
	newMagic->append(magic->buf + nIndex, magic->len - nIndex);

	magic = newMagic;
	return magic->len;
}

int StringBuffer::Insert (int nIndex, const char cInsert)
{
	char strTemp[2];
	sprintf(strTemp,"%c", cInsert);
	return Insert( nIndex, strTemp);
}

int StringBuffer::Delete (int nIndex, int nCount /*= 1*/)
{
	ARCHY_ASSERT( nIndex >= 0  && nIndex <= magic->len && nCount >=1 );

	if ( nCount > magic->len - nIndex )
		nCount = magic->len - nIndex;

	memmove( magic->buf + nIndex, magic->buf + nIndex + nCount, 
		magic->len - nIndex - nCount);
	magic->len -= nCount;
	magic->buf[magic->len] = 0;
	return magic->len;
}

void StringBuffer::TrimLeft (const char* pstrTarget /*= " \t\r\n"*/)
{
	unsigned int iLen = magic->len;
	unsigned int jLen = strlen(pstrTarget);
	unsigned int i,j;
	for(i = 0; i < iLen; i++) {
		for( j = 0; j < jLen; j++ )
			if( pstrTarget[j] == magic->buf[i] )
				break;
			if( j == jLen)
				break;
	}
	if( i != 0 ) {
		Delete( 0, i );
	}
}

void StringBuffer::TrimLeft (const char cTarget)
{
	unsigned int iLen = magic->len;
	unsigned int i;
	for(i = 0; i < iLen; i++) {
		if( cTarget != magic->buf[i] )
			break;
	}
	if( i != 0 ) {
		Delete( 0, i );
	}
}

void StringBuffer::TrimRight (const char* pstrTarget /*= " \t\r\n"*/)
{
	unsigned int iLen = magic->len;
	unsigned int jLen = (int)strlen(pstrTarget);
	int i,j;
	for(i = iLen - 1; i >= 0; i--) {
		for( j = 0; j < jLen; j++ )
			if( pstrTarget[j] == magic->buf[i] )
				break;
			if( j == jLen)
				break;
	}
	magic->buf[i+1] = 0;
	magic->len = i + 1;
}

void StringBuffer::TrimRight (const char cTarget)
{
	unsigned int  iLen = magic->len;
	int i;
	for(i = iLen - 1; i >= 0; i--) {
		if( cTarget != magic->buf[i] )
			break;
	}
	magic->buf[i+1] = 0;
	magic->len = i + 1;
}

//	从指定位置开始搜索字符，默认从头开始
int StringBuffer::Find (const char ch, int nStart /*= 0*/) const
{
	const char* p = magic->buf + nStart;
	const char* q = strchr( p, ch );
	return q == 0 ? -1 : q - magic->buf;
}

int StringBuffer::Find (const char* str, int nStart /*= 0*/) const
{
	const char* p = magic->buf + nStart;
	const char* q = strstr( p, str );
	return q == 0 ? -1 : q - magic->buf;
}

int StringBuffer::ReverseFind (const char ch) const
{
	const char* q = strrchr( magic->buf, ch );
	return q == 0 ? -1 : q - magic->buf;
}

int StringBuffer::FindOneOf (const char* pstrCharSet)
{
	unsigned int iLen = magic->len;
	unsigned  int jLen = strlen(pstrCharSet);
	for( int i = 0; i < iLen; i++ ) 
		for( int j = 0; j < jLen; j++ )
			if( magic->buf[i] == pstrCharSet[j] )
				return i;
	return -1;
}


void StringBuffer::Append(const char *bytes,unsigned int len) {
    magic->append(bytes,len);
}

StringBuffer &StringBuffer::operator<< (int i) {
    char str[16];
    int len=sprintf(str,"%d",i);
    Append(str,len);
    return *this;
}

StringBuffer &StringBuffer::operator<< (unsigned int i) {
    char str[16];
    int len=sprintf(str,"%u",i);
    Append(str,len);
    return *this;
}

StringBuffer &StringBuffer::operator<< (long l) {
    char str[16];
    int len=sprintf(str,"%ld",l);
    Append(str,len);
    return *this;
}

StringBuffer &StringBuffer::operator<< (unsigned long l) {
    char str[16];
    int len=sprintf(str,"%lu",l);
    Append(str,len);
    return *this;
}

StringBuffer &StringBuffer::operator<< (const double &f) {
    char str[512];
#if HAVE_SNPRINTF
    int len=snprintf(str,sizeof(str),"%.25f",f);
#else
    int len=sprintf(str,"%.25f",f);
#endif
    Append(str,len);
    return *this;
}

char StringBuffer::operator>> (char c) {
    c=0;
    unsigned int len=magic->len;
    if (len) {
		c=magic->buf[len-1];
		magic->len=len-1;
		magic->buf[len-1]=0;
    }
    return c;
}


StringBuffer::MutableStringBuffer::MutableStringBuffer(const char *ibuf,unsigned int len) : len(len), buf(0) {
    reallocate(len+1);
    memcpy(buf,ibuf,len);
}

StringBuffer::MutableStringBuffer::MutableStringBuffer(const MutableStringBuffer &other) : len(other.len), buf(0) {
    reallocate(other.len+1);
    memcpy(buf,other.buf,len);
}

StringBuffer::MutableStringBuffer::MutableStringBuffer(unsigned int max) : len(0), buf(0) {
    reallocate(max);
}

StringBuffer::MutableStringBuffer::MutableStringBuffer() : len(0), maxlen(0), buf(0) {
    reallocate(maxlen);
}

void StringBuffer::MutableStringBuffer::append(const char *ap,unsigned int amount) {
    if (ap&&amount) {
		if (len+amount+1>maxlen) {
			reallocate(len+amount+1);
		}
		memcpy(&buf[len],ap,amount);
		len+=amount;
		buf[len]=0;
    }
}

END_NAMESPACE_ARCHY

ostream &operator<< (ostream &o,const archy::StringBuffer &s) {
    o.write(s.c_str(),s.GetLength());
    return o;
}

archy::StringBuffer operator + (const archy::StringBuffer& left, const archy::StringBuffer& right)
{
	archy::StringBuffer newStr(left);
	newStr += right;
	return newStr;
}
bool operator <=(const archy::StringBuffer& left, const archy::StringBuffer& right)
{
	return left.Compare(right) <= 0;
}
bool operator <=(const archy::StringBuffer& left, const char* right)
{
	return left.Compare(right) <= 0;
}
bool operator <=(const char* left, const archy::StringBuffer& right)
{
	return right.Compare(left) >= 0;

}
bool operator <(const archy::StringBuffer& left, const archy::StringBuffer& right)
{
	return left.Compare(right) < 0;
}
bool operator <(const archy::StringBuffer& left, const char* right)
{
	return left.Compare(right) < 0;
}
bool operator <(const char* left, const archy::StringBuffer& right)
{
	return right.Compare(left) > 0;
}
bool operator >=(const archy::StringBuffer& left, const archy::StringBuffer& right)
{
	return left.Compare(right) >= 0;
}
bool operator >=(const archy::StringBuffer& left, const char* right)
{
	return left.Compare(right) >= 0;
}
bool operator >=(const char* left, const archy::StringBuffer& right)
{
	return right.Compare(left) <= 0;
}
bool operator >(const archy::StringBuffer& left, const archy::StringBuffer& right)
{
	return left.Compare(right) > 0;
}
bool operator >(const archy::StringBuffer& left, const char* right)
{
	return left.Compare(right) > 0;
}
bool operator >(const char* left, const archy::StringBuffer& right)
{
	return right.Compare(left) < 0;
}
bool operator ==(const archy::StringBuffer& left, const archy::StringBuffer& right)
{
	return left.Compare(right) == 0;
}
bool operator ==(const char* left, const archy::StringBuffer& right)
{
	return right.Compare(left) == 0;
}
bool operator !=(const archy::StringBuffer& left, const archy::StringBuffer& right)
{
	return left.Compare(right) != 0;
}
bool operator !=(const archy::StringBuffer& left, const char* right)
{
	return left.Compare(right) != 0;
}

bool operator !=(const char* left, const archy::StringBuffer& right)
{
	return right.Compare(left) != 0;
}

bool operator ==(const archy::StringBuffer& left, const char* right)
{
	return left.Compare(right) == 0;
}
