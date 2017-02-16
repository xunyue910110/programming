

#include "SimpleConfigure.h"
#include "CryptUtil.h"  //add by gaojf 2006-4-13 20:02 for decode

#include <string.h>
#include <errno.h>


#define COMMENT           1
#define KEYWORD           2
#define VALUE             3

const int MAX_BUFFER_SIZE_BIG  =1024*2;
const int MAX_BUFFER_SIZE_MID  =1024*1;
const int MAX_BUFFER_SIZE_TINY =256;


SimpleConfigure::SimpleConfigure() {
    cRemark='#';
    cSpacemark='=';
}

SimpleConfigure::~SimpleConfigure()  {
    if(hCfg)
        fclose(hCfg);
}

int SimpleConfigure::initialize(const char *filename) {
    if((hCfg=fopen(filename,"rb"))==NULL)
        return -1;
    return 0;
}

int SimpleConfigure::end() {
    if(fclose(hCfg))
        return -1;
    return 0;
}

int SimpleConfigure::geterrmsg(char *errmsg, const int len)
{
    strncpy(errmsg,strerror(errno),len);
    return 0;
}

int SimpleConfigure::setremark(const char remark)
{
    cRemark=remark;
    return 0;
}

int SimpleConfigure::setspacemark(const char spacemark)
{
    cRemark=spacemark;
    return 0;
}

string SimpleConfigure::getval(const char *sect, 
                               const char *param,
                               const char *defaultvalue) {
  char buffer[MAX_BUFFER_SIZE_BIG];
  memset(buffer,0,sizeof(buffer));
  
  if ( getval(sect,param,buffer,sizeof(buffer))==0 ) {
    return buffer;  	
  }
  
  return defaultvalue;
}


int SimpleConfigure::getval(const char *sect, const char *param, char *val, int len)
{
    char buf[MAX_BUFFER_SIZE_BIG]="";
    char title[MAX_BUFFER_SIZE_TINY]="";
    int i;
    int j;
    char keyword[MAX_BUFFER_SIZE_TINY]="";
    char value[MAX_BUFFER_SIZE_BIG]="";
    int inputstat=0;
    int nextinput;
    int inchar;
    
    fseek(hCfg,0,SEEK_SET);
    while(!feof(hCfg))
    {
        fgets(buf,MAX_BUFFER_SIZE_BIG,hCfg);
        if(buf[0]=='[')
        {
            j=0;
            for(i=1;i<strlen(buf);i++)
            {
                if(buf[i]==' '&&j==0)continue;
                if(buf[i]==']')
                {
                    for(j=j-1;j>0;j--)
                    {
                        if(title[j]!=' ')
                        {
                            title[j+1]=0;
                            break;
                        }
                    }
                    break;
                }
                title[j]=buf[i];
                j++;
            }
        }
        else
        {
            if(sect&&strcmp(sect,title))continue;
            keyword[0]=0;
            value[0]=0;
            nextinput=KEYWORD;
            for(i=0;i<strlen(buf)-1;i++)
            {
                inchar=buf[i];
                if((inchar=='\'')&&(nextinput!=COMMENT)&&(inputstat!=2))
                {
                    inputstat=1-inputstat;
                    continue;
                }
                if((inchar=='\"')&&(nextinput!=COMMENT)&&(inputstat!=1))
                {
                    inputstat=2-inputstat;
                    continue;
                }
                if(inputstat==0)
                {
                    if(inchar==cRemark)
                    {
                        nextinput=COMMENT;
                        continue;
                    }
                    if(inchar==cSpacemark)
                    {
                        nextinput=VALUE;
                        continue;
                    }
                    if(inchar==' '||inchar=='\t'||nextinput==COMMENT)
                        continue;
                }
                if(nextinput==KEYWORD)
                {
                    sprintf(keyword,"%s%c",keyword,inchar);
                }
                else
                {
                    sprintf(value,"%s%c",value,inchar);
                }
            }
            if(strcmp(keyword,param))
                continue;
            
            //strncpy(val,value,len);
            //为支持加密解码而加 2006-4-13 20:01 
            string  decodeValue;
            decodeValue = value;
            CryptUtil::decrypt_pakage(decodeValue);
            strncpy(val,decodeValue.c_str(),len);
            return 0;
        }
    }
    return -1;
}



