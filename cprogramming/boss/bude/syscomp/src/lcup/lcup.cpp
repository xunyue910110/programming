 /**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2003</p>
 * <p>Company:Linkage </p>
 * <p>File:lcuprocess.cpp </p>
 * <p>author:sunxx </p>
 * @version 1.0
 */

#include <string>
#include <iostream>
#include "mwci/mwci.h"
#include "udbi/udbi.h"
#include "mwlcu/mwlcu.h"
#include "log4cpp/Logger.h"

using namespace std;
using namespace UDBI;
using namespace log4cpp;

JConnect *pcon;
Logger& logger = Logger::getLogger("lcup");

//进程启动代码，加载资源
int init()
{
	ifstream oTotalStream;
	string username,password,sid;
	/* userlog writes to the central TUXEDO message log */

	char *pFileName="DB_CONF";

  	oTotalStream.open(pFileName, ios::in);

	if (oTotalStream.rdstate() != ios::goodbit)
	{
		oTotalStream.close();
    	LOG_TRACE_P1(logger, "读取配置文件失败：%s",pFileName);
		return(-1);
	}

	oTotalStream>>username>>password>>sid;
	oTotalStream.close();
	jdb::jinit(sid, username, password);
	pcon = jdb::getConnect();
	return(0);
}


//进程退出代码，释放资源
void done()
{
    LOG_TRACE(logger, "done...");
	pcon->DisConnect();
	jdb::jdeinit();
}

//MWLCU入口函数完成流程调用功能
PCFmlBuf process(char* transcode)
{
    LOG_TRACE(logger, "process...");
	PCFmlBuf inBuf(new CFmlBuf());

	if(transcode==NULL)
		inBuf->SetString("X_TRANS_CODE","TPU_BatchProc");
	else
		inBuf->SetString("X_TRANS_CODE",transcode);

	MWLCU&  mwlcu = MWLCU::getInstance();
	PCFmlBuf outBuf = mwlcu.execute(inBuf);
    LOG_TRACE_P1(logger, "流程调用结果输出：\n%s",outBuf->ToString().c_str());
	return outBuf;
}

//记录流程调用日志
void execlog(PCFmlBuf &inBuf)
{
    LOG_TRACE(logger, "execlog...");
	inBuf->SetString("X_TRANS_CODE","TPU_BatchProcLog");
	MWLCU&  mwlcu = MWLCU::getInstance();
	PCFmlBuf outBuf = mwlcu.execute(inBuf);
    LOG_TRACE_P1(logger, "流程调用日志记录结果输出：\n%s",outBuf->ToString().c_str());
}

//主函数
int main(int argc,char ** argv)
{
    LOG_TRACE(logger, "main...");
	init();
	PCFmlBuf outBuf = process(argv[1]);
	execlog(outBuf);
	done();
}
