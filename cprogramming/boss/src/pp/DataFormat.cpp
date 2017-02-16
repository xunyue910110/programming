// ##########################################
// Source file : DataFormat.cpp
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#include "DataFormat.h"
#include "base/StringUtil.h"
#include "FieldInfo.h"
extern "C++" double pow(double,int); //added by zhongh 2011/1/18 11:45:43
//##ModelId=4851CF90006D
//##Documentation
//## 用于对各类BCD码的转换
bool DataFormat::getBCDValue(FieldInfo *pFieldInfo)
{
    bool flag;
    int i, j, length;

    flag = true;

    //针对从复合字段中取出的类型为fix的电话号码
    //对于用'\0'来填充的字符串数组,若不判其长度为0而返回的话,则其bcd解码后不为空,而是n个'0'字符

    length = pFieldInfo->m_iSrcValueLen * 2;
    pFieldInfo->reserveDesStr(length);
    extractValue(pFieldInfo->m_FieldDesValue, pFieldInfo->m_FieldSrcValue, pFieldInfo->m_iSrcValueLen);
    
    if(pFieldInfo->m_dataType.compare(0, 4, "PBCD") == 0)
    {
        for (i = 0; i < length; i++)
        {
            if (pFieldInfo->m_FieldDesValue[i] != pFieldInfo->m_padding)
            {
                if(!isdigit(pFieldInfo->m_FieldDesValue[i]))
                {
                    flag=false;
                }
            }
        }
    }
    else if(pFieldInfo->m_dataType.compare(0, 4, "TBCD") == 0)
    {
        for (i = 0; i < length; i+=2)
        {
            char cTmp;
            cTmp = pFieldInfo->m_FieldDesValue[i];                    // 高低位交换
            pFieldInfo->m_FieldDesValue[i] = pFieldInfo->m_FieldDesValue[i+1];
            pFieldInfo->m_FieldDesValue[i+1] = cTmp;
            
            if (pFieldInfo->m_FieldDesValue[i] != pFieldInfo->m_padding)
            {
                if( !isdigit(pFieldInfo->m_FieldDesValue[i]))
                {
                    flag=false;
                }
            }
            if (pFieldInfo->m_FieldDesValue[i+1] != pFieldInfo->m_padding)
            {
                if(!isdigit(pFieldInfo->m_FieldDesValue[i+1]))
                {
                    flag=false;
                }
            }
        }
    }
    else if(pFieldInfo->m_dataType.compare(0, 4, "EBCD") == 0)
    {
        for (i = 0; i < length; i++)
        {
            switch (pFieldInfo->m_FieldDesValue[i])
            {
            case 'A':
                pFieldInfo->m_FieldDesValue[i] = '0';
                break;
            case 'B':
                pFieldInfo->m_FieldDesValue[i] = '#';
                break;
            case 'C':
                pFieldInfo->m_FieldDesValue[i] = '*';
                break;
            default:
                if (pFieldInfo->m_FieldDesValue[i] != pFieldInfo->m_padding)
                {
                    if(!isdigit(pFieldInfo->m_FieldDesValue[i]))
                    {
                        flag=false;
                    }
                }
            }
        }
    }
    else if(pFieldInfo->m_dataType.compare(0, 4, "UBCD") == 0)
    {
        for (i = 0; i < length; i++)
        {
            pFieldInfo->m_FieldDesValue[i] = (pFieldInfo->m_FieldDesValue[i]&0xf);         // 取低位，高位为0
            if (pFieldInfo->m_FieldDesValue[i] != pFieldInfo->m_padding)
            {
                if(!isdigit(pFieldInfo->m_FieldDesValue[i]))
                {
                    flag=false;
                }
            }
        }
    }
    // 正对字符串中的00 13 BB BF 的情况要去掉B 和 F
    else if(pFieldInfo->m_dataType.compare(0, 4, "FBCD") == 0)
    {
        for (i = 0; i < length; i++)
        {
            switch (pFieldInfo->m_FieldDesValue[i])
            {
            case 'f':
                pFieldInfo->m_FieldDesValue[i] = '\0';
                break;
            case 'F':
                pFieldInfo->m_FieldDesValue[i] = '\0';
                break;
            default:
                if (pFieldInfo->m_FieldDesValue[i] != pFieldInfo->m_padding)
                {
                    if (!isdigit(pFieldInfo->m_FieldDesValue[i]))
                    {
                        flag = false;
                    }
                }
                break;
            }
        }
    }
    // 针对新疆和天津爱立信交换机出现的BCD码要求出现字母而增加
    else if(pFieldInfo->m_dataType.compare(0, 4, "MBCD")==0)       //扩展BCD码(编码范围：0-9,A,B,C,D,E)    added by zaiww 20070718
    {
		for (i=0; i<length; i++)
		{
			if (pFieldInfo->m_FieldDesValue[i] != pFieldInfo->m_padding)
			{
			    if( (!isdigit(pFieldInfo->m_FieldDesValue[i]))
			    	&&(pFieldInfo->m_FieldDesValue[i]<'A'
			    	   ||pFieldInfo->m_FieldDesValue[i]>'E') )
			    {
			        flag = false;
			    }
			}
		}
    }
    else if(pFieldInfo->m_dataType.compare(0, 4, "NBCD")==0)       //扩展交换BCD码(编码范围：0-9,A,B,C,D,E)    added by zaiww 20070718
    {
		for (i=0; i<length; i+=2)
		{
			char cTmp;
            cTmp = pFieldInfo->m_FieldDesValue[i];                    // 高低位交换
            pFieldInfo->m_FieldDesValue[i] = pFieldInfo->m_FieldDesValue[i+1];
            pFieldInfo->m_FieldDesValue[i+1] = cTmp;
            
			if (pFieldInfo->m_FieldDesValue[i] != pFieldInfo->m_padding)
			{
				if( (!isdigit(pFieldInfo->m_FieldDesValue[i]))
					 &&( pFieldInfo->m_FieldDesValue[i]<'A'
					     ||pFieldInfo->m_FieldDesValue[i]>'E') )
				{
					flag = false;
				}
			}
			if (pFieldInfo->m_FieldDesValue[i+1] != pFieldInfo->m_padding)
			{
				if( (!isdigit(pFieldInfo->m_FieldDesValue[i+1]))
						&&( pFieldInfo->m_FieldDesValue[i+1]<'A'
							||pFieldInfo->m_FieldDesValue[i+1]>'E') )
				{
					flag = false;
				}
			}
		}
	}

    for (i = 0, j = 0; i < length; i++)
    {
        if(pFieldInfo->m_FieldDesValue[i] != pFieldInfo->m_padding)               // 移去填充字符
        {
            pFieldInfo->m_FieldDesValue[j] = pFieldInfo->m_FieldDesValue[i];
            j++;
        }
    }
    pFieldInfo->m_FieldDesValue[j] = 0;
    pFieldInfo->m_iDesValueLen = j;
    
    return flag;
}

//##ModelId=4851D02A01C5
//##Documentation
//## 用于对二进制数据的转换
bool DataFormat::getBinaryValue(FieldInfo *pFieldInfo)
{
    bool flag;
    long number;
    if(pFieldInfo->m_iSrcValueLen == 0)
    {
         return true;
    }

	if(pFieldInfo->m_iSrcValueLen < 0)
    {
         return false;
    }

    char * chTmpBuf = new char[pFieldInfo->m_iSrcValueLen+10];
    unsigned char* tmpBuf = (unsigned char* )new char[pFieldInfo->m_iSrcValueLen+10];

    flag   = true;
    number = 0;

    if (pFieldInfo->m_order == 0)                         //不需要高低位交换
    {
        memcpy(tmpBuf, pFieldInfo->m_FieldSrcValue, pFieldInfo->m_iSrcValueLen);
    }
    else
    {
        switch (pFieldInfo->m_iSrcValueLen)
        {
        case 1:                             //单字节不需要交换
            tmpBuf[0] = pFieldInfo->m_FieldSrcValue[0];
            tmpBuf[1] = pFieldInfo->m_FieldSrcValue[1];
            break;
        case 2:                             //二字节高低位交换
            tmpBuf[0] = pFieldInfo->m_FieldSrcValue[1];
            tmpBuf[1] = pFieldInfo->m_FieldSrcValue[0];
            break;
        case 3:                             //三字节高低位交换
            tmpBuf[0] = pFieldInfo->m_FieldSrcValue[2];
            tmpBuf[1] = pFieldInfo->m_FieldSrcValue[1];
            tmpBuf[2] = pFieldInfo->m_FieldSrcValue[0];
            break;
        case 4:                             //四字节高低位交换
            tmpBuf[0] = pFieldInfo->m_FieldSrcValue[3];
            tmpBuf[1] = pFieldInfo->m_FieldSrcValue[2];
            tmpBuf[2] = pFieldInfo->m_FieldSrcValue[1];
            tmpBuf[3] = pFieldInfo->m_FieldSrcValue[0];
            break;
        default:
            flag = false;
        }
    }

    for( int i=1; i <= pFieldInfo->m_iSrcValueLen; i++ )
    {
        number +=  tmpBuf[i - 1] * (int)(pow((double)256,(pFieldInfo->m_iSrcValueLen - i)));
    }

    sprintf(chTmpBuf, "%u", number);
    pFieldInfo->setDesValue( chTmpBuf );
    delete[] chTmpBuf;
    delete[] tmpBuf;
    return flag;
}

//##ModelId=4851D08D02DE
//##Documentation
//## 用于对 HEX 及 Ascii 数据的转换(=0:hex; =1:ascii; =2:HexToDec; =3:HexToBin; =4:AsciitoDec; =5:AsciiToInt)
bool DataFormat::getHexAsciiValue(FieldInfo *pFieldInfo,const char *formatType)
{
    int x;
    long z;
    char desTmp[5000];
    unsigned char srcTmp[2048];

    if(strcmp(formatType, (char *)"0") == 0)    // is HEX
    {
        if(pFieldInfo->m_order == 1 )                             // 字节高低位交换
        {
            switch (pFieldInfo->m_iSrcValueLen)
            {
            case 1:                                 //单字节不需要交换
                srcTmp[0] = pFieldInfo->m_FieldSrcValue[0];
                srcTmp[1] = pFieldInfo->m_FieldSrcValue[1];
                break;
            case 2:                                 //二字节高低位交换
                srcTmp[0] = pFieldInfo->m_FieldSrcValue[1];
                srcTmp[1] = pFieldInfo->m_FieldSrcValue[0];
                break;
                    case 3:                         //三字节高低位交换
                srcTmp[0] = pFieldInfo->m_FieldSrcValue[2];
                srcTmp[1] = pFieldInfo->m_FieldSrcValue[1];
                srcTmp[2] = pFieldInfo->m_FieldSrcValue[0];
                break;
            case 4:                                 //四字节高低位交换
                srcTmp[0] = pFieldInfo->m_FieldSrcValue[3];
                srcTmp[1] = pFieldInfo->m_FieldSrcValue[2];
                srcTmp[2] = pFieldInfo->m_FieldSrcValue[1];
                srcTmp[3] = pFieldInfo->m_FieldSrcValue[0];
                break;
            default:
                return false;
            }

            extractValue(desTmp, srcTmp, pFieldInfo->m_iSrcValueLen);
        }
        else
        {
            extractValue(desTmp, pFieldInfo->m_FieldSrcValue, pFieldInfo->m_iSrcValueLen);
        }
        pFieldInfo->setDesValue(desTmp);
    }
    else if(strcmp(formatType, (char *)"1") == 0) // is ascii
    {
        pFieldInfo->setDesValue((char *)pFieldInfo->m_FieldSrcValue,pFieldInfo->m_iSrcValueLen);
        StringUtil::trim(pFieldInfo->m_FieldDesValue);
    }
    else if(strcmp(formatType, (char *)"2") == 0)   // is HexToDec
    {
        z=0;
        for(int i=1;i<=pFieldInfo->m_iSrcValueLen;i++)
        {
            if(pFieldInfo->m_FieldSrcValue[i-1] == ' ')
            {
                continue;
            }
            if( (x = tt(pFieldInfo->m_FieldSrcValue[i-1])) == -1)
            {
                return false;
            }
            z +=  x * (int)(pow((double)16, (pFieldInfo->m_iSrcValueLen - i )));
        }
    	sprintf(desTmp, "%d", z);
    	pFieldInfo->setDesValue(desTmp);
    }
    else if (strcmp(formatType, (char *)"3") == 0)  // is HexToBin
    {
        memset(srcTmp, 0, sizeof(srcTmp));
        extractValue((char *)srcTmp, pFieldInfo->m_FieldSrcValue, pFieldInfo->m_iSrcValueLen);

        memset(desTmp, 0, sizeof(desTmp));
        CharToBin((char *)srcTmp, desTmp);
        pFieldInfo->setDesValue(desTmp);
    }
    else if (strcmp(formatType,(char *)"4") == 0 )  // is AsciiToDec
    {
         //去前后的空格
        pFieldInfo->setDesValue((char *)pFieldInfo->m_FieldSrcValue,pFieldInfo->m_iSrcValueLen);
        StringUtil::trim( pFieldInfo->m_FieldDesValue);
    }
    else if (strcmp(formatType,(char *)"5") == 0 )  // is AsciiToInt
    {
        //去前后的空格
        int iPreFix2, iPostFix2;
        char tmpdes[5000];
        //前空格位置
        for( iPreFix2 = 0; iPreFix2 < pFieldInfo->m_iSrcValueLen; iPreFix2++)
        {
            if(pFieldInfo->m_FieldSrcValue[iPreFix2] != ' ')
            {
                break;
            }
        }
        //后空格位置
        for(iPostFix2 = 0; iPostFix2 < pFieldInfo->m_iSrcValueLen; iPostFix2++)
        {
            if(pFieldInfo->m_FieldSrcValue[pFieldInfo->m_iSrcValueLen - iPostFix2 - 1] != ' ')
            {
                break;
            }
        }

        if((pFieldInfo->m_iSrcValueLen - iPostFix2 - iPreFix2) < 0)    //若该字段都为空格时
        {
            tmpdes[0] = 0;
        }
        else
        {
          memcpy(tmpdes, pFieldInfo->m_FieldSrcValue + iPreFix2, pFieldInfo->m_iSrcValueLen - iPostFix2);
          tmpdes[pFieldInfo->m_iSrcValueLen - iPostFix2 - iPreFix2] = 0;
        } 
        int length = strlen(tmpdes);
        z=0;
        for(int i=1;i<=length;i++)
        {
            if(tmpdes[i-1] == ' ')
            {
                continue;
            }
            if( (x = tt(tmpdes[i-1])) == -1)
            {
                return false;
            }
            z +=  x * (int)(pow((double)16, (length - i )));
        }
    	sprintf(desTmp, "%d", z);
        pFieldInfo->setDesValue(desTmp);
    }
    
    return true;
}

//##ModelId=4851D0EF03B9
// 功能：提供gprs话单中特殊的accessPointNameNI数据类型的格式转换
bool DataFormat::getGprsAscii(FieldInfo *pFieldInfo)
{
    char *tmp=NULL;
    char *tmp1 = new char[pFieldInfo->m_iSrcValueLen + 1];

    memset(tmp1, 0, pFieldInfo->m_iSrcValueLen + 1);
    tmp = (char *)pFieldInfo->m_FieldSrcValue;
    if(tmp[0] < 32 || tmp[0] > 127)
    {
        memset(tmp1, 0, sizeof(tmp1));
        int t = 0;
        for(int i = 0; i < pFieldInfo->m_iSrcValueLen; )
        {
            t += tmp[i];
            strncat(tmp1, tmp + i + 1, tmp[i]);
            if((tmp[i] + 1 + i) < pFieldInfo->m_iSrcValueLen)
            {
                strcat(tmp1, ".");
                t++;
            }
            else
            {
                tmp1[t] = '\0';
                break;
            }
            i = tmp[i] + 1 + i;
        }
    }
    else
    {
        strncpy(tmp1, tmp, pFieldInfo->m_iSrcValueLen);
    }

    // 转换成小写
    string strTmp = tmp1;
    
    pFieldInfo->setDesValue(StringUtil::toLowerCase(strTmp));
    delete tmp1;
    return true;
}

//##ModelId=4851D12F00DA
// 功能：提供gprs话单中ggsn文本格式的IP地址,转换成16进制的IP地址
bool DataFormat::getGprsHexIp(FieldInfo *pFieldInfo)
{
    int iPos = 0;
    int iValue, i;
    char cValue[2 + 1];
    char *cpTmp;
    string desValue;

    for(i = 0; i < pFieldInfo->m_iSrcValueLen; ++i)
    {
        if(pFieldInfo->m_FieldSrcValue[i] == '.')
        {
            cpTmp = new char[i - iPos + 1];
            memset(cpTmp, 0, i - iPos + 1);
            strncpy(cpTmp, (char*)(pFieldInfo->m_FieldSrcValue + iPos),i - iPos);
            iValue = atoi(cpTmp);
            sprintf(cValue, "%02X\0", iValue);
            desValue += cValue;

            iPos = i + 1 ;
            delete cpTmp;
        }
    }
    if(iPos < i)
    {
        cpTmp = new char[i - iPos + 1];
        memset(cpTmp, 0, i - iPos + 1);
        strncpy(cpTmp, (char*)(pFieldInfo->m_FieldSrcValue + iPos), i - iPos);
        iValue = atoi(cpTmp);
        sprintf(cValue, "%02X\0", iValue);
        desValue += cValue;

        iPos = i + 1 ;
        delete cpTmp;
    }
    pFieldInfo->setDesValue(desValue);
    
    return true;
}

//##ModelId=4851D1AB0242
// 功能：提供gprs话单中ggsn16进制的IP地址,转换成10进制的IP地址，并且加点"."
bool DataFormat::getGprsDecIp(FieldInfo *pFieldInfo)
{
    int x, y, z, len;
    char cpTmp[10];
    char desTmp[5000];
    string desValue;

    extractValue(desTmp, pFieldInfo->m_FieldSrcValue, pFieldInfo->m_iSrcValueLen);
    len = strlen(desTmp);

    for(int i = 0; i < len; i += 2)
    {
         x = tt(desTmp[i]);

         y = tt(desTmp[i + 1]);

         z =  x * 16 + y;

         sprintf(cpTmp, "%d", z);

         desValue += cpTmp;

         if (i != (len - 2))
         {
            desValue += ".";
         }
    }
    pFieldInfo->setDesValue(desValue);
    return true;
}

//##ModelId=4851D1EA033C
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
        if(i < 0 || i > 9)
        {
            i = -1;
        }
        return i;
    }
}

//##ModelId=4851D2060128
bool DataFormat::extractValue(char *theDesValue, const unsigned char *theSrcValue, const int theSrcLength)
{
   static char s[] ={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

    for (int i = 0; i < theSrcLength; i++)
    {
        theDesValue[i * 2] = s[(theSrcValue[i]&0xf0)>>4];
        theDesValue[i * 2 + 1] = s[theSrcValue[i]&0x0f];
    }
    theDesValue[(theSrcLength - 1) * 2 + 2] = 0;

    return true;
}

//##ModelId=4851D24903A9
void DataFormat::movePadding(char *thestring, int padding)
{
}

//##ModelId=4851D2C502BF
int DataFormat::CharToBin(char *Source, char *Dest)
{
    int SourceLen;
    int i = 0, j = 0;
    char *tmpstr;
    int chch;
    SourceLen = strlen(Source);
    if (SourceLen == 0)
    {
        Dest = NULL;
        return 1;
    }
    tmpstr = new char[SourceLen * 4];
    memset(tmpstr, 0, sizeof(tmpstr));
    for (i = 0; i < SourceLen; i++)
    {
          chch = Source[i] - 48;
          tmpstr[i * 4 + 0] = (chch&0x01) + '0';
          tmpstr[i * 4 + 1] = ((chch>>1)&0x01) + '0';
          tmpstr[i * 4 + 2] = ((chch>>2)&0x01) + '0';
          tmpstr[i * 4 + 3] = ((chch>>3)&0x01) + '0';
    }
    strcpy(Dest, tmpstr);
    Dest[SourceLen * 4] = '\0';
    delete tmpstr;
    return 1;
}

bool DataFormat::getFmtValue(FieldInfo *pFieldInfo)
{
	bool flag;
	
	if ((pFieldInfo->m_dataType).compare(1, 3, "BCD") == 0) 
	{
		flag = getBCDValue(pFieldInfo);
	} 
	else if ((pFieldInfo->m_dataType).compare("Binary") == 0) 
	{
		flag = getBinaryValue(pFieldInfo);
	} 
	else if ((pFieldInfo->m_dataType).compare("HEX") == 0) 
	{
		flag = getHexAsciiValue(pFieldInfo,(char *)"0");
	} 
	else if ((pFieldInfo->m_dataType).compare("Ascii") == 0) 
	{
	    flag = getHexAsciiValue(pFieldInfo,(char *)"1");
	} 
	else if ((pFieldInfo->m_dataType).compare("HexToDec") == 0) 
	{
		flag = getHexAsciiValue(pFieldInfo, (char *)"2");
	}
	else if ((pFieldInfo->m_dataType).compare("AsciiToDec") == 0)
	{
	    flag = getHexAsciiValue(pFieldInfo, (char *)"4");
	}
	else if ((pFieldInfo->m_dataType).compare("AsciiToInt") == 0)
	{
	    flag = getHexAsciiValue(pFieldInfo, (char *)"5");
	} 
	else if ((pFieldInfo->m_dataType).compare("HexToBin") == 0) 
	{
		flag = getHexAsciiValue(pFieldInfo, (char *)"3");
	} 
	else if ((pFieldInfo->m_dataType).compare("GprsAsc") == 0) 
	{
		flag = getGprsAscii(pFieldInfo);
	}
	else if ((pFieldInfo->m_dataType).compare("GprsHexIp") == 0) 
	{
		flag = getGprsHexIp(pFieldInfo);
	}
	else if ((pFieldInfo->m_dataType).compare("GprsDecIp") == 0) 
	{
		flag = getGprsDecIp(pFieldInfo);
	}
	else 
	{
	    flag = false;
	}		
	
	return flag;
}
