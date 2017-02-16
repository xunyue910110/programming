
#ifndef __TYPES_H__
#define __TYPES_H__

#include "config-all.h"

#include "PatternMatcher.h"

#include <string>
#include <iostream>
#include <stdio.h>
#include <assert.h>

USING_NAMESPACE(std)

class AttributeValue;


template<int size>
class String;


class Integer  {
  public:
  	//modified by sunam 2005-6-2
    //Integer(int I = 0) : i(I) {}
    Integer(long I = 0) : i(I) {}

	//modified by sunam 2005-6-2
    //operator int(){return i;}
    //operator int() const {return i;}
    operator long(){return i;}
    operator long() const {return i;}

    const Integer& operator=(const string &a);
    const Integer& operator=(const Integer &a);
    const Integer& operator=(const int &a);
    //added by sunam 2005-6-2
    const Integer& operator=(const long &a);
    const Integer& operator=(const AttributeValue &a);

	//modified by sunam 2005-6-2
	/*
    template<int size>
    const Integer& operator=(const String<size> &a) 
    {
        i = atoi(a.c_str());
        return *this;
    }

    template<int size>
    const Integer& operator=(String<size> &a) 
    {
        i = atoi(a.c_str());
        return *this;
    }
    */
    template<int size>
    const Integer& operator=(const String<size> &a) 
    {
        i = atol(a.c_str());
        return *this;
    }

    template<int size>
    const Integer& operator=(String<size> &a) 
    {
        i = atol(a.c_str());
        return *this;
    }
    
    const Integer& operator+=(const Integer &a) {
        i += a.i;
      return *this ;
    }
    const Integer & operator -=(const Integer &a) {
        i -= a.i;
      return *this ;
    }
    
    friend const Integer & operator ++(Integer &a);
    friend const Integer   operator ++(Integer &a,int);
    friend const Integer & operator --(Integer &a);
    friend const Integer   operator --(Integer &a,int);
    
    friend ostream& operator<<(ostream& os,const Integer& a);
    friend istream& operator>>(istream& is,Integer& a);
    
    //added by sunam 2005-6-2
    /*
    friend const int operator /(const Integer& a, const int& b) {
      return a.i / b;
    }
    
    friend const int operator %(const Integer& a, const int& b) {
      return a.i % b;
    }
    */
    friend const Integer operator /(const Integer& a, const int& b) {
      return a.i / b;
    }
    
    friend const Integer operator %(const Integer& a, const int& b) {
      return a.i % b;
    }
    
    friend const Integer operator /(const Integer& a, const long& b) {
      return a.i / b;
    }
    
    friend const Integer operator %(const Integer& a, const long& b) {
      return a.i % b;
    }

  private:
  	//modified by sunam 2005-6-2
    //int i;
    long i;
};


inline const Integer & operator ++(Integer &a)
{
    a.i++;
    return a ;
}

inline const Integer operator ++(Integer &a,int)
{
    Integer r(a.i);
    a.i++;
    return r ;
}

inline const Integer & operator --(Integer &a)
{
    a.i--;
    return a ;
}

inline const Integer operator --(Integer &a,int)
{
    Integer r(a.i);
    a.i--;
    return r ;
}


typedef Integer Float;

class Number
{
  public:
    Number(int H = 0, int L = 0) : h(H), l(L) {}

    template<int size>
    Number(const String<size> &a) {
        *this = string(a.c_str());
    }
  
    const Number& operator=(const string &a);
    const Number& operator=(const int &a);
    const Number& operator=(const Number &a);
  
    operator char *();
    operator char *() const;
    friend bool operator==(const Number &a, const Number &b)
    {return a.h==b.h&&a.l==b.l;}
    friend bool operator!=(const Number &a, const Number &b)
    {return (!(a==b));}
    friend bool operator<(const Number& a, const Number& b)
    {return a.h==b.h?a.l<b.l:a.h<b.h;}
  
    friend ostream& operator<<(ostream& os,const Number& a);
  
  private:
    int h;
    int l;
};



class AttributeValue
{
 public:
  char m_value[20];

  AttributeValue(){m_value[0] = 0;}
  AttributeValue(const AttributeValue &a)
  {
    *this=a;
  }
  operator int() { return (atoi(m_value)); }
  operator int() const {return (atoi(m_value));}

  const AttributeValue & operator =(const AttributeValue &a)
  {
    strncpy(m_value,a.m_value,19);
    m_value[19]=0;
    return *this;
  }

  const AttributeValue & operator =(const float &value)
  {
    memset(m_value,0,sizeof(m_value));
    sprintf(m_value,"%d",(int)value) ;
    return *this;
  }

  const AttributeValue & operator =(const long &value)
  {
    memset(m_value,0,sizeof(m_value));
    sprintf(m_value,"%ld",value) ;
    return *this;
  }

  const AttributeValue & operator =(const int &value)
  {
    memset(m_value,0,sizeof(m_value));
    sprintf(m_value,"%d",value) ;
    return *this;

  }
  const AttributeValue & operator =(const string &value)
  {
    strncpy(m_value,value.c_str(),19);
    m_value[19]=0;
    return *this;
  }
  
  const AttributeValue & operator = (const char &value)
   {
    memset(m_value,0,sizeof(m_value));
    m_value[0] = value;
    return *this;
   };

  friend bool operator==(const AttributeValue& a, const AttributeValue& b)
  {return (strcmp(a.m_value,b.m_value)==0);}
  friend bool operator<(const AttributeValue& a, const AttributeValue& b)
  {return (strcmp(a.m_value,b.m_value)<0);}

//   friend bool operator==(const AttributeValue &b ,const Float &a )
//  {return a==b;}
//
//  friend bool operator<(const AttributeValue& b,const Float& a )
//  {return a>b;}

  friend bool operator==(const AttributeValue &b ,const int &a )
  {return a==b;}

  friend bool operator<(const AttributeValue& b,const int & a )
  {return a>b;}

  friend bool operator!=(const AttributeValue& b,const int & a )
  {return a!=b;}

   friend bool operator==(const AttributeValue &b ,const char a )
  {return b.m_value[0] == a;}

  friend bool operator<(const AttributeValue& b,const char a )
  {return b.m_value[0] < a ;}

  friend bool operator!=(const AttributeValue& b,const char a )
  {return b.m_value[0] != a ;}

   friend bool operator==(const AttributeValue &b ,const string &a )
  {return (strcmp(b.m_value,a.c_str())== 0);}

  friend bool operator < (const AttributeValue& b,const string & a )
  {return (strcmp(b.m_value,a.c_str()) < 0);}

   friend bool operator != (const AttributeValue& b,const string & a )
  {return (!strcmp(b.m_value,a.c_str()) == 0);}

  friend bool operator==(const AttributeValue &b ,const char *a )
  {return (strcmp(b.m_value,a)== 0);}

  friend bool operator!=(const AttributeValue &b ,const char *a )
  {return (strcmp(b.m_value,a)!= 0);}

  friend ostream& operator<<(ostream& os,const AttributeValue& a);

};




template<int size=20>
class String
{
  public:
  String(){m_s[0]=0;}
  String(const String<size> &a){*this=a;}
  String(const string &a){*this=a;}
  String(const char *a){*this=a;}
  
  const String<size>& operator=(const String<size> &a);
  const String<size>& operator=(const string &a);
  const String<size>& operator=(const char *a);
  const String<size>& operator=(const char a);
  
  string operator+(char *a);
  string operator+(char a);
  string operator+(String<size> &a);
  string operator+(string &a);

  //后置+运算符
  friend string operator+(char *a,String<size> &b)
  {
    string c;
    c=a;
    c=c + b.m_s;
    return c;
  }
  friend string operator+(string &a,String<size> &b)
  {
    string c;
    c=a;
    c=c + b.m_s;
    return c;
  }

  const char &operator[] (int s ) const
  {
    assert(s>=0 && s < length());
    return m_s[s];
  }

  char &operator[] (int s ) 
  {
    assert(s>=0 && s < length());
    return m_s[s];
  }

  friend bool operator==(const String<size>& a, const String<size>& b)
  {return strcmp(a.m_s,b.m_s)==0;}
  
  friend bool operator!=(const String<size>& a, const String<size>& b)
  {return !(strcmp(a.m_s,b.m_s)==0);}
  
  friend bool operator==(const String<size>& a,const string& b)
  {
    return strncmp(a.m_s,b.substr(0,size).c_str(),size)==0;
  }

  friend bool operator!=(const String<size>& a,const string& b)
  {
    return !(strncmp(a.m_s,b.substr(0,size).c_str(),size)==0);
  }

  friend bool operator==(const string& b, const String<size>& a)
  {
    return strncmp(a.m_s,b.substr(0,size).c_str(),size)==0;
  }
  
  friend bool operator!=(const string& b, const String<size>& a)
  {
    return !(strncmp(a.m_s,b.substr(0,size).c_str(),size)==0);
  }
  
  friend bool operator==(const String<size> &a,const char *b)
  {
    return strncmp(a.m_s,b,size)==0;
  }

  friend bool operator!=(const String<size> &a,const char *b)
  {
    return !(strncmp(a.m_s,b,size)==0);
  }

  /*
	modified by sunam 2005-7-1
  */
  /*
  friend bool operator==(const String<size> &a,const char b)
  {
    return (a.m_s[0]==b);
  }
  */


  friend bool operator<(const String<size>& a, const String<size>& b)
  {return strcmp(a.m_s,b.m_s)<0;}

  friend bool operator<(const String<size>& a, const string& b)
  {return strcmp(a.m_s,b.c_str())<0;}
  
  friend bool operator>(const String<size>& a, const string& b)
  {return strcmp(a.m_s,b.c_str())>0;}
  
  //add by gaojf 2008-9-4 13:49
  friend bool operator>(const String<size>& a, const String<size>& b)
  {return strcmp(a.m_s,b.m_s)>0;}

  friend bool operator>=(const String<size>& a, const string& b)
  {return strcmp(a.m_s,b.c_str())>=0;}

  friend bool operator<=(const String<size>& a, const string& b)
  {return strcmp(a.m_s,b.c_str())<=0;}
  
  /*
	modified by sunam 2005-7-1
  */
  /*
  friend bool operator>(const String<size>& a,const char b )
  {return a.m_s[0] > b ;}

  friend bool operator<=(const String<size>& a,const char b )
  {return a.m_s[0] <= b ;}
  */

  friend bool operator<(const int &a ,const String<size>&b)
  {return  a < atoi(b.m_s);}

  friend bool operator==(const String<size>&a ,const int &b )
  {return atoi(a.m_s)==b;}

  friend bool operator!=(const String<size>&a ,const int &b )
  {return !(atoi(a.m_s)==b);}

  //added by sunam 2005-7-1
  friend bool operator<(const String<size>&a ,const long &b )
  {return atol(a.m_s)<b;}
  
  friend bool operator<=(const String<size>&a ,const long &b )
  {return atol(a.m_s)<=b;}
  
  friend bool operator>(const String<size>&a ,const long &b )
  {return atol(a.m_s)>b;}
  
  friend bool operator>=(const String<size>&a ,const long &b )
  {return atol(a.m_s)==b;}
  
  friend bool operator==(const String<size>&a ,const long &b )
  {return atol(a.m_s)==b;}

  friend bool operator!=(const String<size>&a ,const long &b )
  {return atol(a.m_s)!=b;}
  //added by sunam 2005-7-1

	/*
		sunam 2005-7-1 注释
  		friend bool operator<(const String<size>& a,const char b )
  		{return a.m_s[0] < b ;}
  		
  		friend bool operator!=(const String<size>& a,const char b )
  		{return a.m_s[0] != b ;}
  	*/

  operator string() const {return this->m_s;}

  //modified by sunam 2005-6-2
  /*
  operator Integer() { return (atoi(m_s)); }
  operator Integer() const {return (atoi(m_s));}
  */
  operator Integer() { return (atol(m_s)); }
  operator Integer() const {return (atol(m_s));}

  const String<size> & operator =(const float &value)  {
    memset(m_s,0,sizeof(m_s));
    sprintf(m_s,"%d",(int)value) ;
    return *this;
  }
  
  const String<size> & operator =(const int &value)  {
    memset(m_s,0,sizeof(m_s));
    sprintf(m_s,"%d",(int)value) ;
    return *this;
  }
  
  //added by sunam 2005-6-2
  const String<size> & operator =(const long &value)  {
    memset(m_s,0,sizeof(m_s));
    sprintf(m_s,"%ld",(long)value) ;
    return *this;
  }
  
	template <int size1>
	String<size>& operator = (const String<size1> &a)
	{
		*this = a.c_str();
		return *this;
	}
	
	template <int size1>
	bool operator == (const String<size1> &a)
	{
		return (strcmp(m_s,a.c_str()) == 0);
	}
	
	template <int size1>
	bool operator != (const String<size1> &a)
	{
		return !(strcmp(m_s,a.c_str()) == 0);
	}
  
  friend ostream& operator<<(ostream& os,const String<size>& a)
  {
    os<<a.m_s;
    return os;
  }

  //若相等返回0，否则返回1
  int compare(int start,int length,const string& str);
  int compare(int start,int length,const char *str);
               
  //在字符串的第start位后加上str
  String<size>& append(int start,const string& str);
  //在字符串的第start位后加上str
  String<size>& append(int start,const char* str);
  //在字符串的末尾添加count个element,标准string用法
  String<size>& append(int count,const char element);
  //in 匹配  
  bool in( const char * p01=0, const char * p02=0, const char * p03=0, const char * p04=0, const char * p05=0, const char * p06=0, const char * p07=0, const char * p08=0, const char * p09=0, const char * p10=0, const char * p11=0, const char * p12=0, const char * p13=0, const char * p14=0, const char * p15=0, const char * p16=0, const char * p17=0, const char * p18=0, const char * p19=0, const char * p20=0);
  bool in(const string & p01="", const string & p02="", const string & p03="", const string & p04="", const string & p05="", const string & p06="", const string & p07="", const string & p08="", const string & p09="", const string & p10="", const string & p11="", const string & p12="", const string & p13="", const string & p14="", const string & p15="", const string & p16="", const string & p17="", const string & p18="", const string & p19="", const string & p20="");
  //like 匹配
  bool like( const char * pattern );

  const char * c_str() const {return this->m_s;}
  
  //modified by sunam 2005-6-2
  /*
  const int value() const {return atoi(this->m_s);}
  */
  const long value() const {return atol(this->m_s);}
  
  const String<size> substr(int s,int l) const;
  String<size> substr(int s,int l);
  
  const String<size> substrN(int s,int l) const;
  String<size> substrN(int s,int l);
  
  void clear(){m_s[0]=0;}
  bool empty(){return m_s[0]==0;}
  int length() const {return strlen(m_s);}
  int maxSize() const {return size;}
  //add by gaojf 为了提高性能
  void setStrNValue(const char * str,const int n)
  {
    assert(n>=0 && n <= size);
    memcpy(m_s,str,n);
    m_s[n]=0;
  }

 private:
  char m_s[size+1];
};

template<int size>
const String<size>& String<size>::operator=(const String<size> &a)
{
	strncpy(m_s,a.m_s,size);
	m_s[size]=0;
	return *this;
}

template<int size>
const String<size>& String<size>::operator=(const string &a)
{
	strncpy(m_s,a.c_str(),size);
	m_s[size]=0;
	return *this;
}

template<int size>
const String<size>& String<size>::operator=(const char *a)
{
    if (a != 0) {
	    strncpy(m_s,a,size);
	    m_s[size]=0;        
    }
	return *this;            
}

//2005-03-27 11:57
template<int size>
const String<size>& String<size>::operator=(const char a)
{
	memset(m_s,0,sizeof(m_s));
	m_s[0]=a;
	return *this;
}

template<int size>
string String<size>::operator+(char *a)
{
	string b;
	b=m_s;
	b=b + a;
	return b;
}

template<int size>
string String<size>::operator+( char a)
{
	string b;
	char tmp[2];
	sprintf(tmp,"%c\0",a);
	b=m_s;
	b=b + tmp;
	return b;
}

template<int size>
string String<size>::operator+(String<size> &a)
{
	string b;
	b=m_s;
	b=b + a.m_s;
	return b;
	
}

template<int size>
string String<size>::operator+(string &a)
{
	string b;
	b=m_s;
	b=b + a;
	return b;
	
}

template<int size>
const String<size> String<size>::substr(int s,int l) const
{
	String<size> b;
	if(s<length()&&l>0)
	{
		if(s<0)s=0;
		strncat(b.m_s,m_s+s,s+l>length()?length()-s:l);
	}
	return b;
}

template<int size>
String<size> String<size>::substr(int s,int l)
{
	String<size> b;
	if(s<length()&&l>0)
	{
		if(s<0)s=0;
		strncat(b.m_s,m_s+s,s+l>length()?length()-s:l);
	}
	return b;
}

//zhaogc add 2008-10-17 begin
template<int size>
const String<size> String<size>::substrN(int s,int l) const
{
	String<size> b;
	if(s<0) s+=length();
	if(s<length()&&l>0)
	{
		if(s<0)s=0;
		strncat(b.m_s,m_s+s,s+l>length()?length()-s:l);
	}
	return b;
}

template<int size>
String<size> String<size>::substrN(int s,int l)
{
	String<size> b;
	if(s<0) s+=length();//zhaogc add 2008-10-17
	if(s<length()&&l>0)
	{
		if(s<0)s=0;
		strncat(b.m_s,m_s+s,s+l>length()?length()-s:l);
	}
	return b;
}
//zhaogc add 2008-10-17 end

template<int size>
int  String<size>::compare(int start,int length,const string& str)
{
   String<size> b;
   b=this->substr(start,length);
   if (strcmp(b.c_str(),str.c_str())==0)
      return 0;
   else
      return 1;
}

template<int size>
int  String<size>::compare(int start,int length,const char *str)
{
  String<size> b;
   b=this->substr(start,length);
   if (strcmp(b.c_str(),str)==0)
      return 0;
   else
      return 1;
}

template<int size>
String<size>& String<size>::append(int start,const string& str)
 {
 	 String<size> b,a;
 	 char tmp[size];
 	 b = this->substr(start,strlen(m_s));
 	 if (start >0)	a = this->substr(0,start);
 	 if (((strlen(m_s)+strlen(str.c_str()))<=size) && (start >=0)) 
 	 {
 	 	if (start == 0)
 	 	{
 	 		strcpy(m_s,str.c_str());
 	 		strcat(m_s,b.c_str());
 	 	}
 	 	else
 	 	{
 	 		memset(tmp,0,sizeof(tmp));
 	 		strcpy(tmp,a.c_str());
 	 		strcat(tmp,str.c_str());
 	 		strcat(tmp,b.c_str());
 	 		strcpy(m_s,tmp);
 	 	}
 	 }     
    m_s[size]='\0';
    return *this;
}

template<int size>
String<size>& String<size>::append(int start,const char* str)
 {
 	 String<size> b,a;
 	 char tmp[size];
 	 b = this->substr(start,strlen(m_s));
 	 if (start >0)	a = this->substr(0,start);
 	 if (((strlen(m_s)+strlen(str))<=size) && (start >=0)) 
 	 {
 	 	if (start == 0)
 	 	{
 	 		strcpy(m_s,str);
 	 		strcat(m_s,b.c_str());
 	 	}
 	 	else
 	 	{
 	 		memset(tmp,0,sizeof(tmp));
 	 		strcpy(tmp,a.c_str());
 	 		strcat(tmp,str);
 	 		strcat(tmp,b.c_str());
 	 		strcpy(m_s,tmp);
 	 	}
 	 }     
    m_s[size]='\0';
    return *this;
}

template<int size>
String<size>& String<size>::append(int count,const char element)
 {
    int strlength=strlen(m_s);
    for (int i=0;i<count;i++)
     { if (i+strlength>=size) break; 
       m_s[i+strlength]=element;
     }   
    m_s[size]='\0';
    return *this;
}

template<int size> 
bool String<size>::like( const char * pattern ) {
  	return PatternMatcher::matches(pattern, m_s);
}

template<int size> 
bool String<size>::in(const string & p01, const string & p02, const string & p03, const string & p04, const string & p05, const string & p06, const string & p07, const string & p08, const string & p09, const string & p10, const string & p11, const string & p12, const string & p13, const string & p14, const string & p15, const string & p16, const string & p17, const string & p18, const string & p19, const string & p20) 
{
    if (p01 != "") { if ( strcmp(m_s,p01.c_str())==0 )return true ; } else return false; 
    if (p02 != "") { if ( strcmp(m_s,p02.c_str())==0 )return true ; } else return false; 
    if (p03 != "") { if ( strcmp(m_s,p03.c_str())==0 )return true ; } else return false; 
    if (p04 != "") { if ( strcmp(m_s,p04.c_str())==0 )return true ; } else return false; 
    if (p05 != "") { if ( strcmp(m_s,p05.c_str())==0 )return true ; } else return false; 
    if (p06 != "") { if ( strcmp(m_s,p06.c_str())==0 )return true ; } else return false; 
    if (p07 != "") { if ( strcmp(m_s,p07.c_str())==0 )return true ; } else return false; 
    if (p08 != "") { if ( strcmp(m_s,p08.c_str())==0 )return true ; } else return false; 
    if (p09 != "") { if ( strcmp(m_s,p09.c_str())==0 )return true ; } else return false; 
    if (p10 != "") { if ( strcmp(m_s,p10.c_str())==0 )return true ; } else return false; 
    if (p11 != "") { if ( strcmp(m_s,p11.c_str())==0 )return true ; } else return false; 
    if (p12 != "") { if ( strcmp(m_s,p12.c_str())==0 )return true ; } else return false; 
    if (p13 != "") { if ( strcmp(m_s,p13.c_str())==0 )return true ; } else return false; 
    if (p14 != "") { if ( strcmp(m_s,p14.c_str())==0 )return true ; } else return false; 
    if (p15 != "") { if ( strcmp(m_s,p15.c_str())==0 )return true ; } else return false; 
    if (p16 != "") { if ( strcmp(m_s,p16.c_str())==0 )return true ; } else return false; 
    if (p17 != "") { if ( strcmp(m_s,p17.c_str())==0 )return true ; } else return false; 
    if (p18 != "") { if ( strcmp(m_s,p18.c_str())==0 )return true ; } else return false; 
    if (p19 != "") { if ( strcmp(m_s,p19.c_str())==0 )return true ; } else return false; 
    if (p20 != "") { if ( strcmp(m_s,p20.c_str())==0 )return true ; } else return false; 

    return false;
} 

template<int size>
bool String<size>::in( const char * p01, const char * p02, const char * p03, const char * p04, const char * p05, const char * p06, const char * p07, const char * p08, const char * p09, const char * p10, const char * p11, const char * p12, const char * p13, const char * p14, const char * p15, const char * p16, const char * p17, const char * p18, const char * p19, const char * p20) 
{
    if (p01 != 0) { if ( strcmp(m_s,p01)==0 )return true ; } else return false; 
    if (p02 != 0) { if ( strcmp(m_s,p02)==0 )return true ; } else return false; 
    if (p03 != 0) { if ( strcmp(m_s,p03)==0 )return true ; } else return false; 
    if (p04 != 0) { if ( strcmp(m_s,p04)==0 )return true ; } else return false; 
    if (p05 != 0) { if ( strcmp(m_s,p05)==0 )return true ; } else return false; 
    if (p06 != 0) { if ( strcmp(m_s,p06)==0 )return true ; } else return false; 
    if (p07 != 0) { if ( strcmp(m_s,p07)==0 )return true ; } else return false; 
    if (p08 != 0) { if ( strcmp(m_s,p08)==0 )return true ; } else return false; 
    if (p09 != 0) { if ( strcmp(m_s,p09)==0 )return true ; } else return false; 
    if (p10 != 0) { if ( strcmp(m_s,p10)==0 )return true ; } else return false; 
    if (p11 != 0) { if ( strcmp(m_s,p11)==0 )return true ; } else return false; 
    if (p12 != 0) { if ( strcmp(m_s,p12)==0 )return true ; } else return false; 
    if (p13 != 0) { if ( strcmp(m_s,p13)==0 )return true ; } else return false; 
    if (p14 != 0) { if ( strcmp(m_s,p14)==0 )return true ; } else return false; 
    if (p15 != 0) { if ( strcmp(m_s,p15)==0 )return true ; } else return false; 
    if (p16 != 0) { if ( strcmp(m_s,p16)==0 )return true ; } else return false; 
    if (p17 != 0) { if ( strcmp(m_s,p17)==0 )return true ; } else return false; 
    if (p18 != 0) { if ( strcmp(m_s,p18)==0 )return true ; } else return false; 
    if (p19 != 0) { if ( strcmp(m_s,p19)==0 )return true ; } else return false; 
    if (p20 != 0) { if ( strcmp(m_s,p20)==0 )return true ; } else return false; 

    return false;
}



#endif // TYPES_H



