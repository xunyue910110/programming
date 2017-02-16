/**
* <p>Title: </p>
* <p>Description: </p>
* <p>Copyright: Copyright (c) 2003</p>
* <p>Company:Linkage </p>
* <p>File:mwsvc.cpp </p>
* <p>author:sunxx </p>
* @version 1.0
*/
#include <string>
#include <iostream>
#include "mwci/mwci.h"
#include "mwlcu/mwlcu.h"
#include "inParam_Mem.h"
#include "MwsvcConf.h"
#include "busiapi.h"
#include "dynlib/DynamicLibrary.h"
#include "log4cpp/Logger.h"

using namespace std;
using namespace log4cpp;

JConnect *pcon;
MwsvcConf mwsvcconf;
static Logger& logMwsvc = Logger::getLogger("gboss.bude.mwsvc");

//内存信息打印参数判断
bool is_mem_check()
{
	ifstream cfgfile("../etc/mem_check.cfg");
	if (cfgfile)
	{
		char spid[30] = "\0";
		memset(spid,0,30);
		while(cfgfile.getline(spid,30))
		{
			if (atol(spid) == getpid())
				return true;
			else
				memset(spid,0,30);
		}
	}	
	return false;
}

//服务进程启动代码，加载资源
int tpsvrinit(int argc,char ** argv)
{
    try{
        //jdb::jinit("eboss", "scott", "tiger");
        //pcon = jdb::getConnect();
        tpopen();
        jdb::jinitXa();
        pcon=jdb::getConnect();
        MWLCU&  mwlcu = MWLCU::getInstance();
        mwlcu.tpsvrinit(argc,argv);
        
        //加载接触管理配置文件
		string sLogFilePath = getenv("HOME");	
		cout<<"sLogFilePath="<<sLogFilePath<<endl;
		if(sLogFilePath.length()==0)
			return -1;		
		sLogFilePath = sLogFilePath + "/etc/MwsvcConf.xml";		
		mwsvcconf.Init(sLogFilePath);
		if(!mwsvcconf.ParserMwsvcConfXmlFile())
			return -1;
		mwsvcconf.print();
		
    }catch(...){
        userlog("tpsvrinit exception ...");
    }
    return 0;
}

//服务进程退出代码，释放资源
void tpsvrdone()
{
    try{
        tpclose();
        pcon->DisConnect();
        jdb::jdeinit();
    }catch(...){
        userlog("tpsvrdone exception ...");
    }
}

//服务代码，调用EBMP4C引擎入口函数完成业务功能
extern "C" void MWSVC(TPSVCINFO *rqst)
{
    //获取输入数据，构造通用输入数据缓冲区
    //CFmlBuf inBuf((FBFR *)rqst->data);
    //CFmlBuf outBuf;
    FBFR * srcBuf = NULL;  
		FBFR * rqstBuf = NULL;
		FBFR * fb = NULL;
    long resultcode = 0;
    char resultinfo[200] = "\0";
		
    //调用引擎入口函数，完成业务处理，获取输出数据
    try
    {  	    
	    //输入buffer赋值	
	  	srcBuf = (FBFR *)rqst->data;
	  	
	  	//分配内存给请求buffer
	  	rqstBuf = (FBFR *)tpalloc((char *)"FML32",NULL,(FLDLEN)Fsizeof(srcBuf));
	  	if (rqstBuf == (FBFR *)NULL){
				userlog((char *)"MWSVC:分配缓存失败!");
				resultcode = -1;
				strcpy(resultinfo,"MWSVC:分配缓存失败!");
				if( Fchg(srcBuf,Fldid("X_RESULTCODE"),0, (char *)&resultcode, (FLDLEN)0 ) == -1 ||
						Fchg(srcBuf,Fldid("X_RESULTINFO"),0, (char *)resultinfo, (FLDLEN)sizeof(resultinfo)) == -1 ){
					userlog((char *)"MWSVC:Fchg赋值失败!");
				}
				tpreturn(TPFAIL,0,(char *)srcBuf,0L,0);
			}
			
			//初始化请求buffer
/*			if (Finit(rqstBuf,(FLDLEN)Fsizeof(srcBuf)) < 0){
				userlog((char *)"MWSVC:内存初始化失败!");
				tpfree((char *)rqstBuf);
				tpreturn(TPFAIL,-1L,(char *)srcBuf,0L,0);
			}
*/			
			//复制输入buffer到请求buffer
			if (Fcpy(rqstBuf,srcBuf) < 0 ){
				userlog((char *)"MWSVC:内存复制失败!");
				resultcode = -1;
				strcpy(resultinfo,"MWSVC:内存复制失败!");
				if( Fchg(srcBuf,Fldid("X_RESULTCODE"),0, (char *)&resultcode, (FLDLEN)0 ) == -1 ||
						Fchg(srcBuf,Fldid("X_RESULTINFO"),0, (char *)resultinfo, (FLDLEN)sizeof(resultinfo) ) == -1 ){
					userlog((char *)"MWSVC:Fchg赋值失败!");
				}
				if (rqstBuf != (FBFR *)NULL)tpfree((char *)rqstBuf);
				rqstBuf = (FBFR *)NULL;
				tpreturn(TPFAIL,0,(char *)srcBuf,0L,0);						
			}
			
	    PCFmlBuf inBuf(new CFmlBuf(rqstBuf));
	   
	    MWLCU&  mwlcu = MWLCU::getInstance();
	    	
	    //内存检查代码Begin................
			InParamMem inParMem;
		
			if (is_mem_check()){
				inParMem.init(inBuf);		
				inParMem.intoFile(inBuf);
			}
			//内存检查代码End................
	    PCFmlBuf outBuf = mwlcu.execute(inBuf);
	


			LOG_TRACE(logMwsvc, "----entry------");
			//接触信息生成
			if(mwsvcconf.chkPrintTag())
			{
				LOG_TRACE(logMwsvc, "--------chkPrintTag-----------");

				if(outBuf->IsFldExist("X_SAVE_CONTACT"))
				{
					LOG_TRACE(logMwsvc, "--------X_SAVE_CONTACT  exist-----------");

					ContactInfo contactinfo;					
					//判断 当前接入方式,当前地州 是否生成接触信息
					if(outBuf->IsFldExist("CT_INMODECODE"))
						contactinfo.ct_inmodecode = outBuf->GetString("CT_INMODECODE");

					LOG_TRACE_P1(logMwsvc, "Mwsvc接入方式[%s]",contactinfo.ct_inmodecode.c_str());

					if(outBuf->IsFldExist("CT_EPARCHYCODE"))
						contactinfo.ct_eparchycode = outBuf->GetString("CT_EPARCHYCODE");

					LOG_TRACE_P1(logMwsvc, "Mwsvc-CT_EPARCHYCODE[%s]",contactinfo.ct_eparchycode.c_str());

					if(mwsvcconf.chkInmodePrintTag(contactinfo.ct_inmodecode) && mwsvcconf.chkEparchyPrintTag(contactinfo.ct_eparchycode))
					{
						LOG_TRACE(logMwsvc, "--------chk inmode && eparchycode-----------");

						if(outBuf->IsFldExist("CT_CITYCODE"))
							contactinfo.ct_citycode = outBuf->GetString("CT_CITYCODE");
						if(outBuf->IsFldExist("CT_CONTACTTIME"))
							contactinfo.ct_contacttime = outBuf->GetString("CT_CONTACTTIME");
						if(outBuf->IsFldExist("CT_CONTACTMODE"))
							contactinfo.ct_contactmode = outBuf->GetString("CT_CONTACTMODE");
						if(outBuf->IsFldExist("CT_CHANNELID"))
							contactinfo.ct_channelid = outBuf->GetString("CT_CHANNELID");
						if(outBuf->IsFldExist("CT_CONTACTERID"))
							contactinfo.ct_contacterid = outBuf->GetString("CT_CONTACTERID");
						if(outBuf->IsFldExist("CT_CUSTID"))
							contactinfo.ct_custid = outBuf->GetString("CT_CUSTID");
						if(outBuf->IsFldExist("CT_CONTACTTRADETYPE"))
							contactinfo.ct_contacttradetype = outBuf->GetString("CT_CONTACTTRADETYPE");
						if(outBuf->IsFldExist("CT_CONTACTTRADEID"))
							contactinfo.ct_contacttradeid = outBuf->GetString("CT_CONTACTTRADEID");
						if(outBuf->IsFldExist("CT_CONTACTDESC"))
							contactinfo.ct_contactdesc = outBuf->GetString("CT_CONTACTDESC");
						if(outBuf->IsFldExist("CT_CONTACTFINISHTIME"))
							contactinfo.ct_contactfinishtime = outBuf->GetString("CT_CONTACTFINISHTIME");

						string x_save_contact = outBuf->GetString("X_SAVE_CONTACT");

						LOG_TRACE_P1(logMwsvc, "Mwsvc-CT_CITYCODE[%s]",contactinfo.ct_citycode.c_str());
						LOG_TRACE_P1(logMwsvc, "Mwsvc-CT_CONTACTTIME[%s]",contactinfo.ct_contacttime.c_str());
						LOG_TRACE_P1(logMwsvc, "Mwsvc-CT_CONTACTMODE[%s]",contactinfo.ct_contactmode.c_str());
						LOG_TRACE_P1(logMwsvc, "Mwsvc-CT_CHANNELID[%s]",contactinfo.ct_channelid.c_str());
						LOG_TRACE_P1(logMwsvc, "Mwsvc-CT_CONTACTERID[%s]",contactinfo.ct_contacterid.c_str());
						LOG_TRACE_P1(logMwsvc, "Mwsvc-CT_CUSTID[%s]",contactinfo.ct_custid.c_str());
						LOG_TRACE_P1(logMwsvc, "Mwsvc-CT_CONTACTTRADETYPE[%s]",contactinfo.ct_contacttradetype.c_str());
						LOG_TRACE_P1(logMwsvc, "Mwsvc-CT_CONTACTDESC[%s]",contactinfo.ct_contactdesc.c_str());
						LOG_TRACE_P1(logMwsvc, "Mwsvc-CT_CONTACTFINISHTIME[%s]",contactinfo.ct_contactfinishtime.c_str());
						LOG_TRACE_P1(logMwsvc, "Mwsvc-X_SAVE_CONTACT[%s]",x_save_contact.c_str());

						string libname;
						string method;
						
						DynamicLibraryManager dLibMgr;
						DynamicLibraryManager * manager;
						manager = &dLibMgr;
						string sHome = getenv("HOME");
						sHome = sHome + "/lculib";
						manager->setWorkPath(sHome);

						if((x_save_contact.compare("a")==0)||(x_save_contact.compare("A")==0))
						{
							LOG_TRACE(logMwsvc, "--------a------");
							/*从配置文件中获取要调用的"动态库","函数"名称*/
							libname = mwsvcconf.getLibName("APPENDTRADE2CONTACT");
							method = mwsvcconf.getApiName("APPENDTRADE2CONTACT");

							LOG_TRACE_P1(logMwsvc, "libname=[%s]",libname.c_str());
							LOG_TRACE_P1(logMwsvc, "method=[%s]",method.c_str());

							AppendTrade2Contact appendTrade2Contact = (AppendTrade2Contact)manager->getSymbol(libname, method);

							LOG_TRACE(logMwsvc, "----a----manager->getSymbol------");
							
							appendTrade2Contact(contactinfo.ct_eparchycode, contactinfo.ct_citycode, contactinfo.ct_contacttime, contactinfo.ct_contactmode, contactinfo.ct_inmodecode, contactinfo.ct_channelid, contactinfo.ct_contacterid, contactinfo.ct_custid, contactinfo.ct_contacttradetype, contactinfo.ct_contacttradeid, contactinfo.ct_contactdesc);
							
							LOG_TRACE(logMwsvc, "----a----appendTrade2Contact------");
						}
						else if((x_save_contact.compare("s")==0)||(x_save_contact.compare("S")==0))
						{
							/*从配置文件中获取要调用的"动态库","函数"名称*/
							libname = mwsvcconf.getLibName("STARTCONTACT");
							method = mwsvcconf.getApiName("STARTCONTACT");
							cout<<"libname="<<libname<<endl;
							cout<<"method="<<method<<endl;
							StartContact startContact = (StartContact)manager->getSymbol(libname, method);
							startContact(contactinfo.ct_eparchycode, contactinfo.ct_citycode, contactinfo.ct_contacttime, contactinfo.ct_contactmode, contactinfo.ct_inmodecode, contactinfo.ct_channelid, contactinfo.ct_contacterid, contactinfo.ct_custid, contactinfo.ct_contactdesc);
						}
						else if((x_save_contact.compare("f")==0)||(x_save_contact.compare("F")==0))
						{
							/*从配置文件中获取要调用的"动态库","函数"名称*/
							libname = mwsvcconf.getLibName("FINISHCONTACT");
							method = mwsvcconf.getApiName("FINISHCONTACT");
							cout<<"libname="<<libname<<endl;
							cout<<"method="<<method<<endl;
							FinishContact finishContact = (FinishContact)manager->getSymbol(libname, method);
							finishContact(contactinfo.ct_eparchycode, contactinfo.ct_citycode, contactinfo.ct_contacttime, contactinfo.ct_channelid, contactinfo.ct_contacterid, contactinfo.ct_custid);
						}
					}
				}
			}


      //内存检查代码Begin................
			if (is_mem_check()){
				inParMem.init(outBuf);			
				inParMem.intoFileEnd(outBuf);
			}
			//内存检查代码End................
			
      fb = (FBFR*)outBuf->GetData();
      outBuf->SetData(NULL);
/*      if (rqstBuf != (FBFR *)NULL) tpfree((char *)rqstBuf);
      rqstBuf = (FBFR *)NULL;
      //防止inBuf再次自动释放rqstBuf
      inBuf->SetData(NULL);*/
    }catch(...){
      userlog("service execute exception ...");
//    	if (rqstBuf != (FBFR *)NULL) tpfree((char *)rqstBuf);
    	tpreturn(TPFAIL, 0,(char *)fb, 0L, 0);
    }
    tpreturn(TPSUCCESS, 0,(char *)fb, 0L, 0);
}

//通过编译发布出不同的进程名和服务名
