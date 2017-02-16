#include "udbi.h"
#include "atmi.h"
#include "userlog.h"

using namespace UDBI;
using namespace std;

JConnect *pcon;

int tpsvrinit(int argc, char *argv[])
{
	if (tpopen() < 0) {
		userlog("tpopen() error ... ");
		exit(1);
	}
	
	pcon = 0;
	//jdb::jinit("eboss", "scott", "tiger");
	jdb::jinitXa();
	pcon = jdb::getConnect();
	if (pcon) {
		userlog("connect success ...");
	}
	
	return 0;
}

void tpsvrdone()
{
	jdb::getConnect()->DisConnect();

	if (tpclose() < 0) {
		userlog("tpclose() error ... ");
		exit(0);
	}
}

extern "C" 
void BOSS_SVC(TPSVCINFO *rqst)
{
	long iRet=0, ilen=0;
	int  iID=0;
    char sID[13]="", sName[31]="", sMsg[512]="";
	char *rcvbuf, *sendbuf;

	userlog("BOSS_SVC begin ...");
	
	// recieve data
	ilen = strlen((char *)rqst->data);
	rcvbuf = (char *)tpalloc("STRING", NULL,ilen+2);	
	strcpy(rcvbuf, rqst->data);
	
	sendbuf = (char *)tpalloc("STRING",NULL, 200);

	cout << "RCVBUF:" << rcvbuf << endl;
	
	strncpy(sID, rcvbuf, 12);
	sID[12]='\0';
	strncpy(sName, rcvbuf+12, 30);
	sName[30]='\0';
	iID = atoi(sID);
	
	cout << sID << ":" << sName << endl;
	
	// transaction control
	tpbegin((long)0, (long)0);
		
	// business logic
	JTable *pTab = new JTable(pcon); 
	try
	{
		pTab->AddSql("update udbi_test set name=:name, dtime=sysdate where id=:id");
		pTab->SetParam(":id", iID);
		pTab->SetParam(":name", sName, 31);

		//pcon->TransBegin();
		pTab->ExecUpdate();
		//pcon->TransCommit();
		
		tpcommit((long)0);
	} catch (UdbiError &err) {
		cout << err.message << endl;
	}
	delete pTab;
	
	
	/** 
	 * 测试嵌套数据库连接情况是否可以 BEGIN
	 * 如果是初始化在xa模式下，则此处获取的是同一个svc句柄，所以等效于一个连接
	 * 如果是初始化在非xa模式下，则此处可以创建一个新的数据库连接
	 * 
	 */	
	/*
	JConnect *lpcon = jdb::Connect();
	JTable *opTab = new JTable(lpcon);
	try
	{
		opTab->AddSql("update udbi_test set type = 'AAA' where id = 1100000001");
		opTab->ExecUpdate();
	} catch (UdbiError &err) {
		cout << err.message << endl;
	}
	delete opTab;
	*/
	/*
	JConnect *lpcon = jdb::Connect("eboss", "guest", "guest");
	JTable *opTab = new JTable(lpcon);
	try
	{
		opTab->AddSql("update wangfj_test set name = 'AAA'");
		lpcon->TransBegin();
		opTab->ExecUpdate();
		lpcon->TransCommit();
	} catch (UdbiError &err) {
		cout << err.message << endl;
	}
	delete opTab;
	delete lpcon;
	*/
	
	/**< 测试嵌套数据库连接情况是否可以 END*/
	
	// return results
    strcpy(sendbuf, "OK");
	userlog("return call ... ");
    tpreturn(TPSUCCESS, 0, (char *)sendbuf, 0L,0);
}

	