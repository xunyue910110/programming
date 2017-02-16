#include "PString.h"
#include <assert.h>

PString::PString()
{
  m_capLen = PSTRING_LEN_CONST;
  m_value  = new char[m_capLen+1];
  //assert(m_value!=NULL);
  m_value[0]=0;
  m_length=0;
}
//m_length肯定不会超过m_capLen
PString::PString(const string &str)
{
  m_length = m_capLen = str.length();
  if(m_capLen == 0)m_capLen = PSTRING_LEN_CONST;
  m_value  = new char[m_capLen+1];
  //assert(m_value!=NULL);
  if(m_length > 0)memcpy(m_value,str.c_str(),m_length);
  m_value[m_length]=0;
}
//m_length肯定不会超过m_capLen
PString::PString(const PString &str)
{
  m_length=m_capLen=str.m_length;
  if(m_capLen == 0)m_capLen = PSTRING_LEN_CONST;
  m_value  = new char[m_capLen+1];
  //assert(m_value!=NULL);
  if(m_length > 0)memcpy(m_value,str.m_value,m_length);
  m_value[m_length]=0;
}
PString::PString(const char * pStr)
{
  m_length = m_capLen = strlen(pStr);
  if(m_capLen == 0)m_capLen = PSTRING_LEN_CONST;
  m_value  = new char[m_capLen+1]; 
  //assert(m_value!=NULL);
  if(m_length > 0)memcpy(m_value,pStr,m_length);
  m_value[m_length]=0;
}
PString::PString(const char &c)
{
  m_length = m_capLen = 1;
  m_value  = new char[m_capLen+1];
  //assert(m_value!=NULL);
  m_value[0]=c;
  m_value[1]=0;
}
PString::~PString()
{
  if(m_value!=NULL)
  {
    delete[] m_value;
    m_value  = NULL;
    m_length = m_capLen = 0;
  }
}

PString& PString::operator=(const PString &str)
{
  if( this == &str) return *this;
  m_length=str.m_length;
  if(m_capLen < m_length )
  {
    if(m_value!=NULL) delete[] m_value;
    m_capLen = m_length;
    m_value  = new char[m_capLen+1];
    //assert(m_value!=NULL);
  }  
  //strcpy(m_value,str.m_value);
  memcpy(m_value,str.m_value,m_length);
  m_value[m_length]=0;
  return *this;
}
PString& PString::operator=(const string &str)
{
  m_length=str.length();
  if(m_capLen < m_length )
  {
    if(m_value!=NULL) delete[] m_value;
    m_capLen = m_length;
    m_value  = new char[m_capLen+1];
    //assert(m_value!=NULL);
  }  
  //strcpy(m_value,str.m_value);
  memcpy(m_value,str.c_str(),m_length);
  m_value[m_length]=0;
  return *this;
}


PString& PString::operator=(const char * pStr)
{
  m_length=strlen(pStr);
  if(m_capLen < m_length)
  {
    if(m_value!=NULL) delete[] m_value;
    m_capLen = m_length;
    m_value  = new char[m_capLen+1];
  }
  //strcpy(m_value,pStr);
  memcpy(m_value,pStr,m_length);
  m_value[m_length]=0;
  return *this;
}

PString& PString::operator=(const char &c)
{
  if(m_capLen < 1)
  {
    if(m_value!=NULL) delete[] m_value;
    m_capLen = 1;
    m_value  = new char[m_capLen+1];
  }
  m_value[0]=c;m_value[1]=0;
  m_length = 1;
  return *this;
}

PString  PString::operator+(const char *pStr)
{
  PString tmpStr(*this);
  tmpStr+=pStr;
  return tmpStr;
}

PString  PString::operator+(const char &c)
{
  PString tmpStr(*this);
  tmpStr+=c;
  return tmpStr;
}
PString  PString::operator+(const PString &str)
{
  PString tmpStr(*this);
  tmpStr+=str;
  return tmpStr;
}
PString  PString::operator+(const string &str)
{
  PString tmpStr(*this);
  tmpStr+=str;
  return tmpStr;
}
const char& PString::operator[] (const int &lab) const
{
  assert(lab>=0 && lab<=m_length);
  return m_value[lab];
}
char& PString::operator[] (const int &lab)
{
  assert(lab>=0 && lab<=m_length);
  return m_value[lab];
}

PString::operator int()
{
  return atoi(m_value);
}

PString::operator int() const
{
  return atoi(m_value);
}

PString::operator char *()
{
  return m_value;
}

PString::operator char *() const
{
  return m_value;
}

bool operator==(const PString& left, const PString& right)
{
  return (strcmp(left.m_value,right.m_value)==0);
}

bool operator==(const PString& left, const char * pStr)
{
  return (strcmp(left.m_value,pStr)==0);
}

bool operator==(const char * pStr,const PString& right)
{
  return (strcmp(right.m_value,pStr)==0);
}

bool operator==(const PString& left, const char & c)
{
  return ((left.m_value[0]==c)&&(left.m_value[1]==0));
}

bool operator==(const char & c,const PString& right)
{
  return ((right.m_value[0]==c)&&(right.m_value[1]==0));
}

bool operator!=(const PString& left, const PString& right)
{
  return (strcmp(left.m_value,right.m_value)!=0);
}

bool operator!=(const PString& left, const char * pStr)
{
  return (strcmp(left.m_value,pStr)!=0);
}

bool operator!=(const PString& left, const char & c)
{
  return ((left.m_value[0]!=c)||(left.m_value[1]!=0));
}

bool operator<(const PString& left, const PString& right)
{
  return (strcmp(left.m_value,right.m_value)<0);
}

bool operator<(const PString& left, const char * pStr)
{
  return (strcmp(left.m_value,pStr)<0);
}

bool operator<(const char *   pStr, const PString& right)
{
  return (strcmp(pStr,right.m_value)<0);
}

bool operator<(const PString& left, const char & c)
{
  return (left.m_value[0]<c);
}

bool operator<(const char & c,const PString& right)
{
  //D,BC应该是D比BC大 ,D,DC
  //return ((c<right.m_value[0]) || (right.m_value[1]>0));
  return (c<right.m_value[0]||(c == right.m_value[0] && right.m_value[1]>0));
}

bool operator>(const PString& left, const PString& right)
{
  return (strcmp(left.m_value,right.m_value)>0);
}

bool operator>(const PString& left, const char * pStr)
{
  return (strcmp(left.m_value,pStr)>0);
}

bool operator>(const char *   pStr, const PString& right)
{
  return (strcmp(pStr,right.m_value)>0);
}

bool operator>(const PString& left, const char & c)
{
  //CC,D应该是CC比D小
  //return ((left.m_value[0]>c)||(left.m_value[1]>0));
  return (left.m_value[0]>c||(left.m_value[0] == c && left.m_value[1]>0));
}

bool operator>(const char & c,const PString& right)
{
  return (c>right.m_value[0]);
}

ostream& operator<<(ostream& os,const PString& right)
{
  os<<right.m_value;
  return os;
}

const char * PString::c_str() const
{
  return m_value;
}

int   PString::length() const
{
  //assert(m_value!=NULL);
  return m_length;
}
void  PString::clear()
{
  if(m_value!=NULL)
    m_value[0] = 0;
  m_length = 0;
}
bool  PString::empty() const
{
  //assert(m_value!=NULL);
  return (m_length==0);
}
int PString::capacity() const
{
  return m_capLen;
}

PString operator+(const string& str, const PString& right)
{
  PString tmpStr;
  int len=str.length();
  tmpStr.m_length=right.m_length+len;
  if(tmpStr.m_capLen < tmpStr.m_length)
  {
    if(tmpStr.m_value!=NULL) delete[] tmpStr.m_value;
    tmpStr.m_capLen = tmpStr.m_length;
    tmpStr.m_value  = new char[tmpStr.m_capLen+1];
    //assert(tmpStr.m_value!=NULL);
  }
  //sprintf(tmpStr.m_value,"%s%s\0",pStr,right.m_value);
  memcpy(tmpStr.m_value,str.c_str(),len);
  memcpy(tmpStr.m_value+len,right.m_value,right.m_length);
  tmpStr.m_value[tmpStr.m_length]=0;
  return tmpStr;
}
PString operator+(const PString& str, const PString& right)
{
  PString tmpStr;
  int len=str.m_length;
  tmpStr.m_length=right.m_length+len;
  if(tmpStr.m_capLen < tmpStr.m_length)
  {
    if(tmpStr.m_value!=NULL) delete[] tmpStr.m_value;
    tmpStr.m_capLen = tmpStr.m_length;
    tmpStr.m_value  = new char[tmpStr.m_capLen+1];
    //assert(tmpStr.m_value!=NULL);
  }
  //sprintf(tmpStr.m_value,"%s%s\0",pStr,right.m_value);
  memcpy(tmpStr.m_value,str.m_value,len);
  memcpy(tmpStr.m_value+len,right.m_value,right.m_length);
  tmpStr.m_value[tmpStr.m_length]=0;
  return tmpStr;
}
PString operator+(const char* pStr, const PString& right)
{
  PString tmpStr;
  int len=strlen(pStr);
  tmpStr.m_length=right.m_length+len;
  if(tmpStr.m_capLen < tmpStr.m_length)
  {
    if(tmpStr.m_value!=NULL) delete[] tmpStr.m_value;
    tmpStr.m_capLen = tmpStr.m_length;
    tmpStr.m_value  = new char[tmpStr.m_capLen+1];
    //assert(tmpStr.m_value!=NULL);
  }
  //sprintf(tmpStr.m_value,"%s%s\0",pStr,right.m_value);
  memcpy(tmpStr.m_value,pStr,len);
  memcpy(tmpStr.m_value+len,right.m_value,right.m_length);
  tmpStr.m_value[tmpStr.m_length]=0;
  return tmpStr;
}

PString operator+(const char& c, const PString& right)
{
  PString tmpStr;
  tmpStr.m_length=right.m_length+1;
  if(tmpStr.m_capLen < tmpStr.m_length)
  {
    if(tmpStr.m_value!=NULL) delete[] tmpStr.m_value;
    tmpStr.m_capLen = tmpStr.m_length;
    tmpStr.m_value  = new char[tmpStr.m_capLen+1];
  }
  tmpStr.m_value[0]=c;
  memcpy(tmpStr.m_value+1,right.m_value,right.m_length);
  tmpStr.m_value[tmpStr.m_length]=0;
  return tmpStr;
}

PString&  PString::operator+=(const char *pStr)
{/*
	if(m_capLen < length()+strlen(pStr))
	{
		int newCapLen = length()+strlen(pStr)+1;
		char * tmpSz=new char[newCapLen+1];
		assert(tmpSz!=NULL);
		sprintf(tmpSz,"%s%s",m_value,pStr);
		delete[] m_value;
		m_capLen = newCapLen;
		m_value  = tmpSz;
		tmpSz = NULL;
  	}else
  	{
		sprintf(m_value,"%s%s",m_value,pStr);
	}
  return *this;*/
  int oldLen=m_length;
  m_length=m_length+strlen(pStr);
  if(m_capLen < m_length)
  {
    char* oldValue=m_value;
    m_capLen = m_length;
    m_value  = new char[m_capLen+1];
    memcpy(m_value,oldValue,oldLen);
    delete [] oldValue;
  }
  memcpy(m_value+oldLen,pStr,m_length-oldLen);
  m_value[m_length]=0;
  return (*this);
}

PString&  PString::operator+=(const char &c)
{/*
  if(m_capLen < length()+1)
  {
  	int  newCapLen = m_capLen+1;
  	char * tmpSz = new char[newCapLen+1];
    assert(tmpSz!=NULL);
    sprintf(tmpSz,"%s%c",m_value,c);
    delete[] m_value;
    m_value = tmpSz;
    m_capLen= newCapLen;
    tmpSz = NULL;
  }else
  {
    sprintf(m_value,"%s%c",m_value,c);
  }
  return *this;*/
  int oldLen=m_length;
  m_length=m_length+1;
  if(m_capLen < m_length)
  {
    char* oldValue=m_value;
    m_capLen = m_length;
    m_value  = new char[m_capLen+1];
    memcpy(m_value,oldValue,oldLen);
    delete [] oldValue;
  }
  m_value[m_length-1]=c;
  m_value[m_length]=0;
  return (*this);
}
PString&  PString::operator+=(const PString &str)
{/*
  if(m_capLen < length()+str.length())
  {
  	int  newCapLen = length()+str.length()+1;
  	char * tmpSz = new char[newCapLen+1];
    assert(tmpSz!=NULL);
    sprintf(tmpSz,"%s%s",m_value,str.m_value);
    delete[] m_value;
    m_value = tmpSz;
    m_capLen= newCapLen;
    tmpSz = NULL;
  }else
  {
  	sprintf(m_value,"%s%s",m_value,str.m_value);
  }
  return *this;*/
  int oldLen=m_length;
  m_length=m_length+str.m_length;
  if(m_capLen < m_length)
  {
    char* oldValue=m_value;
    m_capLen = m_length;
    m_value  = new char[m_capLen+1];
    memcpy(m_value,oldValue,oldLen);
    delete [] oldValue;
  }
  memcpy(m_value+oldLen,str.m_value,str.m_length);
  m_value[m_length]=0;
  return (*this);
}
PString&  PString::operator+=(const string &str)
{
  int oldLen=m_length;
  m_length=m_length+str.length();
  if(m_capLen < m_length)
  {
    char* oldValue=m_value;
    m_capLen = m_length;
    m_value  = new char[m_capLen+1];
    memcpy(m_value,oldValue,oldLen);
    delete [] oldValue;
  }
  memcpy(m_value+oldLen,str.c_str(),str.length());
  m_value[m_length]=0;
  return (*this);
}
//注len=0时，认为是取""
const PString PString::substr(const int &sPos,int len) const
{
  int l=len;
  if(m_value==NULL) return PString("");
  if(sPos<0 || sPos>m_length || l==0) return "";
  if(l < 0)l=m_length-sPos;
  PString tmpStr(*this);
  //if(((sPos+len-1)<= m_capLen))
    //tmpStr.m_value[sPos+len] = 0;
  memcpy(tmpStr.m_value,m_value+sPos,l);
  tmpStr.m_value[l]=0;
  tmpStr.m_length=l;
  return tmpStr;
}
//去除字符串尾部的空格
void  PString::rtrim(char* pStr,int& len)
{
  if(pStr==NULL) return ;
  int i=len-1;
  for(;i>=0&&isspace(pStr[i]);i--)
    ;
  pStr[i+1]=0;
  len=i+1;
  return ;
}
void  PString::ltrim(char* pStr,int& len)
{
  if(pStr==NULL) return ;
  int i=0;
  for(;i<len&&isspace(pStr[i]);i++)
    ;
  len=len-i;
  memcpy(pStr,pStr+i,len);
  pStr[len]=0;
  return ;
}
void  PString::rltrim(char* pStr,int& len)
{
  rtrim(pStr,len);
  ltrim(pStr,len);
}
//去除字符串尾部的空格
PString&  PString::rtrim()
{
  rtrim(m_value,m_length);
  return *this;
}
PString&  PString::ltrim()
{
  ltrim(m_value,m_length);
  return *this;
}
//去除字符串首尾部的空格
PString&  PString::trim()
{
  if(m_value==NULL) return *this;
  rltrim(m_value,m_length);
  return *this;
}
//按照c对m_value进行分割，并将结构存放在result中
int PString::split(const char &c,vector<PString> &result,bool isTrim)
{
  /*result.clear();
  PString tmpStr(*this);
  int j=0;
  for(int i=0;i<length();i++)
  {
    if(m_value[i] == c)
    {
      tmpStr.m_value[j] = 0;
      if(isTrim)tmpStr.trim();
      result.push_back(tmpStr);
      tmpStr = (char*)(m_value+i+1);
      j=0;
    }else
      j++;
  }
  tmpStr.m_value[j] = 0;
  if(isTrim)tmpStr.trim();
  result.push_back(tmpStr);
  return result.size();
  */
  result.clear();
  if(m_length == 0)return 0;
  int i,j,len;
  char *buff=new char[m_length+1];
  for(i=0,j=0;j<m_length;j++)
  {
    if(m_value[j] == c)
    {
        if(j == i)
        {
            result.push_back("");
            i=j+1;
            continue;
        }
        len=j-i;
        memcpy(buff,m_value+i,len);
        buff[len]=0;
        if(isTrim)rltrim(buff,len);
        result.push_back(buff);
        i=j+1;
    }
  }
  if(j == i)
  {
    result.push_back("");
  }
  else
  {
    len=j-i;
    memcpy(buff,m_value+i,len);
    buff[len]=0;
    if(isTrim)rltrim(buff,len);
    result.push_back(buff);
  }
  delete [] buff;
  return result.size();
}
void PString::setStrNValue(const char * str,const int n)
{
  if(m_capLen < n)
  {
    delete[] m_value;
    m_capLen = n;
    m_value  = new char[m_capLen+1];
  }
  memcpy(m_value,str,n);
  m_value[n]=0;
  m_length=n;
  m_value[m_length]=0;
}

int PString::splitTrim(const char &c,vector<PString> &result) const
{
  result.clear();
  PString tmpStr(*this);
  int j=0;
  for(int i=0;i<length();i++)
  {
    if(m_value[i] == c)
    {
      tmpStr.m_value[j] = 0;
      tmpStr.trim();
      result.push_back(tmpStr);
      tmpStr = (char*)(m_value+i+1);
      j=0;
    }else
      j++;
  }
  tmpStr.m_value[j] = 0;
  tmpStr.trim();
  result.push_back(tmpStr);
  return result.size();
}

