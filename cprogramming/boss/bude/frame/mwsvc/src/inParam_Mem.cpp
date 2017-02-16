#include "inParam_Mem.h"

#if defined(HPUX_OS_CONFIG)
#define MEM_HPUX
	#include <unistd.h>
	#include <sys/param.h>
	#include <sys/pstat.h>
	void InParamMem::mem_check_rpt(pid_t pid)
	{
		struct pst_status pst;		
		memset((void*)&pst,0,sizeof(struct pst_status));		
		long pagesize = getpagesize();  
		string sCmd;
		//获取进程信息
		pstat_getproc(&pst,sizeof(pst),0,pid);
		uTsize = pst.pst_tsize*pagesize/1024;
		uDsize = pst.pst_dsize*pagesize/1024;
		uSsize = pst.pst_ssize*pagesize/1024;
		uVtsize = pst.pst_vtsize*pagesize/1024;
		uVdsize = pst.pst_vdsize*pagesize/1024;
		uVssize = pst.pst_vssize*pagesize/1024;	
		sCmd = pst.pst_cmd;
		sProcname = sCmd.substr(0,(sCmd.find("server") + 6));
		
	};
#elif defined(IBMAIX_OS_CONFIG)
#define MEM_IBMAIX
	#include <sys/procfs.h>
	void InParamMem::mem_check_rpt(pid_t pid)
	{
		FILE* fd = 0;
		char filename[50];	
		struct pstatus status;
		struct psinfo sinfo;
		
		//读取的文件名status
		sprintf(filename,"/proc/%ld/status",pid);
		fd = fopen(filename,"r");
		if (fd)
		{
			fread((char*)&status,sizeof(struct pstatus),1,fd);	
			iHeapsize = status.pr_brksize / 1000;
			iStacksize = 	status.pr_stksize / 1000;	
			fclose(fd);
			fd = 0;
		}
		
		//读取psinfo
		sprintf(filename,"/proc/%ld/psinfo",pid);
		fd = fopen(filename,"r");
		if (fd)
		{
			fread((char*)&sinfo,sizeof(struct psinfo),1,fd);		
			sUserMemsize = sinfo.pr_rssize ;
			sUserAllMemsize = sinfo.pr_size ;
			char cPrfname[PRFNSZ]="\0";
			memset(cPrfname,0,sizeof(cPrfname));
			strncpy(cPrfname,sinfo.pr_fname,PRFNSZ-1);
			sProcname = cPrfname;
			fclose(fd);
		}	
	}
#elif defined(SUN_OS_CONFIG)
#define MEM_SUN
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/syscall.h>
#include <sys/procfs.h>
#include <sys/param.h>
#include <fcntl.h>
	void InParamMem::mem_check_rpt(pid_t pid)
	{
	        int fd = 0;
	        char filename[50];
	        prstatus status;
	  			prpsinfo sinfo;
	
	        //读取的文件名status
	        sprintf( filename, "/proc/%-d", pid );
	        fd = open(filename,O_RDONLY);
	        
	        iHeapsize = -1;
	        iStacksize = -1;
	        sUserMemsize = -1;
	        sUserAllMemsize = -1;
	        sProcname = "Failed";
	        
	        if (fd < 0) return;
	        	
	        if ( ioctl( fd, PIOCPSINFO, &sinfo ) < 0 || ioctl( fd, PIOCSTATUS, &status ) < 0 ) return;	        
	        iHeapsize = status.pr_brksize / 1000;
					iStacksize = 	status.pr_stksize / 1000;	
       		sUserMemsize = sinfo.pr_bysize ;
					sUserAllMemsize = sinfo.pr_byrssize ;
					sProcname = sinfo.pr_fname;
	        close(fd);
	}		
#else
#define MEM_OTHER_OS
#endif

//构造函数
InParamMem::InParamMem()
{
	//pcon = jdb::getConnect();
	}

//析构函数
InParamMem::~InParamMem()
{}

//初始化
bool InParamMem::init(const PCFmlBuf &buf)
{
	try{
      string pid;     
  		sPid=getPid();//获取进程ID
  		getTime();		 //主机时间
	    if(buf->IsFldExist("X_TRANS_CODE"))
	        sTranscode = buf->GetString("X_TRANS_CODE");//交易编码
	    //sInbuf = inBuf.ToString();//buffer信息
	    uAllBufSize = buf->CalBufSize();//打印输入buffer的大小
	    uUsedBufSize = buf->GetUsed();//使用buffer大小
	    uUnUsedBufSize = buf->GetUnused();//未使用buffer大小
	    pid_t currentPid = getpid();
      mem_check_rpt(currentPid);
	    return true;
	}
	catch( Exception &e ){
	    return false;
	}
}

//获取系统时间
void InParamMem::getTime()
{
	char strTime[30] = "\0";

	time_t currenttime;
	struct tm *timeval;	
	/*取当前原始时间*/
	(void)time(&currenttime);
	/*转换原始时间至本地时*/
	timeval = localtime(&currenttime);
	sprintf(strTime, "%d%02d%02d%02d%02d%02d",1900+timeval->tm_year,1+timeval->tm_mon,
									timeval->tm_mday,timeval->tm_hour,timeval->tm_min,timeval->tm_sec);
	sMachineTime = strTime;
}

//获取系统时间
string InParamMem::getPid()
{
	char pid_s[20] = "\0";
	string spid_s;
	pid_t pid = -1;
	pid = getpid();	
	sprintf(pid_s,"%d",pid);	
	spid_s = pid_s;
	return spid_s;
}

int InParamMem::intoFile(const PCFmlBuf &buf)
{
  FILE* fd = 0;
  char filename[50]="\0";
  sprintf(filename,"../log/%s_%s.rpt",sProcname.substr(0,sProcname.length()-6).c_str(),sPid.c_str());
  
  fd = fopen(filename,"a+");
  if(fd)
  {
  	//写入跟踪信息
  	fprintf(fd,"==========================================================================\n");
  	fprintf(fd,"[流程名] %s\n",sTranscode.c_str());
  	fprintf(fd,"[执行开始时间] %s\n",sMachineTime.c_str());
  	fprintf(fd,"[输入BUFFER信息] 占用大小:%d, 已使用大小:%d, 未使用大小:%d \n",uAllBufSize, uUsedBufSize, uUnUsedBufSize);
  	#ifdef MEM_IBMAIX
  		fprintf(fd,"[开始内存状态] 进程内存(P):%d , 进程内存(A):%d , 堆内存:%d \n",sUserMemsize,sUserAllMemsize,iHeapsize);
  	#endif 
  	#ifdef MEM_SUN
  		fprintf(fd,"[开始内存状态] 进程内存(P):%d , 进程内存(A):%d , 堆内存:%d , 栈内存:%d \n",sUserMemsize,sUserAllMemsize,iHeapsize,iStacksize);
  	#endif
  	#ifdef MEM_HPUX
  		//fprintf(fd,"[开始内存信息PS] 进程内存(ps):%s \n",sUserMemsize.c_str());
  		fprintf(fd,"[开始R内存状态]  代码段:%d , 数据段:%d, 栈空间:%d\n", uTsize, uDsize, uSsize);
  		fprintf(fd,"[开始V内存状态]  代码段:%d , 数据段:%d, 栈空间:%d\n", uVtsize, uVdsize, uVssize);
  	#endif	  	
  	fprintf(fd,"[流程输入数据]\n");
  	//fprintf(fd,"%s\n",sInbuf.c_str());   	
  	buf->printFmlBuffer(fd);
  	fclose(fd);
  }  
	return 0;
}


int InParamMem::intoFileMiddle(const PCFmlBuf &buf)
{
  FILE* fd = 0;
  char filename[50]="\0";
  sprintf(filename,"../log/%s_%s.rpt",sProcname.substr(0,sProcname.length()-6).c_str(),sPid.c_str());
  
  fd = fopen(filename,"a+");
  if(fd)
  {
  	//写入跟踪信息
  	fprintf(fd,"[当前执行时间] %s\n",sMachineTime.c_str());
  	fprintf(fd,"[中间输入BUFFER信息] 占用大小:%d, 已使用大小:%d, 未使用大小:%d \n",uAllBufSize, uUsedBufSize, uUnUsedBufSize);
  	#ifdef MEM_IBMAIX
  		fprintf(fd,"[中间内存状态] 进程内存(P):%d , 进程内存(A):%d , 堆内存:%d \n",sUserMemsize,sUserAllMemsize,iHeapsize);
  	#endif 
  	#ifdef MEM_SUN
  		fprintf(fd,"[中间内存状态] 进程内存(P):%d , 进程内存(A):%d , 堆内存:%d , 栈内存:%d \n",sUserMemsize,sUserAllMemsize,iHeapsize,iStacksize);
  	#endif
  	#ifdef MEM_HPUX
  		//fprintf(fd,"[中间内存信息PS] 进程内存(ps):%s \n",sUserMemsize.c_str());
  		fprintf(fd,"[中间R内存状态]  代码段:%d , 数据段:%d, 栈空间:%d\n", uTsize, uDsize, uSsize);
  		fprintf(fd,"[中间V内存状态]  代码段:%d , 数据段:%d, 栈空间:%d\n", uVtsize, uVdsize, uVssize);
  	#endif
  	fprintf(fd,"[节点输入数据]\n");
  	//fprintf(fd,"%s\n",sInbuf.c_str());  
  	buf->printFmlBuffer(fd); 	
  	fclose(fd);
  }  
	return 0;
}

int InParamMem::intoFileEnd(const PCFmlBuf &buf)
{
  FILE* fd = 0;
  char filename[50];
  sprintf(filename,"../log/%s_%s.rpt",sProcname.substr(0,sProcname.length()-6).c_str(),sPid.c_str());
  
  fd = fopen(filename,"a+");
  if(fd)
  {
  	//写入跟踪信息
  	fprintf(fd,"[执行结束时间] %s\n",sMachineTime.c_str());
  	fprintf(fd,"[结束输出BUFFER信息] 占用大小:%d, 已使用大小:%d, 未使用大小:%d \n",uAllBufSize, uUsedBufSize, uUnUsedBufSize);
  	#ifdef MEM_IBMAIX
  		fprintf(fd,"[结束内存状态] 进程内存(P):%d , 进程内存(A):%d , 堆内存:%d \n",sUserMemsize,sUserAllMemsize,iHeapsize);
 	#endif 
 	  #ifdef MEM_SUN
  		fprintf(fd,"[结束内存状态] 进程内存(P):%d , 进程内存(A):%d , 堆内存:%d , 栈内存:%d \n",sUserMemsize,sUserAllMemsize,iHeapsize,iStacksize);
  	#endif
  	#ifdef MEM_HPUX
  		//fprintf(fd,"[结束内存信息PS] 进程内存(ps):%s \n",sUserMemsize.c_str());
  		fprintf(fd,"[结束R内存状态]  代码段:%d , 数据段:%d, 栈空间:%d\n", uTsize, uDsize, uSsize);
  		fprintf(fd,"[结束V内存状态]  代码段:%d , 数据段:%d, 栈空间:%d\n", uVtsize, uVdsize, uVssize);
  	#endif
  	//fprintf(fd,"%s\n",sInbuf.c_str());
  	string strEnv;
  	if(!getenv("OUT_BUFFER_TRACE")) strEnv = "0";
		else strEnv = getenv("OUT_BUFFER_TRACE");
  	
  	if ( "1" == strEnv){
			fprintf(fd,"[流程输出数据]\n");
			buf->printFmlBuffer(fd);
		}
  	fprintf(fd,"==========================================================================\n");	
  	fclose(fd);
  } 
	return 0;
}
