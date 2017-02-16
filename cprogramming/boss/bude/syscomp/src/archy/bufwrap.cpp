  //////////////////////////////////////////////////////////////////////
// 
// 类名: bufwrap
// 描述: 二进制缓冲区包装类
// 作者: 黄进兵(huangjb@lianchuang.com)
// 时间: 2003-3-5 
// 版本: V1.0.0 
// 版权: 南京联创科技股份有限公司
//////////////////////////////////////////////////////////////////////

#include "bufwrap.hpp"
#include <stdlib.h>
#include <string.h>

namespace archy {

bufwrap::bufwrap()
:m_iLen(0)
,m_iSize(STEP_SIZE)
{
	m_vBuf = (char*)malloc(m_iSize);
	memset(m_vBuf, 0, m_iSize);
}

bufwrap::bufwrap(const void* vBuf, unsigned int iLen)
:m_iLen(iLen)
{
	m_iSize = m_iLen + STEP_SIZE;
	m_vBuf = (char*)malloc(m_iSize);
	memset(m_vBuf, 0, m_iSize);
	memcpy(m_vBuf, vBuf, m_iLen);
}

bufwrap::~bufwrap()
{
	free(m_vBuf);
}

bool bufwrap::empty()const
{
	return m_iLen == 0;
}

void bufwrap::erase(unsigned int iStart, unsigned int iLen)
{
	if( iStart >= m_iLen )
		return;
	if( iStart + iLen > m_iLen )
		iLen = m_iLen - iStart;
	m_iLen -= iLen;
	memmove(m_vBuf, m_vBuf + iStart, m_iLen);
	m_vBuf[m_iLen] = 0;
}


/*
int find(const void* search, unsigned int len)
{
	mem
}
*/

int bufwrap::null_deal(char chReplaceChar)
{
	int bHas;
	bHas = 0;

	for( unsigned int i = 0; i < m_iLen; ++i) {
		if( m_vBuf[i] == 0 ) {
			m_vBuf[i] = chReplaceChar;
			++bHas;
		}
	}
	return bHas;	
}

void bufwrap::resize(unsigned int iLen)
{
	if( iLen < m_iLen ) {
		memset(m_vBuf + iLen, 0, m_iLen - iLen);
	}
	else if( iLen > m_iSize ) {
		m_iSize = iLen + STEP_SIZE;
		char* vBuf = (char*)malloc(m_iSize);
		memset(vBuf, 0, m_iSize);
		memcpy(vBuf, m_vBuf, m_iLen);
		free(m_vBuf);
		m_vBuf = vBuf;
	}
	m_iLen = iLen;
	
}

// 从iStart位置开始，缓冲区后面的内容替换为vBuf
void bufwrap::replace(unsigned int iStart, const void* vBuf, unsigned int iLen)
{
	if( iStart >= m_iLen ) {
		append(vBuf, iLen);
	}
	else {
		char* pvBuf = (char*)malloc(iStart + iLen + 1);
		memcpy(pvBuf, m_vBuf, iStart);
		memcpy(pvBuf + iStart, vBuf, iLen );
		free(m_vBuf);
		m_vBuf = pvBuf;
		m_iLen = iStart + iLen;
		m_vBuf[m_iLen] = 0;
	}
}


bufwrap::bufwrap(const bufwrap &r)
:m_iLen(r.m_iLen)
,m_iSize(r.m_iSize)
{
	m_vBuf = (char*)malloc(m_iSize);
	memcpy(m_vBuf, r.m_vBuf, m_iSize);
}

void bufwrap::setbuf(const void* vBuf, unsigned int iLen)
{
	bufwrap::clear();
	bufwrap::append(vBuf, iLen);
}

bufwrap& bufwrap::operator =(const bufwrap &r)
{
	if( &r != this ) {
		setbuf(r.m_vBuf, r.m_iLen);
	}
	
	return *this;
}

void bufwrap::assign(const char* cBuf)
{
	if( cBuf ) {
		setbuf( cBuf, strlen(cBuf));
	}
}

void bufwrap::assign(const void* vBuf, unsigned int iLen)
{
	setbuf(vBuf, iLen);
}

char& bufwrap::operator[](int iIndex)
{
	if((unsigned)iIndex >= m_iLen )
		bufwrap::resize(iIndex + 1);
	return m_vBuf[iIndex];
}

char bufwrap::operator[](int iIndex)const
{
	if( (unsigned)iIndex >= m_iLen )
		return 0;
	return m_vBuf[iIndex];
}

void bufwrap::append(const void* vBuf, unsigned int iLen)
{
	unsigned int iTotalLen = m_iLen + iLen;
	if( iTotalLen < m_iSize ) {
		memcpy( m_vBuf + m_iLen, vBuf, iLen);
	}
	else {
		m_iSize = iTotalLen + STEP_SIZE;
		char* pvBuf = (char*)malloc(m_iSize);
		memcpy(pvBuf, m_vBuf, m_iLen);
		memcpy(pvBuf + m_iLen, vBuf, iLen);
		memset(pvBuf + iTotalLen, 0, m_iSize - iTotalLen);
		free(m_vBuf);
		m_vBuf = pvBuf;
	}
	m_iLen = iTotalLen;
}

bufwrap bufwrap::subbuf(unsigned int iStart)const
{
	if( iStart >= m_iLen )
		return bufwrap();
	int iLen = m_iLen - iStart;
	return bufwrap( m_vBuf + iStart, iLen);
}


bufwrap bufwrap::subbuf(unsigned int iStart, unsigned int iLen)const
{
	if( iStart >= m_iLen )
		return bufwrap();
	if( iStart + iLen >= m_iLen )
		iLen = m_iLen - iStart;
	
	return bufwrap( (char*)m_vBuf + iStart, iLen);
}

// 清空缓冲
void bufwrap::clear()
{
	m_iLen = 0;
	m_iSize = STEP_SIZE;
	free(m_vBuf);
	m_vBuf = (char*)malloc(m_iSize);
	memset(m_vBuf, 0, m_iSize);
}



bufwrap& bufwrap::operator +=(const bufwrap &r)
{
	append(r.m_vBuf, r.m_iLen);
	return *this;
}

bufwrap operator + (const bufwrap& r1, const bufwrap& r2)
{
	bufwrap buf(r1);
	buf += r2;
	return buf;
}

bool operator == (const bufwrap& r1, const bufwrap& r2)
{
	if( r1.size() != r2.size() ) 
		return false;
	return memcmp(r1.getbuf(), r2.getbuf(), r1.size()) == 0;
}

bool operator != (const bufwrap& r1, const bufwrap& r2)
{
	return !(r1 == r2);
}

} // namespace archy


