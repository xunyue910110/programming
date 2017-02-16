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
#include "mwci.h"
#include "udbi.h"
#include "sotest.h"


using namespace std;
using namespace UDBI;
JConnect *pcon;

//服务进程启动代码，加载资源
int tpsvrinit(int argc,char ** argv)
{
        //todo somethings
        //jdb::jinit("eboss", "scott", "tiger");
        //pcon = jdb::getConnect();
        tpopen();
        jdb::jinitXa();
        pcon=jdb::getConnect();
        //jdb::init();
        //pcon = jdb::Connect("EBOSS", "scott", "tiger");
        return 0;
}

//服务进程退出代码，释放资源
void tpsvrdone()
{
        tpclose();
        pcon->DisConnect();
        jdb::jdeinit();
        //todo somethings
}

//服务代码，调用EBMP4C引擎入口函数完成业务功能
extern "C" void MWSVCSOTEST(TPSVCINFO *rqst)
{
//获取输入数据，构造通用输入数据缓冲区
    //CFmlBuf inBuf((FBFR *)rqst->data);
    //CFmlBuf outBuf;
    PCFmlBuf inBuf(new CFmlBuf((FBFR *)rqst->data));
   cout<< "[sotest][begin]-------------------"<< endl; 
    //调用引擎入口函数，完成业务处理，获取输出数据
    //EBPM4C::execute(inBuf,outBuf);
   //cout<< "inBuf->Print():"<< endl;
   //inBuf->Print(); 
   //tp开事务  
   MWSOTEST& sotest = MWSOTEST::getInstance();
   PCFmlBuf outBuf = sotest.testExecute(inBuf);
            
   //cout<< "outBuf->Print():"<< endl;
   //outBuf->Print();   
   cout<< "[sotest][end]---------------------"<< endl; 
    FBFR * fb = (FBFR*)outBuf->GetData();
    //delete ctx;
    tpreturn(TPSUCCESS, 0,(char *)fb, 0L, 0);
}   

//通过编译发布出不同的进程名和服务名
