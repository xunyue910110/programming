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
#include "udbi/udbi.h"
#include "mwlcu/mwlcu.h"
using namespace std;
using namespace UDBI;
JConnect *pcon;

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
		FBFR * fb = NULL;
		try
		{
		    //获取请求业务
		    PCFmlBuf inBuf(new CFmlBuf((FBFR *)rqst->data));
		    string xTransCode;
		    string ibRequestTransCode("ITF_IBCO_IBRequestLog");
		    string ibResponseTransCode("ITF_IBCO_IBResponseLog");
	        
            
		    MWLCU&  mwlcu = MWLCU::getInstance();
		    PCFmlBuf outBuf1(new CFmlBuf());
		    PCFmlBuf outBuf2(new CFmlBuf());
		    PCFmlBuf outBuf3(new CFmlBuf());
		    
		    
		    //记录请求日志
		    if(inBuf->IsFldExist("X_TRANS_CODE"))
 		    {
		    	xTransCode = inBuf->GetString("X_TRANS_CODE"); 
		    	inBuf->SetString("X_TRANS_CODE",ibRequestTransCode,0);
		    	outBuf1 = mwlcu.execute(inBuf);
		    	inBuf->SetString("X_TRANS_CODE",xTransCode,0);
		    }
            
            
		    //完成业务处理
		    outBuf2 = mwlcu.execute(inBuf);
		    
            
            
		    //更新请求日志
		    if(outBuf2->IsFldExist("X_TRANS_CODE"))
 		    {
		    	xTransCode = outBuf2->GetString("X_TRANS_CODE"); 
		    	outBuf2->SetString("X_TRANS_CODE",ibResponseTransCode,0);
		    	outBuf3 = mwlcu.execute(outBuf2);
		    	outBuf2->SetString("X_TRANS_CODE",xTransCode,0);
		    }
            
            
		    FBFR * fb = (FBFR*)outBuf2->GetData();
		    outBuf1->SetData(NULL);
		    outBuf2->SetData(NULL);
		    outBuf3->SetData(NULL);
		}
		catch(...)
		{
            userlog("service execute exception ...");
	        tpreturn(TPFAIL, 0,(char *)fb, 0L, 0);
        }

		tpreturn(TPSUCCESS, 0,(char *)fb, 0L, 0);
}

//通过编译发布出不同的进程名和服务名
