


#include "Types.h"



const Integer& Integer::operator=(const string &a)
{
   i=atoi(a.c_str());
   return *this;
}
const Integer& Integer::operator =(const Integer &a)
{
   i=a.i;
   return *this;
}
const Integer& Integer::operator=(const int &a)
{
   i=a;
  return *this;
}

//added by sunam 2005-6-2
const Integer& Integer::operator=(const long &a)
{
   i=a;
  	return *this;
}

const Integer& Integer::operator=(const AttributeValue &a)
{
   i = atoi(a.m_value);
   return *this;
}


ostream& operator<<(ostream& os,const Integer& a)
{
	os<<a.i;
	return os;
}

istream& operator>>(istream& is,Integer& a)
{
	is>>a.i;
	return is;
}


const Number& Number::operator=(const string &a)
{
	if(a.length()>9)
	{
		h=atoi(a.substr(0,a.length()-9).c_str());
		l=atoi(a.substr(a.length()-9,9).c_str());
	}
	else
	{
		h=0;
		l=atoi(a.c_str());
	}
	return *this;
}

const Number& Number::operator=(const int &a)
{
	h=0;
	l=a;
	return *this;
}

const Number& Number::operator=(const Number &a)
{
	h=a.h;
	l=a.l;
	return *this;
}

Number::operator char *()
{
	static char s[20];

	if(h>0)
		sprintf(s,"%d%09d",h,l);
	else
		sprintf(s,"%d",l);
	return s;
}

Number::operator char *() const
{
	static char s[20];

	if(h>0)
		sprintf(s,"%d%09d",h,l);
	else
		sprintf(s,"%d",l);
	return s;
}

ostream& operator<<(ostream& os,const Number& a)
{
	char n[18];

	if(a.h>0)
		sprintf(n,"%d%09d",a.h,a.l);
	else
		sprintf(n,"%d",a.l);
	os<<n;
	return os;
}


ostream& operator<<(ostream& os,const AttributeValue& a)
{
	os<<a.m_value;
	return os;
}




