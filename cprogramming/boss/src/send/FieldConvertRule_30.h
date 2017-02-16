// ##########################################
// Source file : FieldConvertRule_30.h
// System      : Mobile Billing System
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Xulr
// E_mail      : Xulr@lianchuang.com
// Create      : 20100401
// Update      : 20100401
// Copyright(C): 2010 by Xulr, Linkage.
// ##########################################

#ifndef CONVERTRULE_30_H_INCLUDED_C3809835
#define CONVERTRULE_30_H_INCLUDED_C3809835

#include "FieldConvertRule.h"
#include "FieldInfo.h"
#include "DataFormat.h"

//##ModelId=3C7F4CED02A6
#define LPAD(src,len,padchar) padchar(src,len,padchar,true)
#define RPAD(src,len,padchar) padchar(src,len,padchar,false)

static string padchar(const string src,int ilen,const char padchar,bool left)
{
	ilen = ilen - src.length();

	string tmp=string(ilen,padchar);

	return (left ? (tmp+src) : (src+tmp));
}

static int Hex2Dec(const char *src)
{
   if(*src>='A' && *src<='F')
     return (*src-'A'+10);
   if(*src>='a' && *src<='f')
     return (*src-'a'+10);
   if(*src>='0' && *src<='9')
     return (*src-'0');
   return 0;
}

static unsigned long int Char2Int(const char *src,int type)
{
  int i,ilen;
  unsigned long int des=0,factor=0;
  ilen = strlen(src) - 1;

  if(ilen<0) return 0;

  factor = 1;
  switch(type)
  {
    case 16:  //16进制转int
      des = Hex2Dec(src+ilen);

      for(i=(ilen-1);i>=0;i--)
      {
         factor *= 16;
         des += Hex2Dec(src+i)*factor;
      }
      break;
    case 10:  //10进制转int
      des = src[ilen]-'0';

      for(i=(ilen-1);i>=0;i--)
      {
         factor *= 10;
         des += (src[i]-'0')*factor;
      }
      break;
    case 8:  //8进制转int
      des = src[ilen]-'0';

      for(i=(ilen-1);i>=0;i--)
      {
         factor *= 8;
         des += (src[i]-'0')*factor;
      }
      break;
    case 2:  //2进制转int
      des = src[ilen]-'0';

      for(i=(ilen-1);i>=0;i--)
      {
         factor *= 2;
         des += (src[i]-'0')*factor;
      }
      break;
    default:
      des=0;
      break;
  }

  return des;
}

//srcType：原进制(取值2，8，10，16)   desType：目标进制  isUpper:转16进制输出0=小写，1=大写
static string Conv2(const char *src,int srcType,int desType,int isUpper)
{
  char tmp[100],tmp1[100];
  int i,imod;
  unsigned long int des;

  memset(tmp,0,sizeof(tmp));

  //先转成int
  des = Char2Int(src,srcType);
  //再转成需要的进制
  switch(desType)
  {
  	 case 16:
  	 	if(isUpper==1)
  	 		sprintf(tmp,"%X",des);
  	 	else
  	 		sprintf(tmp,"%x",des);
  	 	break;
  	 case 10:
  	 	sprintf(tmp,"%u",des);
  	 	break;
  	 case 8:
  	 	sprintf(tmp,"%o",des);
  	 	break;
  	 case 2:
  	 	  memset(tmp1,0,sizeof(tmp1));
        i=99;
        do{
            imod = des % 2;
            tmp1[i]='0'+imod;
            i--;
        }
        while((des /= 2) > 0);

        memcpy(tmp,tmp1+i+1,100-i-1);
        break;
  	 default:
  	 	tmp[0]='0';
  	 	break;
  }

  return tmp;
}

class FieldConvertRule_30 : public FieldConvertRule
{
  public:

	//##ModelId=3C7F4D3A0076
	virtual bool initialize(string ruleText);

	//##ModelId=3C7F578300D6
    virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

	DataFormat m_DataFormat;

  private:
  	string m_conditionFieldName;
  	string m_conditionFieldValue;
  	string m_conditionExp;
  	string m_callType;

  	vector<string>  m_srcfield;
  	vector<string>  m_desfield;
   vector<string>  m_func;
	vector<string>  m_arg1;
	vector<string>  m_arg2;
	vector<string>  m_arg3;
};

#endif /* CONVERTRULE_5_H_INCLUDED_C3809835 */
