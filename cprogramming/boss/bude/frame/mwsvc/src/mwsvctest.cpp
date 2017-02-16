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
//#include "mwlcu/mwlcu.h"
#include "prodmanm/ProductMgr.h"
//#include "ParamMgr.h"

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
	//todo somethings
}

//服务代码，调用EBMP4C引擎入口函数完成业务功能
extern "C" void MWSVC(TPSVCINFO *rqst)
{
    string strTrancCode;

    //获取输入数据，构造通用输入数据缓冲区
    CFmlBuf inBuf((FBFR *)rqst->data);
    CFmlBuf outBuf;

    //PCFmlBuf inBuf(new CFmlBuf((FBFR *)rqst->data));

    //调用引擎入口函数，完成业务处理，获取输出数据
    //EBPM4C::execute(inBuf,outBuf);
    //MWLCU&  mwlcu = MWLCU::getInstance();
    //PCFmlBuf outBuf = mwlcu.execute(inBuf);

    strTrancCode = inBuf.GetString("X_TRANS_CODE");

    //cout <<"Before Call Function"<<endl;

    if (strTrancCode == "QPM_GetProducts")
        GetProductsByType(inBuf,outBuf);
    else if (strTrancCode == "QPM_GetProductCatalog")
        GetProductTypeTree(inBuf,outBuf);
    else if (strTrancCode == "QPM_GetProductService")
        GetServiceByPId(inBuf,outBuf);
    /*else if (strTrancCode == "QPU_GetNormalPara")
        GetNormalPara(inBuf,outBuf);*/
    else
    {
        outBuf.SetInt("X_RESULTCODE",-1);
        outBuf.SetString("X_RESULTINFO","本服务中不存在该交易编码:"+strTrancCode);
    }

    //cout <<"After Call Function"<<endl;

    //cout<< "outBuf->Print():"<< endl;
    outBuf.Print();

    FBFR * fb = (FBFR*)outBuf.GetData();
    tpreturn(TPSUCCESS, 0,(char *)fb, 0L, 0);
}

//通过编译发布出不同的进程名和服务名
