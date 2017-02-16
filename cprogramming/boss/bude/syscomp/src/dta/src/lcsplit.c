//-------------------------------------------------------------------
// Name      : lcsplit.c
// Version   : 0.9.3
// Create    : 2000.8.28
// Update    : 2002.3.23
// Auther    : QianYiXin
// Copyright : All Rights Reserved.
// Description  : This is a utility tool to split file.
//-------------------------------------------------------------------
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#define USAGE  "\
USAGE: %s [-ssize|-lline|-hhour] [-kkeep] [-ccircle] [-a] file [name] \n\
  size  Number of bytes \n\
  line  Number of lines \n\
  hour  Number of hours \n\
  keep  Number of hours to keep file before deleting \n\
  circle  Number of circle files \n\
  file  Source file name, '-' means standard input \n\
  name  The format of output file such as \n\
    %%N or %%03N similarly as '%%d' '%%03d' in printf() \n\
    %%Y %%y %%m %%d %%H %%M %%S like format in strftime() \n"

long circle=0;
long keep=0;
char nameformat[100];
char namepath[100];

#ifdef __cplusplus
extern "C"
#endif
void timer(int);

int getfilename(const char *filename,char *truefilename);
int matchname(const char *filename,const char *nameformat);
int deloldfile();
int isfile(char *name);

int main(int argc,char *argv[])
{
  int opt;
  //extern char *optarg;
  //extern int optind;
  //extern int optopt;
  //extern int opterr;
  long size=0;
  long line=0;
  long hour=0;
  long sizes=0;
  long lines=0;
  long hours=0;
  long readsize;
  char srcfilename[100];
  char desfilename[100];
  char truefilename[200];
  char str[500];
  char *charpath;
  FILE *fin;
  FILE *fout;
  time_t now;
  struct tm *nowtime;

  while((opt=getopt(argc,argv,":s:l:h:k:c:"))!=-1)
  {
    switch(opt)
    {
    case 's':
      size=atol(optarg);
      break;
    case 'l':
      line=atol(optarg);
      break;
    case 'h':
      hour=atol(optarg);
      break;
    case 'k':
      keep=atol(optarg);
      break;
    case 'c':
      circle=atol(optarg);
      break;
    case '?':
      fprintf(stderr,USAGE,argv[0]);
      exit(-1);
    }
  }
  if(argc==optind)
  {
    fprintf(stderr,USAGE,argv[0]);
    exit(-1);
  }
  strcpy(srcfilename,argv[optind]);
  strcpy(desfilename,argv[optind+1]);
  if((charpath=strrchr(desfilename,'/'))==NULL)
  {
    strcpy(namepath,"");
    strcpy(nameformat,desfilename);
  }
  else
  {
    strncpy(namepath,desfilename,charpath-desfilename);
    namepath[charpath-desfilename]='\0';
    strcpy(nameformat,charpath+1);
  }
  if(keep>0)timer(0);

  getfilename(desfilename,truefilename);
  if(strcmp(srcfilename,"-")==0)fin=stdin;
  else
  {
    if((fin=fopen(srcfilename,"rb"))==NULL)
    {
      fprintf(stderr,"%s open error!\n",srcfilename);
      exit(-1);
    }
  }
  fcntl(fileno(fin),F_SETFL,fcntl(fileno(fin),F_GETFL,0)|O_NONBLOCK);
  fout=fopen(truefilename,"wb");

  if(line>0)
  {
    while(!feof(fin))
    {
      while(fgets(str,500,fin)!=NULL)
      {
        if(lines==line)
        {
          fclose(fout);
          getfilename(desfilename,truefilename);
          fout=fopen(truefilename,"wb");
          lines=0;
        }
        lines++;
        fputs(str,fout);
        fflush(fout);
      }
      if(feof(fin))break;
      sleep(2);
    }
  }
  else if(size>0)
  {
    while(!feof(fin))
    {
      if(sizes==size)
      {
        if(size>=500)readsize=fread(str,1,500,fin);
        else readsize=fread(str,1,size,fin);
      }
      else
      {
        if(size-sizes>=500)readsize=fread(str,1,500,fin);
        else readsize=fread(str,1,size-sizes,fin);
      }
      if(readsize==0)
      {
        if(feof(fin))break;
        sleep(2);
        continue;
      }
      if(sizes==size)
      {
        fclose(fout);
        getfilename(desfilename,truefilename);
        fout=fopen(truefilename,"wb");
        sizes=0;
      }
      sizes+=readsize;
      fwrite(str,1,readsize,fout);
      fflush(fout);
    }
  }
  else if(hour>0)
  {
    time(&now);
    nowtime=localtime(&now);
    hours=nowtime->tm_hour;
    while(!feof(fin))
    {
      readsize=fread(str,1,500,fin);
      if(readsize==0)
      {
        if(feof(fin))break;
        sleep(2);
        continue;
      }
      time(&now);
      nowtime=localtime(&now);
      if(nowtime->tm_hour!=hours&&nowtime->tm_hour%hour==0)
      {
        fclose(fout);
        getfilename(desfilename,truefilename);
        fout=fopen(truefilename,"wb");
        sizes=0;
        hours=nowtime->tm_hour;
      }
      fwrite(str,1,readsize,fout);
      fflush(fout);
    }
  }
  fclose(fin);
  fclose(fout);
  return 0;
}


int getfilename(const char *filename,char *truefilename)
{
  int i;
  int fmt=0;
  char fmtstr[20];
  static int sn=0;
  char str[100];
  time_t now;
  struct tm *nowtime;

  time(&now);
  nowtime=localtime(&now);
  truefilename[0]=0;

  for(i=0;i<strlen(filename);i++)
  {
    if(fmt)
    {
      switch(filename[i])
      {
      case '%':
        strcat(truefilename,fmtstr);
        fmt=1;
        memset(fmtstr,0,sizeof(fmtstr));
        fmtstr[0]='%';
        break;
      case 'Y':
        strftime(str,5,"%Y",nowtime);
        strcat(truefilename,str);
        fmt=0;
        break;
      case 'y':
        strftime(str,3,"%y",nowtime);
        strcat(truefilename,str);
        fmt=0;
        break;
      case 'm':
        strftime(str,3,"%m",nowtime);
        strcat(truefilename,str);
        fmt=0;
        break;
      case 'd':
        strftime(str,3,"%d",nowtime);
        strcat(truefilename,str);
        fmt=0;
        break;
      case 'H':
        strftime(str,3,"%H",nowtime);
        strcat(truefilename,str);
        fmt=0;
        break;
      case 'M':
        strftime(str,3,"%M",nowtime);
        strcat(truefilename,str);
        fmt=0;
        break;
      case 'S':
        strftime(str,3,"%S",nowtime);
        strcat(truefilename,str);
        fmt=0;
        break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        fmtstr[strlen(fmtstr)]=filename[i];
        break;
      case 'N':
        sprintf(fmtstr,"%sd",fmtstr);
        sprintf(str,fmtstr,++sn);
        strcat(truefilename,str);
        fmt=0;
        break;
      default:
        strcat(truefilename,fmtstr);
        fmt=0;
        truefilename[strlen(truefilename)+1]=0;
        truefilename[strlen(truefilename)]=filename[i];
      }
    }
    else
    {
      if((filename[i]=='%'))
      {
        fmt=1;
        memset(fmtstr,0,sizeof(fmtstr));
        fmtstr[0]='%';
      }
      else
      {
        truefilename[strlen(truefilename)+1]=0;
        truefilename[strlen(truefilename)]=filename[i];
      }

    }
  }
  if(circle>0&&sn==circle)sn=0;
  return 0;
}


void timer(int a)
{
  static int hour=-1;
  time_t now;
  struct tm *nowtime;

  alarm(60);
  signal(SIGALRM,timer);

  time(&now);
  nowtime=localtime(&now);

  if(hour!=nowtime->tm_hour)
  {
    deloldfile();
    hour=nowtime->tm_hour;
  }
}


int deloldfile()
{
  char filename[80];
  char fullfilename[200];
  FILE *pipe;
  struct stat s;
  time_t now;
  char cmd[200];

  sprintf(cmd,"ls -1rt %s",namepath);
  if((pipe=popen(cmd,"rb"))==NULL)
  {
    return(-1);
  }
  time(&now);
  while(fgets(filename,80,pipe)!=NULL)
  {
    filename[strlen(filename)-1]='\0';
    sprintf(fullfilename,"%s/%s",namepath,filename);
    if(isfile(fullfilename)==1)
    {
      if(!matchname(filename,nameformat))continue;
      if(stat(fullfilename,&s)<0)continue;
      if(difftime(now,s.st_ctime)>keep*3600)
      {
        sprintf(cmd,"rm %s",fullfilename);
        system(cmd);
      }
      else break;
    }
  }
  pclose(pipe);
  return(0);
}


int isfile(char *name)
{
  struct stat s;
  if(stat(name,&s)<0)return -1;
  if(S_ISREG(s.st_mode))return 1;
  else return 0;
}


int matchname(const char *filename,const char *nameformat)
{
  unsigned int f;
  unsigned int d;
  int i;
  int j;
  char *charn;
  char str[50];
  char fmtstr[20];

  for(i=0,j=0;i<strlen(filename)&&j<strlen(nameformat);)
  {
    if(nameformat[j]==filename[i]){i++;j++;continue;}
    if(nameformat[j]!='%')return 0;
    j++;
    switch(nameformat[j])
    {
    case 'Y':
      if(sscanf(&filename[i],"%04d",&f)!=1)return 0;
      i+=4;
      j++;
      break;
    case 'y':
      if(sscanf(&filename[i],"%02d",&f)!=1)return 0;
      i+=2;
      j++;
      break;
    case 'm':
      if(sscanf(&filename[i],"%02d",&f)!=1)return 0;
      if(f>12)return 0;
      i+=2;
      j++;
      break;
    case 'd':
      if(sscanf(&filename[i],"%02d",&f)!=1)return 0;
      if(f>31)return 0;
      i+=2;
      j++;
      break;
    case 'H':
      if(sscanf(&filename[i],"%02d",&f)!=1)return 0;
      if(f>23)return 0;
      i+=2;
      j++;
      break;
    case 'M':
      if(sscanf(&filename[i],"%02d",&f)!=1)return 0;
      if(f>59)return 0;
      i+=2;
      j++;
      break;
    case 'S':
      if(sscanf(&filename[i],"%02d",&f)!=1)return 0;
      if(f>61)return 0;
      i+=2;
      j++;
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 'N':
      if((charn=(char *)strchr(&nameformat[j],'N'))==NULL)return 0;
      strncpy(fmtstr,&nameformat[j]-1,charn-&nameformat[j]+2);
      fmtstr[charn-&nameformat[j]+1]='d';
      fmtstr[charn-&nameformat[j]+2]='\0';
      if(sscanf(&filename[i],fmtstr,&f)!=1)return 0;
      sprintf(str,fmtstr,f);
      i+=strlen(str);
      j=charn-nameformat+1;
      break;
    default:
      return 0;
    }
  }
  if(i==strlen(filename)&&j==strlen(nameformat))return 1;
  else return 0;
}

