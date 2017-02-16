/******************************************************************
 *Copyright (c) 2002-2003 南京联创公司联通软件开发中心
 *创建人: 陈忠敏
 *日　期: 2006.11.7
 *描　述:
 *版　本: 1.0
 *****************************************************************/

#include "daetools.h"

string GetEnv(const char * envstr)
{
	string strEnv = "";
	
	char*ptr = getenv(envstr);
	if (ptr == NULL)
		return strEnv;
	else
	{
		strEnv = ptr;
		return strEnv;
	}			
}

string GetHostDate(const char *format)
{
	//format:"%Y%m%d%H%M%S","%Y%m%d","%Y-%m-%d %H:%M:%S"
	string strDate="";
	tzset();

	time_t secs_now;
	time(&secs_now);
	
	struct tm *time_now = localtime(&secs_now);
	char vv_localtime[40]="\0";
	memset(vv_localtime,0,40);
	//strftime(vv_localtime,15,"%Y%m%d%H%M%S",time_now);
	strftime(vv_localtime,40,format,time_now);
	strDate=vv_localtime;
	return strDate;
}

int muserlog(const char * formate,...)
{
    int ret;
	
    FILE* fp=fopen(LogFileName.c_str(),"a+");
    if (fp == NULL)
        return -1;

    va_list marker;
    va_start( marker,formate);
    char *c_formate=new char[strlen(formate)+3];
    memset(c_formate,0,strlen(formate)+3);
    strcpy(c_formate,formate);
    strcat(c_formate,"\n");
    
    ret=vfprintf(fp,(const char *)c_formate,marker);
    if (ret==EOF)
    {
        va_end( marker );
        fclose(fp);
        delete []c_formate;
        return -1;
    }
    va_end( marker );
    fclose(fp);
    delete []c_formate;
    return 0;
}
int strinlist(const char *srcstr,...)
{
    va_list marker;
    va_start( marker,srcstr);
	char *tmpptr=NULL;
	while(1){
		tmpptr=va_arg( marker,char *);
		if (tmpptr==NULL)
			break;
		if (strcmp(srcstr,tmpptr)==0){
			va_end( marker );
			return 1;
		}
	}
    va_end( marker );
	return 0;
}
int GetSubStr(char* SourceString,int cIndex,char SplitChar,char* cDest)
{
    int SplitNum=0;
    int StartPos,EndPos;
    int i;
    char *lString=(char *)malloc(strlen(SourceString)+2);
    strcpy(lString,SourceString);
    if (lString[strlen(lString) - 1]!=SplitChar)
        strcat(lString,&SplitChar);
    if (cIndex<=0)
    {
    	free(lString);
        return -1;
    }

    for (i=0;i<(int)strlen(lString);i++){
        if (lString[i]==SplitChar){
            SplitNum ++;
            if (SplitNum==cIndex - 1)
                StartPos=i;
            if (SplitNum==cIndex){
                EndPos=i;
                if (cIndex==1)
                    StartPos=-1;
                break;
            }
        }
    }
    if (SplitNum<cIndex){
        free(lString);
        return -1;
    }
    else{
        memcpy(cDest,lString + StartPos + 1,EndPos - StartPos - 1);
        free(lString);
        return EndPos - StartPos - 1;
    }
}
/************************************
	守护进程声明函数
************************************/
void daemon_init(const char * pname,int facility)
{
    
	int i,MAXFD;
	pid_t pid;
	char str[10];
	
	/********************************************************************
		第一次fork父亲进程退出shell会认为命令结束 
		子进程自动变成后台程序继承了父进程的组号，
		拥有自己的进程号 保证不是组头
	********************************************************************/
	if (pid=fork())	
	{
		exit(0);
	}
	/*************************************************
		创建新的会话 使该进程成为会话头和新进程组长
	*************************************************/
	setsid();
	signal(SIGHUP,SIG_IGN);//忽略SIGHUP信号
	
	/************************************
		第二次fork，确保不会获得终端
		这次生成的进程不再是会话头
	************************************/
	if (pid=fork()) 
	{
		exit(0);
	}
	char path[256];
	memset(path,0,sizeof(256));
	getcwd(path,256);
	chdir(path);
	//chdir("/");//改变工作目录到 /  可以在任意文件系统中启动
	//umask(0);//创建文件不受原来的权限位影响
	
	//关闭所有打开的文件描述字 //重定向到/dev/null也可以
	MAXFD=sysconf(_SC_OPEN_MAX);
	for (i=0;i<MAXFD;i++ ) 
	{
		close(i);
	}
	//int lfp;
	//lfp=open(LOCK_FILE,O_RDWR|O_CREAT,0640); 
	//if (lfp<0) exit(1); /* can not open */ 
	//if (lockf(lfp,F_TLOCK,0)<0) exit(0); /* can not lock */ 
	///* first instance continues */ 
	//sprintf(str,"%d\n",getpid()); 
	//write(lfp,str,strlen(str)); /* record pid to lockfile */ 
}
void StrToUpper(char *str)
{
    int i;
    for (i=0;i<(int)strlen(str);i++)
            str[i]=toupper(str[i]);
}

void StrToLower(char *str)
{
    int i;
    for (i=0;i<(int)strlen(str);i++)
            str[i]=tolower(str[i]);
}
//空格:32,TAB:9,Enter:13,NewLine:10
char *ltrim(char *string)
{
        int  i,iLen;
        iLen=(int)strlen(string);

        char *strtemp=new char[iLen + 1];
        memset(strtemp,0,iLen + 1);
        
        char *strtmp=strtemp;
        
        strcpy(strtemp,string);
        for(i=0;i<iLen;i++ )
        {
		    if (string[i]==32 || string[i]==9 || string[i]==13 || string[i]==10)
            	strtemp ++;
            else
            	break;
        }
        strcpy(string,strtemp);
        delete [] strtmp;
        return string;
}
char * rtrim(char *string)
{
        int  i,iLen;
        iLen=(int)strlen(string);
        for(i=iLen - 1;i>=0;i--)
        {
        	if (string[i]==32 || string[i]==9 || string[i]==13 || string[i]==10)
        		string[i]='\0';
        	else
        		break;
        }
        return string;
}
int ReadIniFileInfo(const char *cFileName,
					const char *cSectionName,
					const char *cKeyName,
					string &strValue,
					string &strErrMsg)
{
	FILE *fp=fopen(cFileName,"r");
	if (fp == NULL){
		strErrMsg = string("配置文件:")+string(cFileName)+string("打开失败！");
		return -1;
	}
	char cLineStr[2048]="\0";
	while(1)
	{
		memset(cLineStr,0,sizeof(cLineStr));
		if (fgets(cLineStr,2048,fp) == NULL)
			break;
			
		//去除前后的空格，tab，回车，换行
		ltrim(cLineStr);rtrim(cLineStr);
		
		//忽略空行
		if (strlen(cLineStr) == 0)
			continue;

		//忽略注释:“#”或“;”开头的行
		if (cLineStr[0] == '#' || cLineStr[0] == ';')
			continue;

		if (cLineStr[0] != '[' || cLineStr[strlen(cLineStr) - 1] != ']')
			continue;
		
		//找到了对应的Section
		if (strncmp(cLineStr + 1,cSectionName,strlen(cSectionName)) == 0)
		{
			int iFound = 0;//是否找到对应的Key
			while(1)
			{
				memset(cLineStr,0,sizeof(cLineStr));
				if (fgets(cLineStr,2048,fp) == NULL)
					break;
				//去除前后的空格，tab，回车，换行
				ltrim(cLineStr);rtrim(cLineStr);
				
				//忽略空行
				if (strlen(cLineStr) == 0)
					continue;
		
				//忽略注释:“#”或“;”开头的行
				if (cLineStr[0] == '#' || cLineStr[0] == ';')
					continue;
		
				if (cLineStr[0] == '[' && cLineStr[strlen(cLineStr) - 1] == ']')
					break;

				char cTempKeyName[80]="\0";
				memset(cTempKeyName,0,sizeof(cTempKeyName));
				
				GetSubStr(cLineStr,1,'=',cTempKeyName);
				
				if (strcmp(cKeyName,cTempKeyName)!=0)
					continue;

				char cTempKeyValue[2048]="\0";
				memset(cTempKeyValue,0,sizeof(cTempKeyValue));
				
				//GetSubStr(cLineStr,2,'=',cTempKeyValue);
				strcpy(cTempKeyValue,strstr(cLineStr,"=") + 1);
				ltrim(cTempKeyValue);rtrim(cTempKeyValue);
				
				strValue = cTempKeyValue;
				
				iFound = 1;
				break;
			}
			if (iFound == 0)
			{
				fclose(fp);
				strErrMsg=string("ini文件未找到对应的值,Section=")+string(cSectionName)+string(",Key=")+string(cKeyName);
				return -1;
			}
			else
			{
				//muserlog("Section:%s,Key:%s,Value:%s",cSectionName,cKeyName,strValue.c_str());
				fclose(fp);
				return 0;
			}
		}
	}
	fclose(fp);
	return 0;
}
bool file_exists(const char * filename)
{
	return (access(filename,0) == 0 ? true:false);
}
long file_size(FILE *fp)
{
	long lCurrPos=0, lLength=0;
	//保存当前位置
	lCurrPos = ftell(fp);
	//移动指针到文件末尾
	fseek(fp, 0L, SEEK_END);
	//获取文件长度
	lLength = ftell(fp);
	//指针恢复到初始位置
	fseek(fp, lCurrPos, SEEK_SET);
	
	return lLength;
}
long file_size(const char *filename)
{
	if (!file_exists(filename))
		return -1;
	FILE *fp = fopen(filename,"r");
	if (fp == NULL)
		return -1;
	
	long lLength=0;

	fseek(fp, 0L, SEEK_END);

	lLength = ftell(fp);
	
	fclose(fp);
	
	return lLength;
}

/*

TuxCall::TuxCall(long lSendBufSize,long lRecvBufSize)
{
	errmsg.clear();
	sendBuf=(FBFR32 *)tpalloc((char*)"FML32",NULL,Fneeded((FLDOCC32)60,(FLDLEN32)lSendBufSize));
	recvBuf=(FBFR32 *)tpalloc((char*)"FML32",NULL,Fneeded((FLDOCC32)60,(FLDLEN32)lRecvBufSize));
	errorfieldid = 0;
}
TuxCall::~TuxCall()
{
	tpfree((char *)sendBuf);
	tpfree((char *)recvBuf);
}
int TuxCall::fchg32(FLDID32 fieldid,FLDOCC32 rownum,const string& value)
{
	long lneeded=Fneeded32(1,value.length()+1);
	if (Funused32(sendBuf)<lneeded)
	{
	    sendBuf = (FBFR32*)tprealloc((char *)sendBuf,Fsizeof32(sendBuf)+1024*64);
	}
	int ifldtype=Fldtype32(fieldid);
	long l_val=0;
	int ret = 0;
	if (ifldtype == FLD_STRING)
	{
		ret=Fchg32(sendBuf,fieldid,rownum,(char *)value.c_str(),0);
	}
	else if (ifldtype == FLD_LONG)
	{
		l_val = atol(value.c_str());
		ret=Fchg32(sendBuf,fieldid,rownum,(char *)&l_val,sizeof(long));
	}
	if (ret<0)
	{
		string str1,str2;
		stringstream ss;
		ss<<errorfieldid;
		ss>>str1;
		ss.clear(0);
		ss<<Ferror32;
		ss>>str2;
	    errorfieldid=fieldid;
	    errmsg=string("TuxCall::域(")+str1+string(")Fchg32失败:[")+str2+string("]")+string(Fstrerror32(Ferror32));
    }
	return ret;
}
int TuxCall::fget32(FLDID32 fieldid,FLDOCC32 rownum,string& value)
{
	value.clear();
	
	int ilen=Flen32(recvBuf,fieldid,rownum);
	char *tmpptr=new char[ilen + 1];
    memset(tmpptr,0,ilen + 1);

	int ifldtype=Fldtype32(fieldid);
	long l_val=0;
	int ret = 0;
	FLDLEN32 len;
	if (ifldtype == FLD_STRING)
	{
		len = 0;
		ret=Fget32(recvBuf,fieldid,rownum,tmpptr,&len);
	}
	else if (ifldtype == FLD_LONG)
	{
		len = sizeof(long);
		ret=Fget32(recvBuf,fieldid,rownum,(char *)&l_val,&len);
		sprintf(tmpptr,"%ld",l_val);
	}
	if (ret<0)
	{
		string str1,str2;
		stringstream ss;
		ss<<errorfieldid;
		ss>>str1;
		ss.clear(0);
		ss<<Ferror32;
		ss>>str2;
	    errorfieldid=fieldid;
	    errmsg=string("TuxCall::域(")+str1+string(")Fget32失败:[")+str2+string("]")+string(Fstrerror32(Ferror32));
	}
	else
		value=tmpptr;

	delete []tmpptr;
	return ret;
}
int TuxCall::fchg32(const char *fldname,FLDOCC32 rownum,const string& value)
{
	char FldName[128]="\0";
	memset(FldName,0,sizeof(FldName));
	strcpy(FldName,fldname);
	ltrim(FldName);rtrim(FldName);
	StrToUpper(FldName);

	FLDID32 fld=Fldid32((char *)FldName);
    if (fld == BADFLDID)
    {
        errmsg=string("TuxCall::fchg32根据域名获取域值失败(")+string(fldname)+string("):")+Fstrerror32(Ferror32);
        return -1;
    }
    return fchg32(fld,rownum,value);
}
int TuxCall::fget32(const char *fldname,FLDOCC32 rownum,string& value)
{
	char FldName[128]="\0";
	memset(FldName,0,sizeof(FldName));
	strcpy(FldName,fldname);
	ltrim(FldName);rtrim(FldName);
	StrToUpper(FldName);

	FLDID32 fld=Fldid32((char *)FldName);
    if (fld == BADFLDID)
    {
        errmsg=string("TuxCall::fget32根据域名获取域值失败(")+string(fldname)+string("):")+Fstrerror32(Ferror32);
        return -1;
    }
    return fget32(fld,rownum,value);
}
int TuxCall::callsvc(const char *svcname)
{
    if (tpinit((TPINIT *) NULL) == -1)
    {
        errmsg=string("TuxCall::无法建立和服务器的连接:") + string(tpstrerror(tperrno));
        return -1;
    }
	
	long len = 0;
	int ret = tpcall((char *)svcname,(char *)sendBuf,0,(char **)&recvBuf,&len,0);
	if (ret < 0)
	{
		stringstream ss;
		ss<<tperrno;
		string str;
		ss>>str;
		errmsg=string("TuxCall::调用服务(")+string(svcname)+string(")失败:[")+str+string("]")+string(tpstrerror(tperrno));
		tpterm();
		return ret;
	}
	tpterm();
	string strResultCode,strResultInfo;
	fget32("X_RESULTCODE",0,strResultCode);
	fget32("X_RESULTINFO",0,strResultInfo);
	if (strResultCode != "0")
	{
		errmsg = strResultInfo;
		return -1;
	}
	return 0;
}

int TuxCall::vartobuf(int rownum,...)
{
    int ret;
    const char *tmpptrin;
    const char *tmpptrname=NULL;
    va_list marker,markerin;
    va_start( marker,rownum);
    va_start( markerin,rownum);
    va_arg( markerin,const char *);
    string strValue;
    int HasError=0;
    while(1)
    {
    	 tmpptrname=NULL;
    	 tmpptrname=va_arg( marker,const char *);
         if (tmpptrname==NULL) break;
         
         tmpptrin=NULL;
         tmpptrin=va_arg(markerin,const char *);
         if (tmpptrin==NULL) break;
		 
		 //muserlog("%s\t%s",tmpptrname,tmpptrin);
		 
         strValue = tmpptrin;
         ret=fchg32(tmpptrname,(FLDOCC32)rownum,strValue);
         if (ret<0)
         {
         	HasError = 1;
         }
         va_arg( marker,const char *);
         va_arg(markerin,const char *);
    }
    va_end( marker );
    va_end( markerin );
    if (HasError == 1)
    	return -1;
    return 0;
}
void TuxCall::loginbuff(const char * servername,const char *logfilename)
{
	logbuff(servername,sendBuf,logfilename);
}
void TuxCall::logoutbuff(const char * servername,const char *logfilename)
{
	logbuff(servername,recvBuf,logfilename);
}
void TuxCall::logbuff(const char * servername,FBFR32 *rqst,const char *logname)
{
    //记录日志
	string vc_sysdate = GetHostDate("%Y-%m-%d %H:%M:%S");
    
    FILE* fp=fopen(logname,"a");
    if (fp==NULL)
    {
    	return;         
    }
    char tempstr[256]="\0";        
    sprintf(tempstr,"[时间：%s 服务名称：%s]\n",vc_sysdate.c_str(),servername);
    fwrite(tempstr,strlen(tempstr),1,fp);
    Ffprint32(rqst,fp);
    fclose(fp);               
}
*/
