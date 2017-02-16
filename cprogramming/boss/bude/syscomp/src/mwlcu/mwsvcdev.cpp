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
		//todo somethings
		//jdb::jinit("gboss", "scott", "tiger");
		//pcon = jdb::getConnect();
		tpopen();
		jdb::jinitXa();
		pcon=jdb::getConnect();
		//jdb::init();
		//pcon = jdb::Connect("gboss", "scott", "tiger");
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
extern "C" void MWSVC(TPSVCINFO *rqst)
{
		//获取输入数据，构造通用输入数据缓冲区
		//CFmlBuf inBuf((FBFR *)rqst->data);
		//CFmlBuf outBuf;
		FBFR * fb;

		//调用引擎入口函数，完成业务处理，获取输出数据
		//EBPM4C::execute(inBuf,outBuf);
		{
			PCFmlBuf inBuf(new CFmlBuf((FBFR *)rqst->data));
			MWLCU&  mwlcu = MWLCU::getInstance();
			PCFmlBuf outBuf = mwlcu.execute(inBuf);

			fb = (FBFR*)outBuf->GetData();
			outBuf->SetData(0);
	    }

		tpreturn(TPSUCCESS, 0,(char *)fb, 0L, 0);
}

//通过编译发布出不同的进程名和服务名
