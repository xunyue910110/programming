// ##########################################
// Source file : DataFormat.cpp
// System      : Move Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010912
// Update      : 20011207
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "DataFormat.h"
#include "base/StringUtil.h"
extern "C++" double pow(double,int);

//##ModelId=3B9F5BE20131
DataFormat::~DataFormat()
{
}

//##ModelId=3B9F5D9D023C
DataFormat::DataFormat()
{
}


//##ModelId=3BA1AE11009A
//##Documentation
//## 用于对 HEX 及 Ascii 数据的转换(=0:hex; =1:ascii; =2:HexToDec)
bool DataFormat::getFmtValue(string &desValue,const unsigned char *srcValue,
							const int srcLength,const char * formatType,const int order)
{
   int x;
   long z;
   char desTmp[5000];
   unsigned char srcTmp[2048];

   if( strcmp(formatType,(char *)"0") == 0 )   // is HEX
   {
      if(order==1 )           // 字节高低位交换
      {
		switch (srcLength)
		{
		case 1:								//单字节不需要交换
			srcTmp[0]=srcValue[0];
			srcTmp[1]=srcValue[1];
			break;
		case 2:								//二字节高低位交换
			srcTmp[0]=srcValue[1];
			srcTmp[1]=srcValue[0];
			break;
                case 3:								//三字节高低位交换
			srcTmp[0]=srcValue[2];
			srcTmp[1]=srcValue[1];
			srcTmp[2]=srcValue[0];
			break;
		case 4:								//四字节高低位交换
			srcTmp[0]=srcValue[3];
			srcTmp[1]=srcValue[2];
			srcTmp[2]=srcValue[1];
			srcTmp[3]=srcValue[0];
			break;
		default:
			return false;
		}
         /*for(int i=0;i<srcLength;i+=2)
         {
            srcTmp[i]   = srcValue[i+1];
            srcTmp[i+1] = srcValue[i];
         }*/
         extractValue(desTmp,srcTmp,srcLength);
      } 
      else 
      {
	      extractValue(desTmp,srcValue,srcLength);
      }
   } 
   else if( strcmp(formatType,(char *)"1") == 0 ) // is ascii
   { 
      //去前后的空格 by chenmin 02-06-05
      int iPreFix,iPostFix;
      //前空格位置
      for( iPreFix=0;iPreFix<srcLength;iPreFix++)
      {
         if( srcValue[iPreFix] != ' ')
         	break;	
      }
      //后空格位置
      for( iPostFix=0;iPostFix<srcLength;iPostFix++)
      {
      	 if( srcValue[srcLength-iPostFix-1] != ' ')
         	break;	
      }
      
      if( (srcLength-iPostFix-iPreFix) < 0 ) //若该字段都为空格时
      {
      	  desTmp[0]=0;
      }
      else
      {	  
      	  memcpy(desTmp,srcValue+iPreFix,srcLength-iPostFix);
      	  desTmp[srcLength-iPostFix-iPreFix]=0;
      }
      //over by chenmin 02-06-05
   } 
   else if( strcmp(formatType,(char *)"2") == 0 ) 
   { // is HexToDec
      z=0;
      for(int i=1;i<=srcLength;i++)
      {
         if( srcValue[i-1] == ' ' )
         	continue;
         if( ( x=tt(srcValue[i-1]) ) == -1 ) 
         {

            return false;
         }
         z +=  x * (int)( pow((double)16,(srcLength - i )) );
      }
      sprintf(desTmp,"%d",z);
   }
   desValue=string(desTmp);

   return true;
}

//##ModelId=3BA083C90203
//##Documentation
//## 用于对二进制数据的转换
bool DataFormat::getFmtValue(string &desValue,const unsigned char *srcValue,
							const int srcLength,const int order)
{
	bool flag;
	long number;
	char chTmpBuf[10];
	unsigned char tmpBuf[10];

	flag	   = true;
	number	= 0;
	//add by chenmin 2002-4-30
	if(srcLength==0)
	{
	     return true;
	}

	if (order==0) {							//不需要高低位交换
		memcpy(tmpBuf,srcValue,srcLength);
	} else {
		switch (srcLength)
		{
		case 1:								//单字节不需要交换
			tmpBuf[0]=srcValue[0];
			tmpBuf[1]=srcValue[1];
			break;
		case 2:								//二字节高低位交换
			tmpBuf[0]=srcValue[1];
			tmpBuf[1]=srcValue[0];
			break;
        case 3:								//三字节高低位交换
			tmpBuf[0]=srcValue[2];
			tmpBuf[1]=srcValue[1];
			tmpBuf[2]=srcValue[0];
			break;
		case 4:								//四字节高低位交换
			tmpBuf[0]=srcValue[3];
			tmpBuf[1]=srcValue[2];
			tmpBuf[2]=srcValue[1];
			tmpBuf[3]=srcValue[0];
	 		break;
		default:
			flag=false;
		}
	}

	for( int i=1 ; i <= srcLength ; i++ ) {
		number +=  tmpBuf[i-1] * (int)( pow((double)256,(srcLength - i)) );
	}

	sprintf(chTmpBuf,"%u",number);
	desValue=chTmpBuf;

	return flag;
}

//##ModelId=3B515F6F0389
//##Documentation
//## 用于对各类BCD码的转换
bool DataFormat::getFmtValue(string &desValue,const unsigned char *srcValue,
								const int srcLength,const int padding,const string &bcdType)
{
	bool flag;
	int i,j,length,len;
	char desTmp[100],Tmp[100],Tmp1[100];

	flag=true;
	
	//add by chenmin 2002-5-20
	//针对从符合字段中取出的类型为fix的电话号码
	//对于用'\0'来填充的字符串数组,若不判其长度为0而返回的话,则其bcd解码后不为空,而是n个'0'字符
	if( (srcValue[0]=='\0' && srcValue[1]=='\0' && srcLength > 10) || srcLength <= 0 )
	{
	     return true;
	}

	extractValue(desTmp,srcValue,srcLength);
	length=srcLength * 2;

   if(bcdType.compare(0,4,"PBCD")==0) {
		for (i=0;i<length;i++)
		{
			Tmp[i]=desTmp[i];
			if (Tmp[i] != padding)
			   if( !isdigit(Tmp[i]) )
					flag=false;
		}
   } else if(bcdType.compare(0,4,"TBCD")==0) {
		for (i=0;i<length;i+=2)
		{
			Tmp[i]=desTmp[i+1];		// 高低位交换
			Tmp[i+1]=desTmp[i];
			if (Tmp[i] != padding)
				if( !isdigit(Tmp[i]) )
					flag=false;
			if (Tmp[i+1] != padding)
				if( !isdigit(Tmp[i+1]) )
					flag=false;
		}
   } else if(bcdType.compare(0,4,"EBCD")==0) {
		for (i=0;i<length;i++)
		{
			switch (desTmp[i]) {
			case 'A':
				Tmp[i]='0';
				break;
			case 'B':
				Tmp[i]='#';
				break;
			case 'C':
				Tmp[i]='*';
				break;
			default:
				Tmp[i]=desTmp[i];
				if (Tmp[i] != padding )
					if( !isdigit(Tmp[i]) )
						flag=false;
			}
		}
	} else if(bcdType.compare(0,4,"UBCD")==0) {
		for (i=0;i<length;i++)
		{
			Tmp[i]=desTmp[i]&0xf;		// 取低位，高位为0
			if (Tmp[i] != padding)
				if( !isdigit(Tmp[i]) )
					flag=false;
		}
	}

	for (i=0,j=0;i<length;i++)
	{
		if( Tmp[i]!=padding ) {			// 移去填充字符
		   Tmp1[j]=Tmp[i];
		   j++;
		}
	}
   Tmp1[j]=0;

	desValue=Tmp1;

	return flag;
}

//##ModelId=3BA0A45301E3
bool DataFormat::extractValue(char *theDesValue,const unsigned char *theSrcValue,
								const int theSrcLength)
{
   static char s[] ={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

	for (int i=0;i<theSrcLength;i++)
	{
		theDesValue[i*2]=s[(theSrcValue[i]&0xf0)>>4];
		theDesValue[i*2+1]=s[theSrcValue[i]&0x0f];
	}
	theDesValue[(theSrcLength-1)*2+2]=0;

	return true;
}

//##ModelId=3C39B80B0221
int DataFormat::tt(char x)
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

// 功能：提供gprs话单中特殊的accessPointNameNI数据类型的格式转换
bool DataFormat::getGprsAscii(string &desValue,const unsigned char *srcValue,
							const int srcLength)
{
	char *tmp;
	char *tmp1 = new char[srcLength+1];
	
	memset(tmp1,0,srcLength+1);
	tmp = (char *)srcValue;
	if(tmp[0]<32||tmp[0]>127)
	{
		memset(tmp1,0,sizeof(tmp1));
		int t=0;
		for(int i=0;i<srcLength;)
		{
			t+=tmp[i];
			strncat(tmp1,tmp+i+1,tmp[i]);
			if( (tmp[i]+1+i) < srcLength )
			{
				{strcat(tmp1,".");t++;}
			}
			else
			{
				tmp1[t]='\0';
				break;
			}
			i=tmp[i]+1+i;
		}
	}
	else
	{
		strncpy(tmp1,tmp,srcLength);
	}			

	desValue = tmp1;
	// 转换成小写
	desValue = StringUtil::toLowerCase(desValue);
	delete tmp1;
	
	return true;
}

// 功能：提供gprs话单中ggsn文本格式的IP地址,转换成16进制的IP地址
bool DataFormat::getGprsHexIp(string &desValue,const unsigned char *srcValue,
							const int srcLength)
{
	int iPos = 0;
	int iValue,i;
	char cValue[2+1];
	char *cpTmp;
	
	for(i=0;i<srcLength;++i)
	{
		if( srcValue[i]=='.' )
		{
			cpTmp = new char[i-iPos+1];
			memset(cpTmp,0,i-iPos+1);
			strncpy(cpTmp,(char*)(srcValue+iPos),i-iPos);
			iValue = atoi(cpTmp);
			sprintf(cValue,"%02X\0",iValue);
			desValue += cValue;	
			
			iPos = i+1 ;
			delete cpTmp;
		}
	}
	if( iPos < i )
	{
		cpTmp = new char[i-iPos+1];
		memset(cpTmp,0,i-iPos+1);
		strncpy(cpTmp,(char*)(srcValue+iPos),i-iPos);
		iValue = atoi(cpTmp);
		sprintf(cValue,"%02X\0",iValue);
		desValue += cValue;	
		
		iPos = i+1 ;
		delete cpTmp;		
	}
	
	return true;
}
