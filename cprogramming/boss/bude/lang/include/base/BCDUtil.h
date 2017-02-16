

#ifndef __BCDUTIL_H__   
#define __BCDUTIL_H__   

                       
#include "config-all.h"

#include <string>
#include <cctype>
#include <vector>
#include <list>
#include <stdio.h>
#include <math.h>



USING_NAMESPACE(std);

/**
  @class BCDUtil 
  
  @brief BCD处理类
 */


class BCDUtil {
  public:

	static int Dec2Hex(char *str) {
		long value=atol(str);
		return sprintf(str,"%X",value);
	};
	
	static int Hex2Dec(char *str)
	{
		int i,data,value=0;
		int length = strlen(str);
		for(i=0;i<length;i++) {
			data=str[i];
			if ((data>='a')&&(data<='f'))
				data=data-'a'+10;
			else if ((data>='A')&&(data<='F'))
				data=data-'A'+10;
			else
				data-='0';
	
			value=value*16+data;	
		}
	
		return sprintf(str,"%d",value);		
	};	


	static int toString(char *str, double d, int l, int w)	{
	  int n;
	  char f[256];
	
	  if (w != 0) l++;
	
	  sprintf (f, "%%0%d.%dlf", l, w);
	  sprintf (str, f, d); 
	
	  for (n = 0; n < strlen (str); n++) {
	    if (str[n] == '.')
	      break;
	  }
	  
	  strcpy (&str[n], &str[n + 1]);
	  str[l] = '\0';
	
	  return 0;
	};


	static int hexValue(char x) {
	   int i;
	   switch (x) {
	     case 'A': case 'a': return 10;
	     case 'B': case 'b': return 11;
	     case 'C': case 'c': return 12;
	     case 'D': case 'd': return 13;
	     case 'E': case 'e': return 14;
	     case 'F': case 'f': return 15;
	      default:
	         i = x - 48;
	         if(i<0 || i>9) {
	            i=-1;
	         }
	         return i;
	   }
	};

	static int HexToInt(char *str,char *des) {
		 long value,singlenum;
	     long length = strlen(str);
		 for(int i=1;i<=length;i++) {
	       if( ( singlenum=hexValue(str[i-1]) ) == -1 ) {
	          break;
	       }
	       value +=  singlenum * (int)( pow((double)16,(int)(length - i )) );
	     }
	     return sprintf(str,"%d",value);
	};


	static int bcdstr(char *str,char *bcd,int n)
	{
		int i;
		char *p;
	
		for(i=0;i<n;i++) {
			/* convert 4 high bit to char */
			p=str+2*i;
			*p=(*(bcd+i)&0xF0)>>4;
	
			if(*p<10)
				*p+='0';
			else		
				*p+='A'-10;
	
			/* convert 4 low bit to char */
			p=str+2*i+1;
			*p=(*(bcd+i)&0x0F);
			
			if(*p<10)
				*p+='0';
			else		
				*p+='A'-10;
		}
		*(str+2*n)=0;
		
		return 2*n;
	};


	/* Convert TBCD to STRING, one byte convert two char, return length of string */
	static int tbcdstr(char *str,char *bcd,int n)
	{
		int i;
		char *p;
	
		for(i=0;i<n;i++)
		{
			/* convert 4 high bit to char */
			p=str+2*i;
			*p=(*(bcd+i)&0x0F);
			
			if(*p<10)
				*p+='0';
			else		
				*p+='A'-10;
	
			/* convert 4 low bit to char */
			p=str+2*i+1;
			*p=(*(bcd+i)&0xF0)>>4;
			
			if(*p<10)
				*p+='0';
			else		
				*p+='A'-10;
		}
		*(str+2*n)=0;
		
		return 2*n;
	
	};
	
	
	/* Convert TBCD to STRING, one byte convert two char, return length of string */
	static int tbcdstr(char *str,char *bcd,int n,char pad)
	{
		int i;
		char *p;
		int count=0;
		
		for(i=0;i<n;i++)
		{
			/* convert 4 high bit to char */
			p=str+2*i;
			*p=(*(bcd+i)&0x0F);
			
			if(*p<10)
				*p+='0';
			else		
				*p+='A'-10;
			if(*p==pad)
				break;
			count++;
			/* convert 4 low bit to char */
			p=str+2*i+1;
			*p=(*(bcd+i)&0xF0)>>4;
			
			if(*p<10)
				*p+='0';
			else		
				*p+='A'-10;
			
			if(*p==pad)
				break;
			count++;
		}
		*(str+count)=0;
		
		return count;
	
	};
	
	/* Convert BCD to int*/
	static int bcdint(char *bcd,int n)
	{
		char *p;
		int i,tmp,len=0;
		
		for(i=0;i<n;i++)
		{
			p=bcd+i;
			tmp=(*p&0xF0)>>4;
			
			len+=sums(tmp,2*n-2*i-1);
			tmp=*p&0x0F;
			
			len+=sums(tmp,2*n-2*i-2);
		}
		
		return len;
	};
	
	static int sums(int tmp,int n)
	{
		int i;
		
		for(i=0;i<n;i++)
		{
			tmp*=16;
		}
		return tmp;
	};

    /// 对字符串进行编码
    static int EBCD_incode(char *DST, const char *SRC)
    {
      	int  i, j, n;
      	
      	int Nstr_len = strlen(SRC);
      	n = (Nstr_len + 1) / 2;    	
     
      	for(i = 0, j = 0; i < Nstr_len; i++, j++)
      	{
      	  	if( (SRC[i] < '0' || SRC[i] > '9') && 
      	  	     SRC[i] != '#' && SRC[i] != '*' ) 
      	  	   return -1; 	  				

      	  	DST[j] = (SRC[i] - '0') << 4;
      	  	if( SRC[i] == '0' ) DST[j] |= 0xA0;
      	  	if( SRC[i] == '#' ) DST[j] |= 0xB0;
      	  	if( SRC[i] == '*' ) DST[j] |= 0xC0;	
      	  	i++;

      	  	if(i < Nstr_len) {
      	  	  	if( (SRC[i] < '0' || SRC[i] > '9') && 
      	  	  	    SRC[i] != '#' && SRC[i] != '*' ) 
      	  	  	  return -1; 
      	  	  	DST[j] |= (SRC[i] - '0');
      	  	  	if( SRC[i] == '0' ) DST[j] |= 0x0A;
      	  		if( SRC[i] == '#' ) DST[j] |= 0x0B;
      	  		if( SRC[i] == '*' ) DST[j] |= 0x0C;	
      	  	}
      	  	
      	}
      	if ((Nstr_len % 2) !=0 )  
      	   DST[n-1] |= 0x0F; 

      	DST[n]=0;

      	return n;
    };


    /// 对EBCD 编码的字串进行解码
    static void EBCD_decode(char *DST, const char *SRC)
    {
    	int len 	= strlen(SRC);
    	char s[] 	= {'0','1','2','3','4','5','6','7','8','9','0','#','*','D','E','F'};
    
    	for (int i=0;i<len;i++)
    	{
    		DST[ i*2 ]	= s[(SRC[i]&0xf0) >> 4 ];
    		if( DST[ i*2 ] == 'F' ) {
    			DST[ i*2 ] = 0;
    			return;
    		}
    		DST[i*2+1]	= s[ SRC[i]&0x0f ];
    		if( DST[ i*2+1 ] == 'F' ) {
    			DST[ i*2+1 ] = 0;
    			return;
    		}
    	}
    	
    	DST[(len-1)*2+2]=0;
    
    	return ;
    };

};


#endif //__BCDUTIL_H__



