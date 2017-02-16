
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ftw.h>
#include <ctype.h>
#include "base/SimpleConfigure.h"
#include "dbi/dbi.h"

#ifndef NOALARM
#include "mmv_interface.h"
#endif

#define SYS_NUMBER     3
#define USAGE          "USAGE: %s [-f configfile]\n"
#define ALARM_FATAL    "0001"
#define ALARM_SERIOUS  "0002"
#define ALARM_GENERIC  "0003"
#define ALARM_KILLED   "0004"
#define ALARM_DEBUG    "0005"

extern "C" int FindFile(const char *, const struct stat *, int);
int ProcessProc(char *);
void fieldsvalues(char (*)[50],char (*)[50],int ,int ,char *,char *);
void fieldsconvert(char fieldname[MAXBIND][50],char todatefieldbuf[20][50],
                   int todatefieldnum,int fieldnum,char *fields,char *values);
int getfieldname(char *fieldnamebuf,char fieldname[MAXBIND][50]);
void trim(char *s);
int processfile(const char *inputfile,const char *errorpath,int fieldnum,
                int &total,int &success,int &error);
int dbtofile(const char *outputfile,int fieldnum,int &total);
int writelog(const char *msg);
int writealarm(const char *level,const char *msg);
int writestat(const char stat);
extern "C" void  userterm(int);
char *truepath(const char *path);

int term=0;
char DataFileName[200];
char NotinPathFileName[200];
char NoPathFileName[50];

char val[10][50];
char Val[10][40];
char Sym[10][10];
char PName[500];
int sum;

char Temp[10][40];
char insertarray[3000];
char selectarray[2000];
char selectsql[2000]="";
char sqlfieldnum[10]="";
DbInterface cdbinterface;
int nTotal,nSuccess,nError;

#ifndef NOALARM
CLog logalert;
CAlert alert;
CState state;
ApiStatStruct statstru;
#endif

char msgbuf[200];
char spacemark[10]=",";

//-----------------------------------------------------------------------------
int main(int argc,char **argv)
{
    SimpleConfigure cconfig;
    char configfile[100]="";
    char username[50]="";
    char password[50]="";
    char server[50]="";
    char dbname[50]="";
    char action[10]="";
    char table[50]="";
    char procedure[100]="";
    char flagofexeproc[10]="";
    char truncate[10]="";
    char tablefield[2000]="";
    char sourcepath[100]="";
    char notinpath[100]="";
    char errorpath[100]="";
    char targetpath[100]="";
    char outputfile[100]="";
    char channelnum[10]="";
    char fields[2000]="";
    char values[2000]="";
    char sqlcmd[2000]="";
    char fieldlength[10]="";
    char arrayline[10]="";
    char fieldname[MAXBIND][50];
    char todatefieldbuf[20][50];
    char ToDateFieldBuf[200]="";
    int fieldnum=0;
    int todatefieldnum=0;
    int ret=0;
    int opt=0;
    int outerr=0;
    int total=0;
    int success=0;
    int error=0;
    PROCPARAM pp[3];
    
    signal(SIGINT,SIG_IGN);
    signal(SIGTERM,userterm);

#ifndef NOALARM
    if (!logalert.Init( ))
        printf("logalert init error\n");
    if (!alert.Init( ))
        printf("alert init error\n");;
    if (!state.Init( ))
        printf("state init error\n");
#endif

    sprintf(configfile,"%s.cfg",argv[0]);
    while((opt=getopt(argc,argv,":f:"))!=-1)
    {
        switch(opt)
        {
            case 'f':
                strcpy(configfile,optarg);
                break;
            case '?':
                fprintf(stderr,USAGE,argv[0]);
                exit(-1);
        }
    }
    
    if(cconfig.initialize(configfile))
    {
        printf("cconfig.initialize() failed.\n");
        return -1;
    }
    if(cconfig.getval("DB","USER",username,50))
    {
        printf("USERNAME not found in config file.\n");
        return -1;
    }
    if(cconfig.getval("DB","PASSWD",password,50))
    {
        printf("PASSWD not found in config file.\n");
        return -1;
    }
    if(cconfig.getval("DB","SERVER",server,50))
    {
        printf("SERVER not found in config file.\n");
        return -1;
    }
    if(cconfig.getval("GENERAL1","ACTION",action,10))
    {
        printf("ACTION not found in config file.\n");
        return -1;
    }
    if(strcmp(action,"in") == 0)
    {
        if(cconfig.getval("GENERAL1","SOURCEPATH",sourcepath,100))
        {
            printf("SOURCEPATH not found in config file.\n");
            return -1;
        }
        strcpy(sourcepath,truepath(sourcepath));
        if(cconfig.getval("GENERAL1","NOTINPATH",notinpath,100))
        {
            printf("NOTINPATH not found in config file.\n");
            return -1;
        }
        strcpy(notinpath,truepath(notinpath));
        if(cconfig.getval("GENERAL1","ERRORPATH",errorpath,100))
        {
            printf("ERRORPATH not found in config file.\n");
            return -1;
        }
        strcpy(errorpath,truepath(errorpath));
    }
    if(strcmp(action,"out")==0)
    {
        if(cconfig.getval("GENERAL1","TARGETPATH",targetpath,100))
        {
            printf("TARGETPATH not found in config file.\n");
            return -1;
        }
        strcpy(targetpath,truepath(targetpath));
        if(cconfig.getval("GENERAL1","OUTPUTFILE",outputfile,100))
        {
            printf("OUTPUTFILE not found in config file.\n");
            return -1;
        }
        cconfig.getval("GENERAL1","SELECTSQL",selectsql,2000);
        cconfig.getval("GENERAL1","SQLFIELDNUM",sqlfieldnum,10);
    }
    if(cconfig.getval("GENERAL1","DATABASE",dbname,50))
    {
        printf("DATABASE not found in config file.\n");
        return -1;
    }
    if(cconfig.getval("GENERAL1","TABLE",table,50))
    {
        printf("TABLE not found in config file.\n");
        return -1;
    }
    if(cconfig.getval("GENERAL1","PROCEDURE",procedure,100))
    {
        printf("PROCEDURE not found in config file.\n");
        return -1;
    }
    if(cconfig.getval("GENERAL1","FLAGOFEXEPROC",flagofexeproc,10))
    {
        printf("FLAGOFEXEPROC not found in config file.\n");
        return -1;
    }
    if(cconfig.getval("GENERAL1","TRUNCATE",truncate,10))
    {
        printf("TRUNCATE not found in config file.\n");
        return -1;
    }
    if(cconfig.getval("GENERAL1","TABLEFIELD",tablefield,2000))
    {
        printf("TABLEFIELD not found in config file.\n");
        return -1;
    }
    if(cconfig.getval("GENERAL1","DATEFIELD",ToDateFieldBuf,200))
    {
        printf("DATEFIELD not found in config file.\n");
        //return -1;
    }
    if(cconfig.getval("GENERAL1","CHANNELNUM",channelnum,10))
    {
        printf("CHANNELNUM not found in config file.\n");
        //return -1;
    }
    if(cconfig.getval("GENERAL1","SPACEMARK",spacemark,10))
    {
        printf("SPACEMARK not found in config file.\n");
        //return -1;
    }
    if(cconfig.getval("GENERAL1","FIELDLENGTH",fieldlength,10))
    {
        printf("FIELDLENGTH not found in config file.\n");
        //return -1;
    }
    if(cconfig.getval("GENERAL1","ARRAYLINE",arrayline,10))
    {
        printf("ARRAYLINE not found in config file.\n");
        //return -1;
    }
    if(cconfig.end())
    {
        printf("cconfig.end() failed.\n");
        return -1;
    }
    printf("read config file:\n");
    printf("[username]\t%s\n",username);
    printf("[password]\t%s\n",password);
    printf("[server]\t%s\n",server);
    printf("[sourcepath]\t%s\n",sourcepath);
    printf("[notinpath]\t%s\n",notinpath);
    printf("[errorpath]\t%s\n",errorpath);
    printf("[targetpath]\t%s\n",targetpath);
    printf("[outputfile]\t%s\n",outputfile);
    printf("[selectsql]\t%s\n",selectsql);
    printf("[sqlfieldnum]\t%s\n",sqlfieldnum);
    printf("[action]\t%s\n",action);
    printf("[dbname]\t%s\n",dbname);
    printf("[table]\t\t%s\n",table);
    printf("[tablefield]\t%s\n",tablefield);
    printf("[procedure]\t%s\n",procedure);
    printf("[flagofexeproc]\t%s\n",flagofexeproc);
    printf("[truncate]\t%s\n",truncate);
    printf("[channelnum]\t%s\n",channelnum);
    printf("[spacemark]\t%s\n",spacemark);
    printf("[fieldlength]\t<%s>\n",fieldlength);
    printf("[arrayline]\t<%s>\n",arrayline);

#ifndef NOALARM
    statstru.sys_number = SYS_NUMBER;
    statstru.instance = atoi(channelnum);
    strcpy(statstru.alias,argv[0]);
    statstru.status = PROC_RUN;
    strcpy(statstru.rem,"bulk copy program");
    char cwd[200];
    getcwd(cwd,200);
    strcpy(statstru.exec_path,cwd);
    //  strcpy(statstru.exec_path,getenv("PWD"));
    strcpy(statstru.exec_name,argv[0]);
    strcpy(statstru.exec_arg,"");
    strcpy(statstru.reserve,"");
    statstru.pid = getpid();
#endif
    if(strlen(fieldlength)>0)
        cdbinterface.setFieldLength(atoi(fieldlength));
    if(strlen(arrayline)>0)
        cdbinterface.setArrayLine(atoi(arrayline));
    
    fieldnum = getfieldname(tablefield,fieldname);
    todatefieldnum = getfieldname(ToDateFieldBuf,todatefieldbuf);
    if(strcmp(action,"in")==0)
        fieldsvalues(fieldname,todatefieldbuf,todatefieldnum,fieldnum,fields,values);
    if(strcmp(action,"out")==0)
        fieldsconvert(fieldname,todatefieldbuf,todatefieldnum,fieldnum,fields,values);

    if(cdbinterface.connect(username,password,server))
    {
        printf("cdbinterface.connect() failed.\n");
        return -1;
    }

    if(strlen(dbname)>0)
    {
        if(cdbinterface.useDb(dbname))
        {
            printf("cdbinterface.useDb() failed.\n");
            return -1;
        }
    }

    fflush(stdout);

    if(strcmp(action,"in")==0)
    {
        //sprintf(insertarray,"INSERT INTO %s(%s)",table,fields);
        sprintf(insertarray,"INSERT INTO %s(%s) VALUES(%s)",table,fields,values);
        printf("values: %s\n",values);
        while(!term)
        {
            ret=ftw(sourcepath,FindFile,1);
            if(ret==-1)
            {
#ifndef NOALARM
                if(statstru.status!=PROC_IDLE)
                {
                    strcpy(statstru.rem,"bulk copy program");
                    writestat(PROC_IDLE);
                }
#endif
                sleep(1);
                continue;
            }
            if(DataFileName[0]=='\0')
            {
#ifndef NOALARM
                if(statstru.status!=PROC_IDLE)
                {
                    strcpy(statstru.rem,"bulk copy program");
                    writestat(PROC_IDLE);
                }
#endif
                sleep(1);
                continue;
            }
            total=0;
            success=0;
            error=0;
            ret=0;
            outerr=0;
#ifndef NOALARM
            strcpy(statstru.rem,DataFileName);
            writestat(PROC_RUN);
#endif

            if(strcmp(truncate,"yes")==0)
            {
                sprintf(sqlcmd,"TRUNCATE TABLE %s",table);
                if(cdbinterface.executeSql(sqlcmd,nTotal,nSuccess,nError))
                {
                    sprintf(msgbuf,"cdbinterface.executeSql(%s) failes",sqlcmd);
                    writealarm(ALARM_SERIOUS,msgbuf);
                    return -1;
                }
            }
            if(processfile(DataFileName,errorpath,fieldnum,total,success,error))
            {
                sprintf(msgbuf,"process file %s failed",DataFileName);
                writealarm(ALARM_FATAL,msgbuf);
                return -1;
            }

            sprintf(msgbuf,"process file %s total:%d success:%d error:%d",
                        DataFileName,total,success,error);
            writelog(msgbuf);
            if(strcmp(flagofexeproc,"EXE")==0)
            {
                if(ProcessProc(procedure))
                {
                    writealarm(ALARM_SERIOUS,"procedure not in correct pattern");
#ifndef NOALARM
                    writestat(PROC_ABORT);
#endif
                    return -1;
                }

                //strcpy(pp[0].name,Val[0]);
                pp[0].action=PARAM_ACT_IN;
                pp[0].type=PARAM_TYPE_STR;
                strcpy(pp[0].strValue,NoPathFileName);
                //strcpy(pp[1].name,Val[1]);
                pp[1].action=PARAM_ACT_OUT;
                pp[1].type=PARAM_TYPE_INT;
                pp[1].intValue=0;
                pp[2].action=PARAM_ACT_OUT;
                pp[2].type=PARAM_TYPE_STR;
                strcpy(pp[2].strValue,"");

                ret=cdbinterface.callProcedure(PName,3,pp);
                outerr=pp[1].intValue;
                printf("return value:%d errno=%d, errtext=%s\n",ret,outerr,pp[2].strValue);
            }
            if(ret||outerr)
            {
                sprintf(NotinPathFileName,"%s/%s",notinpath,NoPathFileName);
                if(rename(DataFileName,NotinPathFileName))
                {
                    sprintf(msgbuf,"rename %s to %s failed.\n",DataFileName,NotinPathFileName);
                    writealarm(ALARM_SERIOUS,msgbuf);
#ifndef NOALARM
                    writestat(PROC_ABORT);
#endif
                    return -1;
                }
            }
            else
            {
                if(remove(DataFileName))
                {
                    sprintf(msgbuf,"remove %s failed.\n",DataFileName);
                    writealarm(ALARM_SERIOUS,msgbuf);
#ifndef NOALARM
                    writestat(PROC_ABORT);
#endif
                    return -1;
                }
            }
        }
        writealarm(ALARM_KILLED,"program be killed by user");
#ifndef NOALARM
        strcpy(statstru.rem,"");
        statstru.pid = 0;
        writestat(PROC_EXIT);
#endif
    }
    if(strcmp(action,"out")==0)
    {
        sprintf(selectarray,"SELECT %s FROM %s",fields,table);
        if(strcmp(flagofexeproc,"EXE")==0)
        {
            if(ProcessProc(procedure))
            {
                writealarm(ALARM_SERIOUS,"procedure not in correct pattern");
#ifndef NOALARM
                writestat(PROC_ABORT);
#endif
                return -1;
            }

            //strcpy(pp[0].name,Val[0]);
            pp[0].action=PARAM_ACT_IN;
            pp[0].type=PARAM_TYPE_STR;
            strcpy(pp[0].strValue,NoPathFileName);
            //strcpy(pp[1].name,Val[1]);
            pp[1].action=PARAM_ACT_OUT;
            pp[1].type=PARAM_TYPE_INT;
            pp[1].intValue=0;

            ret=cdbinterface.callProcedure(PName,2,pp);
            outerr=pp[1].intValue;
            printf("return value:%d out param %d\n",ret,outerr);
            if(ret||outerr)
            {
                writealarm(ALARM_SERIOUS,"procedure call error");
#ifndef NOALARM
                writestat(PROC_ABORT);
#endif
                return -1;
            }
        }
        sprintf(DataFileName,"%s/%s",targetpath,outputfile);
        if(strlen(sqlfieldnum)>0)
            fieldnum=atoi(sqlfieldnum);
        ret=dbtofile(DataFileName,fieldnum,total);
        if(ret!=0)
        {
            writealarm(ALARM_SERIOUS,"copy out failed.\n");
        }
        else
        {
            sprintf(msgbuf,"output file %s total:%d",outputfile,total);
            writelog(msgbuf);
            if(strcmp(truncate,"yes")==0)
            {
                sprintf(sqlcmd,"TRUNCATE TABLE %s",table);
                ret=cdbinterface.executeSql(sqlcmd,nTotal,nSuccess,nError);
                if(ret!=0)
                {
                    sprintf(msgbuf,"cdbinterface.executeSql(%s) failes.\n",sqlcmd);
                    writealarm(ALARM_SERIOUS,msgbuf);
                }
            }
        }
        cdbinterface.disconnect();
    }
    return ret;
}


//-----------------------------------------------------------------------------
int dbtofile(const char *outputfile,int fieldnum,int &total)
{
    StringVector outvector;
    StringVector errvector;
    StringVectorIterator it;
    FILE *hOutputFile;
    int i;

    if(cdbinterface.dataBind(outvector,errvector))
    {
        writealarm(ALARM_SERIOUS,"cdbinterface.dataBind() failed");
        return -1;
    }
    total=0;
    nTotal=DEFAULTARRAYLINE;
    if((hOutputFile=fopen(outputfile,"wb"))==NULL)
    {
        sprintf(msgbuf,"outputfile %s open failed",outputfile);
        writealarm(ALARM_SERIOUS,msgbuf);
        return -1;
    }
    while(nTotal==DEFAULTARRAYLINE)
    {
        if(strlen(selectsql)>0)
        {
            if(cdbinterface.executeSql(selectsql,nTotal,nSuccess,nError))
            {
                sprintf(msgbuf,"cdbinterface.executeSql() failed. errormessage:%s",
                        cdbinterface.errorMessage());
                //writealarm(ALARM_SERIOUS,"cdbinterface.executeSql() failed");
                writealarm(ALARM_SERIOUS,msgbuf);
                if(hOutputFile)fclose(hOutputFile);
                remove(outputfile);
                return -1;
            }
        }
        else
        {
            if(cdbinterface.bulkCopy(selectarray,nTotal,nSuccess,nError))
            {
                writealarm(ALARM_SERIOUS,"cdbinterface.bulkCopy() failed");
                if(hOutputFile)fclose(hOutputFile);
                remove(outputfile);
                return -1;
            }
        }
        if(nTotal>0)
        {
            total+=nTotal;
            for(it=outvector.begin(),i=0;it!=outvector.end();it++)
            {
                i++;
                fprintf(hOutputFile,"%s",(*it).c_str());
                if(i<fieldnum)
                    //fprintf(hOutputFile,",");
                    fprintf(hOutputFile,spacemark);
                if(i==fieldnum)
                {
                    fprintf(hOutputFile,"\n");
                    i=0;
                 }
            }
        }
    }
    if(hOutputFile)fclose(hOutputFile);
    return 0;
}

//-----------------------------------------------------------------------------
int processfile(const char *inputfile,const char *errorpath,int fieldnum,
                int &total,int &success,int &error)
{
    StringVector invector;
    StringVector errvector;
    StringVectorIterator it;
    FILE *hInputFile=NULL;
    FILE *hBadFile=NULL;
    char badfile[200];
    char buf[500];
    char field[100][1500];
    int recordindex=0;
    int totalrecord=0;
    int badrecord=0;
    int charindex;
    int fieldindex;
    int fieldcharindex;
    int i;

    cdbinterface.dataBind(invector,errvector);
    sprintf(badfile,"%s/%s%s",errorpath,"E",NoPathFileName);

    if((hInputFile=fopen(inputfile,"rb"))==NULL)
    {
        sprintf(msgbuf,"inputfile %s open failed",inputfile);
        writealarm(ALARM_SERIOUS,msgbuf);
        return -1;
    }

    while(1)
    {
        if((fgets(buf,500,hInputFile))==NULL)break;
        totalrecord++;
        for(fieldindex=0,charindex=0,fieldcharindex=0;
            fieldindex<fieldnum;fieldindex++)
        {
            while(buf[charindex]!=spacemark[0]&&buf[charindex]!='\n'&&buf[charindex]!='\r')
            {
                field[fieldindex][fieldcharindex]=buf[charindex];
                charindex++;
                fieldcharindex++;
            }
            field[fieldindex][fieldcharindex]='\0';
            if(buf[charindex]=='\n'||buf[charindex]=='\r')break;
            charindex++;
            fieldcharindex=0;
        }
        fieldindex++;
        if(fieldindex>=fieldnum)
        {
            recordindex++;
            for(i=0;i<fieldnum;i++)
            {
                invector.push_back(field[i]);
            }
        }
        else
        {
            badrecord++;
            if(hBadFile)
            {
                fprintf(hBadFile,"%s",buf);
            }
            else
            {
                if((hBadFile=fopen(badfile,"wb"))==NULL)
                {
                    sprintf(msgbuf,"badfile %s open failed",badfile);
                    writealarm(ALARM_SERIOUS,msgbuf);
                    if(hInputFile)fclose(hInputFile);
                    return -1;
                }
                fprintf(hBadFile,"%s",buf);
            }
        }

        if(recordindex==DEFAULTARRAYLINE)
        {
            if(cdbinterface.bulkCopy(insertarray,nTotal,nSuccess,nError))
            {
                writealarm(ALARM_SERIOUS,"cdbinterface.bulkCopy() failed");
                if(hInputFile)fclose(hInputFile);
                if(hBadFile)fclose(hBadFile);
                return -1;
            }
            if(nError)
            {
                for(it=errvector.begin(),i=0;it!=errvector.end();it++)
                {
                    i++;
                    if(hBadFile)
                    {
                        fprintf(hBadFile,"%s",(*it).c_str());
                    }
                    else
                    {
                        if((hBadFile=fopen(badfile,"wb"))==NULL)
                        {
                            sprintf(msgbuf,"badfile %s open failed.\n",badfile);
                            writealarm(ALARM_SERIOUS,msgbuf);
                            if(hInputFile)fclose(hInputFile);
                            return -1;
                         }
                         fprintf(hBadFile,"%s",(*it).c_str());
                    }
                    if(i<fieldnum)fprintf(hBadFile,",");
                    if(i==fieldnum){fprintf(hBadFile,"\n");i=0;}
                }
            }
            total+=nTotal;
            success+=nSuccess;
            error+=nError;
            invector.clear();
            errvector.clear();
            recordindex=0;
            nTotal=0;
            nSuccess=0;
            nError=0;
        }
    }
    if(recordindex)
    {
        printf("insertarray=%s, recordindex=%d , size=%d\n",insertarray,recordindex,invector.size());
        if(cdbinterface.bulkCopy(insertarray,nTotal,nSuccess,nError))
        {
            writealarm(ALARM_SERIOUS,"cdbinterface.bulkCopy() failed");
            if(hInputFile)
                fclose(hInputFile);
            if(hBadFile)fclose(hBadFile);
            return -1;
        }
        if(nError)
        {
            for(it=errvector.begin(),i=0;it!=errvector.end();it++)
            {
                i++;
                if(hBadFile)
                {
                    fprintf(hBadFile,"%s",(*it).c_str());
                }
                else
                {
                    if((hBadFile=fopen(badfile,"wb"))==NULL)
                    {
                        sprintf(msgbuf,"badfile %s open failed",badfile);
                        writealarm(ALARM_SERIOUS,msgbuf);
                        if(hInputFile)fclose(hInputFile);
                        return -1;
                    }
                    fprintf(hBadFile,"%s",(*it).c_str());
                }
                if(i<fieldnum)fprintf(hBadFile,",");
                if(i==fieldnum)
                {
                    fprintf(hBadFile,"\n");i=0;
                }
            }
        }
        total+=nTotal;
        success+=nSuccess;
        error+=nError;
        invector.clear();
        errvector.clear();
        recordindex=0;
        nTotal=0;
        nSuccess=0;
        nError=0;
    }
    if(hInputFile)
        fclose(hInputFile);
    if(hBadFile)
        fclose(hBadFile);
    return 0;
}

//-----------------------------------------------------------------------------
int FindFile(const char *name, const struct stat *status, int type)
{
    if (type == FTW_F)
    {
        strcpy(DataFileName,name);
        strcpy(NoPathFileName,strrchr(DataFileName,'/')+1);
        if(!strncmp("!",NoPathFileName,1))
        {
            strcpy(DataFileName,"\0");
            strcpy(NoPathFileName,"\0");
            return 0;
        }
        else
            return 1;
    }
    else
    {
        strcpy(DataFileName,"\0");
        strcpy(NoPathFileName,"\0");
        return 0;
    }
}

//-----------------------------------------------------------------------------
//p_double(:filename$input,:return$output)
int ProcessProc(char * ProName)
{
    if(ProName==NULL)
        return -1;

    int len=strlen(ProName);
    int i,h,g,loc,j[10];

    i=0;
    for(h=0;;h++)
    {
        for(;i<len;i++)
        {  
            if(ProName[i]=='(')
            {
                j[h]=i;
                break;
            }
            else if(ProName[i]==',')
            {
                j[h]=i;
                break;
            }
            else if(ProName[i]==')')
            {
                j[h]=i;
                break;
            }
        }
        if(ProName[i]==')')
            break;
        i++;
    }

    sum=++h;

    if(j[0]==0)
        return -1;

    for(i=0;i<j[0];i++)
        PName[i]=ProName[i];

    for(i=0;i<sum-1;i++)
    {
        if(j[i]+1==j[i+1])
        return -1;

        for(h=0,g=j[i]+1;g<j[i+1];h++,g++)
            val[i][h]=ProName[g];
    }


    for(i=0;i<sum-1;i++)
    {
        len=strlen(val[i]);
        loc=len-strlen(strchr(val[i],'$'));
        if((loc==0)||(loc==len-1))
            return -1;

        for(h=0;h<loc;h++)
        Val[i][h]=val[i][h];

        for(g=0,h=loc+1;h<len;g++,h++)
            Sym[i][g]=val[i][h];
    }
    return 0;
}

//-----------------------------------------------------------------------------
void fieldsvalues(char fieldname[MAXBIND][50],char todatefieldbuf[20][50],
                  int todatefieldnum,int fieldnum,char *fields,char *values)
{
    int i,j,dateflag=0;
    for(i=0;i<fieldnum;i++)
    {
        for(j=0;j<todatefieldnum;j++)
        {
            if(strcmp(fieldname[i],todatefieldbuf[j])==0)
                dateflag=1;
        }

        if(dateflag==0)
        {
            if(i==0)
            {
                strcpy(fields,fieldname[i]);
                sprintf(values,":%s",fieldname[i]);
            }
            else
            {
                sprintf(fields,"%s,%s",fields,fieldname[i]);
                sprintf(values,"%s,:%s",values,fieldname[i]);
            }
        }
        else
        {
            if(i==0)
            {
                sprintf(values,"to_date(:%s,'yyyymmdd HH24:MI:SS')",fieldname[i]);
                sprintf(fields,"%s",fieldname[i]);
            }
            else
            {
                sprintf(values,"%s,to_date(:%s,'yyyymmdd HH24:MI:SS')",values,fieldname[i]);
                sprintf(fields,"%s,%s",fields,fieldname[i]);
            }
            dateflag=0;
        }
    }
}


//-----------------------------------------------------------------------------
void fieldsconvert(char fieldname[MAXBIND][50],char todatefieldbuf[20][50],
                   int todatefieldnum,int fieldnum,char *fields,char *values)
{
    int i,j,dateflag=0;
    for(i=0;i<fieldnum;i++)
    {
        for(j=0;j<todatefieldnum;j++)
        {
            if(strcmp(fieldname[i],todatefieldbuf[j])==0)
                dateflag=1;
        }

        if(dateflag==0)
        {
            if(i==0)
            {
                strcpy(fields,fieldname[i]);
                //sprintf(values,":%s",fieldname[i]);
            }
            else
            {
                sprintf(fields,"%s,%s",fields,fieldname[i]);
                //sprintf(values,"%s,:%s",values,fieldname[i]);
            }
        }
        else
        {
            if(i==0)
            {
                //sprintf(values,"to_date(%s,'yyyymmdd HH24:MI:SS')",fieldname[i]);
                sprintf(fields,"to_char(%s,'yyyymmdd HH24:MI:SS')",fieldname[i],i+1);
                //sprintf(fields,"%s",fieldname[i]);
            }
            else
            {
                //sprintf(values,"%s,to_date(%s,'yyyymmdd HH24:MI:SS')",values,fieldname[i]);
                sprintf(fields,"%s,to_char(%s,'yyyymmdd HH24:MI:SS')",fields,fieldname[i]);
                //sprintf(fields,"%s,%s",fields,fieldname[i]);
            }
            dateflag=0;
        }
    }
}


//-----------------------------------------------------------------------------
int getfieldname(char *fieldnamebuf,char fieldname[MAXBIND][50])
{
    int charindex,fieldindex,i;
    for(i=0,charindex=0,fieldindex=0;;i++,fieldindex++)
    {
        while((fieldnamebuf[i]!=',')&&(fieldnamebuf[i]!='\0'))
        {
            fieldname[fieldindex][charindex] = fieldnamebuf[i];
            i++;
            charindex++;
        }
        fieldname[fieldindex][charindex] = '\0';
        charindex = 0;
        trim(fieldname[fieldindex]);
        if(fieldnamebuf[i]=='\0' )
            break;
    }
    return ++fieldindex;
}

//-----------------------------------------------------------------------------
void trim(char *s)
{
    int l,i,k;
    l=strlen(s);
    for(i=l-1;i>=0;i--)
    {
        if(isspace(s[i]))
            s[i]=0;
        else
            break;
    }
    l=strlen(s);
    for(i=0;i<l;i++)
    {
        if(isspace(s[i]))
            continue;
        else
            break;
    }
    if(i!=0)
    {
        for(k=0;k<l-i;k++)
        {
            s[k]=s[k+i];
        }
        s[k]=0;
    }
}


//-----------------------------------------------------------------------------
int writelog(const char *msg)
{
    char logmsg[200];
    char sNowTime[20];
    time_t lNowTime;
    struct tm *stNowTime;
    
    time(&lNowTime);
    stNowTime=localtime(&lNowTime);
    strftime(sNowTime,20,"%Y-%m-%d %H:%M:%S",stNowTime);
    
    sprintf(logmsg,"<%s> %s",sNowTime,msg);
#ifndef NOALARM
    if (!logalert.Put(SYS_NUMBER,logmsg))
        fprintf(stderr,"error Api_Putlogalert!\n");
#endif
    fprintf(stdout,"%s\n",logmsg);
    fflush(stdout);
    return 0;
}

//-----------------------------------------------------------------------------
int writealarm(const char *level,const char *msg)
{
    char alarmmsg[200];
    char hostname[20]="";
    char sNowTime[20];
    time_t lNowTime;
    struct tm *stNowTime;
    
    time(&lNowTime);
    stNowTime=localtime(&lNowTime);
    strftime(sNowTime,15,"%Y%m%d%H%M%S",stNowTime);
    
    gethostname(hostname,20);
    sprintf(alarmmsg,"%s@RK%s@%s@%s",hostname,level,sNowTime,msg);
#ifndef NOALARM
    if (!alert.Put(alarmmsg))
        fprintf(stderr,"error Api_Putalert!\n");
#endif
    fprintf(stderr,"%s\n",alarmmsg);
    fflush(stderr);
    return 0;
}

//-----------------------------------------------------------------------------
int writestat(const char stat)
{
#ifndef NOALARM
    statstru.status = stat;
    if (!state.Put(&statstru))fprintf(stderr,"state error!\n");
#endif
    return 0;
}

//-----------------------------------------------------------------------------
void userterm(int a)
{
    char logmsg[100];
    char sNowTime[20];
    time_t lNowTime;
    struct tm *stNowTime;
    
    time(&lNowTime);
    stNowTime=localtime(&lNowTime);
    strftime(sNowTime,20,"%Y-%m-%d %H:%M:%S",stNowTime);
    
    signal(SIGINT,SIG_IGN);
    signal(SIGTERM,userterm);
    fprintf(stderr,"\n<%s>  process is working now,please wait a while.\n",sNowTime);
    fflush(stderr);
    term=1;
}


//-----------------------------------------------------------------------------
char *truepath(const char *path)
{
    static char truepath[500];
    char env[100];
    char *c;
    
    if(*path=='~')
    {
        strcpy(truepath,getenv("HOME"));
        strcat(truepath,path+1);
    }
    else if(*path=='$')
    {
        c=(char *)strchr(path,'/');
        if(c==NULL)
        {
            strcpy(truepath,getenv(path+1));
        }
        else
        {
            strncpy(env,path+1,c-path);
            env[c-path-1]=0;
            strcpy(truepath,getenv(env));
            strcat(truepath,c);
        }
    }
    else
    {
        strcpy(truepath,path);
    }

    return truepath;
}



//---------------------------------- THE END ----------------------------------

