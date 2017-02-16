/***************************************************************************
 * PString  -- PString.h                                                   *
 * 功能描述：实现相当于string的功能，但由于string性能不是很理想            *
 *           所以，编写PString类，来实现相当于string的功能。               *
 *           并支持和char * 直接的接口                                     *
 *           另支持：长度固定的方式                                        *
 * 用法    ：类同string                                                    *
 * 作者    ：高将飞  QQ:52490020                                           *
 ***************************************************************************/
#ifndef PSTRING_H_INCLUDE
#define PSTRING_H_INCLUDE
#include "config-all.h"
#include <string>
#include <vector>
#include <iostream>
USING_NAMESPACE(std);

class PString
{
  public:
    enum 
    { //缺省内部默认分配空间大小,改成8的倍数,分配效率会高些
      PSTRING_LEN_CONST = 32 
    };
  public:
    PString();
    PString(const string &str);
    PString(const PString &str);
    PString(const char * pStr);
    PString(const char &c);
    ~PString();
  public:
    PString& operator=(const PString &str);
    PString& operator=(const string &str);
    PString& operator=(const char * pStr);
    PString& operator=(const char &c);
    PString& operator+=(const char *pStr);
    PString& operator+=(const char &c);
    PString& operator+=(const PString &str);
    PString& operator+=(const string &str);
    PString  operator+(const char *pStr);
    PString  operator+(const char &c);
    PString  operator+(const PString &str);
    PString  operator+(const string &str);
    const char &operator[] (const int &lab) const;
    char &operator[] (const int &lab);
    operator int();
    operator int() const;
    operator char *();
    operator char *() const;
  public:
    friend PString operator+(const char* pStr, const PString& right);
    friend PString operator+(const char& c, const PString& right);
    friend PString operator+(const PString &str, const PString& right);
    friend PString operator+(const string &str, const PString& right);
    friend bool operator==(const PString& left, const PString& right);
    friend bool operator==(const PString& left, const char * pStr);
    friend bool operator==(const char * pStr,const PString& right);
    friend bool operator==(const PString& left, const char & c);
    friend bool operator==(const char & c,const PString& right);
    friend bool operator!=(const PString& left, const PString& right);
    friend bool operator!=(const PString& left, const char * pStr);
    friend bool operator!=(const PString& left, const char & c);
    friend bool operator<(const PString& left, const PString& right);
    friend bool operator<(const PString& left, const char * pStr);
    friend bool operator<(const char *   pStr, const PString& right);
    friend bool operator<(const PString& left, const char & c);
    friend bool operator<(const char & c,const PString& right);
    friend bool operator>(const PString& left, const PString& right);
    friend bool operator>(const PString& left, const char * pStr);
    friend bool operator>(const char *   pStr, const PString& right);
    friend bool operator>(const PString& left, const char & c);
    friend bool operator>(const char & c,const PString& right);
    friend ostream& operator<<(ostream& os,const PString& right);
  public:
    const char *c_str() const;
    const PString substr(const int &sPos,int len=-1) const;
    int   length() const;
    void  clear();
    bool  empty() const;
    int   capacity() const;    
    PString&  rtrim();  //去除字符串尾部的空格
    PString&  ltrim();  //去除字符串尾部的空格
    PString&  trim();   //去除字符串首尾部的空格
    //add by gaojf 为了提高性能
    void setStrNValue(const char * str,const int n);
    //按照c对m_value进行分割，并将结果存放在result中
    int   split(const char &c,vector<PString> &result,bool isTrim=false);
    //按照c对m_value进行分割，并去空格将结果存放在result中
    int   splitTrim(const char &c,vector<PString> &result) const;
  private:
    void  rtrim(char* pStr,int& len);
    void  ltrim(char* pStr,int& len);
    void  rltrim(char* pStr,int& len);
  private:
    char * m_value;     //存放字符串值
    int    m_capLen;    //可存放的最大容量,不包括结束符
    int    m_length;    //目前的大小
};

#endif //PSTRING_H_INCLUDE


