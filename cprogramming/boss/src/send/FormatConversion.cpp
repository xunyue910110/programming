// Language:    C++
//
// OS Platform: UNIX
//
// Authors:     cheng haidong
//
// History:
// Copyright (C) Linkage. All Rights Reserved.
//
// Comments:
//
#include "FormatConversion.h"

int FormatConversion::rtrim(char *strCheck)
{
  int i,length;
  length=strlen(strCheck);
  for(i=length-1;i>=0;i--)
  if(strCheck[i]==' ')
    strCheck[i]='\0';
  else
    break;
  return 1;
}

int FormatConversion::ltrim(char *strCheck)
{
  int i,j,length;
  length=strlen(strCheck);
  for(i=0;i<length;i++)
  {
    if((strCheck[i]!=' ')&&(strCheck[i]!='\t'))
    {
      break;
    }
  }
  if(i>0)
  {
    for(j=0;j<length-i;j++) strCheck[j]=strCheck[j+i];
    strCheck[j]=0;
  }
  return 1;
}

int FormatConversion::alltrim(char *strCheck)
{
  int i,j,length;
  length=strlen(strCheck);
  for(i=length-1;i>=0;i--)
  if(strCheck[i]==' ')
    strCheck[i]='\0';
  else
    break;

  length=strlen(strCheck);
  for(i=0;i<length;i++)
  {
    if((strCheck[i]!=' ')&&(strCheck[i]!='\t'))
    {
      break;
    }
  }
  if(i>0)
  {
    for(j=0;j<length-i;j++) strCheck[j]=strCheck[j+i];
    strCheck[j]=0;
  }
  return 1;
}

//1,remove the left space
//2,add space on the right
int FormatConversion::changeFormatLs(char *strCheck,int strlength)
{
  int i,length;
  ltrim(strCheck);
  length=strlen(strCheck);
  if (length == strlength)
    return 1;

  if (length < strlength)
  {
    for(i=1;i <= strlength-length;i++)
      strCheck[i+length-1] = ' ';
    strCheck[i+length-1] = '\0';
  }
  else
  {
    strCheck[strlength] = '\0';
  }
  return 1;
}

//1,remove the left space
//2,add 0 on the right
int FormatConversion::changeFormatLz(char *strCheck,int strlength)
{
  int i,length;
  ltrim(strCheck);
  length=strlen(strCheck);
  if (length == strlength)
    return 1;

  if (length < strlength)
  {
    for(i=1;i <= strlength-length;i++)
      strCheck[i+length-1] = '0';
    strCheck[i+length-1] = '\0';
  }
  else
  {
    strCheck[strlength] = '\0';
  }
  return 1;
}

//1,remove the space
//2,add 0 on the left
int FormatConversion::changeFormatRz(char *strCheck,int strlength)
{
  int i,j,length;
  alltrim(strCheck);
  length=strlen(strCheck);
  if (length == strlength)
    return 1;

  if (length < strlength)
  {
    strCheck[strlength] = '\0';
    for(j=length-1,i=strlength-1;j>=0;i--,j--)
    {
      strCheck[i] = strCheck[j];
    }
    for(i=strlength-length-1;i>=0;i--)
    {strCheck[i] = '0';}
  }
  else
  {
    strCheck[strlength] = '\0';
  }
  return 1;
}

//1.remove the right space
//2.add space on the left
int FormatConversion::changeFormatRs(char *strCheck,int strlength)
{
  int i,j,length;
  alltrim(strCheck);
  length=strlen(strCheck);
  if (length == strlength)
    return 1;

  if (length < strlength)
  {
    strCheck[strlength] = '\0';
    for(j=length-1,i=strlength-1;j>=0;i--,j--)
    {
      strCheck[i] = strCheck[j];
    }
    for(i=strlength-length-1;i>=0;i--)
    {strCheck[i] = ' ';}
  }
  else
  {
    strCheck[strlength] = '\0';
  }
  return 1;
}

int FormatConversion::digitCheck(const char *strCheck)
{
  int i,length;
  length=strlen(strCheck);
  for(i=0;i<length;i++)
  if(!(isdigit(strCheck[i])||(strCheck[i]==' ')||(strCheck[i]=='.'))) 
    return 0;
  return 1;
}

int FormatConversion::xdigitCheck(const char *strCheck)
{
  int i,length;
  length=strlen(strCheck);
  for(i=0;i<length;i++)
  if(!(isxdigit(strCheck[i])))
    return 0;
  return 1;
}

int FormatConversion::toNumber(char *strCheck)
{
  int i,j,length;
  char ctemp[40];
  length=strlen(strCheck);
  for(i=0,j=0;i<length;i++)
  {
    if(isdigit(strCheck[i]))
    {
      ctemp[j] = strCheck[i];
      j++;
    }
  }
  ctemp[j] = 0;
  strcpy (strCheck,ctemp);
  
  return 1;
}

int FormatConversion::numToChar1 (char *s, double d, int l, int w)
{
  int n;
  char f[20];

  if (w != 0)
    l++;
  sprintf (f, "%%0%d.%dlf", l, w);
  sprintf (s, f, d);
  for (n = 0; n < strlen (s); n++)
  {
    if (s[n] == '.')
      break;
  }
  strcpy (&s[n], &s[n + 1]);
  s[l] = '\0';
  return 0;
}

int FormatConversion::copyFile(const char *fromFile, const char *toFile)
{
  FILE *fpTo,*fpFrom;
  char cTemp[2000];

  if((fpFrom=fopen(fromFile,"r"))==NULL)
    return 1224;

  if((fpTo=fopen(toFile,"a"))==NULL)
    return 1224;

  while(1)
  {
    fgets(cTemp,2000,fpFrom);
    if(feof(fpFrom))
        break;
    fprintf(fpTo,cTemp);
  }

  fclose(fpFrom);
  fclose(fpTo);
  return 1;
}

long FormatConversion::timeDiff(const char *dt)
{
  struct tm t;
  time_t t1,t2;
  char tmp[10];
  time(&t1);

  memcpy(tmp,dt,4);
  tmp[4]=0;
  t.tm_year=atoi(tmp)-1900;
  memcpy(tmp,dt+4,2);
  tmp[2]=0;
  t.tm_mon=atoi(tmp)-1;
  memcpy(tmp,dt+6,2);
  tmp[2]=0;
  t.tm_mday=atoi(tmp);
  memcpy(tmp,dt+8,2);
  tmp[2]=0;
  t.tm_hour=atoi(tmp);
  memcpy(tmp,dt+10,2);
  tmp[2]=0;
  t.tm_min=atoi(tmp);
  memcpy(tmp,dt+12,2);
  tmp[2]=0;
  t.tm_sec=atoi(tmp);
  t.tm_isdst = -1;
  t2=mktime(&t);

  return (long)(difftime(t1,t2)/(60));
}


int FormatConversion::addSecond(const char *dt,const long nSecond,char *dt1)
{
  struct tm t;
  time_t t2;
  char tmp[25];

  memcpy(tmp,dt,4);
  tmp[4]=0;
  t.tm_year=atoi(tmp)-1900;
  memcpy(tmp,dt+4,2);
  tmp[2]=0;
  t.tm_mon=atoi(tmp)-1;
  memcpy(tmp,dt+6,2);
  tmp[2]=0;
  t.tm_mday=atoi(tmp);
  memcpy(tmp,dt+8,2);
  tmp[2]=0;
  t.tm_hour=atoi(tmp);
  memcpy(tmp,dt+10,2);
  tmp[2]=0;
  t.tm_min=atoi(tmp);
  memcpy(tmp,dt+12,2);
  tmp[2]=0;
  t.tm_sec=atoi(tmp);
  t.tm_isdst = -1;
  t2=mktime(&t);
  t2 += nSecond;
  t = *(localtime(&t2));

  sprintf(dt1,"%04d%02d%02d%02d%02d%02d\0",
    t.tm_year+1900,t.tm_mon+1,t.tm_mday,
    t.tm_hour,t.tm_min,t.tm_sec);
  return 1;
}

int FormatConversion::dayadd(char *d,char *s,long n)/* add n from s to d formed as 'YYYYMMDD' */
{
  char tmp[10];
  time_t now,ret;
  struct tm when;

  time(&now);
  when=*localtime(&now);
  
  memcpy(tmp,s,4);
  tmp[4]=0;
  when.tm_year=atoi(tmp)-1900;
  memcpy(tmp,s+4,2);
  tmp[2]=0;
  when.tm_mon=atoi(tmp)-1;
  memcpy(tmp,s+6,2);
  tmp[2]=0;
  when.tm_mday=atoi(tmp)+n;
    when.tm_isdst = -1;  // please attention 
  if((ret=mktime(&when))!=-1)
  {
    sprintf(d,"%04d%02d%02d",when.tm_year+1900,when.tm_mon+1,when.tm_mday);
  }
  return ret;
}

//转换完之后再传回p_buff
void FormatConversion::intToHex(char *p_buff)
{
	char temp_buff[5],buff[12],main_buff[12];
	char tt[20];
	
	int l_value,s_value;
	long int value;
	
	memset(temp_buff,0,sizeof(temp_buff));
	memset(main_buff,0,sizeof(main_buff));
	memset(tt,0,sizeof(tt));
	
	strcpy(tt,p_buff);
	value=atoi(tt);
	while(1)
	{
		s_value=value/16;//商
		l_value=value%16;//余数
		switch(l_value)
		{
			case 10:
				strcpy(temp_buff,"A");
				break;			
			case 11:
				strcpy(temp_buff,"B");
				break;
			case 12:
				strcpy(temp_buff,"C");
				break;
			case 13:
				strcpy(temp_buff,"D");
				break;
			case 14:
				strcpy(temp_buff,"E");
				break;
			case 15:
				strcpy(temp_buff,"F");
				break;
			default:
				sprintf(temp_buff,"%d",l_value);
				break;
		}
		
		strcpy(buff,main_buff);//上次的拷入buff
		sprintf(main_buff,"%s%s",temp_buff,buff);//这次，上次
		
		if(!s_value)
			break;
		else
				value=s_value;
		
	}
	strcpy(p_buff,main_buff);		
}


int FormatConversion::tt(char x)
{
   int i;

   switch (x)
   {
   case 'A':
     return 10;
   case 'a':
     return 10;
   case 'B':
     return 11;
   case 'b':
     return 11;
   case 'C':
     return 12;
   case 'c':
      return 12;
   case 'D':
      return 13;
   case 'd':
      return 13;
   case 'E':
      return 14;
   case 'e':
      return 14;
   case 'F':
      return 15;
   case 'f':
      return 15;
   default:
      i = x - 48;
      if(i<0 || i>9) {
         i=-1;
      }
      return i;
   }
}

void FormatConversion::hexToInt(char *source,char *des)
{
      char source_temp_buff[20],destination_temp_buff[20];
      int  sourceLength;
      int  result=0;
      int  singlenum;
      
      memset(source_temp_buff,0,sizeof(source_temp_buff));
      memset(destination_temp_buff,0,sizeof(destination_temp_buff));
      
      strcpy(source_temp_buff,source);
      sourceLength=strlen(source_temp_buff);
      
      for(int i=1;i<=sourceLength;i++)
      {
         if( ( singlenum=tt(source_temp_buff[i-1]) ) == -1 ) 
         {
            break;
         }
         result +=  singlenum * (int)( pow((double)16,(sourceLength - i )) );
      }
      sprintf(destination_temp_buff,"%d",result);
      
      strncpy(des,destination_temp_buff,sizeof(destination_temp_buff));		
}

